#include "Menu.h"

#define MENU_WIDTH 14
#define MENU_HEIGHT 8

#define MENU_ENEMY_COUNT 1

unsigned int menu_data[] =
{
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3
};

void Menu::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet@2.png");
    fontTextureID = Util::LoadTexture("font1.png");

    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 14, 7);

    //Initialize Player
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
    state.player->isActive = false;
    

}

void Menu::Update(float deltaTime) {

    state.player->Update(deltaTime, state.player, NULL, 0, state.map);

}

void Menu::Render(ShaderProgram* program) {
    
    Util::DrawText(program, fontTextureID, "George's Advanture", 0.8f, -0.4f, glm::vec3(1.6f, -3.3f, 0));
    Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.5f, -0.25f, glm::vec3(2.65f, -5.0f, 0));
    state.map->Render(program);
    state.player->Render(program);

}