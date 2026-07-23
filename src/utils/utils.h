


#ifndef UTILS
#define UTILS

#include <string>
#include <unordered_map>


// Verification on :
#define VERIF
#define LOWER_VERIF

#define TAILLE_PAQUET 6

#define MODCARDS 23

#define player_type int

#define DECK_SIZE 10

#define AGENT_BOARD_TYPE double 



enum PlayerType {
	none = 0,
	pl1 = 1,
	pl2 = 2,
	draw = 3,
};

enum typeOfCMB {
	somme = 0,
	suite = 1,
	couleur = 2,
	brelan = 3,
	suite_couleur = 4,
};

#define nbCOLORS 6

class utils {

public:

	static inline int getValue(int card);
	static inline int getColor(int card);
	static inline int getNumber(int value, int color);
	static std::string numberCardToStr(int card);
	static int getColorFromChar(char c);
	static bool isColor(char c);


};


inline int utils::getValue(int card)
{
	return (card / TAILLE_PAQUET);
}

inline int utils::getColor(int card)
{
	return card % TAILLE_PAQUET;
}

inline int utils::getNumber(int value, int color) {
	return  value * TAILLE_PAQUET + color;
}


#endif