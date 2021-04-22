#include "Level2.h"

#define LEVEL2_WIDTH 24
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 3

unsigned int level2_data[] =
{
    17, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    17, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16,
    17, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16, 16, 17,
    17, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16, 16, 17, 17, 17,
    17, 15, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16, 16, 17, 13, 13, 75, 17,
    17, 17, 15, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16, 16, 17,
    17, 17, 17, 15, 15,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, 15, 15, 16, 16, 16, 16, 17, 17, 17,
    17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 17, 17, 17, 17, 17, 17, 17, 17,
};

void Level2::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet@2.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 14, 7);

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);

    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.player->height = 0.8f;
    state.player->width = 0.8f;

    state.player->jumpPower = 5.0f;


    //enemies
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("enemy2.png");

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = FLOATER;
        state.enemies[i].position = glm::vec3(6.7f + i * 2.7f, -6.2f, 0);
        state.enemies[i].acceleration = glm::vec3(0);

        state.enemies[i].speed = 1.0f;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].width = 1.0f;
        state.enemies[i].height = 0.8f;
    }
    state.enemies[0].movement = glm::vec3(0, 0.2, 0);
    state.enemies[1].movement = glm::vec3(0, -0.2, 0);
    state.enemies[2].movement = glm::vec3(0, 0.2, 0);


}

void Level2::Update(float deltaTime) {
    
    /*
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        if (fabs(state.player->position.y - state.enemies[i].position.y) <= (state.player->height + state.enemies[i].height) / 2 + 0.05 &&
            fabs(state.player->position.x - state.enemies[i].position.x) <= (state.player->width + state.enemies[i].width) / 2 &&
            state.enemies[i].isActive)
        {
            if (state.player->velocity.y < 0
                && state.player->position.y > state.enemies[i].position.y) {
                state.enemies[i].isActive = false;
                state.player->velocity.y = 7.0f;
                //state.player->jump = true;
            }
            else {
                state.player->health -= 1;
                //Mix_PlayChannel(-1, damageSound, 0);
                state.player->position = glm::vec3(1, 0, 0);
                for (int j = 0; j < LEVEL2_ENEMY_COUNT; j++) {
                    state.enemies[j].isActive = true;
                }
            }
        }
    }
    */

    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        if (fabs(state.player->position.y - state.enemies[i].position.y) <= (state.player->height + state.enemies[i].height) / 2 + 0.05 &&
            fabs(state.player->position.x - state.enemies[i].position.x) <= (state.player->width + state.enemies[i].width) / 2 &&
            state.enemies[i].isActive)
        {
            state.enemies[i].isActive = false;
            state.player->velocity.y = 7.0f;
            //state.player->jump = true;
        }
    }
    

    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].UpdateEnemy(deltaTime, state.player, state.map);
    }

    if (state.player->position.x >= 22) {
        state.nextScene = 3;
    }
}

void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }

    state.player->Render(program);
}