

#ifndef PLAYER
#define PLAYER

#include <array>
#include <unordered_set>
#include <armadillo>
#include "../utils/utils.h"
#include "../utils/deck.h"


class GameEngine;

class Player {

public:
	Player();
	virtual ~Player();

	GameEngine* owningGame;;

	Deck<DECK_SIZE> cards;

	std::unordered_set<int> remaining_cards;

	PlayerType playerID;
	bool isLinked;

	// Toutes les fonctions pourraient être virtuelles 

	// Fonction à conserver ici

	// A reimplementer dans agent.cpp
	virtual void distribuer_cartes(int* cards);

	bool hasCard(int card) const;

	bool link(GameEngine* game, PlayerType id);

	virtual void play();


	// Doit être appelé
	virtual void reset();


	/*
	* Mettre à jour connsole_player
	* agent
	* policy
	* game engine
	* 
	*/

	// Quels events sont appelés par qui ?
	// 
	// 
	// Après le tour de l'autre joueur seulement
	// Doit être réimplenté 
	// Aucun sens d'avoir un playerTYPE

	// Doit être réimplenté si on conserve la version initiale de play
	virtual std::array<int, 2> choose_cardNumber_pos();


	// Purement virtuels
	virtual void winFrontier_EVENTC(int n_frontier, PlayerType player);
	virtual void win_EVENTC(PlayerType playerID);

	/*
	* Si player != playerID par game engine
	* 
	* 0 si une carte est drawn, -1 sinon
	* 
	* pourrait aussi faire un overload avec bool hasdrawnn mais flm
	* 
	*/ 
	virtual void card_drawn_EVENTC(int card, PlayerType player);

	// DOIT ÊTRE RAPPELE
	virtual void card_played_EVENTC(int card, int frontier, PlayerType player);


	virtual void end_turn_EVENTC(PlayerType player);

	// Choose_cardNumber_pos() doit être appelé si la fonction Player::play est utilisé, sinon c'est inutile
	// winFrontier n'a pas besoin d'être appelé
	// Win n'a pas besoin d'être appelé
	// 

	// EVENT_end_turn
	// end_turn_EVENT
	// card_played_EVENTC
	// card_drawn_EVENTCALL
	


};

#endif