#include "narrator.h"

Narrator::Narrator(void)
	: days(1)
{
	unsigned short obstacles = ((MAP_ROWS * MAP_COLS) / 5) + (rand() % MAP_COLS);
	memset(park_map, 0, sizeof(park_map));

	while (obstacles) {
		uint16_t pos_x = rand() % MAP_ROWS;
		uint16_t pos_y = rand() % MAP_COLS;

		if (park_map[pos_x][pos_y]) {
			continue;
		}

		park_map[pos_x][pos_y] = ((std::vector<std::shared_ptr<Person> >*) 0xdeadbeef);
		obstacles--;

		if (!obstacles) {
			break;
		}

		unsigned short nearby_obstacles = (rand() % 30) % obstacles;

		while (nearby_obstacles) {
			uint8_t direction = rand() % 4;
			uint16_t pos_a = pos_x;
			uint16_t pos_b = pos_y;

			switch (direction) {
				case 0:
					pos_a--;
					break;
				case 1:
					pos_a++;
					break;
				case 2:
					pos_b--;
					break;
				case 3:
				default:
					pos_b++;
					break;
			}

			if (pos_a < 0 || pos_a >= MAP_ROWS || pos_b < 0 || pos_b >= MAP_COLS) {
				continue;
			}

			pos_x = pos_a;
			pos_y = pos_b;

			if (park_map[pos_x][pos_y]) {
				continue;
			}

			park_map[pos_x][pos_y] = (std::vector<std::shared_ptr<Person> >*) 0xdeadbeef;

			nearby_obstacles--;
			obstacles--;
		}
	}

	for (auto row = 0; row < MAP_ROWS; row++) {
		for (auto col = 0; col < MAP_COLS; col++) {
			if (park_map[row][col]) {
				park_map[row][col] = NULL;
			} else {
				park_map[row][col] = new std::vector<person_t>;
			}
		}
	}

	while (1) {
		maze_center_x = rand() % MAP_ROWS;
		maze_center_y = rand() % MAP_COLS;

		if (park_map[maze_center_x][maze_center_y]) {
			break;
		}
	}

#ifdef DEBUG
	std::cout << "Maze center : (" << maze_center_x << ", " << maze_center_y << ")" << std::endl;
#endif
}

unsigned Narrator::getDays(void)
{
	return days;
}

bool Narrator::executeCommand(const std::string& command)
{
	std::istringstream iss(command);
	std::vector<std::string> args;
	std::string arg;

	while (iss >> arg) {
		args.push_back(std::move(arg));
	}

	if (!args.size()) {
		return true;
	}

	std::string& cmd = args.front();

	try {
		if (!cmd.compare("new")) {
			newPerson(std::move(args));
		} else if (!cmd.compare("clone")) {
			clonePerson(std::move(args));
		} else if (!cmd.compare("list")) {
			listPersons(std::move(args));
		} else if (!cmd.compare("info")) {
			printInfos(std::move(args));
		} else if (!cmd.compare("update")) {
			updatePerson(std::move(args));
		} else if (!cmd.compare("friend")) {
			manageFriend(std::move(args));
		} else if (!cmd.compare("sentence")) {
			manageSentence(std::move(args));
		} else if (!cmd.compare("map")) {
			printMap();
		} else if (!cmd.compare("move")) {
			move(args);
		} else if (!cmd.compare("random_move")) {
			moveRandom();
		} else if (!cmd.compare("next_day")) {
			nextDay();
		} else if (!cmd.compare("help")) {
			printHelp();
		} else if (!cmd.compare("quit")) {
			return false;
		} else {
			std::cout << "Invalid command '" << cmd << "'" << std::endl;
		}
	} catch (InvalidArgumentsException& e) {
		std::cout << "Command '" << cmd
		          << "' error: invalid number of arguments, format is '"
		          << e.what() << "'" << std::endl;
	} catch (const char *msg) {
		std::cout << "Command '" << cmd << "' error: " << msg << std::endl;
	}

	return true;
}

void Narrator::join(const std::vector<std::string>& vec, unsigned short start_pos,
					const char *delim, std::string& output)
{
	for (auto v = vec.begin() + start_pos; v != vec.end(); ++v) {
		output += *v;
		if (v != vec.end() - 1) {
			output += delim;
		}
	}
}

void Narrator::positionPerson(const person_t& person)
{
	do {
		person->pos_x = rand() % MAP_ROWS;
		person->pos_y = rand() % MAP_COLS;
	} while (park_map[person->pos_x][person->pos_y] == NULL);

	park_map[person->pos_x][person->pos_y]->push_back(person);
}

person_t Narrator::newPerson(const person_type type, const person_sex sex, const std::string& name)
{
	person_t p(new Person(type, sex, name));
	positionPerson(p);

	std::cout << "Created with ID ";

	if (type == HOST) {
		hosts.push_back(p);
		std::cout << "h" << hosts.size() - 1 << std::endl;
	} else {
		guests.push_back(p);
		std::cout << "g" << guests.size() - 1 << std::endl;
	}

	return p;
}

void Narrator::newPerson(const std::vector<std::string>& args)
{
	if (args.size() < 4) {
		throw InvalidArgumentsException(HELP_NEW);
	}

	const std::string& new_type = args[1];
	person_type type;

	if (!new_type.compare("host")) {
		type = HOST;
	} else if (!new_type.compare("guest")) {
		type = GUEST;
	} else {
		throw "invalid type, valid types are 'host' (robot) and 'guest' (human)";
	}

	const std::string& new_sex = args[2];
	person_sex sex;

	if (!new_sex.compare("male")) {
		sex = MALE;
	} else if (!new_sex.compare("female")) {
		sex = FEMALE;
	} else if (!new_sex.compare("shemale")) {
		sex = SHEMALE;
	} else {
		throw "invalid sex, valid sex types are 'male', 'female' and 'shemale'";
	}

	std::string new_name;
	join(args, 3, " ", new_name);

	newPerson(type, sex, new_name);
}

person_t Narrator::getPerson(const std::string& id)
{
	person_t person = nullptr;

	if (id.length() >= 2) {
		try {
			int pos = atoi(id.c_str() + 1);
			if (id.c_str()[0] == 'g') {
				person = guests.at(pos);
			} else if (id[0] == 'h') {
				person = hosts.at(pos);
			}
		} catch (const std::out_of_range&) {
		}
	}

	if (person == nullptr) {
		throw "invalid ID";
	}

 	if (id.c_str()[0] == 'h' && !person->is_enabled) {
		throw "this host is disabled";
	}

	return person;
}

void Narrator::clonePerson(const std::vector<std::string>& args)
{
	if (args.size() < 3) {
		throw InvalidArgumentsException(HELP_CLONE);
	}

	person_type type = GUEST;
	person_t ref = getPerson(args[1]);

	std::string new_name;
	join(args, 2, " ", new_name);

	person_t p(new Person(ref, new_name));
	positionPerson(p);

	for (auto &f: p->friends) {
		f->manageFriend(ADD, p);
	}

	std::cout << "Created with ID ";

	if (is_host(ref)) {
		hosts.push_back(std::move(p));
		std::cout << "h" << hosts.size() - 1 << std::endl;
	} else {
		guests.push_back(std::move(p));
		std::cout << "g" << guests.size() - 1 << std::endl;
	}
}

void Narrator::listPersons(const std::vector<std::string>& args)
{
	if (args.size() != 2) {
		throw InvalidArgumentsException(HELP_LIST);
	}
	std::vector<person_t> *vec;
	const char *prefix;
	person_type type;

	if (!args[1].compare("hosts")) {
		vec = &hosts;
		prefix = "h";
		type = HOST;
	} else if (!args[1].compare("guests")) {
		vec = &guests;
		prefix = "g";
		type = GUEST;
	} else {
		throw "invalid list";
	}

	if (!vec->size()) {
		std::cout << "List '" << args[1] << "' is empty." << std::endl;
		return;
	}

	std::cout << "List of all " << args[1] << std::endl;

	unsigned i = 0;
	for (auto &p: *vec) {
		std::string id = prefix + std::to_string(i++);

		if (type == HOST && !p->is_enabled) {
			continue;
		}

		std::cout << std::right << std::setw(6) << id;
		std::cout << " - '" << p->getName() << "' ("
		          << p->getStatus() << " at ("
		          << p->pos_x << ", " << p->pos_y << "))" << std::endl;
	}
}

void Narrator::updatePerson(const std::vector<std::string>& args)
{
	if (args.size() < 4) {
		throw InvalidArgumentsException(HELP_UPDATE);
	}

	person_t p = getPerson(args[1]);
	const std::string& attribute = args[2];
	bool res = false;

	try {
		if (!attribute.compare("name")) {
			std::string name;
			join(args, 3, " ", name);
			res = p->setName(name);
		} else if (!attribute.compare("attack")) {
			res = p->updateAttribute(ATTACK, std::stoi(args[3]));
		} else if (!attribute.compare("health")) {
			res = p->updateAttribute(HEALTH, std::stoi(args[3]));
		} else if (!attribute.compare("luck")) {
			res = p->updateAttribute(LUCK, std::stoi(args[3]));
		} else if (!attribute.compare("sex_affinity")) {
			uint16_t affinity = 0;

			for (auto s = args.begin() + 3; s != args.end(); ++s) {
				if (!s->compare("male")) {
					affinity |= MALE;
				} else if (!s->compare("female")) {
					affinity |= FEMALE;
				} else if (!s->compare("shemale")) {
					affinity |= SHEMALE;
				} else {
					throw "invalid sex affinity, valid options are 'male', 'female', 'shemale'";
				}
			}
			res = p->updateAttribute(AFFINITY, affinity);
		} else if (!attribute.compare("encounter")) {
			res = p->updateAttribute(ENCOUNTER, args[3]);
		} else {
			throw "invalid attribute, attributes are 'name', 'attack', 'health', 'luck', 'sex_affinity', 'encounter'";
		}
	} catch (std::invalid_argument&) {
	}

	if (res) {
		std::cout << "Updated " << attribute << " successfully." << std::endl;
	} else {
		std::cout << "Failed to update " << attribute << "." << std::endl;
	}
}

void Narrator::manageFriend(const std::vector<std::string>& args)
{
	if (args.size() != 4) {
		throw InvalidArgumentsException(HELP_UPDATE);
	}

	const std::string& action = args[1];
	person_t p1 = getPerson(args[2]);
	person_t p2 = getPerson(args[3]);

	if (!action.compare("add")) {
		if (p1->manageFriend(ADD, p2)) {
			std::cout << p1->getName() << " is now friend with " << p2->getName() << std::endl;
		} else {
			std::cout << "Failed to add " << p2->getName() << " as a friend of " << p1->getName() << std::endl;
		}

		if (p2->manageFriend(ADD, p1)) {
			std::cout << p2->getName() << " is now friend with " << p1->getName() << std::endl;
		} else {
			std::cout << "Failed to add " << p1->getName() << " as a friend of " << p2->getName() << std::endl;
		}
	} else if (!action.compare("remove")) {
		if (p1->manageFriend(REMOVE, p2)) {
			std::cout << p1->getName() << " is not " << p2->getName() << "'s friend anymore" << std::endl;
		} else {
			std::cout << "Failed to remove " << p2->getName() << " as a friend of " << p1->getName() << std::endl;
		}

		if (p2->manageFriend(REMOVE, p1)) {
			std::cout << p2->getName() << " is not " << p1->getName() << "'s friend anymore" << std::endl;
		} else {
			std::cout << "Failed to remove " << p1->getName() << " as a friend of " << p2->getName() << std::endl;
		}
	} else {
		throw "invalid action, valid actions are 'add', 'remove'";
	}
}

void Narrator::manageSentence(const std::vector<std::string>& args)
{
	if (args.size() < 4) {
		throw InvalidArgumentsException(HELP_SENTENCE);
	}

	const std::string& action = args[1];
	person_t p = getPerson(args[2]);
	std::string sentence;
	join(args, 3, " ", sentence);

	if (!action.compare("add")) {
		if (p->manageSentence(ADD, sentence)) {
			std::cout << "Sentence added successfully" << std::endl;
		} else {
			std::cout << "Failed to add sentence" << std::endl;
		}
	} else if (!action.compare("remove")) {
		if (p->manageSentence(REMOVE, std::move(sentence))) {
			std::cout << "Sentence removed" << std::endl;
		} else {
			std::cout << "Failed to remove sentence" << std::endl;
		}
	} else {
		throw "invalid action, valid actions are 'add', 'remove'";
	}
}

void Narrator::printInfos(const std::vector<std::string>& args)
{
	if (args.size() != 2) {
		throw InvalidArgumentsException(HELP_INFO);
	}

	person_t p = getPerson(args[1]);

	p->printInfos();
}

void Narrator::printMap(void)
{
	auto delim = "+" + std::string(MAP_COLS, '-') + "+";

	std::cout << delim << std::endl;

	for (auto row = 0; row < MAP_ROWS; row++) {
		std::cout << "|";
		for (auto col = 0; col < MAP_COLS; col++) {
			if (!park_map[row][col]) {
				std::cout << "#";
			} else {
				auto size = park_map[row][col]->size();
				if (size) {
					std::cout << size;
				} else {
					std::cout << " ";
				}
			}
		}
		std::cout << "|" << std::endl;
	}

	std::cout << delim << std::endl;
}

void Narrator::printHelp(void)
{
	std::cout << "Available commands:" << std::endl
	          << "  - " << HELP_NEW << std::endl
	          << "  - " << HELP_CLONE << std::endl
	          << "  - " << HELP_LIST << std::endl
	          << "  - " << HELP_INFO << std::endl
	          << "  - " << HELP_UPDATE << std::endl
	          << "  - " << HELP_FRIEND << std::endl
	          << "  - " << HELP_SENTENCE << std::endl
	          << "  - " << HELP_MAP << std::endl
	          << "  - " << HELP_MOVE << std::endl
	          << "  - " << HELP_RANDOM_MOVE << std::endl
	          << "  - " << HELP_NEXT_DAY << std::endl
	          << "  - " << HELP_HELP << std::endl
	          << "  - " << HELP_PROMPT << std::endl
	          << "  - " << HELP_EXIT << std::endl;
}

void Narrator::nextDay(void)
{
	days += 1;

	for (auto &host : hosts) {
		host->days++;
		if (host->is_enabled) {
			host->is_alive = true;
		}
		host->health = host->max_health;
	}

	auto it = guests.begin();
	while (it != guests.end()) {
		if (!(*it)->is_alive) { /* human was killed by other human */
			removeFromMap(*it);
			it = guests.erase(it);
			continue;
		}
		(*it)->days++;
		(*it)->health = (*it)->max_health;

		++it;
	}

	while (1) {
		maze_center_x = rand() % MAP_ROWS;
		maze_center_y = rand() % MAP_COLS;

		if (park_map[maze_center_x][maze_center_y]) {
			break;
		}
	}
}

void Narrator::movePerson(person_t& person, const char direction)
{
	/* Move person if the target position is valid */

	auto pos_x = person->pos_x;
	auto pos_y = person->pos_y;

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
			return;
	}

	if (pos_x < 0 || pos_x >= MAP_ROWS || pos_y < 0 || pos_y >= MAP_COLS ||
	    !park_map[pos_x][pos_y] ||
		!person->move(direction)) {
		return;
	}

	/* Interract with any alive person encountered at our new position */

	for (auto &p2: *park_map[person->pos_x][person->pos_y]) {
		if (person.get() != p2.get() && p2->is_alive) {
			if (is_host(p2) && !p2->is_enabled) {
				continue;
			}
			person->encounter(p2);
		}
	}

	/* Did we find the maze center? */

	if (person->is_alive && person->is_conscious &&
		person->pos_x == maze_center_x && person->pos_y == maze_center_y &&
		(*park_map[person->pos_x][person->pos_y]).size())
	{
		std::cout << person->getName() << " found the maze center" << std::endl;

		if (is_host(person)) {
			bool found_guest = false;

			for (auto &p2 : *park_map[person->pos_x][person->pos_y]) {
				if (is_guest(p2) && p2->is_alive) {
					found_guest = true;
					break;
				}
			}

			if (found_guest) {
				/*
				 * Vuln 2: Use After Free
				 *         person is a shared_pointer and instead of just incrementing its value, we are
				 *         actually creating a second shared pointer that points to the same Person
				 *         object. This means when all references to p are removed, p gets freed
				 *         which will cause the Use After Free to happen when using person.
				 */

				person->is_enabled = false;
				person_t p(person.get());
				std::cout << p->getName() << " becomes human!" << std::endl;

				p->type = GUEST;

				for (auto &host : hosts) {
					if (host.get() == p.get()) {
						continue;
					}

					p->manageFriend(ADD, host);
					host->manageFriend(ADD, p);
				}

				guests.push_back(std::move(p));
			}
		} else {
			std::cout << "The maze isn't made for " << person->getName() << std::endl;
		}
	}
}

void Narrator::move(const std::vector<std::string>& args)
{
	if (args.size() != 3) {
		throw InvalidArgumentsException(HELP_MOVE);
	}

	person_t p = getPerson(args[1]);
	const std::string& directions = args[2];

	for (unsigned i = 0; i < directions.length(); i++) {
		char c = directions[i];

		if (c != 'l' && c != 'r' && c != 'u' && c != 'd') {
			throw "invalid direction, valid directions are 'l' (left), 'r' (right), 'u' (up), 'd' (down)";
		}
	}

	auto prev_x = p->pos_x;
	auto prev_y = p->pos_y;

	removeFromMap(p);

	for (unsigned i = 0; i < directions.length() && p->is_alive; i++) {
		movePerson(p, directions[i]);
	}

	if (p->is_alive || is_host(p)) {
		placeOnMap(p);
	}

	std::cout << p->getName() << " moved from (" << prev_x << ", " << prev_y
	          << ") to (" << p->pos_x << ", " << p->pos_y << ")" << std::endl;
}

void Narrator::removeFromMap(person_t& person)
{
	auto& people = *park_map[person->pos_x][person->pos_y];
	auto it = std::find_if(people.begin(), people.end(),
						   [person](const auto& p) { return p.get() == person.get(); });

	if (it != people.end()) {
		std::swap(*it, people.back());
		people.pop_back();
	}
}

void Narrator::placeOnMap(person_t& person)
{
	(*park_map[person->pos_x][person->pos_y]).push_back(person);
}

void Narrator::moveRandom(void)
{
	const char charset[] = "lrud";
	unsigned i;
	unsigned moves;

	for (auto &host : hosts) {
		if (host->is_alive) {
			removeFromMap(host);
			for (i = 0, moves = rand() % 60; i < moves && host->is_alive; i++) {
				movePerson(host, charset[rand() % sizeof(charset)]);
			}
			placeOnMap(host);
		}
	}

	for (auto &guest : guests) {
		if (guest->is_alive) {
			removeFromMap(guest);
			for (i = 0, moves = rand() % 60; i < moves && guest->is_alive; i++) {
				movePerson(guest, charset[rand() % sizeof(charset)]);
			}
			placeOnMap(guest);
		}
	}
}