# 🐍 Snake Game By GINF3

![Snake Game](https://img.shields.io/badge/Version-2.0-brightgreen)
![SDL3](https://img.shields.io/badge/SDL-3.0-blue)
![C Language](https://img.shields.io/badge/Language-C-orange)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)

## 🎮 Description

**Snake Game Ultimate** est une implémentation moderne et complète du jeu classique Snake, développée en langage C avec la bibliothèque SDL3 et ses extensions (SDL_image, SDL_mixer, SDL_ttf). 

Ce projet propose une expérience de jeu enrichie avec :
- ✨ Interface graphique moderne avec effets néon et particules
- 🎵 Système audio complet (musique de fond et effets sonores)
- 🎨 Textures personnalisées et animations fluides
- 🏆 Système de scores et highscores
- 🎯 Multiples modes de jeu et niveaux de difficulté
- ⚡ Power-ups 
- 🌟 Effets visuels spectaculaires (particules, explosions, trails)

---

## 🎯 Fonctionnalités Principales

### 🕹️ 5 Modes de Jeu

1. **Mode Classique** 🎮
   - Le Snake traditionnel revisité
   - Augmentation progressive de la vitesse
   - Système de scoring classique

2. **Mode Vitesse** ⚡
   - Vitesse augmente constamment
   - Challenge intense pour joueurs expérimentés
   - Bonus de score multipliés

3. **Mode Zen** 🧘
   - Mode relaxant sans game over
   - Parfait pour se détendre
   - Focus sur le score

### ⚙️ 4 Niveaux de Difficulté

- 🟢 **Facile** : Vitesse lente, idéal pour débuter
- 🟡 **Normal** : Équilibré et amusant
- 🟠 **Difficile** : Pour joueurs confirmés
- 🔴 **Extrême** : Challenge ultime !

### 🎁 Power-ups Collectibles

| Power-up | Effet | Durée | Couleur |
|----------|-------|-------|---------|
| ⚡ Speed Boost | Double la vitesse | 5s | 🔴 Rouge |
| 🕐 Slow Motion | Ralentit le temps | 5s | 🔵 Bleu |
| 🛡️ Invincibilité | Traverse murs et soi-même | 5s | 🟡 Jaune |
| 🧲 Aimant | Attire la nourriture | 10s | 🟢 Vert |
| 💰 Double Points | Score multiplié par 2 | 8s | 🟣 Violet |

### 🌟 Effets Visuels Avancés

- 💥 **Particules explosives** : Effets spectaculaires lors de collecte
- ✨ **Trail lumineux** : Trainée derrière le serpent
- 📳 **Screen shake** : Vibrations lors d'impacts
- 🌈 **Animations fluides** : Interpolation smooth des mouvements
- 🎨 **Effets néon** : Interface cyberpunk stylisée
- ⭐ **Starfield animé** : Fond étoilé dynamique dans les menus

### 🎵 Système Audio Complet

#### Musique
- 🎼 Musique de fond dynamique (Background.wav)
- 🔊 Contrôle du volume en temps réel
- 🔁 Boucle infinie automatique

#### Effets Sonores
- 🍎 Son de collecte de nourriture (eat.wav)
- 💀 Son de game over (game_over.wav)
- 🎯 Son de menu (menu.wav)
- 🎁 Son de bonus (bonus.wav)

### 🏆 Système de Scores

- 📊 **Highscore persistant** : Sauvegarde automatique
- 🥇 **Top 10** : Tableau des meilleurs scores
- 📈 **Progression par niveau** : Barre de progression visuelle
- 💎 **Différent types de nourriture** :
  - 🍎 Normale : +10 points

### 🎨 Interface Utilisateur

#### Menu Principal
- 🎮 **JOUER** : Lance une nouvelle partie
- ⚙️ **OPTIONS** : Paramètres du jeu
- 🚪 **QUITTER** : Ferme le jeu

#### Menu Options
- 🔊 Volume musique (slider)
- 🔉 Volume effets sonores (slider)
- 📏 Afficher/masquer la grille
- 🎯 Sélection de la difficulté

---

## 🛠️ Technologies Utilisées

### Langage
- **C** (Standard C11)
- Compilation avec **GCC** (MinGW sur Windows)

### Bibliothèques SDL3
- **SDL3** : Gestion fenêtre, rendu, événements
- **SDL3_image** : Chargement textures (PNG, JPG)
- **SDL3_ttf** : Rendu de texte avec polices TrueType
- **SDL3_mixer** : Système audio (musique et sons)

### Outils
- **Makefile** : Compilation automatisée
- **GCC** : Compilateur C
- **Git** : Gestion de version

---

## 📁 Structure du Projet

```
Snake_Game_Ultimate/
│
├── 📄 Snake.c                    # Code source principal
├── 📄 include/Snake.h            # Fichier header avec déclarations
├── 📄 Makefile                   # Script de compilation
├── 📄 README.md                  # Ce fichier
│
├── 📁 fonts/                     # Polices de caractères
│   ├── Blackfang.ttf            # Police titre (effet gaming)
│   └── Kids_avenue.ttf          # Police interface
│
├── 📁 audio/                     # Fichiers audio
│   ├── Background.wav           # Musique de fond
│   ├── eat.wav                  # Son collecte nourriture
│   ├── game_over.wav            # Son game over
│   ├── menu.wav                 # Son navigation menu
│   ├── bonus.wav                # Son bonus
│   ├── powerup.wav              # Son power-up
│   └── combo.wav                # Son combo
│
├── 📁 assets/                    # Ressources graphiques
│   ├── background_nature.png    # Fond de jeu
│   ├── snake_head.png           # Tête du serpent
│   ├── snake_body.png           # Corps du serpent
│   ├── food.png                 # Nourriture normale
│         
│  
│
└── 📁 bin/                       # Exécutables compilés
    └── Snake_Game.exe           # Jeu Windows
```

---

## 🚀 Installation et Compilation

### Prérequis

#### Windows (MinGW)
```bash
# Installer MinGW avec GCC
# Télécharger SDL3 development libraries depuis:
# https://github.com/libsdl-org/SDL/releases

# Placer les DLL SDL3 dans le dossier du projet
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev libsdl3-mixer-dev
```

### Compilation

#### Avec Makefile
```bash
make
```

#### Compilation manuelle
```bash
gcc -o Snake_Game Snake.c -lSDL3 -lSDL3_image -lSDL3_ttf -lSDL3_mixer -lm
```

### Exécution
```bash
./Snake_Game.exe        # Windows
./Snake_Game            # Linux
```

---

## 🎮 Contrôles du Jeu

### En Jeu
| Touche | Action |
|--------|--------|
| ⬆️ Flèche Haut | Aller vers le haut |
| ⬇️ Flèche Bas | Aller vers le bas |
| ⬅️ Flèche Gauche | Aller vers la gauche |
| ➡️ Flèche Droite | Aller vers la droite |
| `ESPACE` / `P` | Mettre en pause |
| `M` | Activer/Désactiver musique |
| `ESC` | Retour au menu |

### Menu / Game Over
| Touche | Action |
|--------|--------|
| `Clic Souris` | Sélectionner option |
| `ESPACE` / `ENTRÉE` | Jouer |
| `R` | Recommencer (Game Over) |
| `ESC` | Retour menu / Quitter |

---

## 📊 Système de Scoring

### Points par Nourriture
- 🍎 **Nourriture normale** : +10 points
- 🌟 **Nourriture dorée** : +50 points (rare)
- 🎁 **Bonus** : +25 points
### Bonus de Vitesse
- Chaque augmentation de vitesse : +5 points bonus
- Level up : +100 points

---

## 🏆 Tableau des Highscores

Le jeu sauvegarde automatiquement les 10 meilleurs scores dans `highscores.dat`.

**Format d'affichage :**
```
Rang | Nom           | Score  | Mode      | Difficulté
-----|---------------|--------|-----------|------------
1.   | CHAMPION      | 9999   | Speed     | Extrême
2.   | PRO_GAMER     | 8520   | Survie    | Difficile
3.   | SNAKE_MASTER  | 7300   | Obstacles | Normal
```

---

## 🎨 Personnalisation

### Modifier les Couleurs
Dans `Snake.c`, section couleurs :
```c
SDL_Color snake_color = {255, 100, 200, 255};  // Rose néon
SDL_Color food_color = {255, 255, 0, 255};      // Jaune
SDL_Color bg_color = {10, 15, 30, 255};         // Bleu foncé
```

### Ajuster la Difficulté
Dans `Snake.h` :
```c
#define INITIAL_SPEED 150        // Vitesse initiale (ms)
#define SPEED_INCREMENT 5        // Augmentation par fruit
#define MIN_SPEED 50             // Vitesse maximale
```

### Taille de la Grille
```c
#define GRID_WIDTH 30            // Largeur grille
#define GRID_HEIGHT 24           // Hauteur grille
#define CELL_SIZE 25             // Taille cellule (pixels)
```

---

## 🐛 Dépannage

### Le jeu ne se lance pas
- ✅ Vérifier que toutes les DLL SDL3 sont présentes
- ✅ Vérifier les chemins des fichiers audio et fonts
- ✅ Compiler avec les bonnes options de linkage

### Pas de son
- ✅ Vérifier que les fichiers .wav sont dans `audio/`
- ✅ Appuyer sur `M` pour activer la musique
- ✅ Vérifier le volume système

### Textures manquantes
- ✅ Placer les images dans le dossier `assets/`
- ✅ Le jeu fonctionne sans textures (fallback sur rectangles)

### Performance lente
- ✅ Réduire le nombre de particules dans le code
- ✅ Désactiver les effets visuels dans Options
- ✅ Réduire la résolution de la fenêtre

---

## 📝 Crédits

### 👨‍💻 Développement
- **Hicham Abadour**
- **Zakaria Zahraoui**
- **Ismael Demnati**


### 🎓 Institution
- Projet académique de structures de données en C
- Université : ENSAO
- Année : 2025-2026

### 🎨 Ressources
- Bibliothèque SDL3 : [libsdl.org](https://www.libsdl.org/)
- Police Blackfang : Font gaming
- Police Kids Avenue : Font interface

---

## 📜 Licence

Ce projet est développé dans un cadre éducatif.

```
Copyright (c) 2025 Hicham Abadour, Zakaria Zahraoui, Ismael Demnati
Projet académique - Structures de données en C
Supervisé par Pr Rahmoune
```

---

## 🌟 Remerciements

Un grand merci à :
- 🙏 **Pr Rahmoune** pour son encadrement
- 💻 La communauté **SDL** pour leurs excellentes bibliothèques
- 📚 Les ressources open-source qui ont inspiré ce projet

---

<div align="center">

[![GitHub Stars](https://img.shields.io/github/stars/votre-username/snake-game-ultimate?style=social)](https://github.com/votre-username/snake-game-ultimate)
[![GitHub Forks](https://img.shields.io/github/forks/votre-username/snake-game-ultimate?style=social)](https://github.com/votre-username/snake-game-ultimate)

</div>
