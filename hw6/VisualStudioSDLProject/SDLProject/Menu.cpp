#include "Menu.h"


#define MENU_OBJECT_COUNT 1

unsigned int menuData[] = {
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int menuwallCount() {
    int count = 0;
    for (int block : menuData) {
        if (block == 1) {
            count += 1;
        }
    }
    return count;
}
int MENU_WALL_COUNT = menuwallCount();


glm::vec3 menuwallPosition(int i, int width, int height) {
    int xPos = i - (i / width) * width - (width - 1) / 2;
    int zPos = i / width - (height - 1) / 2;
    glm::vec3 result = glm::vec3(xPos, 1.0f, zPos);
    return result;
}

void Menu::Initialize() {

    fontTextureID = Util::LoadTexture("font1.png");

    state.nextScene = -1;

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0.0f, 0.7f, 3.0f);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;

    state.player->width = 0.8f;
    state.player->height = 0.8f;
    state.player->depth = 0.8f;

    state.objects = new Entity[MENU_OBJECT_COUNT];
    state.walls = new Entity[MENU_WALL_COUNT];


    //floor object
    GLuint floorTextureID = Util::LoadTexture("grassfloor(1).jpg");
    Mesh* floorMesh = new Mesh();
    floorMesh->LoadOBJ("cube.obj", 16);
    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = floorMesh;
    state.objects[0].position = glm::vec3(0, -0.25f, 0);
    state.objects[0].scale = glm::vec3(20.0f, 0.5f, 20.0f);
    state.objects[0].entityType = FLOOR;

    //wall objects
    GLuint wallTextureID = Util::LoadTexture("wall.jpg");
    Mesh* wallMesh = new Mesh();
    wallMesh->LoadOBJ("cube.obj", 1);
    int i = 0; //index of wall in state.walls
    for (int mapIndex = 0; mapIndex < 441; mapIndex++) {
        if (menuData[mapIndex] == 1) {
            state.walls[i].textureID = wallTextureID;
            state.walls[i].mesh = wallMesh;
            state.walls[i].position = menuwallPosition(mapIndex, 21, 21);
            state.walls[i].scale = glm::vec3(1.0f, 2.0f, 1.0f);
            state.walls[i].width = 0.8f;
            state.walls[i].height = 2.0f;
            state.walls[i].depth = 0.8f;
            state.walls[i].entityType = WALL;
            i += 1;
        }
    }

    //door to next level
    state.door = new Entity();
    GLuint doorTextureID = Util::LoadTexture("door.jpg");
    state.door->textureID = doorTextureID;
    state.door->mesh = wallMesh;
    state.door->position = glm::vec3(0.0f, 1.0f, -0.9f);
    state.door->scale = glm::vec3(1.0f, 2.0f, 1.0f);
    state.door->height = 2.0f;
    state.door->entityType = DOOR;

    
}

void Menu::Update(float deltaTime) {

    //check door collision
    if (state.player->CheckCollision(state.door)) {
        state.nextScene = 1;
    }

    state.player->Update(deltaTime, state.player, state.walls, MENU_WALL_COUNT);

    for (int i = 0; i < MENU_OBJECT_COUNT; i++) {
        state.objects[i].Update(deltaTime, state.player, state.objects, MENU_OBJECT_COUNT);
    }
    for (int i = 0; i < MENU_WALL_COUNT; i++) {
        state.walls[i].Update(deltaTime, state.player, state.walls, MENU_WALL_COUNT);
    }
    state.door->Update(deltaTime);

}

void Menu::Render(ShaderProgram* program) {

    //state.player->Render(program);
    for (int i = 0; i < MENU_OBJECT_COUNT; i++) {
        state.objects[i].Render(program);
    }
    for (int i = 0; i < MENU_WALL_COUNT; i++) {
        state.walls[i].Render(program);
    }
    state.door->Render(program);

    Util::DrawText(program, fontTextureID, "Haunted Maze", 0.3f, -0.15f, glm::vec3(-0.8f, 1.0f, 0.0f));
    Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.16f, -0.08f, glm::vec3(-0.75f, 0.2f, 0.0f));

}