
/**
* Author:Regan Zhu
* Assignment: Rise of the AI
* Date due: 2024-7-28, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "Menu.h"
#include "Utility.h"

#include <SDL_mixer.h>
constexpr char SPRITESHEET_FILEPATH[] = "ss.png",
           ENEMY_FILEPATH[]       = "enemy.png";


Menu::~Menu()
{
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Menu::initialise()
{
    m_game_state.next_scene_id = -1;
    
//    GLuint map_texture_id = Utility::load_texture("Screenshot 2024-07-24 at 8.05.41 PM.png");
//    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 4, 1);
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    int player_walking_animation[4][4] =
    {
        { 4,5,6,7 },  // for George to move to the left,
        { 8,9,10,11 }, // for George to move to the right,
        { 12,13,14,15 }, // for George to move upwards,
        { 0,1,2,3 }   // for George to move downwards
    };
    
    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        8.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );
        
    m_game_state.player->set_position(glm::vec3(9.0f, 0.0f, 0.0f));

    /**
    Enemies' stuff */


    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("background.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
}

void Menu::update(float delta_time)
{
    
    
}

void Menu::render(ShaderProgram *program)
{
    
    GLuint font_texture_id = Utility::load_texture("font1.png");
        Utility::draw_text(program,font_texture_id, "Enter to Begin", 0.5f, 0.1f, glm::vec3(5.0f, -2.0f, 0.0f) );
    Utility::draw_text(program,font_texture_id, "Gami", 0.5f, 0.1f, glm::vec3(8.0f, 0.0f, 0.0f) );
   }
