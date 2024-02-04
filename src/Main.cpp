#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Renderer.h"
#include "Rect.h"
#include "CollisionUtil.h"

// -------- GLOBALS -------- //

int windowWidth = 1500, windowHeight = 1200;

Rect movingRect(
    250.0f, 250.0f,
    180.0f, 180.0f,
    { 225 / 255.0f, 216 / 255.0f, 159 / 255.0f }
);

glm::vec2 velocity;
glm::vec2 mousePos;
bool mouseHeld = false;

// -------- CALLBACKS -------- //
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    
    // Allow user to drag the moving rectangle around
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        mouseHeld = true;
    else
        mouseHeld = false;

}

void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    velocity.x = xpos - (movingRect.x + movingRect.w / 2.0f);
    velocity.y = ypos - (movingRect.y + movingRect.h / 2.0f);

    mousePos.x = xpos;
    mousePos.y = ypos;
}

// -------- FUNCTIONS -------- //

GLFWwindow* InitWindow(int width, int height) {
    GLFWwindow* window;
    if (!glfwInit()) {
        std::cout << "Could not initialize GLFW" << std::endl;
        return nullptr;
    }

    window = glfwCreateWindow(width, height, "Swept AABB", NULL, NULL);
    if (!window) {
        std::cout << "Could not create a window" << std::endl;
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4xMSAA
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

void InitGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        std::cout << "Could not initialize GLAD" << std::endl;

    glEnable(GL_MULTISAMPLE);
    glLineWidth(2.0f);
}

// -------- MAIN ------- //

int main()
{
    GLFWwindow* window = InitWindow(windowWidth, windowHeight);
    InitGL();

    Shader shader("Shaders/shader.vs", "Shaders/shader.fs");
    Renderer renderer(windowWidth, windowHeight);

    // ----- SCENE INIT ----- //
    float fixedRectDim = 100.0f;
    Rect fixedRect{
        windowWidth / 2.0f - fixedRectDim / 2.0f, 
        windowHeight / 2.0f - fixedRectDim / 2.0f, 
        fixedRectDim, fixedRectDim,
        { 0.1f, 0.12f, 0.15f }
    };
    // ---------------------- //

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(60 / 255.0f, 96 / 255.0f, 84 / 255.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // ----- UPDATE ----- //
        glm::vec2 projectionLength;
        SweptCollisionInfo sweptInfo = SweptAABB(movingRect, fixedRect, velocity);
        if (sweptInfo.collided) {
            projectionLength = {
                velocity.x * sweptInfo.collisionTime,
                velocity.y * sweptInfo.collisionTime
            };
        }
        else {
            projectionLength = { velocity.x, velocity.y };
        }

        Rect projection{
            movingRect.x + projectionLength.x, 
            movingRect.y + projectionLength.y,
            movingRect.w, movingRect.h,
            movingRect.color
        };

        // Move the moving rect around using the mouse
        if (mouseHeld && SimplePointAABB(movingRect, mousePos)) {
            movingRect.x = mousePos.x - movingRect.w / 2.0f;
            movingRect.y = mousePos.y - movingRect.h / 2.0f;
        }
        // ------------------ //



        // ----- RENDER ----- //
        renderer.RenderRect(fixedRect, shader);
        renderer.RenderRect(movingRect, shader);
        renderer.RenderHollowRect(projection, shader);
        renderer.RenderLine(
            movingRect.x + movingRect.w / 2.0f,
            movingRect.y + movingRect.h / 2.0f,
            movingRect.x + movingRect.w / 2.0f + velocity.x,
            movingRect.y + movingRect.h / 2.0f + velocity.y,
            shader,
            { 152 / 255.0f, 193 / 255.0f, 217 /255.0f }
        );
        // ------------------ //

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}