// ========== FICHIER: Snake.h ==========

#ifndef SNAKE_H
#define SNAKE_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_audio.h>
#include <stdbool.h>

// Constantes
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700
#define CELL_SIZE 25
#define GRID_WIDTH 32
#define GRID_HEIGHT 24
#define GRID_OFFSET_X 100
#define GRID_OFFSET_Y 50
#define INITIAL_SPEED 120

// Directions
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

// États du jeu
typedef enum {
    GAME_MENU,
    GAME_PLAYING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_SETTINGS
} GameState;

// Types de particules
typedef enum {
    PARTICLE_FOOD,
    PARTICLE_TRAIL,
    PARTICLE_SPARKLE
} ParticleType;

// Structure pour les particules du menu
typedef struct {
    float x, y;
    float vx, vy;
    float lifetime;
    float max_lifetime;
    SDL_Color color;
} MenuParticle;

// Structure pour les effets du menu
typedef struct {
    MenuParticle particles[100];
    int particle_count;
    float title_pulse;
    float button_glow[3];
    float stars[50][2];
    float scanline_offset;
} MenuEffects;

// Structure pour les particules normales
typedef struct Particle {
    float x, y;
    float vx, vy;
    float lifetime;
    float max_lifetime;
    SDL_Color color;
    ParticleType type;
    struct Particle *next;
} Particle;

typedef struct SnakeSegment {   
    int x;
    int y;
    float display_x;
    float display_y;
    struct SnakeSegment *next;
} SnakeSegment;

typedef struct {
    SnakeSegment *head;
    SnakeSegment *tail;
    Direction direction;
    Direction next_direction;
    int length;
    float pulse_phase;
} Snake;

typedef struct {
    int x;
    int y;
    bool active;
    int type;
    float animation_phase;
    int value;
} Food;

typedef struct {
    SDL_FRect rect;
    const char *text;
    SDL_Color normal_color;
    SDL_Color hover_color;
    SDL_Color text_color;
    bool is_hovered;
    bool is_pressed;
} Button;


// Structure pour les textures
typedef struct {
    SDL_Texture *background;
    SDL_Texture *snake_head;
    SDL_Texture *snake_body;
    SDL_Texture *food;
    SDL_Texture *grid_overlay;
} GameTextures;

// Structure pour un son chargé
typedef struct {
    Uint8 *buffer;
    Uint32 length;
    SDL_AudioSpec spec;
} SoundEffect;

// Structure pour l'audio avec SDL3
typedef struct {
    SDL_AudioStream *music_stream;
    SDL_AudioStream *sfx_stream;
    
    Uint8 *music_data;
    Uint32 music_length;
    
    SoundEffect eat_sound;
    SoundEffect game_over_sound;
    SoundEffect menu_sound;
    SoundEffect bonus_sound;
    
    bool music_playing;
    float music_volume;
    float sfx_volume;
} GameAudio;

// Structure du jeu
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font_large;
    TTF_Font *font_medium;
    TTF_Font *font_small;
    GameTextures textures;
    GameAudio audio;
    MenuEffects menu_effects;
    Snake *snake;
    Food food;
    Food bonus_food;
    GameState state;
    int score;
    int high_score;
    Uint64 last_update;
    Uint64 last_frame_time;
    int speed;
    bool audio_enabled;
    float animation_time;
    Particle *particles;
    
    Button btn_play;
    Button btn_settings;
    Button btn_quit;
    Button btn_resume;
    Button btn_restart;
    Button btn_menu;
    
    int mouse_x;
    int mouse_y;
    
    bool show_grid;
    bool show_particles;
    float screen_shake;
} Game;

// ========== DÉCLARATIONS DE FONCTIONS ==========

// Fonctions du serpent
Snake* snake_create(int start_x, int start_y);
void snake_destroy(Snake *snake);
void snake_add_segment(Snake *snake, int x, int y);
void snake_move(Snake *snake);
bool snake_check_collision(Snake *snake);
bool snake_check_self_collision(Snake *snake);
void snake_update_display_positions(Snake *snake, float delta);

// Fonctions de la nourriture
void food_spawn(Food *food, Snake *snake, int type);
bool food_check_collision(Food *food, Snake *snake);

// Fonctions des particules
void particle_add(Game *game, float x, float y, SDL_Color color, ParticleType type);
void particle_update(Game *game, float delta);
void particle_render(Game *game);
void particle_clear_all(Game *game);

// Fonctions du menu gaming
void menu_init_starfield(MenuEffects *effects);
void menu_update_particles(MenuEffects *effects, float delta);
void menu_render_particles(SDL_Renderer *renderer, MenuEffects *effects);
void menu_render_starfield(SDL_Renderer *renderer, MenuEffects *effects, float time);
void menu_draw_gradient_bg(SDL_Renderer *renderer);
void menu_render_scanlines(SDL_Renderer *renderer, MenuEffects *effects);
void menu_draw_neon_button(SDL_Renderer *renderer, Button *btn, TTF_Font *font, float glow_intensity);
void menu_render_title(SDL_Renderer *renderer, TTF_Font *font, float pulse);
void menu_render_subtitle(SDL_Renderer *renderer, TTF_Font *font_small, float time);
void menu_render_animated_line(SDL_Renderer *renderer, int y, float time);

// Fonctions des boutons
Button button_create(float x, float y, float w, float h, const char *text);
void button_update(Button *btn, int mouse_x, int mouse_y);
bool button_is_clicked(Button *btn, SDL_Event *event);
void button_render(Game *game, Button *btn);

// Fonctions audio SDL3
bool game_load_audio(Game *game);
void game_free_audio(Game *game);
void game_play_music(Game *game);
void game_stop_music(Game *game);
void game_play_eat_sound(Game *game);
void game_play_game_over_sound(Game *game);
void game_play_menu_sound(Game *game);
void game_play_bonus_sound(Game *game);

// Fonctions du jeu principal
Game* game_create(void);
void game_destroy(Game *game);
void game_reset(Game *game);
void game_handle_input(Game *game, SDL_Event *event);
void game_update(Game *game, float delta);
void game_render(Game *game);
void game_render_text(Game *game, const char *text, int x, int y, 
                     SDL_Color color, TTF_Font *font);
void game_render_text_centered(Game *game, const char *text, int y, 
                               SDL_Color color, TTF_Font *font);
bool game_load_textures(Game *game);
void game_free_textures(Game *game);
void game_render_menu(Game *game);
void game_render_playing(Game *game);
void game_render_game_over(Game *game);
void game_render_grid(Game *game);
void game_save_high_score(Game *game);
void game_load_high_score(Game *game);

#endif