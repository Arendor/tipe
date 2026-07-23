

# Changement à faire



## Fix

Inverser target et estimate pour le dqn_policy
Mettre les méthodes const proprement
Supprimer certains commentaires et en ajouter d'autre



### A ajouter

Malus lorsque l'agent remplit trop de frontière 

Créer un fichier learning_config.json pour stocker tous les autes params d'apprentissage (comme ça ce n'est plus écrit dans le dur)
A la création de l'agent, pouvoir modifier le type de reward / config utilisé :)
Definir le lien vers les fichiers dans éventuellement une directive de prépocesseur / fichier config qui puisse être lu facilement
Pouvoir facilement créer de nouveaux tests et en faire à la suite (création de fichier / dossier)

## Différents tests à faire 

Le pllus simple contre agent aléatoire :
-1/ +1 en cas de fronitère perdu gagnée
-10/+10 en cas de partie gagnée / perdue, 0 égalité
facteur de normalisation de 1
CSTEP : 35
Soft update : 0.75
Adam optimiser pas avant diminution du learning rate : 2000

Entrainement : 3500 parties


Vrai agent :
reward voir current
Cstep 35 (peut être reform en ça btw.. genre en nb de parties)
Soft update 0.75
Adam optimiser pas avt diminution du LR ; 2000

Entrainement 7000 parties
Stratégie epsilon greedy : diminution / 
essayer de faire par 1000 parties ? (introduire compteur de partie)
Initialement 10% (0.1) + diminution de 0.0002 toutes les 200 parties ie 0.01 toutes les 1000 parties


