#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>

#include "Util.h"
#include "Entity.h"

#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"



SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

#define NUMBER_OF_LEVELS 3 //not including menu

//aduio
Mix_Music* bgm;
Mix_Chunk* doorSound;
Mix_Chunk* winSound;
Mix_Chunk* loseSound;
Mix_Chunk* damageSound;

//global variables
Scene* currentScene;
Scene* sceneList[NUMBER_OF_LEVELS + 1];
int currentLevel;
int playerHealth;
GLuint fontTextureID;
bool win;
bool lose;


void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("3D!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 1280, 720);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::perspective(glm::radians(50.0f), 1.777f, 0.1f, 100.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(program.programID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.3f, 0.55f, 0.9f, 1.0f);
    
    fontTextureID = Util::LoadTexture("font1.png");

    
    //audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    bgm = Mix_LoadMUS("bgm0.mp3");
    Mix_PlayMusic(bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 3);
    doorSound = Mix_LoadWAV("woodenDoor.wav");
    winSound = Mix_LoadWAV("win.wav");
    loseSound = Mix_LoadWAV("lose.wav");
    damageSound = Mix_LoadWAV("damage.wav");
    

    //global variables for entire game
    currentLevel = 0; //start from menu
    playerHealth = 3;
    win = false;
    lose = false;


    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();

    currentScene = sceneList[0];
    SwitchToScene(currentScene);

    currentScene->state.player->health = playerHealth;

}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                // Some sort of action
                break;
            case SDLK_RETURN:
                if (currentLevel == 0) {
                    currentScene->state.player->velocity = glm::vec3(0, 0, -3.4f);
                    //currentScene->state.nextScene = 1;
                }
                /*
                if (lose || (currentLevel == NUMBER_OF_LEVELS && win)) {  //restart game
                    /*
                    playerHealth = 5;
                    currentLevel = 0;
                    for (Scene* levels : sceneList) {
                        levels->state.nextScene = -1;
                    }
                    lose = false;
                    win = false;
                    SwitchToScene(sceneList[0]);
                    
                }
                */
                break;
            }
            break;
        }
    }

    if (currentLevel != 0 && playerHealth != 0) {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        //perspective
        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->rotation.y += 1.5f;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->rotation.y -= 1.5f;
        }

        currentScene->state.player->velocity.x = 0;
        currentScene->state.player->velocity.z = 0;
        //move forward and back
        if (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_S]) {
            //do nothing when W and S both pressed
        }
        else if (keys[SDL_SCANCODE_W]) {
            currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
            currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
        }
        else if (keys[SDL_SCANCODE_S]) {
            currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
            currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
        }

        //move left and right
        if (keys[SDL_SCANCODE_A] && keys[SDL_SCANCODE_D]) {
            //do nothing when W and S both pressed
        }
        else if (keys[SDL_SCANCODE_D]) {
            currentScene->state.player->velocity.x = -cos(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
            currentScene->state.player->velocity.z = sin(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
        }
        else if (keys[SDL_SCANCODE_A]) {
            currentScene->state.player->velocity.x = -cos(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
            currentScene->state.player->velocity.z = sin(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
        }
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
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    if (playerHealth != currentScene->state.player->health) {
        playerHealth = currentScene->state.player->health;
        Mix_PlayChannel(-1, damageSound, 0);
    }

    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(currentScene->state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -currentScene->state.player->position);

    //win|lose detection
    if (playerHealth == 0 && lose == false) {
        lose = true;
        Mix_PlayChannel(-1, loseSound, 0);
        currentScene->state.player->speed = 0;
    }

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    /*
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    */
    while (gameIsRunning) {
        ProcessInput();
        Update();

        //change scene
        if (currentScene->state.nextScene >= 0 && win == false) {

            if (currentScene->state.nextScene == NUMBER_OF_LEVELS + 1) {
                win = true; //completion of the last level
                Mix_PlayChannel(-1, winSound, 0);
                //currentScene->state.player->speed = 0;
            }
            else {
                Mix_PlayChannel(-1, doorSound, 0);
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
