#include "Level3.h"

#define LEVEL3_WIDTH 28
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 2

unsigned int level3_data[] =
{
    17, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 17,
    17, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 49, 17,
    17, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16, 17,
    17, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16, 16, 17, 17,
    17, 16, 16, 13, 13, 13, 13, 13, 13, 13, 44, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16, 17, 17, 17, 17,
    17, 17, 17, 16, 13, 13, 13, 13, 44, 13, 13, 13, 13, 13, 13, 13, 44, 13, 13, 13, 13, 13, 16, 17, 17, 17, 17, 17,
    17, 13, 13, 13, 13, 13, 44, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16, 16, 17, 17, 17, 17, 17, 17,
    17, 16, 16, 16, 16, 16, 16, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17,
};

void Level3::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet@2.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 14, 7);

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -7, 0);
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
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("enemy3.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(12.8, -4.2, 0);
    state.enemies[0].acceleration = glm::vec3(0);
    state.enemies[0].aiType = ROVER;
    state.enemies[0].movement = glm::vec3(-0.35, 0, 0);
    state.enemies[0].speed = 1.0f;
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = 1.0f;
    //state.enemies[0].isActive = false;

    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = Util::LoadTexture("enemy.png");
    state.enemies[1].position = glm::vec3(24.5f, 0, 0);
    state.enemies[1].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[1].aiType = WAITANDGO;
    state.enemies[1].aiState = IDLE;
    state.enemies[1].speed = 1.0f;



}

void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].UpdateEnemy(deltaTime, state.player, state.map);
    }

    if (state.player->position.x >= 26) {
        state.nextScene = 4;
    }

}

void Level3::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}