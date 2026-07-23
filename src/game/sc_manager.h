#ifndef SCMANAGER
#define SCMANAGER

#include "../utils/utils.h"
#include <array>
#include <bitset>

// Nombre de combinaisons suite à la couleur par couleur
#define SCSIZE 7

class SCManager {

public:
	SCManager();
	~SCManager();

	int getHighestRank() const;
	void update(int card);

	void delete_adress(int adress);
	void reset();

private:
	
	std::bitset< nbCOLORS* SCSIZE> valeurs;
	int highest_non_null;

};


#endif // !SCMANAGER
