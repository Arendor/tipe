

#include "policy.h"
#include "../player/agent.h"
#include "../game/game_engine.h"
#include "../utils/utils.h"
#include <random>

#include <limits>


extern std::mt19937 gen;

Policy::Policy() : isLinked(false), owningUser(0), global_turns(0), local_turns(0), isTraining(false), debug_mode(false), nb_parties(0), info_mode(false)
{
}

Policy::~Policy()
{
}


void Policy::restart_game()
{

	local_turns = 0;

}


bool Policy::link(Agent* agent)
{
	isLinked = true;
	owningUser = agent;

	return true;
}


std::vector<int> Policy::getAllowedFrontiers()
{
	std::vector<int> allowed_frontiers = {};

	for (int i = 0; i < 9; i++) {
		if (!this->owningUser->owningGame->isFull(i, this->owningUser->playerID)) allowed_frontiers.push_back(i);
	}

	return allowed_frontiers;

}

Policy& Policy::setTraining(bool tr)
{
	isTraining = tr;
	return *this;
}

void Policy::setDebugMode(bool mode)
{
	debug_mode = mode;
}

void Policy::setInfoMode(bool mode)
{
	info_mode = mode;
}


/*
* Random card
*/
std::array<int, 2> Policy::choose_action()
{

	std::vector<int> allowed_frontiers = getAllowedFrontiers();
	std::uniform_int_distribution<> int_dis_fr(0, allowed_frontiers.size() - 1);
	std::uniform_int_distribution<> int_dis_card(0, this->owningUser->cards.size - 1);

	return std::array<int, 2> {this->owningUser->cards[int_dis_card(gen)], allowed_frontiers[int_dis_fr(gen)]};

}


/*bool Policy::is_allowed(int indice) {
	int frontier = indice / 6;
	int card = indice % 6;
	// Vérifie que l'agent a bien le droit de poser cette carte ie frontière pas pleine et suffisamment de carte en main
	return this->owningUser->owningGame->isFull(frontier, this->owningUser->playerID) && this->owningUser->cards.size > card;
}*/





void Policy::start_turn_EVENTC(bool ismyturn)
{
	if (ismyturn) {
		local_turns += 1;
		global_turns += 1;
	};
}


void Policy::end_turn_EVENTC(bool ismyturn) {

}

void Policy::card_drawn_EVENTC(int card, bool ismyturn) {

}

void Policy::card_played_EVENTC(int card, int frontier, bool ismyturn) {
}


void Policy::end_party_EVENTC(PlayerType winner) {

	nb_parties += 1;

}

void Policy::frontier_win_EVENTC(int n_frontier, bool haswin) {

}

