// ========== FICHIER: main.c ==========

#include <SDL3/SDL.h>
#include "include/Snake.h"
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    // Créer le jeu
    Game *game = game_create();
    if (!game) {
        SDL_Log("Erreur: Impossible de créer le jeu");
        return 1;
    }
    
    SDL_Log("=== SNAKE GAME ===");
    SDL_Log("Jeu lancé avec succès!");
    SDL_Log("Appuyez sur ESPACE pour commencer");
    
    bool running = true;
    SDL_Event event;
    
    // Horloge pour FPS
    Uint64 last_frame_time = SDL_GetTicks();
    float target_fps = 60.0f;
    float target_frame_time = 1000.0f / target_fps; // ms par frame
    
    // Boucle principale du jeu
    while (running) {
        Uint64 frame_start = SDL_GetTicks();
        
        // Traiter les événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                SDL_Log("Fermeture du jeu...");
                running = false;
            } else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                SDL_Log("Fenêtre fermée");
                running = false;
            } else {
                // Transmettre l'événement au jeu
                game_handle_input(game, &event);
            }
        }
        
        // Calculer delta time
        Uint64 current_time = SDL_GetTicks();
        float delta = (float)(current_time - last_frame_time) / 1000.0f;
        last_frame_time = current_time;
        
        // Limiter delta pour éviter les sauts
        if (delta > 0.1f) {
            delta = 0.1f;
        }
        
        // Mettre à jour le jeu
        game_update(game, delta);
        
        // Afficher le jeu
        game_render(game);
        
        // Contrôler la fréquence d'images
        Uint64 frame_end = SDL_GetTicks();
        Uint64 frame_time = frame_end - frame_start;
        
        if (frame_time < (Uint64)target_frame_time) {
            SDL_Delay((Uint32)(target_frame_time - frame_time));
        }
    }
    
    // Nettoyage
    SDL_Log("Nettoyage des ressources...");
    game_destroy(game);
    
    SDL_Log("Jeu fermé avec succès!");
    return 0;
}