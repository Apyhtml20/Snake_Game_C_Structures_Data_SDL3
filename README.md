# Snake_Game_C_Structures_Data_SDL3
Ce projet est une implémentation du jeu Snake développée en langage C en utilisant la bibliothèque SDL3 et ses extensions (SDL_image, SDL_mixer).
Le jeu propose une interface graphique, des sons, des textures et une gestion complète du gameplay classique de Snake.
Technologies utilisées:
Langage : C
Bibliothèques :
SDL3
SDL3_image
SDL3_mixer
Compilation : GCC + Makefile
Plateforme : Windows (exécutable .exe)
Snake/
│
├── assets/              # Images du jeu
│   ├── background_nature.png
│   ├── food.png
│   ├── snake_body.png
│   └── snake_head.png
│
├── audio/               # Sons et musiques
│   ├── Background.wav
│   ├── eat.wav
│   ├── game_over.wav
│   └── menu.wav
│
├── fonts/               # Polices (si utilisées)
│
├── lib/                 # Bibliothèques externes
│
├── obj/                 # Fichiers objets (.o)
│
├── src/
│   ├── include/
│   │   ├── SDL3/
│   │   ├── SDL3_image/
│   │   └── Snake.h
│   │
│   ├── main.c           # Point d’entrée du programme
│   └── Snake.c          # Logique du jeu
│
├── Makefile             # Script de compilation
├── README.md            # Documentation du projet
├── Snake_Vr3.exe        # Exécutable du jeu

├── SDL3.dll
├── SDL3_image.dll
├── SDL3_ttf.dll
└── SDL2_mixer.dll
Prérequis:
GCC installé
Make installé
SDL3 + SDL_image + SDL_mixer (DLL présentes dans le dossier)
⌨️ Commandes du jeu
Flèches directionnelles : déplacer le serpent
Échap : quitter le jeu
Objectif : manger la nourriture, grandir et éviter les collisions
⚙️ Fonctionnalités:
Déplacement fluide du serpent
Détection des collisions
Gestion du score
Sons (manger, game over, musique)
Textures graphiques
Architecture modulaire (header + source)
Améliorations possibles :
Menu interactif
Sauvegarde du meilleur score
Niveaux de difficulté
Mode pause
Portage Linux
POUR INFO======>>>>>>[Snake_Game_Ab.pdf](https://github.com/user-attachments/files/24361982/Snake_Game_Ab.pdf)

