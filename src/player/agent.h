


#ifndef AGENT
#define AGENT


#include "player.h"
#include "../policy/policy.h"


class Agent : public Player {


public:

	Agent();
	Agent(Policy* new_policy);

	~Agent();

	Policy* policy;

	arma::Mat<AGENT_BOARD_TYPE>::fixed<109, 10> extended_board_ml;

	/*
	* Exclusive to agent
	*/

	void set_policy(Policy* new_policy);



	/*
	* Override functions
	*/
	void reset() override;

	void win_EVENTC(PlayerType playerID) override;


	void winFrontier_EVENTC(int n_frontier, PlayerType player) override;

	// A implémenter
	std::array<int, 2> choose_cardNumber_pos() override;

	void end_turn_EVENTC(PlayerType player) override;
	void card_drawn_EVENTC(int card, PlayerType player) override;
	void card_played_EVENTC(int card, int frontier, PlayerType player) override;



};

#endif