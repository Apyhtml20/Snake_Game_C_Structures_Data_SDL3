// ========== FICHIER: Snake.c - VERSION COMPLÈTE FINALE GAMING ==========

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_audio.h>
#include "include/Snake.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// ========== FONCTIONS AUDIO SDL3 ==========

static bool load_wav_sound(const char *filepath, SoundEffect *sound) {
    if (!SDL_LoadWAV(filepath, &sound->spec, &sound->buffer, &sound->length)) {
        SDL_Log("Erreur chargement WAV %s: %s", filepath, SDL_GetError());
        return false;
    }
    return true;
}

static bool load_audio_file(const char *filepath, Uint8 **data, Uint32 *length, SDL_AudioSpec *spec) {
    Uint8 *audio_buf;
    Uint32 audio_len;
    SDL_AudioSpec audio_spec;
    
    if (!SDL_LoadWAV(filepath, &audio_spec, &audio_buf, &audio_len)) {
        SDL_Log("Erreur chargement audio %s: %s", filepath, SDL_GetError());
        return false;
    }
    
    *data = audio_buf;
    *length = audio_len;
    *spec = audio_spec;
    return true;
}

bool game_load_audio(Game *game) {
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        SDL_Log("Erreur init audio: %s", SDL_GetError());
        game->audio_enabled = false;
        return false;
    }
    
    SDL_AudioSpec desired_spec;
    SDL_zero(desired_spec);
    desired_spec.freq = 44100;
    desired_spec.format = SDL_AUDIO_S16;
    desired_spec.channels = 2;
    
    game->audio.music_stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &desired_spec,
        NULL, NULL
    );
    
    if (!game->audio.music_stream) {
        SDL_Log("Erreur création stream musique: %s", SDL_GetError());
        game->audio_enabled = false;
        return false;
    }
    
    game->audio.sfx_stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &desired_spec,
        NULL, NULL
    );
    
    if (!game->audio.sfx_stream) {
        SDL_Log("Erreur création stream SFX: %s", SDL_GetError());
        SDL_DestroyAudioStream(game->audio.music_stream);
        game->audio_enabled = false;
        return false;
    }
    
    if (load_audio_file("audio/Background.wav",  &game->audio.music_data,  &game->audio.music_length, &desired_spec)) {
        SDL_Log("Musique Background.wav chargee");
    } else {
        SDL_Log("Avertissement: Background.wav non chargee");
        game->audio.music_data = NULL;
    }
    
    if (load_wav_sound("audio/eat.wav", &game->audio.eat_sound)) {
        SDL_Log("Son eat.wav chargé");
    }
    
    if (load_wav_sound("audio/game_over.wav", &game->audio.game_over_sound)) {
        SDL_Log("Son game_over.wav chargé");
    }
    
    if (load_wav_sound("audio/menu.wav", &game->audio.menu_sound)) {
        SDL_Log("Son menu.wav chargé");
    }
    
    game->audio.music_volume = 0.5f;
    game->audio.sfx_volume = 1.0f;
    game->audio.music_playing = false;
    game->audio_enabled = true;
    
    SDL_Log("Audio chargé avec succès!");
    return true;
}

void game_free_audio(Game *game) {
    if (game->audio.music_stream) {
        SDL_DestroyAudioStream(game->audio.music_stream);
        game->audio.music_stream = NULL;
    }
    
    if (game->audio.sfx_stream) {
        SDL_DestroyAudioStream(game->audio.sfx_stream);
        game->audio.sfx_stream = NULL;
    }
    
    if (game->audio.music_data) {
        SDL_free(game->audio.music_data);
        game->audio.music_data = NULL;
    }
    
    if (game->audio.eat_sound.buffer) {
        SDL_free(game->audio.eat_sound.buffer);
    }
    
    if (game->audio.game_over_sound.buffer) {
        SDL_free(game->audio.game_over_sound.buffer);
    }
    
    if (game->audio.menu_sound.buffer) {
        SDL_free(game->audio.menu_sound.buffer);
    }
    
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void game_play_music(Game *game) {
    if (!game->audio_enabled || !game->audio.music_data || !game->audio.music_stream) {
        return;
    }
    
    if (!game->audio.music_playing) {
        if (SDL_PutAudioStreamData(game->audio.music_stream, 
                                    game->audio.music_data, 
                                    game->audio.music_length)) {
            SDL_ResumeAudioStreamDevice(game->audio.music_stream);
            game->audio.music_playing = true;
            SDL_Log("Musique démarrée");
        }
    }
}

void game_stop_music(Game *game) {
    if (!game->audio_enabled || !game->audio.music_stream) return;
    
    SDL_PauseAudioStreamDevice(game->audio.music_stream);
    SDL_ClearAudioStream(game->audio.music_stream);
    game->audio.music_playing = false;
}

static void play_sound_effect(Game *game, SoundEffect *sound) {
    if (!game->audio_enabled || !sound->buffer || !game->audio.sfx_stream) {
        return;
    }
    
    SDL_ClearAudioStream(game->audio.sfx_stream);
    SDL_PutAudioStreamData(game->audio.sfx_stream, sound->buffer, sound->length);
    SDL_ResumeAudioStreamDevice(game->audio.sfx_stream);
}

void game_play_eat_sound(Game *game) {
    play_sound_effect(game, &game->audio.eat_sound);
}

void game_play_game_over_sound(Game *game) {
    play_sound_effect(game, &game->audio.game_over_sound);
}

void game_play_menu_sound(Game *game) {
    play_sound_effect(game, &game->audio.menu_sound);
}

void game_play_bonus_sound(Game *game) {
    play_sound_effect(game, &game->audio.bonus_sound);
}

// ========== FONCTIONS DU SERPENT ==========

Snake* snake_create(int start_x, int start_y) {
    Snake *snake = malloc(sizeof(Snake));
    snake->head = malloc(sizeof(SnakeSegment));
    snake->head->x = start_x;
    snake->head->y = start_y;
    snake->head->next = NULL;
    snake->tail = snake->head;
    snake->direction = DIR_RIGHT;
    snake->next_direction = DIR_RIGHT;
    snake->length = 1;
    
    snake_add_segment(snake, start_x - 1, start_y);
    snake_add_segment(snake, start_x - 2, start_y);

    return snake;
}

void snake_destroy(Snake *snake) {
    SnakeSegment *current = snake->head;
    while (current != NULL) {
        SnakeSegment *temp = current;
        current = current->next;
        free(temp);
    }
    free(snake);
}

void snake_add_segment(Snake *snake, int x, int y) {
    SnakeSegment *segment = malloc(sizeof(SnakeSegment));
    segment->x = x;
    segment->y = y;
    segment->next = NULL;
    
    if (snake->head == NULL) {
        snake->head = segment;
        snake->tail = segment;
    } else {
        snake->tail->next = segment;
        snake->tail = segment;
    }
    snake->length++;
}

void snake_move(Snake *snake) {
    snake->direction = snake->next_direction;
    
    int new_x = snake->head->x;
    int new_y = snake->head->y;
    
    switch (snake->direction) {
        case DIR_UP:    new_y--; break;
        case DIR_DOWN:  new_y++; break;
        case DIR_LEFT:  new_x--; break;
        case DIR_RIGHT: new_x++; break;
    }
    
    SnakeSegment *new_head = malloc(sizeof(SnakeSegment));
    new_head->x = new_x;
    new_head->y = new_y;
    new_head->next = snake->head;
    snake->head = new_head;
    
    SnakeSegment *current = snake->head;
    SnakeSegment *prev = NULL;
    
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }
    
    if (prev != NULL) {
        prev->next = NULL;
        free(current);
        snake->tail = prev;
    }
}

bool snake_check_collision(Snake *snake) {
    return (snake->head->x < 0 || snake->head->x >= GRID_WIDTH ||
            snake->head->y < 0 || snake->head->y >= GRID_HEIGHT);
}

bool snake_check_self_collision(Snake *snake) {
    SnakeSegment *current = snake->head->next;
    while (current != NULL) {
        if (current->x == snake->head->x && current->y == snake->head->y) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void snake_update_display_positions(Snake *snake, float delta) {
}

// ========== FONCTIONS DE LA NOURRITURE ==========

void food_spawn(Food *food, Snake *snake, int type) {
    bool valid = false;
    while (!valid) {
        food->x = rand() % GRID_WIDTH;
        food->y = rand() % GRID_HEIGHT;
        
        valid = true;
        SnakeSegment *current = snake->head;
        while (current != NULL) {
            if (current->x == food->x && current->y == food->y) {
                valid = false;
                break;
            }
            current = current->next;
        }
    }
    food->active = true;
    food->type = type;
}

bool food_check_collision(Food *food, Snake *snake) {
    return (food->x == snake->head->x && food->y == snake->head->y);
}

// ========== FONCTIONS DES PARTICULES ==========

void particle_add(Game *game, float x, float y, SDL_Color color, ParticleType type) {
    Particle *p = malloc(sizeof(Particle));
    p->x = x;
    p->y = y;
    
    // Vitesse aléatoire pour effet explosion
    float angle = (rand() % 360) * 3.14159f / 180.0f;
    float speed = (rand() % 150 + 100) / 100.0f;
    p->vx = cos(angle) * speed * 100.0f;
    p->vy = sin(angle) * speed * 100.0f;
    
    p->lifetime = 1.2f;
    p->max_lifetime = 1.2f;
    p->color = color;
    p->type = type;
    p->next = game->particles;
    game->particles = p;
}

void particle_update(Game *game, float delta) {
    Particle *current = game->particles;
    Particle *prev = NULL;
    
    while (current) {
        current->lifetime -= delta;
        current->x += current->vx * delta;
        current->y += current->vy * delta;
        
        if (current->lifetime <= 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                game->particles = current->next;
            }
            Particle *temp = current;
            current = current->next;
            free(temp);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void particle_render(Game *game) {
    Particle *current = game->particles;
    while (current) {
        float alpha_ratio = current->lifetime / current->max_lifetime;
        SDL_Color c = current->color;
        c.a = (Uint8)(255 * alpha_ratio);
        
        // Taille variable pour effet fleurissant
        float size = 6.0f * alpha_ratio + 2.0f;
        
        SDL_SetRenderDrawColor(game->renderer, c.r, c.g, c.b, c.a);
        SDL_FRect rect = {current->x - size/2, current->y - size/2, size, size};
        SDL_RenderFillRect(game->renderer, &rect);
        
        current = current->next;
    }
}

void particle_clear_all(Game *game) {
    while (game->particles) {
        Particle *temp = game->particles;
        game->particles = game->particles->next;
        free(temp);
    }
}

// ========== MENU GAMING EFFECTS ==========

void menu_init_starfield(MenuEffects *effects) {
    for (int i = 0; i < 50; i++) {
        effects->stars[i][0] = rand() % WINDOW_WIDTH;
        effects->stars[i][1] = rand() % WINDOW_HEIGHT;
    }
}

void menu_update_particles(MenuEffects *effects, float delta) {
    for (int i = 0; i < effects->particle_count; i++) {
        MenuParticle *p = &effects->particles[i];
        p->lifetime -= delta;
        
        if (p->lifetime <= 0) {
            effects->particles[i] = effects->particles[--effects->particle_count];
            i--;
            continue;
        }
        
        p->x += p->vx * delta;
        p->y += p->vy * delta;
    }
    
    if (effects->particle_count < 100 && rand() % 10 < 3) {
        MenuParticle *p = &effects->particles[effects->particle_count++];
        p->x = rand() % WINDOW_WIDTH;
        p->y = -10;
        p->vx = (rand() % 100 - 50) / 100.0f;
        p->vy = (rand() % 100 + 50) / 100.0f;
        p->lifetime = 5.0f;
        p->max_lifetime = 5.0f;
        p->color = (SDL_Color){0, 200 + rand() % 55, 255, 200};
    }
}

void menu_render_particles(SDL_Renderer *renderer, MenuEffects *effects) {
    for (int i = 0; i < effects->particle_count; i++) {
        MenuParticle *p = &effects->particles[i];
        float alpha_ratio = p->lifetime / p->max_lifetime;
        p->color.a = (Uint8)(255 * alpha_ratio);
        
        SDL_SetRenderDrawColor(renderer, p->color.r, p->color.g, p->color.b, p->color.a);
        SDL_FRect rect = {p->x, p->y, 3, 3};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void menu_render_starfield(SDL_Renderer *renderer, MenuEffects *effects, float time) {
    for (int i = 0; i < 50; i++) {
        float x = effects->stars[i][0];
        float y = effects->stars[i][1];
        
        float twinkle = (sin(time * 3.0f + i) + 1.0f) / 2.0f;
        Uint8 brightness = (Uint8)(100 * twinkle + 55);
        
        SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);
        SDL_FRect star = {x, y, 2, 2};
        SDL_RenderFillRect(renderer, &star);
    }
}

void menu_draw_gradient_bg(SDL_Renderer *renderer) {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        float ratio = (float)y / WINDOW_HEIGHT;
        
        Uint8 r = (Uint8)(15 + ratio * 10);
        Uint8 g = (Uint8)(40 + ratio * 50);
        Uint8 b = (Uint8)(80 + ratio * 100);
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderLine(renderer, 0, y, WINDOW_WIDTH, y);
    }
}

void menu_render_scanlines(SDL_Renderer *renderer, MenuEffects *effects) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 30);
    for (int y = 0; y < WINDOW_HEIGHT; y += 2) {
        SDL_RenderLine(renderer, 0, y, WINDOW_WIDTH, y);
    }
}

void menu_draw_neon_button(SDL_Renderer *renderer, Button *btn, 
                           TTF_Font *font, float glow_intensity) {
    
    if (btn->is_hovered) {
        SDL_SetRenderDrawColor(renderer, 0, 200, 255, (Uint8)(50 * glow_intensity));
        for (int i = 5; i > 0; i--) {
            SDL_FRect glow = {
                btn->rect.x - i,
                btn->rect.y - i,
                btn->rect.w + i * 2,
                btn->rect.h + i * 2
            };
            SDL_SetRenderDrawColor(renderer, 0, 200, 255, (Uint8)(20 * glow_intensity / i));
            SDL_RenderRect(renderer, &glow);
        }
    }
    
    SDL_Color border_color = btn->is_hovered ? 
        (SDL_Color){0, 255, 255, 255} : 
        (SDL_Color){0, 150, 200, 255};
    
    SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, 255);
    SDL_RenderRect(renderer, &btn->rect);
    
    SDL_FRect inner = {
        btn->rect.x + 2,
        btn->rect.y + 2,
        btn->rect.w - 4,
        btn->rect.h - 4
    };
    SDL_SetRenderDrawColor(renderer, border_color.r / 3, border_color.g / 2, 100, 150);
    SDL_RenderRect(renderer, &inner);
    
    SDL_FRect fill = {
        btn->rect.x + 1,
        btn->rect.y + 1,
        btn->rect.w - 2,
        btn->rect.h - 2
    };
    SDL_SetRenderDrawColor(renderer, 0, 100, 150, btn->is_hovered ? 60 : 30);
    SDL_RenderFillRect(renderer, &fill);
    
    size_t text_len = strlen(btn->text);
    SDL_Surface *surface = TTF_RenderText_Solid(font, btn->text, text_len, border_color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        
        SDL_FRect text_rect = {
            btn->rect.x + (btn->rect.w - surface->w) / 2,
            btn->rect.y + (btn->rect.h - surface->h) / 2,
            surface->w,
            surface->h
        };
        
        SDL_RenderTexture(renderer, texture, NULL, &text_rect);
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
}

// ========== TITRE FLEURISSANT GAMING ==========

void menu_render_title(SDL_Renderer *renderer, TTF_Font *font, float pulse) {
    const char *title = " SNAKE GAME ";
    
    float pulse_val = sin(pulse * 3.14159f);
    Uint8 brightness = (Uint8)(200 + 55 * pulse_val);
    Uint8 magenta = (Uint8)(100 + 155 * pulse_val);
    
    SDL_Color title_color = {magenta, brightness, 255, 255};
    
    size_t title_len = strlen(title);
    
    // Ombre violet
    SDL_Surface *shadow_surf = TTF_RenderText_Solid(font, title, title_len,
        (SDL_Color){150, 50, 200, 150});
    if (shadow_surf) {
        SDL_Texture *shadow_tex = SDL_CreateTextureFromSurface(renderer, shadow_surf);
        SDL_FRect shadow_rect = {
            (WINDOW_WIDTH - shadow_surf->w) / 2 + 4,
            80 + 4,
            shadow_surf->w,
            shadow_surf->h
        };
        SDL_RenderTexture(renderer, shadow_tex, NULL, &shadow_rect);
        SDL_DestroyTexture(shadow_tex);
        SDL_DestroySurface(shadow_surf);
    }
    
    // Texte principal rose/magenta
    SDL_Surface *surface = TTF_RenderText_Solid(font, title, title_len, title_color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FRect rect = {
            (WINDOW_WIDTH - surface->w) / 2,
            80,
            surface->w,
            surface->h
        };
        SDL_RenderTexture(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
}

// ========== SOUS-TITRE AVEC CRÉDITS ==========

void menu_render_subtitle(SDL_Renderer *renderer, TTF_Font *font_small, float time) {
    const char *subtitle = "Realise par Hicham Abadour / Zakaria Zahraoui / Ismael Demnati";
    const char *prof = "Professeur : Pr Rahmoune";
    
    float fade = (sin(time * 2.0f) + 1.0f) / 2.0f;
    
    SDL_Color credits_color = {100, 200, 255, (Uint8)(180 * fade)};
    SDL_Color prof_color = {0, 255, 200, (Uint8)(150 * fade)};
    
    size_t subtitle_len = strlen(subtitle);
    SDL_Surface *surface1 = TTF_RenderText_Solid(font_small, subtitle, subtitle_len, credits_color);
    if (surface1) {
        SDL_Texture *texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
        SDL_FRect rect1 = {
            (WINDOW_WIDTH - surface1->w) / 2,
            WINDOW_HEIGHT - 130,
            surface1->w,
            surface1->h
        };
        SDL_RenderTexture(renderer, texture1, NULL, &rect1);
        SDL_DestroyTexture(texture1);
        SDL_DestroySurface(surface1);
    }
    
    size_t prof_len = strlen(prof);
    SDL_Surface *surface2 = TTF_RenderText_Solid(font_small, prof, prof_len, prof_color);
    if (surface2) {
        SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
        SDL_FRect rect2 = {
            (WINDOW_WIDTH - surface2->w) / 2,
            WINDOW_HEIGHT - 100,
            surface2->w,
            surface2->h
        };
        SDL_RenderTexture(renderer, texture2, NULL, &rect2);
        SDL_DestroyTexture(texture2);
        SDL_DestroySurface(surface2);
    }
}

void menu_render_animated_line(SDL_Renderer *renderer, int y, float time) {
    float wave_height = 5.0f;
    for (int x = 0; x < WINDOW_WIDTH - 1; x++) {
        float offset = sin((x + time * 100) / 50.0f) * wave_height;
        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 150);
        SDL_RenderLine(renderer, x, (int)(y + offset), x + 1, 
                      (int)(y + sin((x + 1 + time * 100) / 50.0f) * wave_height));
    }
}

// ========== BOUTONS ==========

Button button_create(float x, float y, float w, float h, const char *text) {
    Button btn;
    btn.rect = (SDL_FRect){x, y, w, h};
    btn.text = text;
    btn.normal_color = (SDL_Color){0, 150, 200, 255};
    btn.hover_color = (SDL_Color){0, 255, 255, 255};
    btn.text_color = (SDL_Color){255, 255, 255, 255};
    btn.is_hovered = false;
    btn.is_pressed = false;
    return btn;
}

void button_update(Button *btn, int mouse_x, int mouse_y) {
    btn->is_hovered = (mouse_x >= btn->rect.x && mouse_x <= btn->rect.x + btn->rect.w &&
                       mouse_y >= btn->rect.y && mouse_y <= btn->rect.y + btn->rect.h);
}

bool button_is_clicked(Button *btn, SDL_Event *event) {
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && btn->is_hovered) {
        return true;
    }
    return false;
}

void button_render(Game *game, Button *btn) {
    menu_draw_neon_button(game->renderer, btn, game->font_medium, 
                         1.0f + 0.3f * sin(SDL_GetTicks() / 1000.0f * 3.0f));
}

// ========== JEU PRINCIPAL ==========

Game* game_create(void) {
    srand(time(NULL));
    
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Erreur SDL_Init: %s", SDL_GetError());
        return NULL;
    }
    
    if (!TTF_Init()) {
        SDL_Log("Erreur TTF_Init: %s", SDL_GetError());
        SDL_Quit();
        return NULL;
    }
    
    Game *game = malloc(sizeof(Game));
    
    game->window = SDL_CreateWindow("Snake Game - Pr Rahmoune",
                                    WINDOW_WIDTH, WINDOW_HEIGHT,
                                    SDL_WINDOW_RESIZABLE);
    if (!game->window) {
        SDL_Log("Erreur création fenêtre: %s", SDL_GetError());
        free(game);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    game->renderer = SDL_CreateRenderer(game->window, NULL);
    if (!game->renderer) {
        SDL_Log("Erreur création renderer: %s", SDL_GetError());
        SDL_DestroyWindow(game->window);
        free(game);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    game->font_large = TTF_OpenFont("fonts/Blackfang.ttf", 64);
    game->font_medium = TTF_OpenFont("fonts/Kids_avenue.ttf", 32);
    game->font_small = TTF_OpenFont("fonts/Kids_avenue.ttf", 20);
    
    if (!game->font_large) {
        game->font_large = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 64);
    }
    if (!game->font_medium) {
        game->font_medium = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 32);
    }
    if (!game->font_small) {
        game->font_small = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 20);
    }
    
    if (!game_load_textures(game)) {
        SDL_Log("Erreur: Impossible de charger les textures");
        game_destroy(game);
        return NULL;
    }
    
    game_load_audio(game);
    
    menu_init_starfield(&game->menu_effects);
    game->menu_effects.particle_count = 0;
    
    game->btn_play = button_create(WINDOW_WIDTH / 2 - 100, 300, 200, 50, "JOUER");
    game->btn_settings = button_create(WINDOW_WIDTH / 2 - 100, 370, 200, 50, "OPTIONS");
    game->btn_quit = button_create(WINDOW_WIDTH / 2 - 100, 440, 200, 50, "QUITTER");
    
    game->snake = snake_create(GRID_WIDTH / 2, GRID_HEIGHT / 2);
    game->state = GAME_MENU;
    game->score = 0;
    game->high_score = 0;
    game->speed = 200;  // Vitesse augmentée (plus lent)
    game->last_update = SDL_GetTicks();
    game->animation_time = 0;
    game->particles = NULL;
    
    food_spawn(&game->food, game->snake, 0);
    
    // Jouer la musique du menu au démarrage
    game_play_music(game);
    
    SDL_Log("Jeu créé avec succès!");
    return game;
}

void game_destroy(Game *game) {
    if (game->font_large) TTF_CloseFont(game->font_large);
    if (game->font_medium) TTF_CloseFont(game->font_medium);
    if (game->font_small) TTF_CloseFont(game->font_small);
    game_free_textures(game);
    game_free_audio(game);
    particle_clear_all(game);
    snake_destroy(game->snake);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    free(game);
    TTF_Quit();
    SDL_Quit();
}

void game_reset(Game *game) {
    snake_destroy(game->snake);
    game->snake = snake_create(GRID_WIDTH / 2, GRID_HEIGHT / 2);
    game->score = 0;
    game->speed = INITIAL_SPEED;
    food_spawn(&game->food, game->snake, 0);
    game->state = GAME_PLAYING;
    game_play_music(game);
}

void game_handle_input(Game *game, SDL_Event *event) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_Keycode key = event->key.key;
        
        if (game->state == GAME_MENU) {
            if (key == SDLK_SPACE || key == SDLK_RETURN) {
                game_play_menu_sound(game);
                game_reset(game);
            } else if (key == SDLK_ESCAPE) {
                SDL_Event quit_event;
                quit_event.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&quit_event);
            } else if (key == SDLK_M) {
                // Toggle musique au menu
                if (game->audio.music_playing) {
                    game_stop_music(game);
                } else {
                    game_play_music(game);
                }
            }
        } else if (game->state == GAME_PLAYING) {
            Direction current = game->snake->direction;
            
            if (key == SDLK_UP && current != DIR_DOWN) {
                game->snake->next_direction = DIR_UP;
            } else if (key == SDLK_DOWN && current != DIR_UP) {
                game->snake->next_direction = DIR_DOWN;
            } else if (key == SDLK_LEFT && current != DIR_RIGHT) {
                game->snake->next_direction = DIR_LEFT;
            } else if (key == SDLK_RIGHT && current != DIR_LEFT) {
                game->snake->next_direction = DIR_RIGHT;
            } else if (key == SDLK_P || key == SDLK_SPACE) {
                game->state = GAME_PAUSED;
            } else if (key == SDLK_ESCAPE) {
                game->state = GAME_MENU;
                game_stop_music(game);
            } else if (key == SDLK_M) {
                // Toggle musique en jeu
                if (game->audio.music_playing) {
                    game_stop_music(game);
                } else {
                    game_play_music(game);
                }
            }
        } else if (game->state == GAME_PAUSED) {
            if (key == SDLK_P || key == SDLK_SPACE) {
                game->state = GAME_PLAYING;
            } else if (key == SDLK_ESCAPE) {
                game->state = GAME_MENU;
                game_stop_music(game);
            } else if (key == SDLK_M) {
                // Toggle musique en pause
                if (game->audio.music_playing) {
                    game_stop_music(game);
                } else {
                    game_play_music(game);
                }
            }
        } else if (game->state == GAME_OVER) {
            if (key == SDLK_R) {
                game_reset(game);
            } else if (key == SDLK_ESCAPE) {
                game->state = GAME_MENU;
            } else if (key == SDLK_M) {
                // Toggle musique game over
                if (game->audio.music_playing) {
                    game_stop_music(game);
                } else {
                    game_play_music(game);
                }
            }
        }
    } else if (event->type == SDL_EVENT_MOUSE_MOTION) {
        game->mouse_x = event->motion.x;
        game->mouse_y = event->motion.y;
        
        if (game->state == GAME_MENU) {
            button_update(&game->btn_play, game->mouse_x, game->mouse_y);
            button_update(&game->btn_settings, game->mouse_x, game->mouse_y);
            button_update(&game->btn_quit, game->mouse_x, game->mouse_y);
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (game->state == GAME_MENU) {
            if (button_is_clicked(&game->btn_play, event)) {
                game_play_menu_sound(game);
                game_reset(game);
            } else if (button_is_clicked(&game->btn_quit, event)) {
                SDL_Event quit_event;
                quit_event.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&quit_event);
            }
        }
    }
}

void game_update(Game *game, float delta) {
    game->animation_time += delta;
    
    if (game->state == GAME_MENU) {
        menu_update_particles(&game->menu_effects, delta);
    } else if (game->state == GAME_PLAYING) {
        particle_update(game, delta);
        
        if (game->audio.music_playing && game->audio.music_stream) {
            int queued = SDL_GetAudioStreamQueued(game->audio.music_stream);
            if (queued < 8192) {
                SDL_PutAudioStreamData(game->audio.music_stream, 
                                      game->audio.music_data, 
                                      game->audio.music_length);
            }
        }
        
        Uint64 current_time = SDL_GetTicks();
        if (current_time - game->last_update >= (Uint64)game->speed) {
            game->last_update = current_time;
            
            snake_move(game->snake);
            
            if (food_check_collision(&game->food, game->snake)) {
                snake_add_segment(game->snake, game->snake->tail->x, game->snake->tail->y);
                game->score += 10;
                
                game_play_eat_sound(game);
                
                // Effets de particules fleurissantes à chaque point gagné
                float food_x = GRID_OFFSET_X + game->food.x * CELL_SIZE + CELL_SIZE / 2;
                float food_y = GRID_OFFSET_Y + game->food.y * CELL_SIZE + CELL_SIZE / 2;
                
                // Explosion de particules colorées
                for (int i = 0; i < 20; i++) {
                    SDL_Color colors[] = {
                        {255, 255, 0, 255},   // Jaune
                        {255, 200, 0, 255},   // Orange
                        {255, 100, 200, 255}, // Rose
                        {0, 255, 200, 255},   // Cyan
                        {200, 100, 255, 255}  // Violet
                    };
                    particle_add(game, food_x, food_y, colors[rand() % 5], PARTICLE_FOOD);
                }
                
                if (game->speed > 50) {
                    game->speed -= 5;
                }
                
                food_spawn(&game->food, game->snake, 0);
            }
            
            if (snake_check_collision(game->snake) || snake_check_self_collision(game->snake)) {
                game_stop_music(game);
                game_play_game_over_sound(game);
                game->state = GAME_OVER;
            }
        }
    }
}

bool game_load_textures(Game *game) {
    SDL_Surface *surface = NULL;
    
    surface = IMG_Load("assets/background_nature.png");
    if (!surface) {
        SDL_Log("Avertissement: background_nature.png non trouvé");
        game->textures.background = NULL;
    } else {
        game->textures.background = SDL_CreateTextureFromSurface(game->renderer, surface);
        SDL_DestroySurface(surface);
    }
    
    surface = IMG_Load("assets/snake_head.png");
    if (!surface) {
        SDL_Log("Avertissement: snake_head.png non trouvé");
        game->textures.snake_head = NULL;
    } else {
        game->textures.snake_head = SDL_CreateTextureFromSurface(game->renderer, surface);
        SDL_DestroySurface(surface);
    }
    
    surface = IMG_Load("assets/snake_body.png");
    if (!surface) {
        SDL_Log("Avertissement: snake_body.png non trouvé");
        game->textures.snake_body = NULL;
    } else {
        game->textures.snake_body = SDL_CreateTextureFromSurface(game->renderer, surface);
        SDL_DestroySurface(surface);
    }
    
    surface = IMG_Load("assets/food.png");
    if (!surface) {
        SDL_Log("Avertissement: food.png non trouvé");
        game->textures.food = NULL;
    } else {
        game->textures.food = SDL_CreateTextureFromSurface(game->renderer, surface);
        SDL_DestroySurface(surface);
    }
    
    SDL_Log("Textures chargées!");
    return true;
}

void game_free_textures(Game *game) {
    if (game->textures.background) SDL_DestroyTexture(game->textures.background);
    if (game->textures.snake_head) SDL_DestroyTexture(game->textures.snake_head);
    if (game->textures.snake_body) SDL_DestroyTexture(game->textures.snake_body);
    if (game->textures.food) SDL_DestroyTexture(game->textures.food);
}

void game_render_text(Game *game, const char *text, int x, int y, 
                     SDL_Color color, TTF_Font *font) {
    if (!font) return;
    
    size_t text_len = strlen(text);
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, text_len, color);
    if (!surface) return;
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    if (texture) {
        SDL_FRect rect = {x, y, surface->w, surface->h};
        SDL_RenderTexture(game->renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}

void game_render_text_centered(Game *game, const char *text, int y, 
                               SDL_Color color, TTF_Font *font) {
    if (!font) return;
    
    size_t text_len = strlen(text);
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, text_len, color);
    if (!surface) return;
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    if (texture) {
        SDL_FRect rect = {(WINDOW_WIDTH - surface->w) / 2, y, surface->w, surface->h};
        SDL_RenderTexture(game->renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}

void game_save_high_score(Game *game) {
    FILE *file = fopen("highscore.txt", "w");
    if (file) {
        fprintf(file, "%d", game->high_score);
        fclose(file);
    }
}

void game_load_high_score(Game *game) {
    FILE *file = fopen("highscore.txt", "r");
    if (file) {
        fscanf(file, "%d", &game->high_score);
        fclose(file);
    } else {
        game->high_score = 0;
    }
}

void game_render_menu(Game *game) {
    float time = SDL_GetTicks() / 1000.0f;
    
    menu_draw_gradient_bg(game->renderer);
    menu_render_starfield(game->renderer, &game->menu_effects, time);
    menu_render_particles(game->renderer, &game->menu_effects);
    menu_render_scanlines(game->renderer, &game->menu_effects);
    
    menu_render_title(game->renderer, game->font_large, time);
    menu_render_animated_line(game->renderer, 150, time);
    menu_render_animated_line(game->renderer, WINDOW_HEIGHT - 200, time);
    
    menu_draw_neon_button(game->renderer, &game->btn_play, game->font_medium,
                         1.0f + 0.3f * sin(time * 3.0f));
    menu_draw_neon_button(game->renderer, &game->btn_settings, game->font_medium,
                         1.0f + 0.3f * sin(time * 3.0f + 2.0f));
    menu_draw_neon_button(game->renderer, &game->btn_quit, game->font_medium,
                         1.0f + 0.3f * sin(time * 3.0f + 4.0f));
    
    menu_render_subtitle(game->renderer, game->font_small, time);
    
    // Bouton muet M
    SDL_Color music_color = game->audio.music_playing ? 
        (SDL_Color){0, 255, 200, 255} : (SDL_Color){255, 100, 100, 255};
    const char *music_text = game->audio.music_playing ? "M - Muet" : "M - Son OFF";
    game_render_text(game, music_text, WINDOW_WIDTH - 150, 20, music_color, game->font_small);

}

void game_render_playing(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 10, 15, 30, 255);
    SDL_RenderClear(game->renderer);
    
    if (game->textures.background) {
        SDL_RenderTexture(game->renderer, game->textures.background, NULL, NULL);
    }
    
    SnakeSegment *current = game->snake->head;
    int index = 0;
    
    while (current != NULL) {
        SDL_FRect rect = {
            GRID_OFFSET_X + current->x * CELL_SIZE,
            GRID_OFFSET_Y + current->y * CELL_SIZE,
            CELL_SIZE,
            CELL_SIZE
        };
        
        if (index == 0 && game->textures.snake_head) {
            SDL_RenderTexture(game->renderer, game->textures.snake_head, NULL, &rect);
            SDL_SetRenderDrawColor(game->renderer, 255, 100, 200, 100);
            SDL_FRect glow = {rect.x - 2, rect.y - 2, rect.w + 4, rect.h + 4};
            SDL_RenderRect(game->renderer, &glow);
        } else if (game->textures.snake_body) {
            SDL_RenderTexture(game->renderer, game->textures.snake_body, NULL, &rect);
        } else {
            if (index == 0) {
                SDL_SetRenderDrawColor(game->renderer, 255, 50, 200, 255);
            } else if (index < 3) {
                SDL_SetRenderDrawColor(game->renderer, 200, 100, 180, 255);
            } else {
                SDL_SetRenderDrawColor(game->renderer, 100, 200, 255, 255);
            }
            SDL_RenderFillRect(game->renderer, &rect);
            
            SDL_SetRenderDrawColor(game->renderer, 0, 255, 200, 200);
            SDL_RenderRect(game->renderer, &rect);
        }
        
        current = current->next;
        index++;
    }
    
    SDL_FRect food_rect = {
        GRID_OFFSET_X + game->food.x * CELL_SIZE,
        GRID_OFFSET_Y + game->food.y * CELL_SIZE,
        CELL_SIZE,
        CELL_SIZE
    };
    
    if (game->textures.food) {
        SDL_RenderTexture(game->renderer, game->textures.food, NULL, &food_rect);
    } else {
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(game->renderer, &food_rect);
        
        SDL_SetRenderDrawColor(game->renderer, 255, 200, 0, 150);
        SDL_FRect food_glow = {food_rect.x - 2, food_rect.y - 2, food_rect.w + 4, food_rect.h + 4};
        SDL_RenderRect(game->renderer, &food_glow);
    }
    
    particle_render(game);
    
    // Barre de score compacte et stylée
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 180);
    SDL_FRect score_bg = {5, 5, 180, 45};
    SDL_RenderFillRect(game->renderer, &score_bg);
    
    SDL_SetRenderDrawColor(game->renderer, 0, 255, 200, 255);
    SDL_RenderRect(game->renderer, &score_bg);
    
    char score_text[64];
    snprintf(score_text, sizeof(score_text), "SCORE: %d", game->score);
    
    SDL_Color score_color = {0, 255, 200, 255};
    game_render_text(game, score_text, 12, 12, score_color, game->font_medium);
    
    // Afficher l'état de la musique
    SDL_Color music_color = game->audio.music_playing ? 
        (SDL_Color){0, 255, 200, 255} : (SDL_Color){255, 100, 100, 255};
    const char *music_text = game->audio.music_playing ? "M: SON ON" : "M: SON OFF";
    game_render_text(game, music_text, WINDOW_WIDTH - 150, 10, music_color, game->font_small);
}

void game_render_game_over(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 20, 220);
    SDL_FRect gameover_bg = {150, 150, WINDOW_WIDTH - 300, 400};
    SDL_RenderFillRect(game->renderer, &gameover_bg);
    
    SDL_SetRenderDrawColor(game->renderer, 255, 0, 150, 255);
    SDL_RenderRect(game->renderer, &gameover_bg);
    
    SDL_FRect inner_border = {152, 152, WINDOW_WIDTH - 304, 396};
    SDL_SetRenderDrawColor(game->renderer, 255, 100, 200, 200);
    SDL_RenderRect(game->renderer, &inner_border);
    
    SDL_Color magenta = {255, 0, 150, 255};
    SDL_Color cyan = {0, 255, 200, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    
    game_render_text_centered(game, "GAME OVER", 180, magenta, game->font_large);
    
    char score_text[64];
    snprintf(score_text, sizeof(score_text), "Score Final: %d", game->score);
    game_render_text_centered(game, score_text, 300, yellow, game->font_medium);
    
    game_render_text_centered(game, "R = Rejouer | ESC = Menu", 400, cyan, game->font_small);
}

void game_render_grid(Game *game) {
    // Grille intérieure très discrète (presque invisible)
    SDL_SetRenderDrawColor(game->renderer, 100, 100, 100, 8);
    for (int x = 0; x <= GRID_WIDTH; x++) {
        SDL_RenderLine(game->renderer, 
                      GRID_OFFSET_X + x * CELL_SIZE, GRID_OFFSET_Y,
                      GRID_OFFSET_X + x * CELL_SIZE, GRID_OFFSET_Y + GRID_HEIGHT * CELL_SIZE);
    }
    
    for (int y = 0; y <= GRID_HEIGHT; y++) {
        SDL_RenderLine(game->renderer, 
                      GRID_OFFSET_X, GRID_OFFSET_Y + y * CELL_SIZE,
                      GRID_OFFSET_X + GRID_WIDTH * CELL_SIZE, GRID_OFFSET_Y + y * CELL_SIZE);
    }
    
    // Cadre fin sur le contour de la fenêtre (bordure discrète)
    SDL_SetRenderDrawColor(game->renderer, 0, 200, 255, 60);
    SDL_FRect window_border = {
        0,
        0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    };
    SDL_RenderRect(game->renderer, &window_border);
    
    // Deuxième bordure interne très légère
    SDL_SetRenderDrawColor(game->renderer, 0, 150, 200, 35);
    SDL_FRect inner_border = {
        2,
        2,
        WINDOW_WIDTH - 4,
        WINDOW_HEIGHT - 4
    };
    SDL_RenderRect(game->renderer, &inner_border);
}

void game_render(Game *game) {
    SDL_RenderClear(game->renderer);
    
    if (game->state == GAME_MENU) {
        game_render_menu(game);
    } else if (game->state == GAME_PLAYING) {
        game_render_playing(game);
        game_render_grid(game);
    } else if (game->state == GAME_PAUSED) {
        game_render_playing(game);
        
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 20, 220);
        SDL_FRect pause_bg = {200, 200, WINDOW_WIDTH - 400, 300};
        SDL_RenderFillRect(game->renderer, &pause_bg);
        
        SDL_SetRenderDrawColor(game->renderer, 0, 255, 200, 255);
        SDL_RenderRect(game->renderer, &pause_bg);
        
        SDL_Color magenta = {255, 50, 200, 255};
        SDL_Color white = {200, 255, 255, 255};
        
        game_render_text_centered(game, "PAUSE", 250, magenta, game->font_large);
        game_render_text_centered(game, "Appuyez sur P pour continuer", 370, white, game->font_small);
    } else if (game->state == GAME_OVER) {
        game_render_playing(game);
        game_render_game_over(game);
    }
    
    SDL_RenderPresent(game->renderer);
}