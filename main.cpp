
/**
* Author:Regan Zhu
* Assignment: Final Proj
* Date due: 2024-8-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0001f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelC.h"
#include "Menu.h"
#include "LevelB.h"
#include "Effects.h"

#include <SDL_mixer.h>

constexpr int CD_QUAL_FREQ    = 44100,  // compact disk (CD) quality frequency
          AUDIO_CHAN_AMT  = 2,
          AUDIO_BUFF_SIZE = 4096;


// ––––– CONSTANTS ––––– //
constexpr int WINDOW_WIDTH  = 640*2,
          WINDOW_HEIGHT = 480*2;

constexpr float BG_RED     = 0.29f,
            BG_BLUE    = 0.28f,
            BG_GREEN   = 0.21f,
            BG_OPACITY = 0.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_lit.glsl",
           F_SHADER_PATH[] = "shaders/fragment_lit.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ––––– GLOBAL VARIABLES ––––– //
Scene  *g_current_scene;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;
Menu *g_menu;
int g_health;

bool g_temp;


constexpr int PLAY_ONCE   =  0,
NEXT_CHNL   = -1;  // next available channel

// Declare your sfx globally...
Mix_Chunk *g_walking_sfx;
Mix_Chunk *g_lose_sfx;
Mix_Chunk *g_rocket_sfx;



Effects *g_effects;
Scene   *g_levels[4];

SDL_Window* g_display_window;


ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;

AppStatus g_app_status = RUNNING;

void swtich_to_scene(Scene *scene);
void initialise();
void process_input();
void update();
void render();
void shutdown();

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

        // Start Audio
        Mix_OpenAudio(
            CD_QUAL_FREQ,        // the frequency to playback audio at (in Hz)
            MIX_DEFAULT_FORMAT,  // audio format
            AUDIO_CHAN_AMT,      // number of channels (1 is mono, 2 is stereo, etc).
            AUDIO_BUFF_SIZE      // audio buffer size in sample FRAMES (total samples divided by channel count)
            );
    g_display_window = SDL_CreateWindow("Final Gami!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_temp = true;
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    g_menu = new Menu();
    g_levels[0] = g_menu;
    g_levels[1] = g_levelB;
    g_levels[2] = g_levelA;
    g_levels[3] = g_levelC;
    g_health = 800;
    
    g_walking_sfx = Mix_LoadWAV("walking.mp3");
    g_rocket_sfx = Mix_LoadWAV("rocket.mp3");


    // Start at level A
    switch_to_scene(g_levels[0]);
    
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    g_effects->start(SHRINK, 2.0f);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                    case SDLK_RETURN:
                        g_current_scene->setsceneid(1);
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if(g_current_scene == g_levelA ||g_current_scene == g_levelC){
        if (key_state[SDL_SCANCODE_LEFT])       {
            glm::vec3 acc =g_current_scene->get_state().player->get_acceleration();
            acc.x -= 60.0f;
            g_current_scene->get_state().player->set_acceleration(acc);
            g_current_scene->get_state().player->face_left();

            Mix_PlayChannel(
                NEXT_CHNL,       // using the first channel that is not currently in use...
                            g_rocket_sfx,  // ...play this chunk of audio...
                PLAY_ONCE);        // ...once.
    }
        else if (key_state[SDL_SCANCODE_RIGHT])  {
            glm::vec3 acc =g_current_scene->get_state().player->get_acceleration();
            acc.x += 60.0f;
            g_current_scene->get_state().player->set_acceleration(acc);
            g_current_scene->get_state().player->face_right();

            Mix_PlayChannel(
                            NEXT_CHNL,       // using the first channel that is not currently in use...
                            g_rocket_sfx,  // ...play this chunk of audio...
                            PLAY_ONCE);
        }// ...once.
        else if (key_state[SDL_SCANCODE_UP])       { 
            glm::vec3 acc =g_current_scene->get_state().player->get_acceleration();
            acc.y += 60.0f;
            g_current_scene->get_state().player->set_acceleration(acc);
            g_current_scene->get_state().player->face_up();
            Mix_PlayChannel(
                            NEXT_CHNL,       // using the first channel that is not currently in use...
                            g_rocket_sfx,  // ...play this chunk of audio...
                            PLAY_ONCE);        // ...once.
        }
        else if (key_state[SDL_SCANCODE_DOWN])  {
            glm::vec3 acc =g_current_scene->get_state().player->get_acceleration();
            acc.y -= 60.0f;
            g_current_scene->get_state().player->set_acceleration(acc);
            g_current_scene->get_state().player->face_right();
            Mix_PlayChannel(
                            NEXT_CHNL,       // using the first channel that is not currently in use...
                            g_rocket_sfx,  // ...play this chunk of audio...
                            PLAY_ONCE);        // ...once.
        }
    }else{
        if (key_state[SDL_SCANCODE_LEFT])       {
            g_current_scene->get_state().player->move_left();
            Mix_PlayChannel(
                NEXT_CHNL,       // using the first channel that is not currently in use...
                g_walking_sfx,  // ...play this chunk of audio...
                PLAY_ONCE);        // ...once.
    }
        else if (key_state[SDL_SCANCODE_RIGHT])  {g_current_scene->get_state().player->move_right();
            Mix_PlayChannel(
                            NEXT_CHNL,       // using the first channel that is not currently in use...
                            g_walking_sfx,  // ...play this chunk of audio...
                            PLAY_ONCE);
        }// ...once.
        else if (key_state[SDL_SCANCODE_UP])       { g_current_scene->get_state().player->move_up();
            Mix_PlayChannel(
                            NEXT_CHNL,       // using the first channel that is not currently in use...
                            g_walking_sfx,  // ...play this chunk of audio...
                            PLAY_ONCE);        // ...once.
        }
        else if (key_state[SDL_SCANCODE_DOWN])  {
            g_current_scene->get_state().player->move_down();
            Mix_PlayChannel(
                            NEXT_CHNL,       // using the first channel that is not currently in use...
                            g_walking_sfx,  // ...play this chunk of audio...
                            PLAY_ONCE);        // ...once.
        }
    }
         
    if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();
    
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    // Prevent the camera from showing anything outside of the "edge" of the level
    g_view_matrix = glm::mat4(1.0f);
    
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, -g_current_scene->get_state().player->get_position().y, 0));

    
    if (g_current_scene == g_levelA && g_current_scene->get_state().player->get_position().y < -10.0f) switch_to_scene(g_levelB);
    
    g_view_matrix = glm::translate(g_view_matrix, g_effects->get_view_offset());
//    if (g_current_scene->get_state().player->check_collision(g_current_scene->get_state().enemies)){
//        g_health -= 1;    }
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
       
    glClear(GL_COLOR_BUFFER_BIT);
       
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_shader_program.set_light_position_matrix(g_current_scene->get_state().player->get_position());

    g_current_scene->render(&g_shader_program);
       

    g_effects->render();
    GLuint font_texture_id = Utility::load_texture("font1.png");
    Utility::draw_text(&g_shader_program ,font_texture_id, "Health ", 0.5f, 0.0001f, glm::vec3(g_current_scene->get_state().player->get_position().x +2.0f, g_current_scene->get_state().player->get_position().y + 3.5f, 0.0f) );
    Utility::draw_text(&g_shader_program ,font_texture_id, std::to_string(g_health), 0.5f, 0.0001f, glm::vec3(g_current_scene->get_state().player->get_position().x +3.3f, g_current_scene->get_state().player->get_position().y + 3.1f, 0.0f) );
    if(g_health<100){
//        g_lose_sfx = Mix_LoadWAV("lose.mp3");
//            Mix_PlayChannel(
//                NEXT_CHNL,       // using the first channel that is not currently in use...
//                g_lose_sfx,  // ...play this chunk of audio...
//                PLAY_ONCE);        // ...once.
        Utility::draw_text(&g_shader_program ,font_texture_id, "You LOSE", 1.0f, 0.0001f, glm::vec3(g_current_scene->get_state().player->get_position().x -4.0f, g_current_scene->get_state().player->get_position().y, 0.0f) );
        g_health = -1;
        g_current_scene->get_state().player->set_speed(0.0f);
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    Mix_FreeChunk(g_walking_sfx);
    delete g_menu;
    delete g_levelA;
    delete g_levelB;
    delete g_effects;
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        
        if (g_current_scene->get_state().next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);
        
        render();
    }
    
    shutdown();
    return 0;
}
