

#ifndef POLICY
#define POLICY


#include "../../neural_network/main/Network.h"
#include "../utils/utils.h"

class GameEngine;
class Agent;

class Policy {

public:

	Policy();
	~Policy();

	bool link(Agent* agent);
   
	Agent* owningUser;
	bool isLinked;
	
	bool isTraining;

	// Tour depuis le début de la partie, tour depuis le début de l'utilisation

	int local_turns;
	int global_turns;
	int nb_parties;

	// EVENTS

	virtual void start_turn_EVENTC(bool ismyturn);

	virtual void frontier_win_EVENTC(int n_frontier, bool haswin);

	virtual void card_drawn_EVENTC(int card, bool ismyturn);

	/*
	* Doit être appelé 
	*/
	virtual void end_turn_EVENTC(bool ismyturn);
	virtual void end_party_EVENTC(PlayerType winner);



	virtual void card_played_EVENTC(int card, int frontier, bool ismyturn);



	// METHODS
	
	
	// REIMPLEMENT

	virtual std::array<int, 2> choose_action();

	// REIMPLEMENT & CALL

	/*
	* Doit être appelé
	*/

	virtual void restart_game();
	

	// A utiliser comme cela

	/*
	* @returns std::vector<int> des frontières sur lesquels le joueur peut jouer
	*/
	std::vector<int> getAllowedFrontiers();

	Policy& setTraining(bool tr);


	
	bool debug_mode;
	void setDebugMode(bool mode);
	bool info_mode;
	void setInfoMode(bool mode);






};






#endif