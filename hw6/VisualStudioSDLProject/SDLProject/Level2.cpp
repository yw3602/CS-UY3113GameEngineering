#include "Level2.h"


#define LEVEL2_OBJECT_COUNT 1
#define LEVEL2_ENEMY_COUNT 3

unsigned int level2Data[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,1,1,
    1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,1,
    1,0,1,1,1,1,0,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,
    1,0,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,1,1,0,1,1,
    1,1,1,0,1,1,0,1,0,0,0,0,1,0,1,1,1,0,1,1,0,1,1,
    1,0,0,0,1,1,0,1,0,1,1,0,0,0,1,1,1,0,1,1,0,1,1,
    1,1,0,1,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,1,
    1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,0,1,1,
    1,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0,1,0,0,0,1,1,
    1,0,0,1,1,0,1,1,1,1,1,0,1,0,0,1,0,1,0,1,1,1,1,
    1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,1,0,0,0,0,0,1,1,
    1,1,0,1,1,0,1,1,0,0,1,0,1,1,1,1,1,1,1,1,0,1,1,
    1,0,0,0,1,0,1,1,0,1,1,0,1,1,0,0,1,1,1,1,0,0,1,
    1,0,1,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,
    1,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,0,1,1,0,1,
    1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,
    1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,
    1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,0,1,0,0,1,
    1,0,1,0,1,1,1,1,0,1,1,1,1,0,1,0,0,0,0,1,0,1,1,
    1,0,1,0,1,1,1,1,0,1,1,0,0,0,1,1,1,1,0,0,0,1,1,
    1,0,0,0,1,1,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

int lv2wallCount() {
    int count = 0;
    for (int block : level2Data) {
        if (block == 1) {
            count += 1;
        }
    }
    return count;
}
int LEVEL2_WALL_COUNT = lv2wallCount();


glm::vec3 lv2wallPosition(int i, int width, int height) {
    int xPos = i - (i / width) * width - (width - 1) / 2;
    int zPos = i / width - (height - 1) / 2;
    glm::vec3 result = glm::vec3(xPos, 1.0f, zPos);
    return result;
}

void Level2::Initialize() {

    fontTextureID = Util::LoadTexture("font1.png");

    state.nextScene = -1;

    state.player = new Entity();
    state.player->entityType = PLAYER;

    state.player->position = glm::vec3(0.0f, 0.6f, 10.2f); //starting point
    //state.player->position = glm::vec3(1.0f, 0.6f, -9.0f); //Just in front of door

    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;

    state.player->width = 0.7f;
    state.player->height = 0.6f;
    state.player->depth = 0.7f;

    state.objects = new Entity[LEVEL2_OBJECT_COUNT];
    state.walls = new Entity[LEVEL2_WALL_COUNT];


    //floor object
    GLuint floorTextureID = Util::LoadTexture("grassfloor(1).jpg");
    Mesh* floorMesh = new Mesh();
    floorMesh->LoadOBJ("cube.obj", 16);
    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = floorMesh;
    state.objects[0].position = glm::vec3(0, -0.25f, 0);
    state.objects[0].scale = glm::vec3(22.0f, 0.5f, 22.0f);
    state.objects[0].entityType = FLOOR;

    //wall objects
    GLuint wallTextureID = Util::LoadTexture("wall.jpg");
    Mesh* wallMesh = new Mesh();
    wallMesh->LoadOBJ("cube.obj", 1);
    int i = 0; //index of wall in state.walls
    for (int mapIndex = 0; mapIndex < 529; mapIndex++) {
        if (level2Data[mapIndex] == 1) {
            state.walls[i].textureID = wallTextureID;
            state.walls[i].mesh = wallMesh;
            state.walls[i].position = lv2wallPosition(mapIndex, 23, 23);
            state.walls[i].scale = glm::vec3(1.0f, 2.0f, 1.0f);
            state.walls[i].width = 0.8f;
            state.walls[i].height = 2.0f;
            state.walls[i].depth = 0.8f;
            state.walls[i].entityType = WALL;
            i += 1;
        }
    }

    //enemies
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("enemyPic.png");
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].billboard = true;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].rotation = glm::vec3(0, 0, 0);
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
        state.enemies[i].scale = glm::vec3(0.7f, 0.7f, 0.7f);
        state.enemies[i].width = 0.7f;
        state.enemies[i].height = 0.7f;
        state.enemies[i].depth = 0.7f;
    }
    state.enemies[0].position = glm::vec3(-9.0f, 0.5f, -1.0f);
    state.enemies[0].velocity = glm::vec3(0, 0, 0.8f);

    state.enemies[1].position = glm::vec3(1.0f, 0.5f, 3.0f);
    state.enemies[1].velocity = glm::vec3(0.8f, 0, 0);

    state.enemies[2].position = glm::vec3(8.0f, 0.5f, -8.0f);
    state.enemies[2].velocity = glm::vec3(0.8f, 0, 0);


    //door to next level
    state.door = new Entity();
    GLuint doorTextureID = Util::LoadTexture("door.jpg");
    state.door->textureID = doorTextureID;
    state.door->mesh = wallMesh;
    state.door->position = glm::vec3(1.0f, 1.0f, -10.9f);
    state.door->scale = glm::vec3(1.0f, 2.0f, 1.0f);
    state.door->height = 2.0f;
    state.door->entityType = DOOR;

}

void Level2::Update(float deltaTime) {

    //check enemy collision
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        if (state.player->CheckCollision(&state.enemies[i])) {
            state.player->health -= 1;
            state.player->position = glm::vec3(0.0f, 0.6f, 10.2f);
            state.player->rotation = glm::vec3(0);
        }
    }
    //check door collision
    if (state.player->CheckCollision(state.door)) {
        state.nextScene = 3;
    }

    state.player->Update(deltaTime, state.player, state.walls, LEVEL2_WALL_COUNT);

    for (int i = 0; i < LEVEL2_OBJECT_COUNT; i++) {
        state.objects[i].Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT);
    }
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT);
    }
    for (int i = 0; i < LEVEL2_WALL_COUNT; i++) {
        state.walls[i].Update(deltaTime, state.player, state.walls, LEVEL2_WALL_COUNT);
        //state.walls[i].Update(deltaTime);
    }
    state.door->Update(deltaTime);

    //enemy move
    if (state.enemies[0].position.z >= 1 || state.enemies[0].position.z <= -4) {
        state.enemies[0].velocity.z *= -1;
    }
    if (state.enemies[1].position.x >= 6 || state.enemies[1].position.x <= 1) {
        state.enemies[1].velocity.x *= -1;
    }
    if (state.enemies[2].position.x >= 10 || state.enemies[2].position.x <= 7) {
        state.enemies[2].velocity.x *= -1;
    }
}

void Level2::Render(ShaderProgram* program) {

    //state.player->Render(program);
    for (int i = 0; i < LEVEL2_OBJECT_COUNT; i++) {
        state.objects[i].Render(program);
    }
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
    for (int i = 0; i < LEVEL2_WALL_COUNT; i++) {
        state.walls[i].Render(program);
    }
    state.door->Render(program);

    Util::DrawText(program, fontTextureID, "Remaining Life:" + std::to_string(state.player->health), 0.1f, -0.05f, glm::vec3(-0.4f, 1.0f, 8.52f));
    if (state.player->health == 0) {
        Util::DrawText(program, fontTextureID, "You Lose!", 0.2f, -0.1f, glm::vec3(-0.4f, 0.6f, 8.52f));
    }

}