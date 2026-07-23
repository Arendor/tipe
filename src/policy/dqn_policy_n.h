


#ifndef DQN_POLICY_N_HEADER

#define DQN_POLICY_N_HEADER

#include "policy.h"
//#include "ReplayBuffer.h"

#include "../utils/utils.h"
#include "ReplayBuffer.h"
#include <random>
#include <memory>

#include <dlib/dnn.h>
#include <dlib/dnn/loss.h>

#define MAX_SIZE_BUFFER_DQN_DLIB 3000
#define CSTEP_DQN_DLIB 50
#define SOFT_UPDATE_FACTOR_DLIB 0.6325
#define LEARNING_STEP_DQN_DLIB 20
#define GAMMA_VALUE_DQN_DLIB 0.99
#define MINI_BATCH_SIZE_DQN_DLIB 50
#define REWARD_END_FR_DQN_DLIB 1
// Diviser les rewards qui train le réseau pour faciliter l'apprentissage
// Maintenant dans rewards
//#define NORMALIZATION_FACTOR_DLIB 3.4
#define DLIB_REWARD_CONFIG "winAgainstRandom"

#define EPSILON_DECAY_STEP_DLIB 1250
#define EPSILON_DECAY_FACTOR_DLIB 0.9
#define EPSILON_VALUE_POLICY_DLIB 0.1

#define SHOWING_DQN_VALUE 75

extern std::mt19937 gen;

class TransitionDLIB {

public:

	arma::Mat<double> board;
	int nb_remaining_cards;
	int card_played_me;
	int frontier_card_played_me;
	int card_drawn;
	int card_played_adversary;
	int frontier_card_played_adversary;
	bool hasdrawn;
	double reward;
	bool isTerminal;
	bool isComplete;
	// Only for the user that use it
	std::vector<int> allowed_frontiers_after_play;
	int timer;


	TransitionDLIB() : board(), nb_remaining_cards(-1), card_played_me(-1), frontier_card_played_me(-1), card_drawn(-1), card_played_adversary(-1), frontier_card_played_adversary(-1), hasdrawn(false), reward(0), isComplete(false), timer(-1), isTerminal(false), allowed_frontiers_after_play(0)
	{

	};

	~TransitionDLIB() {

	};

	void print_board() {
		std::cout << "\n------ Printing board transition ------\n";
		board.print();
	}


	void print() {

		std::cout << "\n------ Printing transition ------\n" << "\nNombre de cartes restantes : " << nb_remaining_cards << "\n Carte jouée : " << card_played_me << " - Frontière : " << frontier_card_played_me << "\n Carte piochée : " << card_drawn << "\n Carte jouée adversaire : "
			<< card_played_adversary << " - Frontière : " << frontier_card_played_adversary << " - a pioché : " << hasdrawn << "\n Récompense : " << reward << "\n Action terminale : " << isTerminal << "\n Est complet : " << isComplete << "\n Temps : " << timer
			<< "\n----- ------";


	}

	arma::Mat<double> build_next_board() {

		arma::Mat<double> next_board(board.memptr(), board.n_rows, board.n_cols);

		next_board(card_played_me, 9) = 0;
		next_board(card_played_me, frontier_card_played_me) = 1;
		if (card_played_adversary != -1) {
			next_board(card_played_adversary + 55, frontier_card_played_adversary) = 1;
			next_board(card_played_adversary + 55, 9) = 0;
		}

		if (card_drawn != -1) {
			next_board(card_drawn, 9) = 1;
		}

		int taille_cartes_adversaire = nb_remaining_cards < 6 ? nb_remaining_cards : 6;

		if (taille_cartes_adversaire != 0) {
			for (int i = 55; i < 109; i++) {
				if (next_board(i, 9) != 0) next_board(i, 9) = 1 / taille_cartes_adversaire;
			};
		}


		return next_board;
	};



};

// Complètement stupide
//using net_type = dlib::loss_mean_squared<dlib::fc<1, dlib::elu<dlib::fc<54, dlib::elu<dlib::fc<545, dlib::elu<dlib::fc<1090, dlib::input<dlib::matrix<double, 1090, 1>>>>>>>>>>;

//using net_type = dlib::loss_mean_squared<dlib::fc<1, dlib::elu<dlib::fc<54, dlib::elu<dlib::fc<545, dlib::input<dlib::matrix<double, 1090, 1>>>>>>>>

//using net_type = dlib::loss_mean_squared<dlib::fc<1, dlib::elu<dlib::fc<256, dlib::elu<dlib::fc<700, dlib::input<dlib::matrix<double, 1090, 1>>>>>>>>;
// using net_type = dlib::loss_mean_squared<dlib::fc<1, dlib::elu<dlib::fc<256, dlib::elu<dlib::fc<700, dlib::input<dlib::matrix<double, 1090, 1>>>>>>>>;

using net_type = dlib::loss_mean_squared<dlib::fc<1, dlib::elu<dlib::fc<200, dlib::elu<dlib::fc<450, dlib::elu<dlib::fc<750, dlib::input<dlib::matrix<double, 1090, 1>>>>>>>>>>;

class DQNN_Policy : public Policy {

public:

	// CALL Policy::Policy(); ?

	DQNN_Policy();
	~DQNN_Policy();

	/* POLICY OVERRIDING */

	// EVENTS

	void start_turn_EVENTC(bool isme) override; // ok

	void end_party_EVENTC(PlayerType winner) override; // ok

	void frontier_win_EVENTC(int n_frontier, bool haswin) override; // ok

	void card_drawn_EVENTC(int card, bool ismyturn) override; // ok

	void end_turn_EVENTC(bool ismyturn) override; // ok

	void card_played_EVENTC(int card, int frontier, bool ismyturn) override; // ok

	// METHODS

	// REIMPLEMENT & CALL
	std::array<int, 2> choose_action() override;

	void restart_game() override;

	/* DQN POLICY */

	void copy_target_to_estimate();
	void soft_copy(float param);

	void update_networks();

	void add_transition();

	void setSynchroFile(std::string file);
	void setSaveFile(std::string file);
	void load();
	void save();
	void loadFrom(std::string file);
	void saveTo(std::string file);

	void setAutoSave(int epochs);
	void setAutoSave(std::string file, int epochs);


	// A utiliser quand on itère le vecteur sortant
	bool is_allowed(int indice);


	// Replay buffer

	net_type targetNetwork;
	dlib::dnn_trainer<net_type, dlib::adam> trainer_target;
	net_type estimateNetwork;
	//dlib::dnn_trainer<net_type, dlib::adam> trainer_estimate;
	// inutile pour le moment

	ReplayBuffer<TransitionDLIB, MAX_SIZE_BUFFER_DQN_DLIB> buffer;


	// Info
	std::vector<float> dqn_values;
	std::vector<float> reward_values;


	double epsilon;
	double learning_rate;
	double epsilon_decay;
	int minibatch_size;

	// gamma
	double gamma;
	double reward;
	// Pour l'update global
	int learning_step;
	// pour l'update de l'estimate network
	int Cstep;

	DQNN_Policy& set_epsilon(double new_epsilon);
	DQNN_Policy& set_learning_rate(double new_learning_rate);




	static std::vector<int> getAllowedFrontiers(arma::Mat<double>& board);
	static std::vector<int> getCards(arma::Mat<double>& ml_board);

private: 

	bool is_save_file;
	std::string save_file;

	bool is_synchro_file;
	std::string synchro_file;




};




#endif