#include "Level1.h"

#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

unsigned int level1_data[] =
{
	3, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	3, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	3, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	3, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	3, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	3, 13, 13, 13, 13, 13, 13,  0, 28, 28, 28, 28, 13, 13,
	3,  0,  0,  0, 13, 13,  0,  3, 75, 13, 13, 13, 13, 13,
	3,  3,  3,  3, 13,  0,  3,  3, 29,  1,  1,  1,  1,  1
};

void Level1::Initialize() {

    state.nextScene = -1;
    enemyCount = LEVEL1_ENEMY_COUNT;

	GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet@2.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 14, 7);
	
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -4, 0);
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
    state.player->width = 0.7f;

    state.player->jumpPower = 5.0f;

    //enemies
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].position = glm::vec3(9, -6, 0);
    state.enemies[0].movement = glm::vec3(0);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].width = 1.0f;
    state.enemies[0].height = 0.8f;

    state.enemies[0].speed = 1;
    state.enemies[0].textureID = Util::LoadTexture("enemy.png");
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    
    //state.enemies[0].isActive = false;

}

void Level1::Update(float deltaTime) {

    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        if (fabs(state.player->position.y - state.enemies[i].position.y) <= (state.player->height + state.enemies[i].height) / 2 + 0.05 &&
            fabs(state.player->position.x - state.enemies[i].position.x) <= (state.player->width + state.enemies[i].width) / 2 &&
            state.enemies[i].isActive)

            //state.player->CheckCollision(&state.enemies[i])) {  // I don't know why checkCollision does not work properly on y axis here

        {

            //state.player->health -= 1;
            //state.player->position = glm::vec3(1, 0, 0);

            if (state.player->velocity.y < 0
                && state.player->position.y > state.enemies[i].position.y) {
                state.player->jump = true;
                //Mix_PlayChannel(-1, stepSound, 0);
                //state.player->velocity = glm::vec3(0, 6, 0);
                state.enemies[i].isActive = false;
            }
            else {
                state.player->health -= 1;
                //Mix_PlayChannel(-1, damageSound, 0);
                state.player->position = glm::vec3(1, 0, 0);
            }
            
        }
    }

    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].UpdateEnemy(deltaTime, state.player, state.map);
    }

    if (state.player->position.x >= 7 &&
        state.player->position.x <= 8 &&
        state.player->position.y <= -5) {
        state.nextScene = 2;
    }

}

void Level1::Render(ShaderProgram* program) {
	state.map->Render(program);
	state.player->Render(program);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}
