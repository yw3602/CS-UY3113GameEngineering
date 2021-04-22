#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <vector>
#include <SDL_mixer.h>


#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"


GLuint fontTextureID;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

//aduio
Mix_Music* bgm;
Mix_Chunk* stepSound;
Mix_Chunk* winSound;
Mix_Chunk* loseSound;
Mix_Chunk* damageSound;


//global variables
Scene* currentScene;
Scene* sceneList[4];
int currentLevel;
int playerHealth;
glm::vec3 healthTextPosition;
bool win = false;
bool lose = false;


#define NUMBER_OF_LEVELS 3 //not including the menu

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("George's Advanture", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fontTextureID = Util::LoadTexture("font1.png");

    //audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    bgm = Mix_LoadMUS("bgm1.mp3");
    Mix_PlayMusic(bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 3);
    stepSound = Mix_LoadWAV("step_on_enemy.wav");
    winSound = Mix_LoadWAV("win.wav");
    loseSound = Mix_LoadWAV("lose.wav");
    damageSound = Mix_LoadWAV("damage.wav");

    //global variables for entire game
    currentLevel = 0;
    playerHealth = 3;

    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);

    currentScene->state.player->health = playerHealth;

}

void ProcessInput() {

    currentScene->state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:
                if (currentScene->state.player->collidedBottom) {
                    currentScene->state.player->jump = true;
                }
                break;
            case SDLK_RETURN:
                if (currentLevel == 0) {
                    currentScene->state.nextScene = 1;
                }
                if (lose || (currentLevel == NUMBER_OF_LEVELS && win)) {  //restart game
                    playerHealth = 3;
                    currentLevel = 0;
                    for (Scene* levels : sceneList) {
                        levels->state.nextScene = -1;
                    }
                    lose = false;
                    win = false;
                    SwitchToScene(sceneList[0]);
                }
                break;
            }

            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }


    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {

    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;

    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        currentScene->Update(FIXED_TIMESTEP);

        deltaTime -= FIXED_TIMESTEP;

    }

    if (playerHealth != currentScene->state.player->health) {
        playerHealth = currentScene->state.player->health;
        Mix_PlayChannel(-1, damageSound, 0);
    }

    /*
    for (int i = 0; i < 1; i++) {
        if (currentScene->state.player->CheckCollision(&currentScene->state.enemies[i])) {
            if (currentScene->state.player->velocity.y < 0
                && currentScene->state.player->position.y > currentScene->state.enemies[i].position.y) {
                
                //currentScene->state.player->velocity = glm::vec3(0);
                currentScene->state.player->jump = true;
                Mix_PlayChannel(-1, stepSound, 0);
                currentScene->state.enemies[i].isActive = false;
            }
            else {
                playerHealth -= 1;
                Mix_PlayChannel(-1, damageSound, 0);
                currentScene->state.player->position = glm::vec3(1, 0, 0);
            }
        }
    }
    */


    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix,
            glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        healthTextPosition = currentScene->state.player->position;
        healthTextPosition.x -= 4.5;
        healthTextPosition.y = -0.5;
    }
    else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        healthTextPosition = currentScene->state.player->position;
        healthTextPosition.x = 0.5;
        healthTextPosition.y = -0.5;
    }

    if (currentScene->state.player->position.y <= -14) {
        playerHealth -= 1;
        currentScene->state.player->health = playerHealth;
        Mix_PlayChannel(-1, damageSound, 0);
        currentScene->state.player->position = glm::vec3(1, 0, 0);
    }


    //win|lose detection
    if (playerHealth == 0 && lose == false) {
        lose = true;
        Mix_PlayChannel(-1, loseSound, 0);
        currentScene->state.player->isActive = false;
    }


}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);

    Util::DrawText(&program, fontTextureID, "Life: " + std::to_string(playerHealth), 0.4f, -0.2f, healthTextPosition);

    //win|lose detection
    if (lose) {
        Util::DrawText(&program, fontTextureID, "You Lose", 0.7f, -0.35f, glm::vec3(healthTextPosition.x + 3, healthTextPosition.y - 3.3, 0));
        Util::DrawText(&program, fontTextureID, "Press Enter to Restart", 0.5f, -0.25f, glm::vec3(healthTextPosition.x + 2.2f, healthTextPosition.y - 4.5f, 0));
    }
    else if (win) {
        Util::DrawText(&program, fontTextureID, "You Win!", 0.7f, -0.35f, glm::vec3(healthTextPosition.x + 3, healthTextPosition.y - 3.3, 0));
        Util::DrawText(&program, fontTextureID, "Press Enter to Restart", 0.5f, -0.25f, glm::vec3(healthTextPosition.x + 2.2f, healthTextPosition.y - 4.5f, 0));
    }

    //Util::DrawText(&program, fontTextureID, std::to_string(currentLevel), 0.7f, -0.35f, currentScene->state.player->position);

    SDL_GL_SwapWindow(displayWindow);

}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();

        //change scene
        if (currentScene->state.nextScene >= 0 && win == false) {

            if (currentScene->state.nextScene == NUMBER_OF_LEVELS + 1) {
                win = true; //completion of the last level
                Mix_PlayChannel(-1, winSound, 0);
                currentScene->state.player->speed = 0;
                currentScene->state.player->jumpPower = 0;
            }
            else {
                playerHealth = currentScene->state.player->health;
                SwitchToScene(sceneList[currentScene->state.nextScene]);
                currentScene->state.player->health = playerHealth;
                currentLevel += 1;
            }
        }

        Render();
    }

    Shutdown();
    return 0;
}
