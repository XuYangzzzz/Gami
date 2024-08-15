
#include "LevelA.h"
#include "Utility.h"

#include <SDL_mixer.h>

#define LEVEL_WIDTH 15
#define LEVEL_HEIGHT 11

constexpr char SPRITESHEET_FILEPATH[] = "ss.png",
           ENEMY_FILEPATH[]       = "egg.png";

bool temp = true;
float bullettimeout = 3.0f;

Mix_Chunk *g_teleport_sfx;

unsigned int LEVELA_DATA[] =
{
    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45
};



LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    g_teleport_sfx =  Mix_LoadWAV("cling.mp3");

    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("Grass.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 11, 7);
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    int player_walking_animation[4][4] =
    {
        { 8,9,10,11 },  // for George to move to the left,
        { 12,13,14,15 }, // for George to move to the right,
        { 4,5,6,7}, // for George to move upwards,
        { 0,1,2,3 }   // for George to move downwards
    };
    
    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    
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
        0.4f,                      // width
        0.4f,                       // height
        PLAYER
    );
        
    m_game_state.player->set_position(glm::vec3(4.0f, -4.0f, 0.0f));

    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("egg.png");
    
    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 0.8f,0.8f, ENEMY, GUARD, IDLE);
    }
    
    //middle
    m_game_state.enemies[0].set_position(glm::vec3(12.0f, -2.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_entity_type(ENEMY);
    m_game_state.enemies[0].set_ai_type(BOUNCER);
    m_game_state.enemies[0].set_ai_state(IDLE);
    
    //third
    m_game_state.enemies[1].set_position(glm::vec3(14.0f, -8.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_entity_type(ENEMY);
    m_game_state.enemies[1].set_ai_type(BOUNCER);
    m_game_state.enemies[1].set_ai_state(IDLE);
    
    //first
    m_game_state.enemies[2].set_position(glm::vec3(1.0f, -7.0f, 0.0f));
    m_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[2].set_entity_type(ENEMY);
    m_game_state.enemies[2].set_ai_type(BOUNCER);
    m_game_state.enemies[2].set_ai_state(IDLE);


    
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("background.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    m_game_state.jump_sfx = Mix_LoadWAV("jump.wav");
    if (!m_game_state.jump_sfx) {
        std::cerr << "Error loading jump sound effect: " << Mix_GetError() << std::endl;
    } else {
        Mix_VolumeChunk(m_game_state.jump_sfx, MIX_MAX_VOLUME );
    }
}

void LevelA::update(float delta_time)
{
    
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    }
    
//        
        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            float x_distance = fabs(m_game_state.enemies[i].get_position().x - m_game_state.player->get_position().x);
            float y_distance = fabs(m_game_state.enemies[i].get_position().y - m_game_state.player->get_position().y) ;
            if(x_distance<0.8f&&y_distance<0.8f){
                std::cout<<"caught an egg";
                ENEMY_COUNT -= 1;
                for (int j = i; j < ENEMY_COUNT; j++)
                    m_game_state.enemies[j] = m_game_state.enemies[j + 1];
    //                m_game_state.enemies[i].deactivate();
                Mix_PlayChannel(
                                -1,       // using the first channel that is not currently in use...
                                g_teleport_sfx,  // ...play this chunk of audio...
                                0);
            }
        }
    if (ENEMY_COUNT==0){
        m_game_state.next_scene_id= 3;
    }
}

void LevelA::render(ShaderProgram *program){
        m_game_state.map->render(program);
        m_game_state.player->render(program);
        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            m_game_state.enemies[i].render(program);
        }
    GLuint font_texture_id = Utility::load_texture("font1.png");
    Utility::draw_text(program ,font_texture_id,  std::to_string(ENEMY_COUNT) +" Enemies", 0.5f, 0.0001f, glm::vec3(m_game_state.player->get_position().x -4.0f, m_game_state.player->get_position().y + 3.5f, 0.0f) );
   
}
