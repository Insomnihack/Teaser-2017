#ifndef WINWORLD_H_
# define WINWORLD_H_

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>

#include "markovchain.h"

#ifdef DEBUG
# define debug(string) { std::cout << string << std::endl; }
#else
# define debug(string)
#endif

#define is_host(p) ((p)->type == HOST)
#define is_guest(p) ((p)->type == GUEST)

enum update_attr { ATTACK, HEALTH, LUCK, AFFINITY, ENCOUNTER };
enum manage_action { ADD, REMOVE };
enum person_sex { MALE = 1, FEMALE = 2, SHEMALE = 4};
enum person_type { HOST, GUEST };

class Person;
typedef std::shared_ptr<Person> person_t;
class Narrator;

class Person {
	public:
		Person(const person_type type, const person_sex sex, const std::string& name);
		Person(person_t person, const std::string& new_name);
		~Person();

		std::string getName(void);
		bool setName(const std::string& name);
		bool move(const char direction);
		bool isFriend(person_t& person);
		bool manageFriend(const manage_action action, person_t& person);
		bool manageSentence(const manage_action action, const std::string& sentence);
		bool updateAttribute(const update_attr attribute, const uint16_t value);
		bool updateAttribute(const update_attr attribute, const std::string& value);
		void printInfos(void);
		void encounter(person_t& person);
		const char *getStatus(void);
		const char *getType(void);
		const char *getSex(void);

	private:
		friend class Narrator;

		void encounterAttack(person_t& person);
		void encounterParrot(person_t& person);
		void encounterFuck(person_t& person);
		void encounterTalk(person_t& person);

		void (Person::*onEncounter)(person_t&);
		std::vector<person_t> friends;
		std::vector<std::string> sentences;

		std::string name;
		person_type type; // host / guest
		person_sex sex;
		bool is_alive;
		bool is_conscious;
		bool is_enabled; // hosts only
		uint32_t days;
		uint32_t moves_count;
		uint32_t death_count;
		uint32_t pos_x;
		uint32_t pos_y;
		uint32_t attack;
		uint32_t health;
		uint32_t max_health;
		uint32_t luck;
		uint32_t sex_affinity;
};

#endif /* !WINWORLD_H_ */
