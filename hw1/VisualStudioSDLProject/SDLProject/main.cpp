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


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, playerMatrix, projectionMatrix;
glm::mat4 rock1Matrix, rock2Matrix, rock3Matrix;

//Player controls
float player_x = 0.0f;
float player_y = 0.0f;
float rock2_x = 0.0f;
float rock3_rotate = 0.0f;
float rock3_scale = 1.0f;
//std::string playerDirection_x = "right";
std::string playerDirection_y = "up";
std::string rock2Direction_x = "right";
std::string rock3Scaling = "mag";
//float player_rotate = 0.0f;
GLuint playerTextureID;
GLuint rock1TexID, rock2TexID;

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

void Initialize() {

    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    // Load the shaders for handling textures!
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);

    //Initialize all object matrices
    playerMatrix = glm::mat4(1.0f);
    rock1Matrix = glm::mat4(1.0f);
    rock2Matrix = glm::mat4(1.0f);
    rock3Matrix = glm::mat4(1.0f);

    //non-moving rock1
    rock1Matrix = glm::translate(rock1Matrix, glm::vec3(-3.5f, -0.8f, 0.0f));

    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.3f, 0.6f, 0.4f, 1.0f);
    glEnable(GL_BLEND);
    //Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    playerTextureID = LoadTexture("ctg.png");
    rock1TexID = LoadTexture("rock1.png");
    rock2TexID = LoadTexture("rock2.png");

}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() {
    //Get delta time
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    
    //X component of player movement
    if (player_x >= 13.0f) {
        player_x = 0.0f;
        player_y = 0.0f;
    }
    player_x += 1.5f * deltaTime;
    //Y component of player movement
    if (player_y >= 1.0f) { playerDirection_y = "down"; }
    else if (player_y <= 0.0f) { playerDirection_y = "up"; }
    if (playerDirection_y == "up") { player_y += (2.0f - player_y * player_y * 1.9f) * deltaTime; }
    else if (playerDirection_y == "down") { player_y += (-2.0f + player_y * player_y * 1.9f) * deltaTime; }
    //player movements
    playerMatrix = glm::mat4(1.0f);
    playerMatrix = glm::translate(playerMatrix, glm::vec3(player_x - 7.0f, player_y, 0.0f));

    //rock2 movements
    if (rock2_x >= 1.0f) { rock2Direction_x = "left"; }
    else if (rock2_x <= 0.0f) { rock2Direction_x = "right"; }
    if (rock2Direction_x == "right") { rock2_x += 1.0f * deltaTime; }
    else if (rock2Direction_x == "left") { rock2_x += -1.0f * deltaTime; }
    rock2Matrix = glm::mat4(1.0f);
    rock2Matrix = glm::translate(rock2Matrix, glm::vec3(rock2_x - 0.8f, -0.8f, 0.0f));
    
    //rock3 movements
    rock3_rotate += 120.0f * deltaTime;
    if (rock3_scale >= 1.5f) { rock3Scaling = "min"; }
    else if (rock3_scale <= 0.8f) { rock3Scaling = "mag"; }
    if (rock3Scaling == "mag") { rock3_scale += 0.125f * deltaTime; }
    else if (rock3Scaling == "min") { rock3_scale += -0.125f * deltaTime; }
    rock3Matrix = glm::mat4(1.0f);
    rock3Matrix = glm::translate(rock3Matrix, glm::vec3(3.2f, -0.8f, 0.0f));
    rock3Matrix = glm::scale(rock3Matrix, glm::vec3(rock3_scale, rock3_scale, 0.0f));
    rock3Matrix = glm::rotate(rock3Matrix, glm::radians(rock3_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    

    float player[] = { -0.5,-0.5, 0.5,-0.5, 0.5,0.5,  -0.5,-0.5, 0.5,0.5, -0.5,0.5 };
    float playerTex[] = { 0.0,1.0, 1.0,1.0, 1.0,0.0,  0.0,1.0, 1.0,0.0, 0.0,0.0 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, player);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, playerTex);
    glEnableVertexAttribArray(program.texCoordAttribute);

    //draw player
    program.SetModelMatrix(playerMatrix);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //draw rocks
    float rocks[] = { -0.4,-0.4, 0.4,-0.4, 0.4,0.4,  -0.4,-0.4, 0.4,0.4, -0.4,0.4 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, rocks);
    glEnableVertexAttribArray(program.positionAttribute);

    program.SetModelMatrix(rock1Matrix);
    glBindTexture(GL_TEXTURE_2D, rock1TexID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(rock2Matrix);
    glBindTexture(GL_TEXTURE_2D, rock2TexID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(rock3Matrix);
    glBindTexture(GL_TEXTURE_2D, rock1TexID);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

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
