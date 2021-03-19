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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <vector>

#define SURFACE_COUNT 8
#define WALL_COUNT 16
#define METEOR_COUNT 3

struct GameState {
    Entity* player;
    Entity* surface;
    Entity* walls;
    Entity* meteors;
    Entity* platform;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;
bool success;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            });
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
            });
    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    fontTextureID = LoadTexture("font1.png");
    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0.0f, 3.75f, 0.0f);
    state.player->movement = glm::vec3(0);
    
    state.player->acceleration = glm::vec3(0.0f, -0.5f, 0.0f);

    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("spaceship.png");
    state.player->jumpForce = 1.5f;
    state.player->fuel = 3;

    state.surface = new Entity[SURFACE_COUNT];
    GLuint surfaceTextureID = LoadTexture("lunerSurface.png");
    for (int i = 0; i < SURFACE_COUNT; i++) {
        state.surface[i].position = glm::vec3(i - 3.5f, -3.25f, 0.0f);
        state.surface[i].textureID = surfaceTextureID;
    }
    for (int i = 0; i < SURFACE_COUNT; i++) {
        state.surface[i].Update(0, NULL, 0);
    }

    GLuint wallTextureID = LoadTexture("lunerRock.png");
    state.walls = new Entity[WALL_COUNT];
    for (int i = 0; i < WALL_COUNT/2; i++) {
        state.walls[i].position = glm::vec3(-4.5f, i - 3.25f, 0.0f);
        state.walls[i].textureID = wallTextureID;
        state.walls[i].Update(0, NULL, 0);
    }
    for (int i = WALL_COUNT / 2; i < WALL_COUNT; i++) {
        state.walls[i].position = glm::vec3(4.5f, i - WALL_COUNT / 2 - 3.25f, 0.0f);
        state.walls[i].textureID = wallTextureID;
        state.walls[i].Update(0, NULL, 0);
    }

    state.meteors = new Entity[METEOR_COUNT];
    GLuint meteorTextureID = LoadTexture("meteor.png");
    state.meteors[0].position = glm::vec3(0.8f, 0.0f, 0.0f);
    state.meteors[1].position = glm::vec3(-3.0f, 2.3f, 0.0f);
    state.meteors[2].position = glm::vec3(3.5f, 1.5f, 0.0f);
    for (int i = 0; i < METEOR_COUNT; i++) {
        state.meteors[i].width = 0.8f;
        state.meteors[i].height = 0.8f;
        state.meteors[i].textureID = meteorTextureID;
        state.meteors[i].Update(0, NULL, 0);
    }

    state.platform = new Entity[2];
    GLuint platformTextureID = LoadTexture("landingPlatform.png");
    state.platform[0].position = glm::vec3(0.2f, -3.25f, 0.0f);
    state.platform[1].position = glm::vec3(1.0f, -3.25f, 0.0f);
    state.platform[0].textureID = platformTextureID;
    state.platform[1].textureID = platformTextureID;
    state.platform[0].Update(0, NULL, 0);
    state.platform[1].Update(0, NULL, 0);


    bool success = false;
}

void ProcessInput() {

    state.player->acceleration = glm::vec3(0.0f, -0.5f, 0.0f);

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
                if (state.player->isActive) {
                    if (state.player->fuel > 0 && state.player->position.y <= 3.75) {
                        state.player->fuel -= 1;
                        state.player->jump = true;
                        break;
                    }
                }
                else {
                    state.player->isActive = true;
                    state.player->velocity = glm::vec3(0);
                    state.player->position = glm::vec3(0.0f, 3.75f, 0.0f);
                    state.player->fuel = 3;
                }
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT] && keys[SDL_SCANCODE_RIGHT]) {}
    else if (keys[SDL_SCANCODE_LEFT]) {
        //state.player->movement.x = -1.0f;
        state.player->acceleration.x = -1.5f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        //state.player->movement.x = 1.0f;
        state.player->acceleration.x = 1.5f;
        state.player->animIndices = state.player->animRight;
    }
    
    //state.player->velocity.x -= (0.05f * state.player->velocity.x);


    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
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
        state.player->Update(FIXED_TIMESTEP, state.surface, SURFACE_COUNT);

        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    //render all the entities
    for (int i = 0; i < SURFACE_COUNT; i++) {
        state.surface[i].Render(&program);
    }
    for (int i = 0; i < WALL_COUNT; i++) {
        state.walls[i].Render(&program);
    }
    for (int i = 0; i < METEOR_COUNT; i++) {
        state.meteors[i].Render(&program);
    }
    state.platform[0].Render(&program);
    state.platform[1].Render(&program);

    //check all collision
    for (int i = 0; i < WALL_COUNT; i++) {
        if (state.player->CheckCollision(&state.walls[i])) {
            state.player->isActive = false;
            success = false;
        }
    }
    for (int i = 0; i < METEOR_COUNT; i++) {
        if (state.player->CheckCollision(&state.meteors[i])) {
            state.player->isActive = false;
            success = false;
        }
    }
    for (int i = 0; i < SURFACE_COUNT; i++) {
        if (state.player->CheckCollision(&state.surface[i])) {
            state.player->isActive = false;
            success = false;
            if (state.player->position.x >= state.platform[0].position.x &&
                state.player->position.x <= state.platform[1].position.x) {
                success = true;
            }
        }
    }

    if (state.player->isActive) {
        state.player->Render(&program);
    }
    else {
        state.player->movement = glm::vec3(0);
        state.player->Render(&program);

        if (success) {
            DrawText(&program, fontTextureID, "Mission Successful", 0.8f, -0.4f, glm::vec3(-3.15f, 0.3f, 0.0f));
        }
        else if (!success) {
            DrawText(&program, fontTextureID, "Mission Failed", 0.8f, -0.4f, glm::vec3(-2.45f, 0.3f, 0.0f));
        }

        DrawText(&program, fontTextureID, "Tap Space to Restart", 0.6f, -0.3f, glm::vec3(-2.7f, -2.0f, 0.0f));
    }

    DrawText(&program, fontTextureID, "Fuel:" + std::to_string(state.player->fuel) + " (space)"
        , 0.5f, -0.25f, glm::vec3(-4.7f, 3.45f, 0.0f));
    

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
        Render();
    }

    Shutdown();
    return 0;
}
