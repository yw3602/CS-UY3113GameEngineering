#include "Level3.h"


#define LEVEL3_OBJECT_COUNT 6

unsigned int level3Data[] = {
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

int lv3wallCount() {
    int count = 0;
    for (int block : level3Data) {
        if (block == 1) {
            count += 1;
        }
    }
    return count;
}
int LEVEL3_WALL_COUNT = lv3wallCount();


glm::vec3 lv3wallPosition(int i, int width, int height) {
    int xPos = i - (i / width) * width - (width - 1) / 2;
    int zPos = i / width - (height - 1) / 2;
    glm::vec3 result = glm::vec3(xPos, 1.0f, zPos);
    return result;
}

void Level3::Initialize() {

    fontTextureID = Util::LoadTexture("font1.png");

    state.nextScene = 4;

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0.0f, 0.6f, 9.0f);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;

    state.player->width = 0.8f;
    state.player->height = 0.8f;
    state.player->depth = 0.8f;

    state.objects = new Entity[LEVEL3_OBJECT_COUNT];
    state.walls = new Entity[LEVEL3_WALL_COUNT];


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
        if (level3Data[mapIndex] == 1) {
            state.walls[i].textureID = wallTextureID;
            state.walls[i].mesh = wallMesh;
            state.walls[i].position = lv3wallPosition(mapIndex, 21, 21);
            state.walls[i].scale = glm::vec3(1.0f, 2.0f, 1.0f);
            state.walls[i].width = 0.8f;
            state.walls[i].height = 2.0f;
            state.walls[i].depth = 0.8f;
            state.walls[i].entityType = WALL;
            i += 1;
        }
    }

    GLuint goldTextureID = Util::LoadTexture("gold.jpg");
    Mesh* goldMesh = new Mesh();
    goldMesh->LoadOBJ("cube.obj", 1);
    for (int i = 1; i < LEVEL3_OBJECT_COUNT; i++) {
        state.objects[i].textureID = goldTextureID;
        state.objects[i].mesh = goldMesh;
        state.objects[i].entityType = CRATE;
        state.objects[i].scale = glm::vec3(0.5f, 0.5f, 1.0f);
    }
    state.objects[1].position = glm::vec3(-0.5, 0.25f, 4);
    state.objects[2].position = glm::vec3(0, 0.25f, 4);
    state.objects[3].position = glm::vec3(0.5, 0.25f, 4);
    state.objects[4].position = glm::vec3(-0.25, 0.75f, 4);
    state.objects[5].position = glm::vec3(0.25, 0.75f, 4);


    /*
    //door to next level
    state.door = new Entity();
    GLuint doorTextureID = Util::LoadTexture("door.jpg");
    state.door->textureID = doorTextureID;
    state.door->mesh = wallMesh;
    state.door->position = glm::vec3(-6.0f, 1.0f, -4.1f);
    state.door->scale = glm::vec3(1.0f, 2.0f, 1.0f);
    state.door->height = 2.0f;
    state.door->entityType = DOOR;
    */

}

void Level3::Update(float deltaTime) {

    /*
    //check enemy collision
    if (state.player->CheckCollision(&state.enemies[0])) {
        state.player->health -= 1;
        state.player->position = glm::vec3(0.0f, 0.8f, 9.0f);
        state.player->rotation = glm::vec3(0);
    }
    
    //check door collision
    if (state.player->CheckCollision(state.door)) {
        state.nextScene = 2;
    }
    */
    state.player->Update(deltaTime, state.player, state.walls, LEVEL3_WALL_COUNT);

    for (int i = 0; i < LEVEL3_OBJECT_COUNT; i++) {
        state.objects[i].Update(deltaTime, state.player, state.objects, LEVEL3_OBJECT_COUNT);
    }
    for (int i = 0; i < LEVEL3_WALL_COUNT; i++) {
        state.walls[i].Update(deltaTime, state.player, state.walls, LEVEL3_WALL_COUNT);
        //state.walls[i].Update(deltaTime);
    }
    //state.door->Update(deltaTime);

    /*
    //enemy move
    if (state.enemies[0].position.x >= 2 || state.enemies[0].position.x <= -2) {
        state.enemies[0].velocity.x *= -1;
    }
    */
}

void Level3::Render(ShaderProgram* program) {

    //state.player->Render(program);
    for (int i = 0; i < LEVEL3_OBJECT_COUNT; i++) {
        state.objects[i].Render(program);
    }
    for (int i = 0; i < LEVEL3_WALL_COUNT; i++) {
        state.walls[i].Render(program);
    }
    //state.door->Render(program);

    Util::DrawText(program, fontTextureID, "You Win!", 0.4f, -0.2f, glm::vec3(-0.6f, 1.3f, 3.52f));

}