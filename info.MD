





# Faire fonctionner le projet

## Installer Armadillo

A modifier dans `CMakeLists.txt`, localement installé ici dans `C:/Armadillo/armadillo-15.0.3`


## Ajouter certaines variables au PATH

[à détailler]

## Lancer localement 

start "C:/Program Files (x86)/Intel/oneAPI/setvars.bat"

Not necessary :
start "C:/Program Files (x86)/Intel/oneAPI/compiler/2025.3/env/vars.bat"

cmake . 

Just for everything to run :
start "C:/Program Files (x86)/Intel/oneAPI/setvars.bat"


## Pour voir les logs précis

cmake . 



# TODO :


### Critique

- initialiser les networks

- Copier les neural networks

- plus de liberté dans le format de réseau de neurone choisis 
- plus de liberté dans les récompenses / malus que recoit l'agent 

- Fix l'erreur 


éviter la copie inutile dans les transitions et utiliser des make_shared et shared_ptr iguess

ptet essayer d'installer un meilleur réseau de neurones MDR ce serait bien
(adam optimizer)

sinon diminuer le espilon decay au fur et à mesure de l'apprentissage

quand istraining == false, ne pas explorer (donc utiliser uniquement le réseau de neurones)



Changer qui commence dans game engine



copy game engine state and all in a file à faire


Mettre à jour gamma
Modifier la stratégie au fur et à mesure



fonction copy board d'urgence absolue!!!!!


et aussi check si l'actuel est le bon, système de turn à régler (selon qui a commencé, bref tout ça tout ça)



Créer une vraie interface pour train, jouer des parties etc. 

mode auto pour console player

### Moins important :

- supprimer && local_turns != 0_
- ajouter des initializiations à policy;

- modifier pour que ca train pas si isTraining = false;



- load array_NCB dynamiquement pour réduire la place prise sur le stack. (juste remplacer par un std::vector)




générer un itérator 


changing replaybuffer std::vector to fixed size std::array (actually not sure because of the stack memory)

Method de save absoluement 


Batch 1 :
Cstep 7
Gamma : 0.1
Buffer : 1000
Learning step : 20

Batch 2 :
Cstep 50
Gamma : 0.92
Buffer : 2000
Learning Step : 17
(Overfitting)

Batch 3 :
Cstep 100
Gamma : 0.98
Buffer : 2000
Learning Step : 20


Batch 4 :
Cstep 100
Gamma : 0.6
Buffer : 2000
Learning Step : 20



Modifier les rewards :


Batch 5 : 
Cstep : 100
Gamma : 0.95
Buffer : 2000
Learning Step : 20
Nombre de parties : 8000

(il faudrait faire un count, pour que si le nb de frontière avec une sommme dessus > 3, alors malus)
Gagner une frontière :
	base : 4
	somme : 0
	suite : 1
	couleur : 2
	brelan : 4
	suite couleur : 6 
Perdre une frontière : 
	base : -5
	somme : -3
	suite : -2
	couleur : -1
	brelan : 0.5
	suite_couleur : 1.5 
Gagner la partie : +30
Perdre la partie : -30
Egalite : -10
+1 par nombre de frontière gagnée

(à l'avenir différencié victoire 3 fr et 5 fr)

Combinaison terminée (pour la personne qui joue)
	somme : -3
	suite : -2
	couleur : 0
	brelan : +5
	suite_couleur : +10



Peut-être faire qqch si toutes les frontières sont occupées trop tôt?
(si remaining_turns > 30 && all frontiers are finished -3)



soft update pour target network and estimate
inverser target et estimate
Faire la présentation !!!



passer Cstep à 50
Passer la taille du buffer à 2000 (~80 parties en mémoire)
gamma à 0.92 (augmenter la vue à long terme) + plus de gamma decay
passer learning step à 17


(je crois que j'ai inversé target et estimate mais ça change en fait rien)


les rewards qui peuvent être modifié 
wowww

penser à inline



déplacer l'update des networks au début du tour.

normalisation des rewards 

Pour optimiser les transitions, on pourrait une fois que le next_board a été créé, ben on le stocke dans la transition, comme ça au besoin on le renvoie :)

Epsilon decay??

si arborescent structure, il faudrait avoir des fct pour check si win, 

il faudrait implémenter une class shallow_check qui ferait tout en static, 
et les datas seraient stockées dans une class, 
le tout après wrap qql part (notamment pour le color module, les trucs seraient wrap dans une big class au dessus)
_
# A faire peut être

Utiliser fmt pour print des é è à




Installer pytorch

Faire une structure arborescente qui soit intelligente (peut être algo min max mais à pondérer par du hasard alors faut faire des recherches)

Pouvoir save le déroulement d'une partie






