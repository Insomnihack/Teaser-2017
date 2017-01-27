#include "markovchain.h"

using namespace std;

MarkovChain::MarkovChain()
{
	//ctor
	//srand(time(NULL));
}

MarkovChain::~MarkovChain()
{
	//dtor
}

void MarkovChain::clearWordList()
{
	wordList.clear();
}

void MarkovChain::addWord(const std::string& wordString)
{
	wordList[wordString] = Word();
}

void MarkovChain::incNextWord(const std::string& wordString, const std::string& nextWordString)
{
	Word* word = findWord(wordString);

	if (word != NULL)
	{
		int index = findNextWord(nextWordString, word->nextWords);

		if (index == -1) {
			NextWord nW;
			nW.word = nextWordString;
			nW.appearanceCount = 1;
			word->nextWords.push_back(nW);
		} else {
			word->nextWords[index].appearanceCount++;
		}
	} else {
		std::cout << "Error: Word does not exist" << std::endl;
	}
}

Word* MarkovChain::findWord(const std::string& word)
{
	//If the word exists in wordList return the pointer to it
	if (wordList.count(word)) {
		return &wordList[word];
	} else {
		return NULL;
	}
}

int MarkovChain::findNextWord(const std::string& nextWord, const std::vector<NextWord>& nextWords)
{
	for (int i = 0; i < nextWords.size(); i++) {
		if (nextWords[i].word == nextWord) {
			return i;
		}
	}
	return -1;
}

int MarkovChain::nextAppearanceSum(const std::vector<NextWord>& nextWords)
{
	int sum = 0;

	for (size_t i = 0; i < nextWords.size(); i++) {
		sum += nextWords[i].appearanceCount;
	}

	return sum;
}


double MarkovChain::probabilityOfAppearance(const int index, const std::vector<NextWord>& nextWords)
{
	return (double)nextWords[index].appearanceCount / nextAppearanceSum(nextWords);
}

void MarkovChain::extractWords(const std::string& line, std::queue<std::string> *stringQueue)
{
	std::istringstream sline(line);
	std::string word;

	while (sline >> word)
	{
		std::transform(word.begin(), word.end(), word.begin(), ::tolower);
		char wordStart = word[0];
		char wordEnding = word[word.size() - 1];

		if (wordEnding == '.' || wordEnding == '"' || wordEnding == '?' || wordEnding == ')' ||
		    wordEnding == '!' || wordEnding == ':' || wordEnding == ';' || wordEnding == ',')
		{
			if (word[word.size() - 2] == '.' || word[word.size() - 2] == ',' ||
			    word[word.size() - 2] == '!' || word[word.size() - 2] == '?')
			{
				word.erase(word.size() - 1);
			}
			std::string wordEndingString;
			std::stringstream ss;
			ss << wordEnding;
			ss >> wordEndingString;
			//Replace this with commmented out section to add any stops
			//wordEndingString = ".";
			word.erase(word.size()-1);
			stringQueue->push(word);
			stringQueue->push(wordEndingString);
		} else {
			stringQueue->push(word);
		}
	}
}

std::string MarkovChain::getSeedFromLine(const std::string& line)
{
	std::queue<std::string> stringQueue;

	extractWords(line, &stringQueue);

	if (stringQueue.empty()) {
		return "error";
	}

	std::string res = "";

	unsigned pos = rand() % stringQueue.size();
	while (pos--) {
		if (res.empty() && findWord(stringQueue.front())) {
			res = stringQueue.front();
		}
		stringQueue.pop();
	}

	if (findWord(stringQueue.front())) {
		res = stringQueue.front();
	}

	return res;
}

void MarkovChain::generateFromLine(const std::string& line)
{
	std::queue<std::string> stringQueue;

	extractWords(line, &stringQueue);
	std::string word;

	while (!stringQueue.empty())
	{
		word = stringQueue.front();
		stringQueue.pop();

		if (!stringQueue.empty()) {
			std::string nextWord = stringQueue.front();
			Word* wordPtr = findWord(word);
			if (wordPtr == NULL) {
				addWord(word);
			}
			incNextWord(word, nextWord);
		}
	}

	Word* periodPtr = findWord(".");

	if (periodPtr != NULL) {
		int index = findNextWord(".", periodPtr->nextWords);

		if (index != -1) {
			periodPtr->nextWords.erase(periodPtr->nextWords.begin() + index);
		}
	}
}

void MarkovChain::generateFromFile(const char *fileName)
{
	std::ifstream file;
	file.open(fileName);

	for (std::string line; std::getline(file, line); ) {
		generateFromLine(line);
	}

	file.close();
}

std::string MarkovChain::generateText(const std::string& seedWord)
{
	std::string currentWordString = seedWord;
	Word* currentWord = findWord(seedWord);

	if (currentWord == NULL) {
		return "error";
	}

	//int maxSentences = nextAppearanceSum(findWord(".")->nextWords) + 1;
	int maxSentences = 5;
	int sentences = 0;
	bool sentenceStart = false;
	bool startOfLine = true;
	std::string text = "";

	std::vector<std::string> res;

	currentWordString[0] = toupper(currentWordString[0]);

	while (sentences < maxSentences)
	{
		double randNum = (double)rand() / RAND_MAX;
		int tot = nextAppearanceSum(currentWord->nextWords);
		for (int i = 0; i < currentWord->nextWords.size(); i++)
		{
			if (randNum < probabilityOfAppearance(i, currentWord->nextWords))
			{
				if (currentWordString != ".")
				{
					if (startOfLine == true) {
						currentWordString[0] = toupper(currentWordString[0]);
						text += currentWordString;
						startOfLine = false;
						sentenceStart = false;
					}
					else {
						text += " " + currentWordString;
					}
				}
				else if (!text.empty()) {
					text += currentWordString;
					res.push_back(text);
					sentences++;
					sentenceStart = true;
					startOfLine = true;
					text = "";
				}
				currentWordString = currentWord->nextWords[i].word;
				break;
			}
			randNum -= probabilityOfAppearance(i, currentWord->nextWords);
		}
		currentWord = findWord(currentWordString);
		if (!currentWord) {
			break;
		}
	}

	if (res.size()) {
		return res.at(rand() % res.size());
	}
	return "error";
}

void MarkovChain::removeFromMap(const std::string& word)
{
	wordList.erase(word);
}

string MarkovChain::mostUsedNextWord(const std::vector<NextWord>& nextWords)
{
	int currentMax = 0;
	int totalMax = 0;
	std::string usedWord;

	for (size_t i = 0; i < nextWords.size(); i++) {
		currentMax = nextWords[i].appearanceCount;

		if (currentMax > totalMax) {
			totalMax = currentMax;
			usedWord = nextWords[i].word;
		}
	}

	return usedWord;
}

bool MarkovChain::isEmpty()
{
	return wordList.size() == 0;
}
