

#include "game_engine.h"
#include <algorithm>
#include "../player/player.h"
#include <iostream>

extern std::mt19937 gen;

GameEngine::GameEngine(Player* ply1, Player* ply2) : _highestBR(8), colors(0), values(0), sc_combs(), c_combs(), board(arma::fill::zeros), deck(), player_1(ply1), player_2(ply2), deck_size(54), existing_cards(), nb_cards_remaining(54) {

	board.fill(0);
	std::fill_n(colors, 6, 9);
	std::fill_n(values, 9, 6);
	
	existing_cards.set();

	std::generate(deck.begin(), deck.end(), [n = 0]() mutable { return n++; });

	player_1->link(this, pl1);
	player_2->link(this, pl2);

	nb_fr_win_pl1 = 0;
	nb_fr_win_pl2 = 0;

	turn_remaining = 54;
	winner = none;
	end = false;
	turn = pl1;
	first_player = pl1;

	debug = false;

}

GameEngine::~GameEngine()
{
}

void GameEngine::reset() {
	board.fill(0);

	existing_cards.set();

	_highestBR = 8;
	std::fill_n(colors, 6, 9);
	std::fill_n(values, 9, 6);
	this->c_combs.reset();
	this->sc_combs.reset();

	std::generate(deck.begin(), deck.end(), [n = 0]() mutable { return n++; });

	deck_size = 54;
	turn_remaining = 54;
	nb_cards_remaining = 54;

	player_1->reset();
	player_2->reset();

	nb_fr_win_pl1 = 0;
	nb_fr_win_pl2 = 0;
	winner = none;
	end = false;
	turn = pl1;
	first_player = pl1;

	debug = false;
}

void GameEngine::distribuer() {

	shuffle_deck();

	deck_size = 42;

	player_1->distribuer_cartes(&deck[48]);
	player_2->distribuer_cartes(&deck[42]);

};

bool GameEngine::doesHighestExist1card(int card, int cmb) const
{

	#ifdef VERIF
		if (nb_cards_remaining < 2) return false;
	#endif

	int c = utils::getColor(card), v = utils::getValue(card);
	// Suite à la couleur

	int highest = -1;
	typeOfCMB type = static_cast<typeOfCMB>(cmb / MODCARDS);
	if (debug) {
		std::cout << "CMB TYPE : " << type << " ";
	}
	

	if (v + 1 < 9 && existing_cards[card + 1 * TAILLE_PAQUET]) {
		if (v + 2 < 9 && existing_cards[card + 2 * TAILLE_PAQUET]) highest = v + MODCARDS * suite_couleur;
		else if (v - 1 >= 0 && existing_cards[card - 1 * TAILLE_PAQUET]) highest = v - 1 + MODCARDS * suite_couleur;
	}
	else if (v - 1 >= 0 && existing_cards[card - 1 * TAILLE_PAQUET] && v - 2 >= 0 && existing_cards[card - 2 * TAILLE_PAQUET]) highest = v - 2 + MODCARDS * suite_couleur;
	// Soit la cmb existe et on compare, soit si c'est plus faible on renvoie false comme highest = -1

	if (debug) {
		std::cout << "Check BFE SC";

	}

	if (highest != -1 || type == suite_couleur)  return highest > cmb;
	
	if (values[v] > 1) highest = v + MODCARDS * brelan;


	if (debug) {
		std::cout << "Check BFE BR";
	}

	if (highest != -1 || type == brelan) return highest > cmb;

	// Maybe ici l'erreur
	if (colors[c] >= 2) {
		int a = -1, b = -1;
		bool d = false;


		int i = 8;

		while (i>=0 && b == -1) {
			if (existing_cards[i * TAILLE_PAQUET + c]) {
				if (!d) {
					a = i;
					d = true;
				}
				else b = i;
			}
			i--;
		}
		highest = a + b + v - 4 + MODCARDS * couleur;

		if (debug) {
			std::cout << "Valeur highest couleur : " << highest << " ";
		}
		return highest > cmb;
	}

	if (debug) {
		std::cout << "Check BFE C";
	}

	if (type == couleur) return false;
	
	if (v + 1 < 9 && values[v + 1] > 0) {
		if (v + 2 < 9 && values[v + 2] > 0) highest = v + MODCARDS * suite;
		else if (v - 1 >= 0 && values[v - 1] > 0) highest = v - 1 + MODCARDS * suite;
	}
	else if (v - 1 >= 0 && values[v - 1] > 0 && v - 2 >= 0 && values[v - 2] > 0) highest = v - 2 + MODCARDS * suite;

	if (debug) {
		std::cout << "Check BFE S";
	}

	if (highest != -1 || type == suite) return highest > cmb;

	if (nb_cards_remaining > 1) {
		std::array<int, 2> ar;
		
		int j = 8;
		int i = 0;
		int p = 0;

		while (p < 2 /* && j >= 0*/) {
			if (values[j] - i > 0) {
				ar[p] = j;
				p++;
				i++;
			}
			else {
				j--;
				i = 0;
			}
		}

		highest = v + ar[0] + ar[1] - 1 + MODCARDS * somme;
	}

	return highest > cmb;

}

/*
* Existe-il une combinaison de valeur supérieur sachant que 2 cartes sont déja posées??
* on a toujours v(card1) <= v(card2)
*/
bool GameEngine::doesHighestExist2cards(int card1, int card2, int cmb) const
{

	#ifdef VERIF
	if (nb_cards_remaining == 0) return false;
	#endif;

	int highest = -1;
	typeOfCMB type = static_cast<typeOfCMB>(cmb / MODCARDS);
	int c1 = utils::getColor(card1), v1 = utils::getValue(card1), c2 = utils::getColor(card2), v2 = utils::getValue(card2);

	if (debug) {
		std::cout << "CMB TYPE : " << type << " ";
	}
	// Suite couleur
	if (c1 == c2) {
		
		if (v2 - v1 == 1) {
			if (v2 + 1 < 9 && existing_cards[card2 + 1 * TAILLE_PAQUET]) highest = v1 + MODCARDS * suite_couleur;
			else if (v1 - 1 >= 0 && existing_cards[card1 - 1 * TAILLE_PAQUET]) highest = v1 - 1 + MODCARDS * suite_couleur;
		};
		if (v2 - v1 == 2) {
			if (existing_cards[card1 + 1 * TAILLE_PAQUET]) highest = v1 + MODCARDS * suite_couleur;
		};
		if (debug) {
			std::cout << "\nDOES_HIGHEST_EXIST2 " << highest << " - " << cmb << " \n";

		}

		if (highest != -1) return highest > cmb;
	};

	if (debug) {
		std::cout << "Check BFE SC";

	}

	#ifdef DEBUG
		std::cout << "Check BFE SC";
	#endif //  DEBUG

	if (type == suite_couleur) return false;

	// Brelan
	if (v1 == v2 && values[v1] > 0) return v1 + MODCARDS * brelan > cmb;

	if (debug) {
		std::cout << "Check BFE BR";
	}
	#ifdef DEBUG
		std::cout << "Check BFE BR";
	#endif //  DEBUG

	if (type == brelan) return false;

	// Couleur
	if (c1 == c2 && colors[c1] > 0) {
		int i = 8;
		while (!existing_cards[c1 + TAILLE_PAQUET * i]) {
			i--;
		}
		
		return (v1 + v2 + i - 4 + MODCARDS * couleur) > cmb;
	};

	if (debug) {
		std::cout << "Check BFE C";
	}
	#ifdef DEBUG
		std::cout << "Check BFE C";
	#endif //  DEBUG

	if (type == couleur) return false;

	// Suite

	if (v2 - v1 == 1) {
		if (v2 + 1 < 9 && values[v2 + 1] > 0) highest = v1 + MODCARDS * suite;
		else if (v1 - 1 >= 0 && values[v1 - 1] > 0) highest = v1 - 1 + MODCARDS * suite;

		if (highest != -1) return highest > cmb;
	}

	if (v2 - v1 == 2 && values[v1 + 1] > 0) return v1 + MODCARDS * suite > cmb;

	if (debug) {
		std::cout << "Check BFE S";
	}
	#ifdef DEBUG
	std::cout << "Check BFE S";
	#endif //  DEBUG

	if (type == suite) return false;

	// Somme
	int i = 8;

	while (values[i] == 0 && i >=0) {
		i--;
	}

	return i + v1 + v2 - 1 + MODCARDS * somme > cmb;

}

int GameEngine::highestExist1card(int card) const
{
	#ifdef VERIF
		if (nb_cards_remaining < 2) return false;
	#endif

	int c = utils::getColor(card), v = utils::getValue(card);
	// Suite à la couleur

	int highest = -1;

	if (v + 1 < 9 && existing_cards[card + 1 * TAILLE_PAQUET]) {
		if (v + 2 < 9 && existing_cards[card + 2 * TAILLE_PAQUET]) return v + MODCARDS * suite_couleur;
		else if (v - 1 >= 0 && existing_cards[card - 1 * TAILLE_PAQUET]) return v - 1 + MODCARDS * suite_couleur;
	}
	else if (v - 1 >= 0 && existing_cards[card - 1 * TAILLE_PAQUET] && v - 2 >= 0 && existing_cards[card - 2 * TAILLE_PAQUET]) return v - 2 + MODCARDS * suite_couleur;
	// Soit la cmb existe et on compare, soit si c'est plus faible on renvoie false comme highest = -1

	if (values[v] > 1) return v + MODCARDS * brelan;

	if (colors[c] >= 2) {
		int a, b;
		bool d = false;

		for (int i = 8; i >= 0; i--) {
			if (existing_cards[i * TAILLE_PAQUET + c]) {
				if (!d) {
					a = i;
					d = true;
				}
				else { b = i; break; }
			}
		}
		highest = a + b + v - 4 + MODCARDS * couleur;

		return highest;
	}  


	if (v + 1 < 9 && values[v + 1] > 0) {
		if (v + 2 < 9 && values[v + 2] > 0) return v + MODCARDS * suite;
		else if (v - 1 >= 0 && values[v - 1] > 0) return v - 1 + MODCARDS * suite;
	}
	else if (v - 1 >= 0 && values[v - 1] > 0 && v - 2 >= 0 && values[v - 2] > 0) return v - 2 + MODCARDS * suite;

	if (nb_cards_remaining > 1) {
		std::array<int, 2> ar;

		int j = 8;
		int i = 0;
		int p = 0;

		while (p < 2 /* && j >= 0*/) {
			if (values[j] - i > 0) {
				ar[p] = j;
				p++;
				i++;
			}
			else {
				j--;
				i = 0;
			}
		}

		highest = v + ar[0] + ar[1] - 1 + MODCARDS * somme;
	}

	return highest;
}

int GameEngine::highestExist2cards(int card1, int card2) const
{

	// v2 >= v1

	#ifdef VERIF
		if (nb_cards_remaining == 0) return false;
	#endif

	int highest = -1;
	int c1 = utils::getColor(card1), v1 = utils::getValue(card1), c2 = utils::getColor(card2), v2 = utils::getValue(card2);

	// Suite couleur
	if (c1 == c2) {
		if (v2 - v1 == 1) {
			if (v2 + 1 < 9 && existing_cards[card2 + 1 * TAILLE_PAQUET]) return v1 + MODCARDS * suite_couleur;
			else if (v1 - 1 >= 0 && existing_cards[card1 - 1 * TAILLE_PAQUET]) return v1 - 1 + MODCARDS * suite_couleur;
		};
		if (v2 - v1 == 2 && existing_cards[card1 + 1 * TAILLE_PAQUET]) return v1 + MODCARDS * suite_couleur;
	};

	// Brelan
	if (v1 == v2 && values[v1] > 0) return v1 + MODCARDS * brelan;

	// Couleur
	if (c1 == c2 && colors[c1] > 0) {
		int i = 8;
		while (!existing_cards[c1 + TAILLE_PAQUET * i]) {
			i--;
		}

		return v1 + v2 + i - 4 + MODCARDS * couleur;
	};


	// Suite
	if (v2 - v1 == 1) {
		if (v2 + 1 < 9 && values[v2 + 1] > 0) return v1 + MODCARDS * suite;
		else if (v1 - 1 >= 0 && values[v1 - 1] > 0) return v1 - 1 + MODCARDS * suite;
	}

	if (v2 - v1 == 2 && values[v1 + 1] > 0) return v1 + MODCARDS * suite;

	// Somme
	int i = 8;

	while (values[i] == 0 && i >= 0) {
		i--;
	}

	return i + v1 + v2 - 1 + MODCARDS * somme;
}

PlayerType GameEngine::eval_frontier(int n)
{

	std::array<int, 5> HALL = getHighestAll();

	if (board(0, n) == 3) {
		// Les éléments du board sont rangés dans l'odre décroissant des valeurs mais les fonctions demandent que ce soit envoyé dans l'odre croissant !

		int cmb_pl1 = evaluate(board(3, n), board(2, n), board(1, n));

		switch (board(5, n)) {

		case 3: {
			int cmb_pl2 = evaluate(board(8, n), board(7, n), board(6, n));

			if (cmb_pl1 > cmb_pl2) return pl1;
			else if (cmb_pl1 == cmb_pl2) return draw;
			else if (cmb_pl1 < cmb_pl2) return pl2;
			break;
		}

		case 2: {
			if (!doesHighestExist2cards(board(7, n), board(6, n), cmb_pl1)) return pl1;
			break;
		}

		case 1: {
			if (!doesHighestExist1card(board(6, n), cmb_pl1)) return pl1;
			break;
		}

		case 0:
		default: {
			if (!doesHighestExist(cmb_pl1, HALL)) return pl1;
			break;
		}

		};

	}
	else if (board(5, n) == 3) {
		int cmb_pl2 = evaluate(board(8, n), board(7, n), board(6, n));

		// Taille 0 1 2 mais pas 3 car déjà testé avant

		switch (board(0, n)) {

		case 2: {
			if (!doesHighestExist2cards(board(2, n), board(1, n), cmb_pl2)) return pl2;
			break;
		}

		case 1: {
			if (!doesHighestExist1card(board(1, n), cmb_pl2)) return pl2;
			break;
		}

		case 0: {
			if (!doesHighestExist(cmb_pl2, HALL)) return pl2;
			break;
		}
		}

	}

	return none;
}

int GameEngine::evaluateCMB(PlayerType playerID, int frontier)
{

	if (playerID == pl1) return evaluate(board(3, frontier), board(2, frontier), board(1, frontier));
	if (playerID == pl2) return evaluate(board(8, frontier), board(7, frontier), board(6, frontier));

	return -1;
}

typeOfCMB GameEngine::getCMBType(PlayerType playerID, int frontier)
{
	return static_cast<typeOfCMB>(evaluateCMB(playerID,frontier) / MODCARDS);
}

std::array<int, 5> GameEngine::getHighestAll() const
{
	return std::array<int, 5>({ getHighestSomme(), getHighestSuite(), getHigestCouleur(), getHighestBrelan(), getHighestSuiteCouleur() });
}

bool GameEngine::doesHighestExist(int cmb, std::array<int, 5> highest_cmbs) const
{
	int type = cmb % MODCARDS;

	for (int i = 4; i > 0; i--) {
		if (highest_cmbs[i] > cmb) return true;
		if (type == i) return false;
	}
	return highest_cmbs[0] > cmb;

}

void GameEngine::update_frontiers()
{
	for (int i = 0; i < 9; i++) {
		// taille 9, 0 1-3 ; 4 ; 5 6-8

		// Frontière non déjà obtenue
		if (board(4,i) == 0) {

			PlayerType who = eval_frontier(i);

			if (who == pl1) winFrontier(i, pl1);
			else if (who == pl2) winFrontier(i, pl2);
			else if (who == draw) winFrontier(i, getUnactivePlayer()->playerID);
			
		}
	}
}

/*
* Rangé par ordre décroissant
* ie 1 est le plus grand 
*/
void GameEngine::play_card(int card, int n_frontier, PlayerType player)
{

	#ifdef VERIF
		if (player != turn) throw std::runtime_error("Unauthorized play : invalid player");
		if (!getActivePlayer()->hasCard(card)) throw std::runtime_error("Illegal play : invalid card");
		if (n_frontier > 8 || n_frontier < 0) throw std::runtime_error("Invalid frontier number");
	#endif;

	int add1 = 0, add2 = 0;
	if (player == pl2) {
		add1 = 55;
		add2 = 5;
	}

	// Si la première case est vide
		
	switch (board(add2, n_frontier)) {

	case 0: {
		board(1 + add2, n_frontier) = card;
		break;
	};

	case 1: {
		if (card > board(1 + add2, n_frontier)) {
			board(2+add2, n_frontier) = board(1 + add2, n_frontier);
			board(1 + add2, n_frontier) = card;
		}
		else board(2 + add2, n_frontier) = card;
		break;
	};

	case 2: {
		if (card > board(1 + add2, n_frontier)) {
			board(3 + add2, n_frontier) = board(2 + add2, n_frontier);
			board(2 + add2, n_frontier) = board(1 + add2, n_frontier);
			board(1 + add2, n_frontier) = card;
		}
		else if (card > board(2 + add2, n_frontier)) {
			board(3 + add2, n_frontier) = board(2 + add2, n_frontier);
			board(2 + add2, n_frontier) = card;
		}
		else board(3 + add2, n_frontier) = card;
		break;
	};

	default:
	case 3: {
		throw std::runtime_error("Illegal play : the frontier is full");
	};


	}
		
	board(add2, n_frontier) += 1;

	nb_cards_remaining -= 1;
	existing_cards[card] = false;
		
	update_cache_card_played(card);
	getActivePlayer()->card_played_EVENTC(card, n_frontier, player);
	getUnactivePlayer()->card_played_EVENTC(card, n_frontier, player);

}

int GameEngine::draw_card(PlayerType player)
{
	
	if (deck_size != 0) {
		deck_size--;
		getActivePlayer()->card_drawn_EVENTC(deck[deck_size], player);
		getUnactivePlayer()->card_drawn_EVENTC(0, player);
		return deck[deck_size];
	}
	else {
		getActivePlayer()->card_drawn_EVENTC(-1, player);
		getUnactivePlayer()->card_drawn_EVENTC(-1, player);
		return -1;
	}
};

// C'est à ce moment là que c'est forward à l'autre joueur je pense
// Le player qui vient de jouer s'occupe d'update son plateau directement, tandis qu'on forward à l'autre. Je pense que c'est pas mal
// Forward le tour à l'adversaire
// Mettre à jour les frontières
// Conditon de victoire
// Si non réalisé, on change de tour et on fait jouer l'autre jour 
/*
* Les joueurs gèrent directement les cartes restantes
*/
bool GameEngine::end_turn()
{

	// Mise à jour des frontières

	update_frontiers();

	// Condition de victoire

	checkWin();

	if (winner != none) {
		end = true;

		//this->getActivePlayer()->end_turn_EVENTC();
		//this->getUnactivePlayer()->end_turn_EVENTC();

		return true;
	}

	this->getActivePlayer()->end_turn_EVENTC(turn);
	this->getUnactivePlayer()->end_turn_EVENTC(turn);

	// Au prochain joueur de jouer !
	
	if (turn == pl1) turn = pl2; else turn = pl1;
	turn_remaining -= 1;


	return true;
}

void GameEngine::checkWin()
{
	bool win_fr_all_pl1 = nb_fr_win_pl1 >= 5;
	bool win_fr_all_pl2 = nb_fr_win_pl2 >= 5;
	bool win_fr_ct_pl1 = false;
	bool win_fr_ct_pl2 = false;

	int c_pl1 = 0;
	int c_pl2 = 0;

	for (int i = 0; i < 9; i++) {

		if (board(4, i) == 1) {
			c_pl1 += 1;
			c_pl2 = 0;
		}
		else {
			if (board(4, i) == 2) {
				c_pl2 += 1;
				c_pl1 = 0;
			}
			else {
				c_pl2 = 0;
				c_pl1 = 0;
			}
		}
		

		if (c_pl1 == 3) {
			win_fr_ct_pl1 = true;
			break;
		}
		else if (c_pl2 == 3) {
			win_fr_ct_pl2 = true;
			break;
		}
	};

	if ((win_fr_all_pl1 || win_fr_ct_pl1) && (win_fr_all_pl2 || win_fr_ct_pl2)) winner = draw;

	else if (win_fr_all_pl1 || win_fr_ct_pl1) winner = pl1;
	else if (win_fr_all_pl2 || win_fr_ct_pl2) winner = pl2;

}
int GameEngine::get_nb_fr_win(PlayerType player)
{
	if (player == pl1) return nb_fr_win_pl1;
	else if (player == pl2) return nb_fr_win_pl2;
	else 0;

}
void GameEngine::interrupt()
{

	std::cout << "interrupting";

};

bool GameEngine::run(PlayerType who_start)
{
	distribuer();
	turn = who_start;
	first_player = who_start;

	while (turn_remaining > 0 && !end) {
		getActivePlayer()->play();
		end_turn();
	}

	player_1->win_EVENTC(winner);
	player_2->win_EVENTC(winner);

	return true;

}

bool GameEngine::run()
{
	distribuer();
	start_random_player();

	while (turn_remaining > 0 && !end) {
		getActivePlayer()->play();
		end_turn();
	}

	player_1->win_EVENTC(winner);
	player_2->win_EVENTC(winner);

	return true;

}

Player* GameEngine::getActivePlayer()
{
	if (turn == pl1) return player_1;
	else return player_2;
}

Player* GameEngine::getUnactivePlayer()
{
	if (turn == pl1) return player_2;
	else return player_1;
}

Player* GameEngine::getPlayer(PlayerType playerID)
{
	if (playerID == pl1) return player_1;
	else return player_2;
}

void GameEngine::winFrontier(int nb, PlayerType playerID) {

	board(4, nb) = playerID;

	if (playerID == pl1) nb_fr_win_pl1 += 1;
	if (playerID == pl2) nb_fr_win_pl2 += 1;

	getActivePlayer()->winFrontier_EVENTC(nb, playerID);
	getUnactivePlayer()->winFrontier_EVENTC(nb, playerID);

}

int GameEngine::doesHighestExist(int cmb) const
{
	typeOfCMB type = static_cast<typeOfCMB>(cmb % MODCARDS);

	if (getHighestSuiteCouleur() > cmb) return true;
	if (type == suite_couleur) return false;

	if (getHighestBrelan() > cmb) return true;
	if (type == brelan) return false;

	if (getHigestCouleur() > cmb) return true;
	if (type == couleur) return false;

	if (getHighestSuite() > cmb) return true;
	if (type == suite) return false;

	if (getHighestSomme() > cmb) return true;
	return false;
}

bool GameEngine::isFull(int frontier, PlayerType player)
{
	#ifdef VERIF
		if (frontier < 0 or frontier > 8) throw std::runtime_error("Frontière invalide");
	#endif;

	return board(player == pl1 ? 0 : 5, frontier) == 3;
}

int GameEngine::getNumberOfCards(int frontier, PlayerType player)
{
	if (player == pl1) return board(0, frontier);
	if (player == pl2) return board(5, frontier);
	return -1;
}

int GameEngine::getCardN(int frontier, int position, PlayerType player)
{
	
	return board((player == pl1 ? 0 : 5) + position, frontier);

}

PlayerType GameEngine::winFrontierState(int frontier)
{
	return static_cast<PlayerType>(board(4, frontier));
}

bool GameEngine::estVierge(int frontier) {
	return board(4, frontier) == 0;
}

// v3 >= v2 >= v1

int GameEngine::evaluate(int card1, int card2, int card3)
{
	
	int c1 = utils::getColor(card1), v1 = utils::getValue(card1), c2 = utils::getColor(card2), v2 = utils::getValue(card2), c3 = utils::getColor(card3), v3 = utils::getValue(card3);


	// Pourrait être optimisé mais ça diminuerait la lisibilité [deleteLine]
	
	// Suite couleur, éléments sont ordonnées
	if (c1 == c2 && c2 == c3 && v1 == v2 - 1 && v3 == v2 + 1) return v1 + MODCARDS *suite_couleur;
	// Brelan
	if (v1 == v2 && v2 == v3) return v1 + MODCARDS * brelan;
	// Couleur
	// -4 car la valeur minimum atteinte est 4 1,2,4 (0,1,3)
	if (c1 == c2 && c2 == c3) return MODCARDS * couleur + v1 + v2 + v3 -4;
	//Suite 
	if (v1 == v2 - 1 && v3 == v2 + 1) return MODCARDS * suite + v1;
	// Somme
	// -1 car on ne peut jamais atteindre 0 sinon, car 1 1 1 (0,0,0)  est un brelan : combinaison minimum (0,0,1)
	return v1 + v2 + v3 - 1 + MODCARDS * somme;
	
}

void GameEngine::update_cache_card_played(int card) {

	this->sc_combs.update(card);
	this->_updateBR(card);
	this->c_combs.update(card);
	this->_updateC(card);
	this->_updateV(card);
}

void GameEngine::shuffle_deck()
{
	std::shuffle(deck.begin(), deck.end(), gen);

}

int GameEngine::getHighestSuiteCouleur() const
{
	return this->sc_combs.getHighestRank();
}

int GameEngine::getHighestBrelan() const
{
	if (_highestBR == -1) return _highestBR;
	else return _highestBR + MODCARDS * brelan;
}

int GameEngine::getHigestCouleur() const
{
	return this->c_combs.getHighestRank();
}

/*
* Fait au plus 15 comparaisons
*/
int GameEngine::getHighestSuite() const
{
	if (values[6] > 0) {
		if (values[7] > 0) {
			// 7,8,9 (6,7,8)
			if (values[8] > 0) return MODCARDS * suite + 6;
			// 6,7,8 (5,6,7)
			if (values[5] > 0) return MODCARDS * suite + 5;
		}
		// 5,6,7 (4,5,6)
		if (values[4] > 0 && values[5] > 0) return MODCARDS * suite + 4;
	}

	if (values[3] > 0) {

		if (values[4] > 0) {
			// (3,4,5)
			if (values[5] > 0) return MODCARDS * suite + 3;
			// (2,3,4)
			if (values[2] > 0) return MODCARDS * suite + 2;
		}

		// (1,2,3)
		if (values[1] > 0 && values[2] > 0) return  MODCARDS * suite + 1;
	}

	// (0,1,2)
	if (values[0] > 0 && values[1] > 0 && values[2] > 0) return MODCARDS * suite + 0;

	return -1;
}

int GameEngine::getHighestSomme() const
{
	std::array<int, 3> ar;

	if (deck.size() < 3) return -1;
	
	int j = 8;
	int i = 0;
	int p = 0;

	while (p < 3 /* && j >= 0*/) {
		if (values[j] - i > 0) {
			ar[p] = j;
			p++;
			i++;
		}
		else {
			j--;
			i = 0;
		}
	}

	return ar[0] + ar[1] + ar[2] - 1 + MODCARDS * somme;

}

void GameEngine::_updateC(int card)
{
	colors[utils::getColor(card)] -= 1;
	return;

}

void GameEngine::_updateV(int card)
{
	values[utils::getValue(card)] -= 1;
	return;
}

void GameEngine::start_random_player()
{

	std::uniform_int_distribution distrib(0, 1);

	if (distrib(gen) == 0) {
		turn = pl1;
		first_player = pl1;
	}
	else {
		turn = pl2;
		first_player = pl2;
	}


}

PlayerType GameEngine::get_first_player()
{
	return first_player;
}

/*
* Mise à jour du cache pour les brelans
* Cette fonction doit être appelée AVANT d'update le nombre de cartes restantes pour une certaine valeur
*/
void GameEngine::_updateBR(int card)
{
	
	if (values[utils::getValue(card)] <= 3 && _highestBR == utils::getValue(card)) {
		_highestBR--;

		while (_highestBR >= 0 && values[_highestBR] < 3) {
			_highestBR--;
		}

	}

	return;

}

void GameEngine::setDebug(bool d)
{

	debug = d;

}