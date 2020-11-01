# miniprojetc
Extension Snake Raylib


Ce projet est une extension du jeu Snake de RAYLIB. Il implémente le jeu fourni par RAYLIB en y ajoutant quelques fonctionnalités.

LANCER LE JEU SNAKE

Afin de lancer le projet, il vous suffit de télécharger "Snake.zip", puis d'exécuter le fichier "snake.exe".
Pour les utilisateurs de linux, exécuter le projet via la commande wine.

JOUER AU JEU SNAKE 

Ce jeu est très ancien et très connu de par sa simplicité et sa popularité.
Le joueur guide un serpent sur une map, et le but du jeu est de manger des fruits afin de voir le score et la  taille du serpent augmenter.
Tout ceci sans heurter la queue du serpent ou le mur.

Le joueur fait changer de direction au serpent avec les flèches du clavier, il peut mettre le jeu en pause grâce à la touche [P].

J'ai implémenter de nouvelles fonctionnalités qui sont les suivantes:

- L'apparition d'obstacles : les obstacles font office de mur, si le serpent rencontre un obstacle, il perd la partie (activable/désactivable au choix).
- La fonctionnalité qui permet de traverser les murs : si un serpent heurte un mur, il réapparait de l'autre côté de la map (activable/désactivable au choix).

- Le score obtenu : Chaque fruit donne 1 point de score supplémentaire.
- Meilleur score : le meilleur score obtenu est enregistré dans un fichier que l'on lit à chaque partie.

- L'affichage du score et des informations de la partie pendant celle-ci, à droite de la fenetre de jeu.

- Menu principal : On peut choisir à quel niveau de difficulté on joue, ou personnaliser son niveau de difficulté soi-même.
- Menu de personnalisation : On change les paramètres de jeu à notre guise ( Obstacles, traverser les murs et Vitesse).
- Modification des fonctions d'affichage afin de prendre en compte tout les nouveaux paramètres implantés.

Toute les fonctions ci-dessus sont fonctionnelles.

Et enfin, une fonction d'IA mouvementAuto() qui permet au serpent de se déplacer tout seul, en évitant les obstacles et sa queue(pour le plaisir des yeux).
La fonction n'est pas encore complète, elle est très difficile à implémenter. Cependant j'ai réussi à atteindre le score de 60 avec cette fonction,
c'est dire qu'elle commence à porter ses fruits.
La difficulté principale est de repérer les culs-de-sac, seule cause pour laquelle le serpent perd la partie actuellement. Tout les autres cas sont traités.
Avec plus de temps, une fonction qui permettrai de détecter les culs de sac est envisageable, et elle permettrai donc au snake de ne (presque) jamais mourir.

TOUCHES 

Toute les touches sont encadrés par des [] afin d'indiquer que ce sont des touches de commandes. Exemple pour choisir le niveau de difficulté Facile, on appuie sur [F].

[ESC] : Permet de fermer le jeu.

Menu Principal :
[F] : Niveau de difficulté FACILE
[I] : Niveau de difficulté INTERMEDIAIRE
[D] : Niveau de difficulté DIFFICILE
[P] : Permet de personnaliser son niveau de difficulté
[A] : Lance le mode de jeu Automatique, ou l'on regarde le snake se déplacer tout seul.

Menu de personnalisation :
[O] : Permet d'activer ou de désactiver l'apparition d'obstacle durant la partie.
[T] : Permet d'activer ou de désactiver la traversée des murs.
[<>] Flèches du clavier : Permettent de modifier la vitesse de jeu.
[M] : Retour au menu principal
[ENTREE] : Lancer la partie avec les paramètres choisis

En jeu:
[<>] Flèches du clavier : Déplacement du snake pendant la partie
[P] : Pause
[M] : Arrêt de la partie et retour au menu principal
[A] : Permet de diminuer la vitesse de jeu 
[Z] : Permet d'augmenter la vitesse de jeu (Surtout pour le mode automatique, qui rend le mode satisfaisant).

En fin de partie:
[R] : Permet de relancer la partie
[M] : Retour au menu principal



Fichiers du projet

Le code est contenu dans un seul fichier snake.c qui contient l'intégralité du code.





