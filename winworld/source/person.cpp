#include "person.h"

void find_and_replace(std::string& source, const std::string& find_str, const std::string& replace_str)
{
    for (std::string::size_type i = 0; (i = source.find(find_str, i)) != std::string::npos;)
    {
        source.replace(i, find_str.length(), replace_str);
        i += replace_str.length();
    }
}

Person::Person(const person_type new_type, const person_sex new_sex, const std::string& new_name)
	: is_alive(true), is_enabled(true), days(0), moves_count(0), death_count(0), pos_x(0), pos_y(0)
{
	type = new_type;
	sex = new_sex;
	is_conscious = (new_type != HOST);
	name = new_name;
	onEncounter = &Person::encounterTalk;
	attack = 5;
	health = 20;
	max_health = 20;
	luck = 5;
	sex_affinity = (new_sex == MALE) ? FEMALE : MALE;

	std::cout << "Creating person '" << name << "'" << " type " << getType() << "..." <<std::endl;
}

/*
 * Vuln 1: Uninitialized heap variable in this copy constructor (used when cloning a person).
 *         The is_conscious attribute isn't set. This will allow us to create a conscious "host".
 */

Person::Person(person_t person, const std::string& new_name)
	: is_alive(true), is_enabled(true), days(0), moves_count(0), death_count(0), pos_x(0), pos_y(0)
{
	std::cout << "Cloning '" << person->name << "', renaming to '" << new_name << "'" << std::endl;

	onEncounter = person->onEncounter;
	sentences = person->sentences;
	name = new_name;
	sex = person->sex;
	type = person->type;
	attack = person->attack;
	health = person->max_health;
	max_health = person->max_health;
	luck = person->luck;
	sex_affinity = person->sex_affinity;
	friends = person->friends;
}

Person::~Person()
{
#ifdef DEBUG
	std::cout << "Removing person '" << name << "'..." << std::endl;
#endif
}

std::string Person::getName(void)
{
	return name;
}

bool Person::setName(const std::string& new_name)
{
	if (new_name.empty()) {
		return false;
	}

	name = new_name;

	return true;
}

bool Person::updateAttribute(const update_attr attribute, const uint16_t value)
{
	switch (attribute) {
		case ATTACK:
			if (value > 0 && value <= 10) {
				attack = value;
			} else {
				std::cout << "Attack must be between 1 and 10";
				return false;
			}
			break;
		case HEALTH:
			if (value > 0 && value <= 50) {
				if (health == max_health) {
					health = value;
				}
				max_health = value;
				if (health > max_health) {
					health = value;
				}
			} else {
				std::cout << "Health must be between 1 and 50";
				return false;
			}
			break;
		case LUCK:
			if (value > 0 && value < 10) {
				luck = value;
			} else {
				std::cout << "Luck must be between 1 and 9";
				return false;
			}
			break;
		case AFFINITY:
			if (!value || (value & ~(MALE | FEMALE | SHEMALE))) {
				return false;
			}
			sex_affinity = value;
			break;
		default:
			return false;
	}

	return false;
}

bool Person::updateAttribute(const update_attr attribute, const std::string& value)
{
	if (attribute == ENCOUNTER) {
		if (!value.compare("attack")) {
			onEncounter = &Person::encounterAttack;
		} else if (!value.compare("parrot")) {
			onEncounter = &Person::encounterParrot;
		} else if (!value.compare("fuck")) {
			onEncounter = &Person::encounterFuck;
		} else if (!value.compare("talk")) {
			onEncounter = &Person::encounterTalk;
		} else {
			return false;
		}
		return true;
	}

	return false;
}

bool Person::move(const char direction)
{
	if (!is_alive) {
		return false;
	}

	switch (direction) {
		case 'u':
			pos_x--;
			break;
		case 'd':
			pos_x++;
			break;
		case 'l':
			pos_y--;
			break;
		case 'r':
			pos_y++;
			break;
		default:
			std::cerr << "Invalid direction '" << direction << "'" << std::endl;
			return false;
	}

	moves_count += 1;

	return true;
}

bool Person::manageSentence(const manage_action action, const std::string& sentence)
{
	auto pos = std::find(sentences.begin(), sentences.end(), sentence);

	switch (action) {
		case ADD:
			if (pos != sentences.end()) {
				return false;
			}
			sentences.push_back(sentence);
			markov->generateFromLine(sentence);
			break;
		case REMOVE:
			if (pos == sentences.end()) {
				return false;
			}
			sentences.erase(pos);
			break;
		default:
			return false;
	}

	return true;
}

bool Person::manageFriend(const manage_action action, person_t& person)
{
	if (person.get() == this) {
		return false;
	}

	auto pos = std::find_if(friends.begin(), friends.end(),
							[person](const auto& p) { return p.get() == person.get();});

	switch (action) {
		case ADD:
			if (pos != friends.end()) {
				return false;
			}
			friends.push_back(person);
			break;
		case REMOVE:
			if (pos == friends.end()) {
				return false;
			}
			friends.erase(pos);
			break;
		default:
			return false;
	}

	return true;
}

bool Person::isFriend(person_t& person)
{
	for (auto &f : friends) {
		if (f.get() == person.get()) {
			return true;
		}
	}

	return false;
}

void Person::encounterAttack(person_t& person)
{
	if (isFriend(person)) {
		encounterTalk(person);
		return;
	}

	if (rand() % (10 - person->luck)) {
		auto damage = attack;

		if (damage >= person->health) {
			damage = person->health;

			if (is_host(person)) {          /* attacking a host */
				person->health = 0;
				person->is_alive = false;
			} else {                        /* attacking a guest */
				if (is_host(this)) {
					damage--;
					person->health = 1;
					std::cout << person->getName() << " escapes death because hosts can't kill guests" << std::endl;
				} else {
					person->health = 0;
					person->is_alive = false;
				}
			}
		} else {
			person->health -= damage;
		}

		std::cout << name << " attacks " << person->getName() << ", inflicting " << damage << " damage." << std::endl;
		std::cout << person->getName() << " " << (person->is_alive ? "survived" : "met a tragic death") << std::endl;
	} else {
		std::cout << name << " attacked " << person->getName()
		          << ", but " << person->getName() << " dodged the attack!" << std::endl;
	}
}

void Person::encounterParrot(person_t& person)
{
	auto methodPtr = onEncounter;

	if (person->onEncounter == onEncounter) {
		return;
	}

	onEncounter = person->onEncounter;
	(*this.*onEncounter)(person);
	onEncounter = methodPtr;
}

void Person::encounterFuck(person_t& person)
{
	if (sex_affinity & person->sex) {
		if (isFriend(person)) {
			std::cout << name << " and " << person->getName() << " make love! <3" << std::endl;
			return;
		} else {
			bool do_fuck = (rand() % (10 - person->luck)) != 0;
			std::cout << name << " rapes " << person->getName() << "!" << std::endl;
		}
	} else {
		std::cout << name << " isn't attracted by " << person->getName() << "..." << std::endl;
	}
}

void Person::encounterTalk(person_t& person)
{
	std::string seed;
	int attempt;

	if (sentences.size()) {
		std::string& sentence = sentences.at(rand() % sentences.size());
		seed = markov->getSeedFromLine(sentence);
	}
	else {
		seed = "_name_";
	}

	for (int i = rand() % 10; i > 0; i--) {
		std::string line = markov->generateText(seed);

		for (attempt = 0; attempt < 3 && sentences.size(); attempt++) {
			if (line.compare("error")) {
				break;
			}

			std::string& sentence = sentences.at(rand() % sentences.size());
			seed = markov->getSeedFromLine(sentence);
			line = markov->generateText(seed);
		}

		if (attempt == 3) {
			line = markov->generateText("_name_");
		}

		seed = markov->getSeedFromLine(line);
		find_and_replace(line, "_name_", person->getName());
		std::cout << "<" << name << "> " << line << std::endl;

		line = markov->generateText(seed);

		for (attempt = 0; attempt < 3 && sentences.size(); attempt++) {
			if (line.compare("error")) {
				break;
			}

			std::string& sentence = sentences.at(rand() % sentences.size());
			seed = markov->getSeedFromLine(sentence);
			line = markov->generateText(seed);
		}

		seed = markov->getSeedFromLine(line);
		find_and_replace(line, "_name_", name);
		std::cout << "<" << person->getName() << "> " << line << std::endl;
	}
}

void Person::encounter(person_t& person)
{
	std::cout << "[+] " << name << " encounters " << person->getName() << std::endl;

	(*this.*onEncounter)(person);
}

void Person::printInfos(void)
{
	const auto delim = std::string(80, '-');

	std::cout << delim << std::endl
	          << "Name: " << name << std::endl
	          << "Type: " << getSex() << " " << getType() << std::endl
	          << "Position: (" << pos_x << ", " << pos_y << ")" << std::endl
	          << "Stats: attack " << attack << ", health " << health << "/" << max_health
	          << ", luck " << luck << std::endl
	          << "Sexual affinity:";

	if (sex_affinity & MALE) {
		std::cout << " male";
	}
	if (sex_affinity & FEMALE) {
		std::cout << " female";
	}
	if (sex_affinity & SHEMALE) {
		std::cout << " shemale";
	}
	std::cout << std::endl;

	std::cout << "Alive: " << (is_alive ? "yes" : "no") << std::endl;

	if (is_host(this)) {
		std::cout << "Dead " << death_count << " times in ";
	} else {
		std::cout << "Has been here for ";
	}

	std::cout << days << " days and did " << moves_count << " moves." << std::endl;
	debug("Conscious: " << is_conscious);

	// wow, such code. Too bad we can't do a switch(funcptr) :(
	const char *action;
	if (onEncounter == &Person::encounterAttack) {
		action = "attack";
	} else if (onEncounter == &Person::encounterParrot) {
		action = "parrot";
	} else if (onEncounter == &Person::encounterFuck) {
		action = "fuck";
	} else {
		action = "talk";
	}
	std::cout << "On encounter: " << action << std::endl;

	std::cout << "Friends:";

	if (friends.size()) {
		std::cout << std::endl;

		for (auto &f: friends) {
			std::cout << "  - " << f->getName()
			          << " (" << f->getType() << ", " << f->getStatus() << ")" << std::endl;
		}
	} else {
		std::cout << " none" << std::endl;
	}
	std::cout << "Seed sentences:";

	if (sentences.size()) {
		std::cout << std::endl;

		for (auto &sentence: sentences) {
			std::cout << "  - " << sentence << std::endl;
		}
	} else {
		std::cout << " none" << std::endl;
	}

	std::cout << delim << std::endl;
}

const char *Person::getStatus(void)
{
	if (is_alive) {
		return "alive";
	}

	return "dead";
}

const char *Person::getType(void)
{
	if (is_host(this)) {
		return "host";
	} else if (is_guest(this)) {
		return "guest";
	}
	return "invalid";
}

const char *Person::getSex(void)
{
	const char *sex_str;

	switch (sex) {
		case MALE:
			sex_str = "male";
			break;
		case FEMALE:
			sex_str = "female";
			break;
		case SHEMALE:
			sex_str = "shemale";
			break;
		default:
			sex_str = "invalid";
	}

	return sex_str;
}
