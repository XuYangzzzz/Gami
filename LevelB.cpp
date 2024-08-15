#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 25
#define LEVEL_HEIGHT 11

constexpr char SPRITESHEET_FILEPATH[] = "ss.png",
           ENEMY_FILEPATH[]       = "enemy.png";
bool shouldFlashRed = false;
float flashDuration = 3.0f; // Flash duration in seconds
float flashTime = 0.0f;
bool isFlashing = false;

Mix_Chunk *g_hurt_sfx;


unsigned int LEVELB_DATA[] =
{
    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
     0,  0,  0,  0, 44,  0,  0,  0,  0,  0,  0,  0, 45,  0,  0,  0,  0, 44,  0,  0,  0,  0,  0,  0, 44,
    44, 45, 45,  0, 44,  0, 44, 45, 45, 45,  0, 45, 45,  0,  0, 45,  0, 44, 45, 45, 45,  0, 45,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,  0,  0,  0,  0,  0,  0, 44,
    44,  0, 45, 45,  0,  0, 44, 45, 45, 45,  0, 45, 45,  0, 45,  0,  0, 44,  0,  0,  0,  0,  0, 45, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0, 44,  0,  0,  0,  0,  0,  0, 45, 44,  0, 45, 45, 45,  0,  0, 44,
    44,  0, 45, 45, 45,  0, 45, 45,  0, 44,  0, 45, 45, 45,  0,  0,  0, 44,  0,  0,  0, 44, 45,  0, 44,
    44,  0,  0,  0,  0,  0, 44,  0,  0,  0,  0,  0,  0,  0, 45, 45,  0,  0,  0,  0,  0, 44,  0,  0, 44,
    44, 45, 45, 45,  0,  0, 44, 45, 45,  0,  0, 45,  0,  0,  0,  0,  0, 44,  0, 45, 45, 44, 45,  0, 44,
    44,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 44,  0,  0,  0,  0,  0,  0, 0,
    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45
};

LevelB::~LevelB()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    delete m_game_state.bullet;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelB::initialise()
{
    g_hurt_sfx =  Mix_LoadWAV("hurt.mp3");

    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("Grass.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 11, 7);
    
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
    
    GLuint egg_texture_id = Utility::load_texture("egg.png");
    m_game_state.bullet = new Entity(
        egg_texture_id,         // texture id
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
    m_game_state.bullet->set_position(glm::vec3(26.0f, -9.0f, 0.0f));
    
    
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
        
    m_game_state.player->set_position(glm::vec3(-5.0f, -1.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(5.0f);
    
    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("enemy.png");

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 0.8f, 0.8f, ENEMY, GUARD, IDLE);
    }


    m_game_state.enemies[0].set_position(glm::vec3(1.0f, -1.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    
    m_game_state.enemies[1].set_position(glm::vec3(1.0f, -3.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_ai_type(GUARD_DOWN);
    m_game_state.enemies[1].set_ai_state(IDLE);
    m_game_state.enemies[1].set_entity_type(ENEMY);

    m_game_state.enemies[2].set_position(glm::vec3(3.0f, -7.0f, 0.0f));
    m_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[2].set_ai_type(GUARD);
    m_game_state.enemies[2].set_ai_state(IDLE);
    m_game_state.enemies[2].set_entity_type(ENEMY);
    
    m_game_state.enemies[3].set_position(glm::vec3(6.0f, -9.0f, 0.0f));
    m_game_state.enemies[3].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[3].set_ai_type(GUARD);
    m_game_state.enemies[3].set_ai_state(IDLE);
    m_game_state.enemies[3].set_entity_type(ENEMY);

    m_game_state.enemies[4].set_position(glm::vec3(6.0f, -1.0f, 0.0f));
    m_game_state.enemies[4].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[4].set_ai_type(GUARD);
    m_game_state.enemies[4].set_ai_state(IDLE);
    m_game_state.enemies[4].set_entity_type(ENEMY);
    
    m_game_state.enemies[5].set_position(glm::vec3(14.0f, -1.0f, 0.0f));
    m_game_state.enemies[5].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[5].set_ai_type(GUARD_DOWN);
    m_game_state.enemies[5].set_ai_state(IDLE);
    m_game_state.enemies[5].set_entity_type(ENEMY);
    
    m_game_state.enemies[6].set_position(glm::vec3(21.0f, -1.0f, 0.0f));
    m_game_state.enemies[6].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[6].set_ai_type(GUARD);
    m_game_state.enemies[6].set_ai_state(IDLE);
    m_game_state.enemies[6].set_entity_type(ENEMY);
    
    m_game_state.enemies[7].set_position(glm::vec3(21.0f, -9.0f, 0.0f));
    m_game_state.enemies[7].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[7].set_ai_type(GUARD);
    m_game_state.enemies[7].set_ai_state(IDLE);
    m_game_state.enemies[7].set_entity_type(ENEMY);
    
    m_game_state.enemies[8].set_position(glm::vec3(16.0f, -9.0f, 0.0f));
    m_game_state.enemies[8].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[8].set_ai_type(GUARD);
    m_game_state.enemies[8].set_ai_state(IDLE);
    m_game_state.enemies[8].set_entity_type(ENEMY);
    
    m_game_state.enemies[9].set_position(glm::vec3(21.0f, -3.0f, 0.0f));
    m_game_state.enemies[9].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[9].set_ai_type(GUARD);
    m_game_state.enemies[9].set_ai_state(IDLE);
    m_game_state.enemies[9].set_entity_type(ENEMY);
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("background.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);

    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    m_game_state.jump_sfx = Mix_LoadWAV("hurt.mp3");
    if (!m_game_state.jump_sfx) {
        std::cerr << "Error loading jump sound effect: " << Mix_GetError() << std::endl;
    } else {
        Mix_VolumeChunk(m_game_state.jump_sfx, MIX_MAX_VOLUME);
    }
}

void LevelB::update(float delta_time)
{
    m_game_state.bullet->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    }
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    if(m_game_state.player->get_position().x>26.0f){
        m_game_state.next_scene_id = 2;
    }
}

void LevelB::render(ShaderProgram *program)
{
    m_game_state.bullet->render(program);
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].render(program);
    }
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if(m_game_state.player->check_collision_bottom(m_game_state.enemies[i]) &&g_health>0){
//            std::cout<<"collision with player, enemy dead"<<std::endl;
            shouldFlashRed = true;
            g_health-=1;

            Mix_PlayChannel(
                            -1,       // using the first channel that is not currently in use...
                            g_hurt_sfx,  // ...play this chunk of audio...
                            0);

//            ENEMY_COUNT -= 1;
//            for (int j = i; j < ENEMY_COUNT; j++)
//                m_game_state.enemies[j] = m_game_state.enemies[j + 1];
////                m_game_state.enemies[i].deactivate();
        }
    }
    if (m_game_state.player->check_collision(m_game_state.enemies)&&g_health>0){
        shouldFlashRed = true;
        g_health-=1;
        Mix_PlayChannel(
                        -1,       // using the first channel that is not currently in use...
                        g_hurt_sfx,  // ...play this chunk of audio...
                        0);
    }
    GLint flashRedLoc = glGetUniformLocation(program->get_program_id(), "flashRed");
    if (shouldFlashRed)
    {
        flashTime += 1.0f;
        if (flashTime < flashDuration)
        {
            // Set the shader uniform to flash red
            glUniform1i(flashRedLoc, 1);
        }
        else
        {
            // Stop flashing after the duration
            glUniform1i(flashRedLoc, 0);
            shouldFlashRed = false;
            flashTime = 0.0f;
        }
    }
}
