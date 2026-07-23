# TIPE 2026 : Code (quasi) complet

Voici le code de mon projet de TIPE pour l'année 2026 :)

L'objectif de ce TIPE est de trouver une stratégie gagnante pour le jeu schotten totten en utilisant l'apprentissage par renforcement (ce n'était pas la meilleure solution à mon avis)

Le code contient principalement un algorithme DQN, tout le code nécessaire pour faire tourner le jeu. Des "events" sont intégrées, les classes player, agent, et policy peuvent être héritées pour créer ses propres stratégies (il faut simplement bien appeler les méthodes héritées et il y a une petite subtilité pour l'évènement de fin de partie détaillée au début d'un fichier)

Il n'y de source pour les réseaux de neurones entrainés, les résultats ne sont pas vraiment satisfaisants.
La taille ou la structure du réseau ainsi que tous les autres paramètres d'apprentissage et les récompenses ne sont pas tellement optimisés. 

Quelques anciens fichiers de sauvegardes (en _save et .ignore) peuvent apparaitre.
Il n'y a pas de manière pratique de lancer ni d'interface UI (ce qui aurait été pratique mais mon temps était très limité). Il y a encore de nombreuses choses à faire pour améliorer ce superbe projet mais je n'ai plus le temps.
Ce fut en tout cas un projet particulièrement intéressant qui m'a beaucoup appris.

## Détails techniques 

Le projet se lance avec CMAKE
Pour utiliser ce contenu il faut installer armadillo, dlib est installé automatiquement, et installer intel mkl (penser à modifier les chemins d'accès dans CMakeLists.txt)
Pour le bon fonctionnement de ce projet j'ai du aussi mettre dans le path les liens vers armadillo (à vérifier) et certains dossier de mkl (certain).

## Quelques références

- An introduction to Reinforcement Learning (second edition) : A. SUTTON, G. BARTO 
- https://perso.liris.cnrs.fr/laetitia.matignon/index/5TCIAR/CM3INSA.pdf

un papier très intéressant que je n'ai pas plus exploiter ici (je ne voyais pas comment le faire correspondre, ça briserait la symétrie cf choix DQN) : https://arxiv.org/pdf/1603.01121

et également quelques trucs  :

- https://arxiv.org/pdf/2511.17598
- https://arxiv.org/pdf/1901.00137
- https://arxiv.org/pdf/1312.5602
- https://fse.studenttheses.ub.rug.nl/21526/1/AI_BA_2020_IONUTALEXANDRUTUTEA.pdf
