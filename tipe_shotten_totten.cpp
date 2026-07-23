// tipe_shotten_totten.cpp : définit le point d'entrée de l'application.
//

#include "tipe_shotten_totten.h"
//#include <armadillo>
#include "src/player/console_player.h"
#include "src/player/agent.h"

#include "src/game/game_engine.h"
#include "src/utils/utils.h"
#include "src/player/player.h"
#include "neural_network/main/Network.h"

#include "src/utils/compile_time_loop.h"

#include "src/policy/ReplayBuffer.h"

#include "src/policy/dqn_policy_n.h"

#include <fstream>
#include <nlohmann/json.hpp>




extern std::random_device rd;

int main() {



	//f.transform(ActivationFunct::Sigmoid);

	//f.print();

	/*arma::Mat<short> mat_short(10, 5, arma::fill::value(1));

	mat_double.print();
	mat_short.print();
	(mat_double * mat_short).print();*/

	/*ConsolePlayer pl1;
	ConsolePlayer pl2;

	GameEngine new_game(&pl1, &pl2);

	new_game.run();*/


	arma::arma_rng::set_seed(rd());

	/*
	std::vector<int> vec = { 5,3,0,8,9 };

	ReplayBuffer<int, 10> RB;

	for (int i = 15; i < 25; i++) {
		RB.add_transition(i);
	}

	auto it = RB.begin(&vec[0]);


	std::cout << *it;
	it++;
	std::cout << *it;
	it++;
	std::cout << *it;
	it++;
	std::cout << *it;
	*/

	
	//train_batch(1500, false, true, false, 0);
	train_batch(0, true, true, false, 0);

	// train against random
	//trainAgainstRandom(0, true, true);

	// test
	//train_batch(0, true, false, true, 5000);

	//trainAgainstRandom(0, true, false, true, 5000);
	
	/*
	Player random_player;

	GameEngine game_random_dqn(&random_player, &pl2);

	int nb_random_win = 0;
	int nb_dqn_win = 0;
	int draw = 0;

	for (int i = 0; i < 5000; i++) {
		game_random_dqn.run();
		if (game_random_dqn.winner == PlayerType::pl1) nb_random_win += 1;
		else {
			if (game_random_dqn.winner == PlayerType::pl2) nb_dqn_win += 1;
			else draw += 1;
		}
		game_random_dqn.reset();
		if (i % 100 == 0) std::cout << " Jeu numero : " << i + 1;
	}

	std::cout << "Nb of nb_random_win : " << nb_random_win << " - Number of dqn win : " << nb_dqn_win << " - Draw / Other : " << draw;
	*/
	/**
	GameEngine game_with_console_player(&pl2, &pl3);

	game_with_console_player.setDebug(true);
	game_with_console_player.run();
	
	game_with_console_player.player_2->play();*/





	

	








	return 0;
}


// A modifier pour que ça renvoie les policy_train


void trainAgainstRandom(int nb_iterations, bool load, bool playAfter, bool testAgainstRandom, int nb_tests) {

	DQNN_Policy policy_dqn1;
	policy_dqn1.setTraining(true);
	policy_dqn1.setInfoMode(true);

	policy_dqn1.setSaveFile("C:/Users/arend/Desktop/development/cpp/tipe/save_networks/dlib16/test1dqn_save.dat");
	policy_dqn1.setAutoSave("C:/Users/arend/Desktop/development/cpp/tipe/save_networks/dlib16/test1dqn_sync", 500);

	if (load) {
		policy_dqn1.load();
	}

	Agent pl1;
	policy_dqn1.link(&pl1);
	pl1.set_policy(&policy_dqn1);

	Player random_player;

	GameEngine game_random_dqn(&random_player, &pl1);

	for (int i = 0; i < nb_iterations; i++) {
		game_random_dqn.run();
		game_random_dqn.reset();
		if (i % 100 == 0) std::cout << " Jeu numero : " << i + 1;
	}

	policy_dqn1.save();

	if (playAfter) {
		ConsolePlayer pl3;

		policy_dqn1.setTraining(false);


		GameEngine game_with_console_player(&pl1, &pl3);

		game_with_console_player.run();
		
		return;
	};

	if (testAgainstRandom) {

		policy_dqn1.setTraining(false);

		int nb_random_win = 0;
		int nb_dqn_win = 0;
		int draw = 0;

		for (int i = 0; i < nb_tests; i++) {
			game_random_dqn.run();
			if (game_random_dqn.winner == PlayerType::pl1) nb_random_win += 1;
			else {
				if (game_random_dqn.winner == PlayerType::pl2) nb_dqn_win += 1;
				else draw += 1;
			}
			game_random_dqn.reset();
			if (i % 100 == 0) std::cout << " Jeu numero : " << i + 1;
		}

		std::cout << "Nb of nb_random_win : " << nb_random_win << " - Number of dqn win : " << nb_dqn_win << " - Draw / Other : " << draw;
	}

	return;
}

void train_batch(int nb_iterations, bool load, bool playAfter, bool testAgainstRandom, int nb_tests) {

	DQNN_Policy policy_dqn1;
	DQNN_Policy policy_dqn2;

	policy_dqn1.setTraining(true);
	policy_dqn1.setInfoMode(true);
	policy_dqn2.setTraining(true);
	policy_dqn2.setInfoMode(true);


	policy_dqn1.setSaveFile("C:/Users/arend/Desktop/development/cpp/tipe/save_networks/dlib17/test1dqn_save.dat");
	policy_dqn1.setAutoSave("C:/Users/arend/Desktop/development/cpp/tipe/save_networks/dlib17/test1dqn_sync", 500);
	policy_dqn2.setSaveFile("C:/Users/arend/Desktop/development/cpp/tipe/save_networks/dlib17/test2dqn_save.dat");
	policy_dqn2.setAutoSave("C:/Users/arend/Desktop/development/cpp/tipe/save_networks/dlib17/test2dqn_sync", 500);

	if (load) {
		policy_dqn1.load();
		policy_dqn2.load();
	}

	Agent pl1;
	policy_dqn1.link(&pl1);
	pl1.set_policy(&policy_dqn1);
	//policy_dqn1.setDebugMode(true);

	Agent pl2;
	policy_dqn2.link(&pl2);
	pl2.set_policy(&policy_dqn2);

	GameEngine new_game(&pl1, &pl2);
	new_game.setDebug(false);

	for (int i = 0; i < nb_iterations; i++) {
		new_game.run();
		new_game.reset();
		if (i % 100 == 0) std::cout << " Jeu numero : " << i + 1;
	}

	policy_dqn1.save();
	policy_dqn2.save();

	if (playAfter) {
		ConsolePlayer pl3;

		policy_dqn1.setTraining(false);
		policy_dqn2.setTraining(false);

		std::cout << "\nContre quel joueur voulez vous jouez ? (1-2) : ";
		std::string nstr;
		std::cin >> nstr;

		if (nstr.length() != 1) throw std::runtime_error("Taille invalide");

		int n = nstr[0] - '0';

		if (n == 1) {
			GameEngine game_with_console_player(&pl1, &pl3);

			game_with_console_player.run();
		}
		else if (n == 2) {
			GameEngine game_with_console_player(&pl2, &pl3);

			game_with_console_player.run();
		} else throw std::runtime_error("Taille invalide");

		return;
	};

	if (testAgainstRandom) {

		policy_dqn1.setTraining(false);
		policy_dqn2.setTraining(false);

		Player random_player;

		GameEngine game_random_dqn(&random_player, &pl1);

		int nb_random_win = 0;
		int nb_dqn_win = 0;
		int draw = 0;

		for (int i = 0; i < nb_tests; i++) {
			game_random_dqn.run();
			if (game_random_dqn.winner == PlayerType::pl1) nb_random_win += 1;
			else {
				if (game_random_dqn.winner == PlayerType::pl2) nb_dqn_win += 1;
				else draw += 1;
			}
			game_random_dqn.reset();
			if (i % 100 == 0) std::cout << " Jeu numero : " << i + 1;
		}

		std::cout << "Nb of nb_random_win : " << nb_random_win << " - Number of dqn win : " << nb_dqn_win << " - Draw / Other : " << draw;
	}



}

/*
void test_network()
{

	std::vector<std::tuple<arma::Col<double>, arma::Col<double>>> set;

	Network network;

	int size = 10;

	network.add_layer(ActivationFunction::ELU, size, size);
	network.add_layer(ActivationFunction::ELU, size, size);
	network.add_layer(ActivationFunction::ELU, size, size);

	network.initialize_fill(0.01);

	int NB = 5;

	for (int i = 0; i < NB; i++) {
		set.push_back(std::tuple<arma::Col<double>, arma::Col<double>>(arma::Col<double>(size, arma::fill::randu), arma::Col<double>(size, arma::fill::randu)));
	}

	double learning_rate = 0.1;

	for (int k = 0; k < 100000; k++) {
		double error_total = 0;

		for (int i = 0; i < NB; i++) {
			arma::Col<double> err = 2 * (network.forward(std::get<0>(set[i])) - std::get<1>(set[i]));
			network.add_gradient(err);
			//network.resolve_ge(learning_rate);

			error_total += accu(pow((0.5 * err), 2));
		}

		if (k % 10000 == 0) {
			std::cout << "\n----------------- Fonction d'erreur, iteration" << k << " -----------------\n";
			std::cout << error_total << "\n";

			//network.print_layer_gradients();
		}

		network.resolve_ge(learning_rate);
		//learning_rate *= 0.9999;
	}


	for (int i = 0; i < NB; i++) {
		arma::Col<double> predict = network.forward(std::get<0>(set[i]));

		std::cout << "\n----------------- Vecteur n" << i << " -----------------\n";

		std::cout << "\n----------------- Prediction -----------------\n";

		predict.print();

		std::cout << "\n----------------- Realite -----------------\n";

		std::get<1>(set[i]).print();

		std::cout << "\n----------------- Erreur -----------------\n";
		arma::Col<double> err = 2 * (predict - std::get<1>(set[i]));

		err.print();

		std::cout << "\n----------------------------------\n\n\n";

	}

}*/

