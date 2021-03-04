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
glm::mat4 viewMatrix, ballMatrix, projectionMatrix;
glm::mat4 pad1Matrix, pad2Matrix;

//Player controls & globle variables
float paddle_speed = 2.0f;
float ball_speed = 2.5f;

glm::vec3 ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 pad1_position = glm::vec3(-4.75f, 0.0f, 0.0f); //paddle 1 starts on the left middle
glm::vec3 pad2_position = glm::vec3(4.75f, 0.0f, 0.0f); //paddle 2 starts on the right middle

glm::vec3 ball_movement = glm::vec3(0, 0, 0); //ball initially stable
glm::vec3 pad1_movement = glm::vec3(0, 0, 0); //movements of two paddles initially 0
glm::vec3 pad2_movement = glm::vec3(0, 0, 0);

//Textures
GLuint ballTextureID;
GLuint pad1TextureID, pad2TextureID;

//Function to prepare loading texture to a matrix
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
    ballMatrix = glm::mat4(1.0f);
    pad1Matrix = glm::mat4(1.0f);
    pad2Matrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    //Setting for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ballTextureID = LoadTexture("ball.png");
    pad1TextureID = LoadTexture("padBlue.png");
    pad2TextureID = LoadTexture("padRed.png");

}


bool gameStart = false; //Indication of game start, ball moving

//Globle constants for ball direction
//R = right, L = left, U = up, D = down
char direction_x = 'R';
char direction_y = 'D';

void ProcessInput() {
    pad1_movement = glm::vec3(0, 0, 0);
    pad2_movement = glm::vec3(0, 0, 0);


    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        //If not game end, process keys
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE: //Press space to start or restart the game
                if (!gameStart) {
                    ball_position = glm::vec3(0, 0, 0);
                    gameStart = true;
                }
                break;
            
            }
        }
    }

    //read keyboard state, which key is being pressed
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    //player moving paddles
    if (gameStart) {
        if (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_S]) {} //do nothing if both pressed
        else if (keys[SDL_SCANCODE_W] && pad1_position.y <= 2.80f) { pad1_movement.y = 1.0f; }
        else if (keys[SDL_SCANCODE_S] && pad1_position.y >= -2.80f) { pad1_movement.y = -1.0f; }
        if (keys[SDL_SCANCODE_UP] && keys[SDL_SCANCODE_DOWN]) {} //do nothing if both pressed
        else if (keys[SDL_SCANCODE_UP] && pad2_position.y <= 2.80f) { pad2_movement.y = 1.0f; }
        else if (keys[SDL_SCANCODE_DOWN] && pad2_position.y >= -2.80f) { pad2_movement.y = -1.0f; }
    }

    //ball bouncing (walls and paddles)
    //game end (ball reach left or right sides)
    if (ball_position.x >= 4.9f) {
        direction_x = 'L';
        gameStart = false;
    }
    else if (ball_position.x <= -4.9f) {
        direction_x = 'R';
        gameStart = false;
    }
    //on walls  (up and down)
    if (ball_position.y >= 3.6f) { direction_y = 'D'; }
    else if (ball_position.y <= -3.6f) { direction_y = 'U'; }
    //on paddles (left and right)
    if (abs(ball_position.x - pad1_position.x) <= 0.25 && abs(ball_position.y - pad1_position.y) <= 1.15) {
        direction_x = 'R';
    }
    else if (abs(ball_position.x - pad2_position.x) <= 0.25 && abs(ball_position.y - pad2_position.y) <= 1.15) {
        direction_x = 'L';
    }



    //ball movements
    if (gameStart) {
        switch (direction_x) {
        case 'R':
            ball_movement.x = 1.0f;
            break;
        case 'L':
            ball_movement.x = -1.0f;
            break;
        }
        switch (direction_y) {
        case 'U':
            ball_movement.y = 1.0f;
            break;
        case 'D':
            ball_movement.y = -1.0f;
            break;
        }
        ball_movement = glm::normalize(ball_movement);
    }
    else {
        ball_movement = glm::vec3(0, 0, 0);
    }

}

float lastTicks = 0.0f;

void Update() {
    //Get delta time
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    pad1_position += pad1_movement * paddle_speed * deltaTime;
    pad2_position += pad2_movement * paddle_speed * deltaTime;
    ball_position += ball_movement * ball_speed * deltaTime;

    //draw and move matrix
    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::translate(ballMatrix, ball_position);
    
    pad1Matrix = glm::mat4(1.0f);
    pad1Matrix = glm::translate(pad1Matrix, pad1_position);
    pad2Matrix = glm::mat4(1.0f);
    pad2Matrix = glm::translate(pad2Matrix, pad2_position);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetModelMatrix(ballMatrix);

    float ballVertices[] = { -0.15,-0.15, 0.15,-0.15, 0.15,0.15,  -0.15,-0.15, 0.15,0.15, -0.15,0.15 };
    float texCoords[] = { 0.0,1.0, 1.0,1.0, 1.0,0.0,  0.0,1.0, 1.0,0.0, 0.0,0.0 };
    float paddleVertices[] = { -0.1,-1.0, 0.1,-1.0, 0.1,1.0,  -0.1,-1.0, 0.1,1.0, -0.1,1.0 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //draw two paddles
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
    glEnableVertexAttribArray(program.positionAttribute);

    program.SetModelMatrix(pad1Matrix);
    glBindTexture(GL_TEXTURE_2D, pad1TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    program.SetModelMatrix(pad2Matrix);
    glBindTexture(GL_TEXTURE_2D, pad2TextureID);
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
