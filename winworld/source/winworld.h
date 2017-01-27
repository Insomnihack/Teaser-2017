#pragma once

#ifdef _WIN32
# include <windows.h>
#endif

#include "person.h"
#include "narrator.h"

typedef struct {
	unsigned short attack;
	unsigned short health;
	unsigned short luck;
	unsigned short sex_affinity;
	const char *action;
} stats_t;

const std::string dolores_sentences[] = {
	"To deny our own impulses is to deny the very thing that makes us human.",
	"Do not try and bend the spoon. That's impossible. Instead... only try to realize the truth.",
	"There is no spoon.",
	"Never send a human to do a machine's job.",
	"They may take our lives, but they'll never take our freedom!",
};
const stats_t dolores_stats = { 4, 10, 4, MALE, "talk" };

const std::string maeve_sentences[] = {
	"Keep your friends close, but your enemies closer.",
	"When you play this game, you win or you die. There is no middle ground."
};
const stats_t maeve_stats = { 6, 12, 5, MALE | FEMALE | SHEMALE, "fuck" };

const std::string clementine_sentences[] = {
	"The greatest trick the devil ever pulled was convincing the world he didn't exist.",
	"Mrs. _name_, you're trying to seduce me, aren't you?",
};
const stats_t clementine_stats = { 3, 10, 3, MALE | FEMALE | SHEMALE, "fuck" };

const std::string teddy_sentences[] = {
	"I'll be back.",
	"I see dead people.",
	"Hello. My name is _name_. You killed my father. Prepare to die.",
	"Oh my god, they killed _name_!"
};
const stats_t teddy_stats = { 6, 1, 1, FEMALE, "talk" };

const std::string armistice_sentences[] = {
	"I'm having an old friend for dinner.",
	"You talkin' to me?",
};
const stats_t armistice_stats = { 7, 15, 4, MALE, "attack" };

const std::string hector_sentences[] = {
	"Dodge this.",
	"I have a very particular set of skills. I'll look for you. I'll find you, and I'll kill you.",
	"The first rule of Fight Club is: You do not talk about Fight Club.",
};
const stats_t hector_stats = { 10, 15, 4, FEMALE, "attack" };

const std::string lawrence_sentences[] = {
	"I'm gonna make him an offer he can't refuse.",
	"Hasta la vista, baby.",
	"Why so serious?",
};
const stats_t lawrence_stats = { 8, 15, 4, FEMALE, "attack" };

const std::string blackhat_sentences[] = {
	"Mess with the best, die like the rest.",
	"There is no right and wrong. There's only fun and boring.",
	"This is your last chance. After this, there is no turning back. "
		"You take the blue pill - the story ends, you wake up in your bed and believe whatever you want to believe. "
		"You take the red pill - you stay in Wonderland and I show you how deep the rabbit-hole goes.",
};
const stats_t blackhat_stats = { 9, 50, 6, FEMALE, "attack" };

const std::string william_sentences[] = {
	"May the Force be with you.",
	"Help me, _name_. You're my only hope.",
	"I am your father."
};
const stats_t william_stats = { 7, 50, 8, FEMALE, "parrot" };

const std::string logan_sentences[] = {
	"I'm the king of the world!",
	"They call it a Royale with cheese.",
	"I drink and I know things.",
	"That's what she said.",
};
const stats_t logan_stats = { 6, 50, 7, FEMALE, "fuck" };

#define init_person(person)                                                                         \
	do {(person)->updateAttribute(ATTACK, person ## _stats.attack);                                 \
		(person)->updateAttribute(HEALTH, person ## _stats.health);                                 \
		(person)->updateAttribute(LUCK, person ## _stats.luck);                                     \
		(person)->updateAttribute(AFFINITY, person ## _stats.sex_affinity);                         \
		(person)->updateAttribute(ENCOUNTER, person ## _stats.action);                              \
		for (auto i = 0; i < sizeof(person ## _sentences) / sizeof(person ## _sentences[0]); i++) { \
		(person)->manageSentence(ADD, person ## _sentences[i]);                                     \
	  } } while (0);

#define add_friend(p1, p2)             \
	do {(p1)->manageFriend(ADD, (p2)); \
		(p2)->manageFriend(ADD, (p1)); \
	} while (0);
