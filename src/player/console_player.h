

#pragma once

#include "player.h"

// pas de policy au final
// car inutile ici


class ConsolePlayer : public Player {



public:

	ConsolePlayer();
	~ConsolePlayer();


	arma::Mat<int>::fixed<9, 9> board_player;

	void reset() override;

	void win_EVENTC(PlayerType playerID) override;

	void winFrontier_EVENTC(int n_frontier, PlayerType player) override;

	void card_drawn_EVENTC(int card, PlayerType player) override;

	void card_played_EVENTC(int card, int n_frontier, PlayerType player) override;


	// A implémenter
	std::array<int, 2> choose_cardNumber_pos() override;

	// Fonctions spécifiques à console player

	std::array<int, 2> askPlay();

	int getCardFromStr(char val, char col) const;

	void display_deck();
	void display_board();

private:

	int random_tours;

};