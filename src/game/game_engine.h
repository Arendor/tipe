
#ifndef GAMEENGINE
#define GAMEENGINE

#include <armadillo>
#include <vector>
#include "../utils/utils.h"
#include <bitset>
#include "sc_manager.h"
#include "color_manager.h"

class Player;


// Spécification : en fin de partie, l'event end_party_EVENTC n'est pas appelé
// Correction possible en vérifiant que la partie n'est pas terminée dans l'event en regardant owningUser->OwningGame->end (actuellement privé), ou la propriété winner != none

class GameEngine {

public:

	GameEngine(Player* ply1, Player* ply2);
	~GameEngine();

	Player* getActivePlayer();
	Player* getUnactivePlayer();
	Player* getPlayer(PlayerType playerID);

	// Fix on the stack board matrix

	// Plateau alloué sur le stack
	// stack allocated board ?
	//int* _board;

	// Actual 7*9 board
	// Plateau réel
	// On peut partir sur un board en 9*9 je pense comme 
	/*
	* nbcard_p1     |
	* card_p1_00    |
	* card_p1_10    | 
	* card_p1_20    |  
	* etat_frontier |
	* nbcard_p2     |
	* card_p2_00    |
	* card_p2_10    | 
	* card_p2_20    |  
	* 
	* tout vaut 0 initalement mais 0 représente une carte.... 
	*/

	arma::Mat<int>::fixed<9, 9> board;

	//arma::Mat<int> board;

	std::bitset<54> existing_cards;

	int deck_size;

	PlayerType turn;

	Player* player_1;
	Player* player_2;


	int turn_remaining;
	// Nombre de cartes restants
	int nb_cards_remaining;

	PlayerType winner;




	/*
	* Evalue la valeur d'une combinaison,
	* 0 - 111 (avec des trous puisqu'il n'y a que 63 valeurs possibles de combinaisons)
	*/
	int evaluate(int c1, int c2, int c3);

	void update_cache_card_played(int card);


	bool run(PlayerType first_pl);
	bool run();

	int doesHighestExist(int cmb) const;

	int getHighestSuiteCouleur() const;
	int getHighestBrelan() const;
	int getHigestCouleur() const;
	int getHighestSuite() const;
	int getHighestSomme() const;


	void reset();


	bool doesHighestExist1card(int card, int cmb) const;
	bool doesHighestExist2cards(int card1, int card2, int cmb) const;

	int highestExist1card(int card) const;
	int highestExist2cards(int card1, int card2) const;

	PlayerType eval_frontier(int n);

	int evaluateCMB(PlayerType playerID, int frontier);
	typeOfCMB getCMBType(PlayerType playerID, int frontier);

	std::array<int, 5> getHighestAll() const;
	bool doesHighestExist(int cmb, std::array<int, 5> highest_cmbs) const;

	void update_frontiers();
	void winFrontier(int nb, PlayerType playerID);
	void play_card(int card, int n_frontier, PlayerType player);
	int draw_card(PlayerType player);
	bool end_turn();
	void checkWin();
	int get_nb_fr_win(PlayerType player);

	void interrupt();

	//void save();
	//void load();

	void setDebug(bool d);
	bool debug;

	// Ces fonctions ont pour objectif d'êtree utilisé pour obtenir des informations sur le jeu par des structures extérieures, 
	// sans avoir à se préoccuper de comment sont vraiment stockées les informations
	bool isFull(int frontier, PlayerType player);
	int getNumberOfCards(int frontier, PlayerType player);
	int getCardN(int frontier, int position, PlayerType player);
	PlayerType winFrontierState(int frontier);
	bool estVierge(int frontier);

private:

	std::array<int, 54> deck;

	bool end;
	
	PlayerType first_player;

	int nb_fr_win_pl1;
	int nb_fr_win_pl2;

	void distribuer();

	void shuffle_deck();


	// Nombre de cartes d'un numero restante
	int values[9];

	// Nombre de cartes d'une couleur restante
	int colors[6];

	void _updateC(int card);
	void _updateV(int card);

	/* Initie à un joueur aléatoire le joueur qui commence */
	void start_random_player();
	/* Renvoie le joueur qui débuté / a débuté la partie */
	PlayerType get_first_player();

	void _updateBR(int card);

	// Initialisé à 8
	int _highestBR;

	SCManager sc_combs;
	ColorManager c_combs;



};


#endif // !GAMEENGINE