#ifndef NARRATOR_H_
# define NARRATOR_H_
#pragma once

#include "person.h"
#include "markovchain.h"

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <exception>
#include <iomanip>

#define MAP_ROWS 60
#define MAP_COLS 150

#define HELP_NEW            "new <type> <sex> <name>"
#define HELP_CLONE          "clone <id> <new_name>"
#define HELP_LIST           "list <hosts|guests>"
#define HELP_INFO           "info <id>"
#define HELP_UPDATE         "update <id> <attribute> <value>"
#define HELP_FRIEND         "friend <add|remove> <id 1> <id 2>"
#define HELP_SENTENCE       "sentence <add|remove> <id> <sentence>"
#define HELP_MAP            "map"
#define HELP_MOVE           "move <id> {<l|r|u|d>+}"
#define HELP_RANDOM_MOVE    "random_move"
#define HELP_NEXT_DAY       "next_day"
#define HELP_PROMPT         "prompt <show|hide>"
#define HELP_HELP           "help"
#define HELP_EXIT           "quit"

struct InvalidArgumentsException : public std::exception {
	public:
		InvalidArgumentsException(const std::string& s)
			: msg(s)
		{
		}
		const char *what() const throw() {
			return msg.c_str();
		}
	private:
		std::string msg;
};

class Narrator {
	public:
		Narrator(void);
		unsigned getDays(void);
		bool executeCommand(const std::string& command);
		person_t newPerson(const person_type type, const person_sex sex, const std::string& name);
		void printHelp(void);
		void nextDay(void);

	private:
		void join(const std::vector<std::string>& vec, unsigned short start_pos,
		          const char *delim, std::string& output);
		void positionPerson(const person_t& person);
		void newPerson(const std::vector<std::string>& args);
		void clonePerson(const std::vector<std::string>& args);
		person_t getPerson(const std::string& id);
		void listPersons(const std::vector<std::string>& args);
		void updatePerson(const std::vector<std::string>& args);
		void manageFriend(const std::vector<std::string>& args);
		void manageSentence(const std::vector<std::string>& args);
		void printInfos(const std::vector<std::string>& args);
		void printMap(void);
		void removeFromMap(person_t& person);
		void placeOnMap(person_t& person);
		void move(const std::vector<std::string>& args);
		void movePerson(person_t& person, const char direction);
		void moveRandom(void);

		std::vector<person_t> *park_map[MAP_ROWS][MAP_COLS];
		unsigned short maze_center_x;
		unsigned short maze_center_y;
		std::vector<person_t> hosts;
		std::vector<person_t> guests;
		unsigned days;
};

#endif /* !NARRATOR_H_ */
