

#ifndef COLORMANAGER
#define COLORMANAGER

#include "../utils/utils.h"
#include <array>
#include <bitset>
#define nbColorCMB 462

// Toutes les lookup tables ont été concues en avance, et le code a été généré via un script generate_array.js disponible sur github
// Une méthode renvoyant un iterator pour itérer directement sur le sous-tableau pourrait être envisagé

class ColorManager {

public:
	
	ColorManager();
	~ColorManager();

	void reset();
	void update(int card);
	int getHighestRank() const;

private:

	/* Indique si une combinaison de couleur existe encore.
	* Un bitset pourrait être utilisé pour gagner énormement de mémoire.
	*/
	std::bitset<nbColorCMB> CMB_exist;

	/* Index de la combinaison la plus haute existant encore (cf tableau ci dessus)
	*/
	int highest_existing_index_cmb;

};

#endif // !COLORMANAGER
