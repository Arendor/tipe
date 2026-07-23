



#include "agent.h"


Agent::Agent() : extended_board_ml(arma::fill::zeros), policy(0)
{
}

Agent::Agent(Policy* new_policy)
{
	policy = new_policy;

	

}



Agent::~Agent()
{
}


void Agent::win_EVENTC(PlayerType playerID) {

	policy->end_party_EVENTC(playerID);

};

void Agent::set_policy(Policy* new_policy) {

	policy = new_policy;

};

void Agent::reset() {
	Player::reset();
	std::fill(extended_board_ml.begin(), extended_board_ml.end(), 0);
	policy->restart_game();

	// Faut faire quelque chose avec ça, faut appelr 
};


std::array<int, 2> Agent::choose_cardNumber_pos() {
	policy->start_turn_EVENTC(true);
	return policy->choose_action();
}


void Agent::end_turn_EVENTC(PlayerType player){
	policy->end_turn_EVENTC(player == playerID);
}

void Agent::card_drawn_EVENTC(int card, PlayerType player)
{
	if (player == playerID && card != -1) {
		extended_board_ml(card, 9) = 1;
	}

	policy->card_drawn_EVENTC(card, player == playerID);

}

/*
* Met à jour les infos du terrain quand l'adversaire a joué une carte
* Carte restante dans un
*
* why PlayerType??
*/


void Agent::card_played_EVENTC(int card, int frontier, PlayerType player)
{

	Player::card_played_EVENTC(card, frontier, player);


	if (player == playerID) {
		// Enlève de la main
		extended_board_ml(card, 9) = 0;
		// Ajoute la carte sur le terrain ML
		extended_board_ml(card, frontier) = 1;
	}
	else {

		extended_board_ml(card + 55, frontier) = 1;

		int unknown_cards_size = remaining_cards.size();
		// p / k
		int taille_cartes_adversaire = 6;
		if (unknown_cards_size < 6) {
			taille_cartes_adversaire = unknown_cards_size;
		}

		for (auto it = remaining_cards.begin(); it != remaining_cards.end(); it++) {
			extended_board_ml(*it + 55, 9) = taille_cartes_adversaire / unknown_cards_size;
		}

	}

	policy->card_played_EVENTC(card, frontier, player == playerID);

};




void Agent::winFrontier_EVENTC(int n_frontier, PlayerType player)
{
	// Appel inutile
	Player::winFrontier_EVENTC(n_frontier, player);

	extended_board_ml(54, n_frontier) = static_cast<int>(playerID == player) * 2 - 1;

	// reward à faire du coup --> policy

	policy->frontier_win_EVENTC(n_frontier, player == playerID);

}

