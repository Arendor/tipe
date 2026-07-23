

#include "console_player.h"
#include <iostream>
#include <charconv>
#include "../game/game_engine.h"


ConsolePlayer::ConsolePlayer() : board_player(arma::fill::zeros), random_tours(0)
{


}

ConsolePlayer::~ConsolePlayer()
{
}


void ConsolePlayer::reset() {
	Player::reset();
	std::fill(board_player.begin(), board_player.end(), 0);
	random_tours = 0;
};


void ConsolePlayer::card_drawn_EVENTC(int card, PlayerType player)
{
	if (player == playerID) {
		std::cout << "\n[" << playerID << "] : Vous avez pioché la carte " << utils::numberCardToStr(card);
		display_deck();
	}
	
}

// Fonctionne differement que pour le main board
/*
*/
// Le player actuel joue sur la deuxième partie du board
void ConsolePlayer::card_played_EVENTC(int card, int n_frontier, PlayerType player)
{
	if (player == playerID) {

		switch (board_player(8, n_frontier)) {

		case 0: {
			board_player(5, n_frontier) = card + 1;
			break;
		};

		case 1: {
			if (utils::getValue(card) > utils::getValue(board_player(5, n_frontier) - 1)) {
				board_player(6, n_frontier) = board_player(5, n_frontier);
				board_player(5, n_frontier) = card + 1;
			}
			else board_player(6, n_frontier) = card + 1;
			break;
		};

		case 2: {
			if (utils::getValue(card) > utils::getValue(board_player(5, n_frontier) - 1)) {
				board_player(7, n_frontier) = board_player(6, n_frontier);
				board_player(6, n_frontier) = board_player(5, n_frontier);
				board_player(5, n_frontier) = card + 1;
			}
			else if (utils::getValue(card) > utils::getValue(board_player(6, n_frontier) - 1)) {
				board_player(7, n_frontier) = board_player(6, n_frontier);
				board_player(6, n_frontier) = card + 1;
			}
			else board_player(7, n_frontier) = card + 1;
			break;
		};

		default:
		case 3: {
			throw std::runtime_error("Impossible de jouer : la frontière : " + std::to_string(n_frontier) + " est pleine");
		};

		}

		board_player(8, n_frontier) += 1;

		//std::cout << "\n[" << playerID << "] : Vous avez bien joue la carte" << utils::numberCardToStr(card) << " sur la frontière" << n_frontier + 1;

		display_board();

	}
	else {
		switch (board_player(0, n_frontier)) {

		case 0: {
			board_player(3, n_frontier) = card + 1;
			break;
		};

		case 1: {
			if (utils::getValue(card) > utils::getValue(board_player(3, n_frontier) - 1)) {
				board_player(2, n_frontier) = board_player(3, n_frontier);
				board_player(3, n_frontier) = card + 1;
			}
			else board_player(2, n_frontier) = card + 1;
			break;
		};

		case 2: {
			if (utils::getValue(card) > utils::getValue(board_player(3, n_frontier) - 1)) {
				board_player(1, n_frontier) = board_player(2, n_frontier);
				board_player(2, n_frontier) = board_player(3, n_frontier);
				board_player(3, n_frontier) = card + 1;
			}
			else if (utils::getValue(card) > utils::getValue(board_player(2, n_frontier) - 1)) {
				board_player(1, n_frontier) = board_player(2, n_frontier);
				board_player(2, n_frontier) = card + 1;
			}
			else board_player(1, n_frontier) = card + 1;
			break;
		};

		default:
			case 3: {
				throw std::runtime_error("Impossible de jouer : la frontiere : " + std::to_string(n_frontier) + " est pleine");
			};

		}

		board_player(0, n_frontier) += 1;

		std::cout << "\n[" << playerID << "] : Votre adversaire a jouer la carte " << utils::numberCardToStr(card) << " sur la frontiere" << n_frontier + 1;

		display_board();
	}


	Player::card_played_EVENTC(card, n_frontier, player);
};




void ConsolePlayer::winFrontier_EVENTC(int n_frontier, PlayerType player)
{

	if (player == playerID) {
		board_player(4, n_frontier) = 1;
		std::cout << "\n[" << playerID << "] : Vous avez gagne la frontière : " << n_frontier +1 << " !";
	}
	else {
		board_player(4, n_frontier) = -1;
		std::cout << "\n[" << playerID << "] : Votre adversaire a gagne la frontière : " << n_frontier+1 << " !";
	}
	


}



void ConsolePlayer::win_EVENTC(PlayerType player)
{

	if (player == draw) std::cout << "\n[" << playerID << "] : Egalite !";
	else if (playerID == player) std::cout << "\n[" << playerID << "] : Vous avez gagne !";
	else std::cout << "\n[" << playerID << "] : Vous avez perdu !";
	
}


std::array<int, 2> ConsolePlayer::choose_cardNumber_pos() {


	std::cout << "\n[" << playerID << "] : C'est a vous de jouer !";
	display_deck();
	display_board();

	if (random_tours > 0) {
		random_tours--;
		return Player::choose_cardNumber_pos();
	};

	std::array<int, 2> pla{ -1,-1 };

	while (pla[1] == -1) {
		try {
			pla = askPlay();
		}
		catch (std::runtime_error err) {
			std::cout << "\n[" << playerID << "] : " << err.what();
		}
	}

	return pla;
}

std::array<int, 2> ConsolePlayer::askPlay()
{
	
	std::string str;

	std::cout << "\n[" << playerID << "] : Ou souhaitez vous jouez ? : (frontiere-carte) ";
	std::cin >> str;

	if (str.starts_with("@give_card")) {
		std::string c;

		std::cin >> c;
		if (c.length() != 2) throw std::runtime_error("Invalid length");

		int card = getCardFromStr(c[0], c[1]);

		cards.add(card);

		// Ptet nécessaire de clear le buffer

		return std::array<int, 2>({ -1,-1 });

	};

	if (str.starts_with("@delete_card")) {
		std::string c;

		std::cin >> c;
		if (c.length() != 2) throw std::runtime_error("Invalid length");

		int card = getCardFromStr(c[0], c[1]);

		if (cards.has(card)) cards.del(card);

		// Ptet nécessaire de clear le buffer

		return std::array<int, 2>({ -1,-1 });

	};

	if (str.starts_with("@deck")) {
		display_deck();
		return std::array<int, 2>({ -1,-1 });
	}

	if (str.starts_with("@board")) {
		std::string nstr;

		std::cin >> nstr;

		if (nstr.length() != 3) throw std::runtime_error("Invalid length");

		int i = nstr[0] - '0';
		int j = nstr[2] - '0';

		std::cout << "Value is : " << owningGame->board(i, j); 

		return std::array<int, 2>({ -1,-1 });

	}

	if (str.starts_with("@show_board")) {
		display_board();
		return std::array<int, 2>({ -1,-1 });
	}

	if (str.starts_with("@evaluate")) {
		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 3) throw std::runtime_error("Invalid length");

		int ply = nstr[0] - '0';
		int col = nstr[2] - '0';

		std::cout << owningGame->evaluateCMB(static_cast<PlayerType>(ply), col);

		return std::array<int, 2>({ -1,-1 });

	}

	if (str.starts_with("@hall")) {
		
		std::cout << "Somme : " << owningGame->getHighestSomme() << " Suite : " << owningGame->getHighestSuite() << " Couleur : " << owningGame->getHigestCouleur() <<  " Brelan : " << owningGame->getHighestBrelan() << " Suite couleur : " << owningGame->getHighestSuiteCouleur();

		return std::array<int, 2>({ -1,-1 });

	}

	if (str.starts_with("@eval_frontier")) {

		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 1) throw std::runtime_error("Taille invalide");

		int n = nstr[0] - '0';	
		
		std::cout << owningGame->eval_frontier(n);

		return std::array<int, 2>({ -1,-1 });
	}

	if (str.starts_with("@highest1")) {

		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 3) throw std::runtime_error("Invalid length");
		
		int ply = nstr[0] - '0';
		int n = nstr[2] - '0';

		if (ply == 1) std::cout << owningGame->highestExist1card(owningGame->board(3, n));
		
		if (ply == 2) std::cout << owningGame->highestExist1card(owningGame->board(6, n));
		

		return std::array<int, 2>({ -1,-1 });
	}

	if (str.starts_with("@doeshighest1")) {

		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 3) throw std::runtime_error("Invalid length");

		int ply = nstr[0] - '0';
		int n = nstr[2] - '0';

		if (ply == 1) std::cout << owningGame->doesHighestExist1card(owningGame->board(3, n), this->owningGame->evaluateCMB(pl2,n));

		if (ply == 2) std::cout << owningGame->doesHighestExist1card(owningGame->board(6, n), owningGame->evaluateCMB(pl1,n));


		return std::array<int, 2>({ -1,-1 });
	}


	if (str.starts_with("@highest2")) {

		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 3) throw std::runtime_error("Invalid length");

		int ply = nstr[0] - '0';
		int n = nstr[2] - '0';

		if (ply == 1) std::cout << owningGame->highestExist2cards(owningGame->board(2, n), owningGame->board(1, n));
		
		if (ply == 2) std::cout << owningGame->highestExist2cards(owningGame->board(7, n), owningGame->board(6, n));

		return std::array<int, 2>({ -1,-1 });
	}

	if (str.starts_with("@doeshighest2")) {

		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 3) throw std::runtime_error("Invalid length");

		int ply = nstr[0] - '0';
		int n = nstr[2] - '0';
		
		if (ply == 1) {
			if (!owningGame->isFull(n,pl2)) throw std::runtime_error("Le joueur 2 n'a pas une frontière complète");
			if (owningGame->board(0,n) != 2) throw std::runtime_error("Le joueur 1 n'a pas 2 cartes sur cette frontière");

			std::cout << owningGame->doesHighestExist2cards(owningGame->board(2, n), owningGame->board(1, n), this->owningGame->evaluateCMB(pl2, n));
		}

		if (ply == 2) {
			if (!owningGame->isFull(n, pl1)) throw std::runtime_error("Le joueur 1 n'a pas une frontière complète");
			if (owningGame->board(5, n) != 2) throw std::runtime_error("Le joueur 2 n'a pas 2 cartes sur cette frontière");

			std::cout << owningGame->doesHighestExist2cards(owningGame->board(7, n), owningGame->board(6, n), this->owningGame->evaluateCMB(pl1, n));
		}

		return std::array<int, 2>({ -1,-1 });
	}

	if (str.starts_with("@doesexist")) {

		std::string nstr;
		std::cin >> nstr;

		int c;

		if (nstr.length() == 1) {
			c = nstr[0] - '0';
		}
		else if (nstr.length() == 2) {
			c = (nstr[1] - '0') + (nstr[0] - '0') * 10;
		}
		else throw std::runtime_error("Invalid length");

		std::cout << owningGame->existing_cards[c];

		return std::array<int, 2>({ -1,-1 });
	};


	if (str.starts_with("@interrupt")) {

		this->owningGame->interrupt();

		return std::array<int, 2>({ -1,-1 });
	};

	if (str.starts_with("@debug_mode")) {

		this->owningGame->setDebug(!this->owningGame->debug);

		std::cout << "\nDebug mode passe à " << this->owningGame->debug;


		return std::array<int, 2>({ -1,-1 });
	};

	if (str.starts_with("@mode_auto")) {

		std::string nstr;
		std::cin >> nstr;

		int c;

		if (nstr.length() == 1) {
			c = nstr[0] - '0';
		}
		else if (nstr.length() == 2) {
			c = (nstr[1] - '0') + (nstr[0] - '0') * 10;
		}
		else if (nstr == "end") {
			c = 30;
		}

		random_tours = c;

		if (random_tours > 0) {
			random_tours--;
			return Player::choose_cardNumber_pos();

			std::cout << "Actions aléatoires pour " << c << " tours.";
		}
		else std::cout << "Pas d'actions aléatoires.";

		return std::array<int, 2>({ -1,-1 });
	};

	/*
	if (str.starts_with("@edit_board")) {
		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 3) throw std::runtime_error("Invalid length");

		int i = nstr[0] - '0';
		int j = nstr[2] - '0';

		std::cin >> nstr;
		int num;

		auto [ptr1, ec1] = std::from_chars(nstr.data(), nstr.data() + nstr.length(), num);
		if (ec1 != std::errc{}) throw std::runtime_error("Echec");

		owningGame->board(i, j) = num;

		return std::array<int, 2>({ -1,-1 });
	}

	if (str.starts_with("@delete_card_board")) {
		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 3) throw std::runtime_error("Invalid length");

		int i = nstr[0] - '0';
		int j = nstr[2] - '0';

		owningGame->board(i, j) = 0;

		return std::array<int, 2>({ -1,-1 });
	}*/
	
	if (str.starts_with("@win_frontier")) {
		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 3) throw std::runtime_error("Invalid length");

		int ply = nstr[0] - '0';
		int n = nstr[2] - '0';

		owningGame->winFrontier(n, static_cast<PlayerType>(ply));

		return std::array<int, 2>({ -1,-1 });
	}
	if (str.starts_with("@win")) {
		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 1) throw std::runtime_error("Invalid length");

		int ply = nstr[0] - '0';

		owningGame->winner = static_cast<PlayerType>(ply);

		return std::array<int, 2>({ -1,-1 });
	}

	if (str.starts_with("@end")) {
		// Have to edit that one.
		return std::array<int, 2>({ -1,-1 });
	}

	
	if (str.length() != 4) throw std::runtime_error("Format invalide");

	int n_frontier = str[0] - '0';

	/*
	auto [ptr1, ec1] = std::from_chars(str.data(), str.data() + 1, n_frontier);
	if (ec1 != std::errc{}) throw std::runtime_error("Frontière invalide 1");*/

	if (n_frontier < 1 || n_frontier > 9)  throw std::runtime_error("Frontière invalide 2");

	// Permet d'obtenir la carte

	int v = str[2] - '0';

	if (v < 1 || v > 9) throw std::runtime_error("Numero de carte invalide");

	char c = str[3];

	if (!utils::isColor(c)) throw std::runtime_error("Cette couleur n'existe pas");
	int color = utils::getColorFromChar(c);

	int real_card = utils::getNumber(v - 1, color);

	if (!cards.has(real_card)) throw std::runtime_error("Vous n'avez pas cette carte dans votre deck");

	if (board_player(8, n_frontier - 1) == 3) throw std::runtime_error("Cette frontière est pleine");

	return std::array<int, 2> {real_card, n_frontier-1};

}

int ConsolePlayer::getCardFromStr(char val, char col) const {

	int v = val - '0';

	if (v < 1 || v > 9) throw std::runtime_error("Numero de carte invalide");

	if (!utils::isColor(col)) throw std::runtime_error("Cette couleur n'existe pas");
	int color = utils::getColorFromChar(col);

	return utils::getNumber(v - 1, color);
}

void ConsolePlayer::display_deck()
{

	std::cout << "\n[" << playerID << "] : Voici votre deck\n     | ";

	for (int i = 0; i < cards.size; i++) {
		std::cout << utils::numberCardToStr(cards[i]) << " | ";
	};
	
}

void ConsolePlayer::display_board()
{

	std::cout << "\n Terrain de jeu pour le joueur : " << playerID << "\n";

	std::array<bool, 9> doesColumnhaveCards({ false, false, false, false, false, false, false, false, false });

	for (int i = 0; i < 9; i++) {

		doesColumnhaveCards[i] = board_player(0, i) != 0 || board_player(5, i) != 0;

	}

	for (int i = 1; i < 4; i++) {
		std::cout << "\n | ";

		
		for (int j = 0; j < 9; j++) {

			std::cout << (board_player(i, j) == 0 ? "0" : utils::numberCardToStr(board_player(i, j) - 1)) << ((doesColumnhaveCards[j] && board_player(i, j) == 0) ? "  | " : " | ");

		}
		
	}

	std::cout << "\n\n | ";

	for (int j = 0; j < 9; j++) {

		std::cout << board_player(4, j) << ( (doesColumnhaveCards[j] && board_player(4, j) != - 1) ? "  | " : " | ");

	}

	std::cout << "\n";

	for (int i = 5; i < 8; i++) {
		std::cout << "\n | ";


		for (int j = 0; j < 9; j++) {

			std::cout << (board_player(i, j) == 0 ? "0" : utils::numberCardToStr(board_player(i, j) - 1)) << ((doesColumnhaveCards[j] && board_player(i, j) == 0) ? "  | " : " | ");

		}

	}

	std::cout << "\n";


	
}

