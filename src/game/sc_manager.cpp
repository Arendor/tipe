#include "sc_manager.h"
#include <algorithm>

SCManager::SCManager() : highest_non_null(nbCOLORS* SCSIZE - 1), valeurs() {
	valeurs.set();
}

SCManager::~SCManager() {

}


void SCManager::reset() {

	valeurs.set();	
	highest_non_null = nbCOLORS * SCSIZE - 1;

}

int SCManager::getHighestRank() const {

	if (highest_non_null == -1) return -1;
	return highest_non_null / nbCOLORS + MODCARDS * suite_couleur;

}

void SCManager::delete_adress(int adress) {

	if (adress >= 0 && adress < nbCOLORS * SCSIZE) {
		if (valeurs[adress]) {
			valeurs[adress] = false;
			if (adress == highest_non_null) {
				highest_non_null--;
				for (; highest_non_null >= 0; highest_non_null--) {
					if (valeurs[highest_non_null]) {
						break;
					}
				}
			}
		}
	}

}


void SCManager::update(int card) {

	if (highest_non_null == -1) return;

	int v = utils::getValue(card), c = utils::getColor(card);

	for (int i = 0; i < 3; i++) {
		delete_adress((v - i) * 6 + c);

	}

	return;
}