
#include "LevelC.h"
#include "Utility.h"

#include <SDL_mixer.h>

#define LEVEL_WIDTH 25
#define LEVEL_HEIGHT 25

constexpr char SPRITESHEET_FILEPATH[] = "ss.png";

//Mix_Chunk *g_teleport_sfx;


unsigned int LEVELC_DATA[] =
{
    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
        44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,
         45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45
    };



LevelC::~LevelC()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelC::initialise()
{
//    g_teleport_sfx =  Mix_LoadWAV("cling.mp3");

    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("Grass.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 11, 7);
    
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
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 2.0f, 0.8f,0.8f, ENEMY, GUARD, IDLE);
    }
    
    //middle
    m_game_state.enemies[0].set_position(glm::vec3(2.0f, -5.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(2.0f,-2.0f,0.0f));
    m_game_state.enemies[0].set_entity_type(ENEMY);
    m_game_state.enemies[0].set_ai_type(WALKER);
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("background.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
}

void LevelC::update(float delta_time)
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
//                Mix_PlayChannel(
//                                -1,       // using the first channel that is not currently in use...
//                                g_teleport_sfx,  // ...play this chunk of audio...
//                                0);
            }
        }
    
    if (m_game_state.enemies[0].get_position().y > -1.0f || m_game_state.enemies[0].get_position().y < -9.0f){
        glm::vec3 temp = m_game_state.enemies[0].get_movement();
        temp.y = temp.y*-1;
        m_game_state.enemies[0].set_movement(temp);
    }
    if (m_game_state.enemies[0].get_position().x > 13.0f || m_game_state.enemies[0].get_position().x< 1.0f){
        glm::vec3 temp = m_game_state.enemies[0].get_movement();
        temp.x = temp.x*-1;
        m_game_state.enemies[0].set_movement(temp);
    }



}

void LevelC::render(ShaderProgram *program){
        m_game_state.map->render(program);
        m_game_state.player->render(program);
        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            m_game_state.enemies[i].render(program);
        }
    GLuint font_texture_id = Utility::load_texture("font1.png");
    if (ENEMY_COUNT==0){
        Utility::draw_text(program ,font_texture_id, "You WIN ", 0.5f, 0.0001f, glm::vec3(m_game_state.player->get_position().x -2.0f, m_game_state.player->get_position().y, 0.0f) );
        m_game_state.player->set_speed(0.0f);
    }
    Utility::draw_text(program ,font_texture_id,  std::to_string(ENEMY_COUNT) +" Enemies", 0.5f, 0.0001f, glm::vec3(m_game_state.player->get_position().x -4.0f, m_game_state.player->get_position().y + 3.5f, 0.0f) );
   
   
}
