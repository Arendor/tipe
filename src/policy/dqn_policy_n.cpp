
#include "dqn_policy_n.h"
#include "../player/agent.h"
#include "../game/game_engine.h"
#include "../utils/utils.h"
#include "../../neural_network/functions/activation_function.h"
#include "policy.h"

#include "../utils/compile_time_loop.h"
#include <random>
#include <memory>
#include <limits>
#include <algorithm>

#include <dlib/cuda/tensor_tools.h>

#include <fstream>
#include <nlohmann/json.hpp>

extern std::mt19937 gen;

using json = nlohmann::json;
std::ifstream file("C:/Users/arend/Desktop/development/cpp/tipe/rewards.json");


nlohmann::json rewards = nlohmann::json::parse(
	std::istreambuf_iterator<char>(file),
	std::istreambuf_iterator<char>(),
	nullptr, true, true);

// A modifier si on change la structure du réseau de neurones
//constexpr std::array<int,3> layers_with_data = { 1, 3, 5 };
// constexpr std::array<int, 4> layers_with_data = { 1, 3, 5, 7 };

constexpr std::array<int, 4> layers_with_data = { 1, 3, 5, 7 };

DQNN_Policy::DQNN_Policy() : learning_rate(0), epsilon(EPSILON_VALUE_POLICY_DLIB), epsilon_decay(EPSILON_DECAY_FACTOR_DLIB), reward(0), learning_step(LEARNING_STEP_DQN_DLIB), minibatch_size(MINI_BATCH_SIZE_DQN_DLIB), Cstep(CSTEP_DQN_DLIB), gamma(GAMMA_VALUE_DQN_DLIB), targetNetwork(), estimateNetwork(), buffer(), is_save_file(false), save_file(), is_synchro_file(false), synchro_file(), dqn_values(), reward_values(),
// Paramètres à modifier ici
trainer_target(targetNetwork, dlib::adam(0.0005, 0.9, 0.999))//, trainer_estimate(estimateNetwork, dlib::adam(0.0005, 0.9, 0.999))
{

	//trainer_target.set_min_learning_rate(0.00001);
	trainer_target.set_mini_batch_size(minibatch_size);

	trainer_target.be_verbose();
	// While the trainer is running it keeps an eye on the training error.  If
	// it looks like the error hasn't decreased for the last 2000 iterations it
	// will automatically reduce the learning rate by 0.1.  You can change these
	// default parameters to some other values by calling these functions.  Or
	// disable the automatic shrinking entirely by setting the shrink factor to 1.
	trainer_target.set_iterations_without_progress_threshold(1750); // 1750
	trainer_target.set_learning_rate_shrink_factor(0.5);
	// The learning rate will start at 1e-3.
	trainer_target.set_learning_rate(1e-3);



}

DQNN_Policy::~DQNN_Policy()
{
}

/************ EVENTS ************/

/*
*
*
* * Ici qu'on update le buffer, si t%learn_step = 0, on lance la mise à jour

*/
// It's non terminal


/*
* GERER LE DEBUT ET LA FIN
*
* Début :
* Si ce n'est l'agent en Question moi qui commence
*
*/

void DQNN_Policy::copy_target_to_estimate() {

	static_for<0, layers_with_data.size()>([&]<int I>() {
		std::copy(dlib::layer<layers_with_data[I]>(targetNetwork).layer_details().get_weights().begin(),
			dlib::layer<layers_with_data[I]>(targetNetwork).layer_details().get_weights().end(),
			dlib::layer<layers_with_data[I]>(estimateNetwork).layer_details().get_weights().begin());

		std::copy(dlib::layer<layers_with_data[I]>(targetNetwork).layer_details().get_biases().begin(),
			dlib::layer<layers_with_data[I]>(targetNetwork).layer_details().get_biases().end(),
			dlib::layer<layers_with_data[I]>(estimateNetwork).layer_details().get_biases().begin());
	});

}


void DQNN_Policy::soft_copy(float param) {


	static_for<0, layers_with_data.size()>([&]<int I>() {
		dlib::tensor&& weights_dest = dlib::layer<layers_with_data[I]>(estimateNetwork).layer_details().get_weights();
		dlib::tensor&& bias_dest = dlib::layer<layers_with_data[I]>(estimateNetwork).layer_details().get_biases();
		//std::cout << *dlib::layer<layers_with_data[I]>(estimateNetwork).layer_details().get_weights().begin();

		dlib::tt::add(1 - param, weights_dest, param, dlib::layer<layers_with_data[I]>(targetNetwork).layer_details().get_weights());
		dlib::tt::add(1 - param, bias_dest, param, dlib::layer<layers_with_data[I]>(targetNetwork).layer_details().get_biases());
		//std::cout << *dlib::layer<layers_with_data[I]>(estimateNetwork).layer_details().get_weights().begin();

	});

};



void DQNN_Policy::end_turn_EVENTC(bool ismyturn) {


	// Buffer temporaire 
	/*
	* Add event
	*
	* jouer card (frontière, carte
	* carte piochée
	* carte piochée adversaire (oui non)
	* carte jouée adversaire (frontière, carte)
	*/
	if (isTraining) {
		if (ismyturn) {
			//if (debug_mode) std::cout << " event end turn myself ";
			TransitionDLIB& tr = buffer.get_current_transition();
			// La condition est ici inutile
			tr.allowed_frontiers_after_play = Policy::getAllowedFrontiers();
		}
		else if (buffer.size != 0) {
			//if (debug_mode) std::cout << " event end turn adversary BFC ";
			TransitionDLIB& tr = buffer.get_current_transition();
			//if (debug_mode) std::cout << "- tr timer " << tr.timer << " local turns : " << local_turns << " - ";
			if (tr.timer == local_turns) {
				//if (debug_mode) std::cout << " event end turn adversary AFC ";
				tr.isComplete = true;
				//std::cout << "global turns : " << global_turns << "learning step : " << learning_step;
				if (info_mode) reward_values.push_back(tr.reward);
				
				if (global_turns % learning_step == 0 && global_turns != 0) {
					update_networks();
				}
			}
		}
	}

	Policy::end_turn_EVENTC(ismyturn);

}


void DQNN_Policy::card_drawn_EVENTC(int card, bool ismyturn) {

	if (isTraining) {
		if (ismyturn) {
			//if (debug_mode) std::cout << " event card drawn myself ";
			TransitionDLIB& tr = buffer.get_current_transition();
			// Ici la condition est non nécessaire
			tr.card_drawn = card;
		}
		else if (buffer.size != 0) {
			TransitionDLIB& tr = buffer.get_current_transition();
			//if (debug_mode) std::cout << " event card drawn adversary BFC ";

			if (tr.timer == local_turns) {
				//if (debug_mode) std::cout << " event card drawn adversary AFC ";
				// Si aucune carte n'a été pioché : -1 sinon 0
				tr.hasdrawn = card == 0;
			}
		}
	}

}


void DQNN_Policy::card_played_EVENTC(int card, int frontier, bool ismyturn) {

	if (isTraining) {
		if (ismyturn) {
			//if (debug_mode) std::cout << " event card played myself ";
			TransitionDLIB& tr = buffer.get_current_transition();
			tr.card_played_me = card;
			tr.frontier_card_played_me = frontier;

			int nb = owningUser->owningGame->getNumberOfCards(frontier, owningUser->playerID);
			
			if (nb == 2) {

				int firstCard = owningUser->owningGame->getCardN(frontier, 1, owningUser->playerID);
				int secondCard = owningUser->owningGame->getCardN(frontier, 2, owningUser->playerID);

				int v1 = utils::getValue(firstCard), v2 = utils::getValue(secondCard), c1 = utils::getColor(firstCard), c2 = utils::getColor(secondCard);

				if (owningUser->owningGame->turn_remaining > rewards[DLIB_REWARD_CONFIG]["deuxiemeCarteJouee"]["tours_restants"] && owningUser->owningGame->estVierge(frontier)) {
					if (v1 != v2 && c1 != c2) tr.reward += rewards[DLIB_REWARD_CONFIG]["deuxiemeCarteJouee"]["negatif"];
					else tr.reward += rewards[DLIB_REWARD_CONFIG]["deuxiemeCarteJouee"]["memeCV"];
				};

			}
			else if (nb == 3) {
				typeOfCMB cmbtype = owningUser->owningGame->getCMBType(owningUser->playerID, frontier);
				int base = rewards[DLIB_REWARD_CONFIG]["combinaisonTerminee"]["base"];

				switch (cmbtype) {
				default:
				case somme: base += rewards[DLIB_REWARD_CONFIG]["combinaisonTerminee"]["somme"]; break; // -3
				case suite: base += rewards[DLIB_REWARD_CONFIG]["combinaisonTerminee"]["suite"]; break; // -2
				case couleur: base += rewards[DLIB_REWARD_CONFIG]["combinaisonTerminee"]["couleur"]; break; // 0
				case brelan: base += rewards[DLIB_REWARD_CONFIG]["combinaisonTerminee"]["brelan"]; break; // +5
				case suite_couleur: base += rewards[DLIB_REWARD_CONFIG]["combinaisonTerminee"]["suite_couleur"]; break; // +10
				}

				tr.reward += base;
			};
		}
		else if (buffer.size != 0) {
			//if (debug_mode) std::cout << " event card played adversary BFC ";
			TransitionDLIB& tr = buffer.get_current_transition();
			if (tr.timer == local_turns) {
				//if (debug_mode) std::cout << " event card played adversary AFC ";
				tr.card_played_adversary = card;
				tr.frontier_card_played_adversary = frontier;
			}
		}
	}

}




// REWARD
/*
* Devra lancer tout seul le processus de reward
* Il faudra indiquer condition si c'est terminal ou non et laisser faire alors cela
* Déjà géré alors c'est good
*/


void DQNN_Policy::start_turn_EVENTC(bool isme)
{
	// Important de le faire AVANT
	Policy::start_turn_EVENTC(isme);
	//std::cout << "start_turn" << isme;
	//if (debug_mode) std::cout << " event start_turn ";
	
	if (isTraining && isme) this->add_transition();
	
}


void DQNN_Policy::end_party_EVENTC(PlayerType winner) {

	Policy::end_party_EVENTC(winner);

	// Modifier reward selon type de victoire (3 frontières continues etc...)

	// Conditon de buffer_size non nécessaire

	if (isTraining) {
		if (buffer.size != 0) {
			TransitionDLIB& tr = buffer.get_current_transition();
			tr.isTerminal = true;
			tr.isComplete = true;
			if (winner == this->owningUser->playerID) tr.reward += rewards[DLIB_REWARD_CONFIG]["finPartie"]["victoire"]; // 30
			else {
				if (winner == draw) tr.reward += rewards[DLIB_REWARD_CONFIG]["finPartie"]["egalite"];
				else tr.reward += rewards[DLIB_REWARD_CONFIG]["finPartie"]["defaite"]; // 30
			}
			// Nombre de frontières gagnées
			if (rewards[DLIB_REWARD_CONFIG]["finPartie"]["+1byfrontier"]) tr.reward += this->owningUser->owningGame->get_nb_fr_win(this->owningUser->playerID) * REWARD_END_FR_DQN_DLIB;
			if (info_mode) reward_values.push_back(tr.reward);
		}

		// Puisque si end_party_EVENTC est appelé, end_turn n'est pas appelé !
		if (global_turns % learning_step == 0 && global_turns != 0) {
			update_networks();
		}

		if ((nb_parties+1) % (SHOWING_DQN_VALUE) == 0 && info_mode) {

			std::cout << "Q-values pendant cette partie : ";
			for (auto it = dqn_values.begin(); it != dqn_values.end(); it++) {

				std::cout << *it << " / ";

			}
			std::cout << "\n";

			std::cout << "Récompenses pendant cette partie : ";
			for (auto it = reward_values.begin(); it != reward_values.end(); it++) {

				std::cout << *it << " / ";

			}
			std::cout << "\n";
		}
	}


}

// REWARD
void DQNN_Policy::frontier_win_EVENTC(int frontier, bool haswin) {

	if (isTraining) {
		// Théoriquement pour que ca arrive le joueur doit avoir joué une fois
		// Conditions de buffer inutile car chaque joueur doit avoir joué au moins 2 fois pour qu'une frontière soit gagnée
		if (buffer.size != 0) {
			TransitionDLIB& tr = buffer.get_current_transition();
			if (tr.timer == local_turns) {
				typeOfCMB cmbtype = owningUser->owningGame->getCMBType(owningUser->playerID, frontier);

				if (haswin) {
					int base = rewards[DLIB_REWARD_CONFIG]["frontiereGagne"]["base"];

					switch (cmbtype) {
					default:
					case somme: base += rewards[DLIB_REWARD_CONFIG]["frontiereGagne"]["somme"]; break; // +0
					case suite: base += rewards[DLIB_REWARD_CONFIG]["frontiereGagne"]["suite"]; break; // +1
					case couleur: base += rewards[DLIB_REWARD_CONFIG]["frontiereGagne"]["couleur"]; break; // +2
					case brelan: base += rewards[DLIB_REWARD_CONFIG]["frontiereGagne"]["brelan"]; break; // +4
					case suite_couleur: base += rewards[DLIB_REWARD_CONFIG]["frontiereGagne"]["suite_couleur"]; break; //+6
					}

					tr.reward += base;
				}
				else {
					int base = rewards[DLIB_REWARD_CONFIG]["frontierePerdue"]["base"];

					switch (cmbtype) {
					default:
					case somme: base += rewards[DLIB_REWARD_CONFIG]["frontierePerdue"]["somme"]; break;
					case suite: base += rewards[DLIB_REWARD_CONFIG]["frontierePerdue"]["suite"]; break;
					case couleur: base += rewards[DLIB_REWARD_CONFIG]["frontierePerdue"]["couleur"]; break;
					case brelan: base += rewards[DLIB_REWARD_CONFIG]["frontierePerdue"]["brelan"]; break;
					case suite_couleur: base += rewards[DLIB_REWARD_CONFIG]["frontierePerdue"]["suite_couleur"]; break; // voir les refs des valeurs avant
					}

					tr.reward += base;
				}
			}
		}
	}


}





/************ METHODS ************/

//int gamma = ;




// Comment le réseau de neurones peut comprendre dans quel ordre on joue les cartes?... en vrai ça peut pas marcher mdr c'est impossible genreeeee

// en fait c'est mauvais
// il vaut mieux évaluer le tableau 
// Follow espilon greedy policy

/*
* Calcul du Q0
* faut supprimer la carte dans la main du joueur pour évaluer le terrain
*
* tree search possible (probabilité sur carte piochées?)
*
* A stocker :
* Etat actuel
* carte choisie + position
* carte piochée (-1 si rien)
* reward
*
*
* Pour calculer Q0 il faudra parcourir tous les états possibles sur la main de l'adversaire à ce moment là et en calculer le Q sur toutes les actions possibles de l'agent
*
* CCL faut modifier tout ça
*
*
*
*	ETAT ACTUEL
*
*
*
*
*/


void DQNN_Policy::restart_game()
{

	Policy::restart_game();
	dqn_values.clear();
	reward_values.clear();


}



std::array<int, 2> DQNN_Policy::choose_action()
{

	std::uniform_real_distribution<> real_dist(0, 1);

	// 
	if (isTraining && real_dist(gen) < epsilon) {
		std::vector<int> allowed_frontiers = Policy::getAllowedFrontiers();
		std::uniform_int_distribution<> int_dis_fr(0, allowed_frontiers.size() - 1);
		std::uniform_int_distribution<> int_dis_card(0, this->owningUser->cards.size - 1);

		return std::array<int, 2> {this->owningUser->cards[int_dis_card(gen)], allowed_frontiers[int_dis_fr(gen)]};

	}
	else {
		// Implémenter la vraie stratégie ici

		// Peut être même créer un .predict qui ne fait pas de truc additionnel ? 
		arma::Col<double> vec = this->owningUser->extended_board_ml.as_col();
		dlib::matrix<double, 1090, 1> dlib_vec = dlib::mat(vec);

		std::vector<int> allowed_frontiers = Policy::getAllowedFrontiers();

		float max_QV = -std::numeric_limits<float>::infinity();
		int fron = -1; int card = -1;

		for (int i = 0; i < this->owningUser->cards.size; i++) {
			dlib_vec(this->owningUser->cards[i] + 9 * 109) = 0;

			for (std::vector<int>::iterator f = allowed_frontiers.begin(); f != allowed_frontiers.end(); f++) {
				dlib_vec(this->owningUser->cards[i] + (*f) * 109) = 1;
				
				float QV = targetNetwork(dlib_vec);
				//std::cout << " - valeur QV : " << QV << " - ";
				if (QV > max_QV) {
					max_QV = QV;
					fron = *f;
					card = this->owningUser->cards[i];
				};

				dlib_vec(this->owningUser->cards[i] + (*f) * 109) = 0;

			}

			dlib_vec(this->owningUser->cards[i] + 9 * 109) = 1;

		};


		if (isTraining && info_mode) dqn_values.push_back(max_QV);



		return std::array<int, 2> { card, fron };
	}

}


bool DQNN_Policy::is_allowed(int indice) {
	int frontier = indice / 6;
	int card = indice % 6;
	// Vérifie que l'agent a bien le droit de poser cette carte ie frontière pas pleine et suffisamment de carte en main
	return this->owningUser->owningGame->isFull(frontier, this->owningUser->playerID) && this->owningUser->cards.size > card;
}


std::vector<int> DQNN_Policy::getCards(arma::Mat<double>& ml_board) {

	std::vector<int> cards = {};

	for (int i = 0; i < 54; i++) {
		if (ml_board(i, 9) == 1) cards.push_back(i);
	}

	return cards;
}

std::vector<int> DQNN_Policy::getAllowedFrontiers(arma::Mat<double>& ml_board)
{
	std::vector<int> allowed_frontiers = {};

	for (int f = 0; f < 9; f++) {
		int nb = 0;
		for (int j = 0; j < 54; j++) {
			nb += ml_board(j, f);
		}
		if (nb < 3) allowed_frontiers.push_back(f);
	}

	return allowed_frontiers;

}

/*
*
* @exception
* @returns void
* @see getAllowedFrontiers
*/
// j'ai inversé le nom de target network et de estimatenetwork par rapport aux docs


// A remplacer par des std::array vis à vis de la mémoire ce sera vachement mieux

void DQNN_Policy::update_networks() {

	std::vector<int> sample = buffer.sample_indexes(minibatch_size);

	if (sample.size() != minibatch_size) return;

	std::vector<dlib::matrix<double, 1090, 1>> inputs;
	std::vector<float> expected;

	for (auto it = sample.begin(); it != sample.end(); it++) {

		TransitionDLIB& tr = buffer.get_transition(*it);

		arma::Col<double> vec_predict = tr.board.as_col();

		inputs.push_back(dlib::mat(vec_predict));

		float target = tr.reward / rewards[DLIB_REWARD_CONFIG]["normalization_factor"];

		if (!tr.isTerminal) {

			arma::Col<double> vec_target = tr.build_next_board().as_col();
			dlib::matrix<double, 1090, 1> dlib_target = dlib::mat(vec_target);

			std::vector<int> cards = DQNN_Policy::getCards(tr.board);

			float max_QV = -std::numeric_limits<float>::infinity();

			for (int i = 0; i < cards.size(); i++) {
				dlib_target(cards[i] + 9 * 109) = 0;

				for (std::vector<int>::iterator f = tr.allowed_frontiers_after_play.begin(); f != tr.allowed_frontiers_after_play.end(); f++) {
					dlib_target(cards[i] + (*f) * 109) = 1;
					
					float QV = estimateNetwork(dlib_target);

					if (QV > max_QV) {
						max_QV = QV;
					};

					dlib_target(cards[i] + (*f) * 109) = 0;

				}

				dlib_target(cards[i] + 9 * 109) = 1;


			}

			if (max_QV == -std::numeric_limits<float>::infinity()) max_QV = 0;
			// ERREUR c'est un +
			target += gamma * max_QV;
		}

		expected.push_back(target);

	}

	//std::cout << "update";

	trainer_target.train_one_step(inputs, expected);
	trainer_target.get_net();

	if (global_turns % (Cstep * learning_step) == 0 && global_turns != 0) {
		//std::cout << "update";
		//copy_target_to_estimate();
		soft_copy(SOFT_UPDATE_FACTOR_DLIB);
	}

	if (global_turns % (EPSILON_DECAY_STEP_DLIB * learning_step) == 0 && global_turns != 0) {
		epsilon *= epsilon_decay;
	}

};


void DQNN_Policy::add_transition() {

	TransitionDLIB& tr = buffer.create_transition();

	//this->owningUser->extended_board_ml.print();

	tr.board = arma::Mat<double>(this->owningUser->extended_board_ml.memptr(), this->owningUser->extended_board_ml.n_rows, this->owningUser->extended_board_ml.n_cols);
	tr.nb_remaining_cards = this->owningUser->remaining_cards.size();
	tr.timer = local_turns;

}

void DQNN_Policy::setSynchroFile(std::string file)
{
	synchro_file = file;
	is_synchro_file = true;
}

void DQNN_Policy::setSaveFile(std::string file)
{
	save_file = file;
	is_save_file = true;
}

void DQNN_Policy::load()
{
	if (!is_save_file) throw std::runtime_error("Il n'y a pas de fichier de sauvegarde configuré");

	dlib::deserialize(save_file) >> targetNetwork;
	dlib::deserialize(save_file) >> estimateNetwork;
}

void DQNN_Policy::save()
{
	if (!is_save_file) throw std::runtime_error("Il n'y a pas de fichier de sauvegarde configuré");

	targetNetwork.clean();
	dlib::serialize(save_file) << targetNetwork;
}

void DQNN_Policy::loadFrom(std::string str)
{
	dlib::deserialize(str) >> targetNetwork;
	dlib::deserialize(str) >> estimateNetwork;
}

void DQNN_Policy::saveTo(std::string file)
{
	targetNetwork.clean();
	dlib::serialize(file) << targetNetwork;
}

void DQNN_Policy::setAutoSave(int epochs)
{
	if (!is_synchro_file) throw std::runtime_error("Il n'y a pas de fichier de sauvegarde de synchronisation configuré");

	trainer_target.set_synchronization_file(synchro_file, std::chrono::seconds(epochs));
	trainer_target.get_net();
	//dlib::deserialize(synchro_file) >> estimateNetwork;
	//copy_target_to_estimate();

}

void DQNN_Policy::setAutoSave(std::string file, int epochs)
{
	synchro_file = file;
	is_synchro_file = true;
	trainer_target.set_synchronization_file(synchro_file, std::chrono::seconds(epochs));
	trainer_target.get_net();
	//dlib::deserialize(file) >> estimateNetwork;
	//copy_target_to_estimate();

}


/************ SETTINGS ************/

DQNN_Policy& DQNN_Policy::set_epsilon(double new_epsilon) {
	epsilon = new_epsilon;
	return *this;
};

DQNN_Policy& DQNN_Policy::set_learning_rate(double new_learning_rate) {
	learning_rate = new_learning_rate;
	trainer_target.set_learning_rate(new_learning_rate);
	return *this;
};

