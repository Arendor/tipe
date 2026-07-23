

#include "player.h"

#include <random>
#include <algorithm>
#include "../game/game_engine.h"
#include <iostream>

extern std::mt19937 gen;

Player::Player() : playerID(none), cards(), isLinked(false), owningGame(0), remaining_cards({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53 })
{

}

Player::~Player()
{

}



void Player::reset() {
	remaining_cards = std::unordered_set<int>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53 });
	cards.empty();

	return;
};

/*
* Appelé par le game engine quand le joueur doit jouer 
*/
void Player::play()
{
	std::array<int,2> ar = choose_cardNumber_pos();

	owningGame->play_card(ar[0],ar[1], playerID);


	int card_drawn = owningGame->draw_card(playerID);

	if (card_drawn == -1) cards.del(ar[0]);
	else {
		remaining_cards.erase(card_drawn);
		cards.replace(ar[0], card_drawn);
		// Ceci va maintenant être appelé par game Engine
		//card_drawn_EVENTC(card_drawn);
	};

	// Ceci va maintenant être appelé par le game engine
	//card_played_call(ar[0], ar[1]);

	return;
}

std::array<int, 2> Player::choose_cardNumber_pos() {
	std::vector<int> allowed_frontiers = {};

	for (int i = 0; i < 9; i++) {
		if (!owningGame->isFull(i, playerID)) allowed_frontiers.push_back(i);
	}

	std::uniform_int_distribution<> int_dis_fr(0, allowed_frontiers.size() - 1);
	std::uniform_int_distribution<> int_dis_card(0, cards.size - 1);

	return std::array<int, 2> {cards[int_dis_card(gen)], allowed_frontiers[int_dis_fr(gen)]};
};



void Player::win_EVENTC(PlayerType playerID)
{

}

void Player::winFrontier_EVENTC(int n_frontier, PlayerType player)
{

}

void Player::card_drawn_EVENTC(int card, PlayerType player)
{


}
void Player::card_played_EVENTC(int card, int frontier, PlayerType player)
{

	if (player != playerID) {
		remaining_cards.erase(card);
	}
}

void Player::end_turn_EVENTC(PlayerType player)
{
};


void Player::distribuer_cartes(int* cards_drawn)
{
	for (int i = 0; i < 6; i++) {
		cards.add(cards_drawn[i]);
		remaining_cards.erase(cards_drawn[i]);
	};

};

bool Player::hasCard(int card) const
{
	return cards.has(card);
}



bool Player::link(GameEngine* game, PlayerType id)
{
	isLinked = true;
	playerID = id;
	owningGame = game;

	return true;
}

