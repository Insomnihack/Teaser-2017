#ifndef MARKOVCHAIN_H_
# define MARKOVCHAIN_H_

#pragma once
#include <map>
#include <vector>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <queue>
#include <fstream>
#include <sstream>
#include <random>
#include <time.h>
#include <algorithm>

struct Word;

struct NextWord {
	std::string word;
	int appearanceCount;
};

struct Word {
	std::vector<NextWord> nextWords;
};

class MarkovChain
{
	public:
		MarkovChain();
		virtual ~MarkovChain();
		void addWord(const std::string& word);
		void incNextWord(const std::string& word, const std::string& nextWord);
		Word* findWord(const std::string& word);
		int findNextWord(const std::string& word, const std::vector<NextWord>& nextWords);
		int nextAppearanceSum(const std::vector<NextWord>& nextWords);
		std::string generateText(const std::string& seedWord);
		std::string getSeedFromLine(const std::string& line);
		void generateFromLine(const std::string& line);
		void generateFromFile(const char *fileName);
		void removeFromMap(const std::string& word);
		double probabilityOfAppearance(const int index, const std::vector<NextWord>& nextWords);
		std::string mostUsedNextWord(const std::vector<NextWord>& nextwords);
		void clearWordList();
		bool isEmpty();

	protected:
	private:
		void extractWords(const std::string& line, std::queue<std::string> *stringQueue);
		std::map<std::string, Word> wordList;

};

extern MarkovChain *markov;

#endif /* !MARKOVCHAIN_H_ */
