// Kursovaya_Tukhvatullina_Otlozhennoe_osveshcenie.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define _USE_MATH_DEFINES 
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime> 
#include <vector>
#include <stb_image_aug.h>

#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "3dMath.h"


using namespace std;

//Константы
bool shadows = true;
bool shadowsKeyPressed = false;
const GLuint WIDTH = 800, HEIGHT = 600;
Camera camera(vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;//Начальное положение мыши
//углы поворота камеры
GLfloat cam_yaw = -90.0f;
GLfloat cam_pitch = 0.0f;
GLfloat fov = 45.0f;
GLfloat deltaTime = 0.0f;	// Время, прошедшее между последним и текущим кадром
GLfloat lastFrame = 0.0f;  	// Время вывода последнего кадра
bool drawScene=false;
vec3 ObjPosition = vec3(0);
int sceneNum = 1;

//Функции
void renderCube();

bool keys[1024];//отслеживание нажатия кнопок
void do_movement()
{

    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_KEY_LEFT_SHIFT])
        camera.MovementSpeed = 10.0f;
    else
        camera.MovementSpeed = 3.0f;

    if (keys[GLFW_KEY_1]) {
        cout << "Scene with direction light" << endl;
        sceneNum = 1;
        camera.SetCamPosition(vec3(0.0f, 2.0f, 0.0f));
    }
    if (keys[GLFW_KEY_2]) {
        cout << "Scene with point light" << endl;
        sceneNum = 2;
        camera.SetCamPosition(vec3(0.0f, 2.0f, 0.0f));
    }

}

unsigned int loadTexture(const char* path);
void renderDirLightScene(const Shader& shader);
void renderPointLightScene(const Shader& shader);

bool mouseset = false;
void Keyboards(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true, 
    // и приложение после этого закроется
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        if (!mouseset) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//Захват мыши
            mouseset = true;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//Захват мыши
            mouseset = false;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed)
    {
        shadows = !shadows;
        shadowsKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        shadowsKeyPressed = false;
    }

    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;

}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void renderQuad();

unsigned int planeVAO;


int ModelDir = 0;
void moveModel(Model& someModel, float sc) {
    if (keys[GLFW_KEY_KP_8]) {
        ObjPosition.x += 1.0f * sc;
        someModel.trans(vec3(1.0f * sc, 0, 0));
        cout << "Key 8 PRESSED" << endl;
    }
    if (keys[GLFW_KEY_KP_2]) {
        ObjPosition.x -= 1.0f * sc;
        someModel.trans(vec3(-1.0f * sc, 0, 0));
        cout << "Key 2 PRESSED" << endl;
    }
    if (keys[GLFW_KEY_KP_6]) {
        ObjPosition.z += 1.0f * sc;
        someModel.trans(vec3(0, 0, 1.0f * sc));
        cout << "Key 6 PRESSED" << endl;
    }
    if (keys[GLFW_KEY_KP_4]) {
        ObjPosition.z -= 1.0f * sc;
        someModel.trans(vec3(0, 0, -1.0f * sc));
        cout << "Key 4 PRESSED" << endl;
    }
    if (keys[GLFW_KEY_ENTER]) {
        cout << ObjPosition.x << "  " << ObjPosition.y << "  " << ObjPosition.z << endl;
        cout << ModelDir << endl;
    }
    if (keys[GLFW_KEY_KP_ADD]) {
        someModel.Rotate(1, vec3(0, 1.0f * sc, 0));
        ModelDir += 1;
        cout << "Key + PRESSED" << endl;
    }
    if (keys[GLFW_KEY_KP_SUBTRACT]) {
        someModel.Rotate(-1, vec3(0, 1.0f * sc, 0));
        ModelDir -= 1;
        cout << "Key - PRESSED" << endl;
    }
    if (keys[GLFW_KEY_UP]) {
        ObjPosition.y += 1.0f * sc;
        someModel.trans(vec3(0, 1.0f * sc, 0));
        cout << "Key UP PRESSED" << endl;
    }
    if (keys[GLFW_KEY_DOWN]) {
        ObjPosition.y -= 1.0f * sc;
        someModel.trans(vec3(0, -1.0f * sc, 0));
        cout << "Key DOWN PRESSED" << endl;
    }
}

Model streetLamp;
Model StopSign;
Model Build;
Model Build2;
Model Alucy;
Model StreetBench;
Model table;
unsigned int floorDiff;
unsigned int floorSpec;
unsigned int floorNRM;

//направленный источник света
void LoadModelsScene1() {
    streetLamp = Model("Content\\obj\\streetLight\\streetlamp.obj", vec3(1.9f,0, -11.7f));
    streetLamp.Rotate(352, vec3(0, 1, 0));
    streetLamp.Scale(vec3(0.5/2));

    StopSign = Model("Content\\obj\\StopSign\\stopsign1.obj", vec3(5.1f,0,-11.85f));
    StopSign.Rotate(300, vec3(0, 1, 0));
    StopSign.Scale(vec3(0.75/2));

    table = Model("Content\\obj\\Table\\Wood_Table.obj", vec3(3.6f,0,-9.0f));
    table.Scale(vec3(2.0f));
    table.Rotate(56, vec3(0, 1, 0));

    //Build=Model("Content\\obj\\Buildings\\Buildings\\Build1_3_2.obj", vec3(0));
    //Build.Scale(vec3(1.0/2));

   // Build2 = Model("Content\\obj\\Buildings\\Buildings\\Build2.obj", vec3(8,0,0));
    //Build2.Scale(vec3(1.0/2));

    Alucy = Model("Content\\obj\\Alucy\\Alucy2.obj", vec3(11.54f,0.0f,-13.46f));
    Alucy.Scale(vec3(0.2f/2));
    Alucy.Rotate(28, vec3(0, 1, 0));

    StreetBench = Model("Content\\obj\\StreetBench\\streetBench.obj", vec3(7.2f,0,-7.2f));
    //StreetBench.Rotate(166, vec3(0, 1, 0));
    StreetBench.Scale(vec3(0.08/2));
    StreetBench.Rotate(218, vec3(0, 1, 0));
   
}
void DrawMyModelsScene1(Shader shader) {
    streetLamp.Draw(shader, drawScene);
    StopSign.Draw(shader, drawScene);
    //Build.Draw(shader, drawScene);
    //Build2.Draw(shader, drawScene);
    Alucy.Draw(shader, drawScene);
    StreetBench.Draw(shader, drawScene);
    table.Draw(shader, drawScene);
    //moveModel(table, 0.1f);
}


Shader shader;
Shader simpleDepthShader;
Shader debugDepthQuad;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
unsigned int depthMapFBO;
unsigned int depthMap;

void initScene1() {
    // build and compile shaders
      // -------------------------
    shader=Shader("shadow_mapping.vs", "shadow_mapping.fs");
    simpleDepthShader= Shader("shadow_mapping_depth.vs", "shadow_mapping_depth.fs");
    debugDepthQuad= Shader("debug_quad.vs", "debug_quad_depth.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, 0,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, 0,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, 0, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, 0,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, 0, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, 0, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);



    // load textures
    // -------------
    floorDiff = loadTexture("Content\\textures\\brick2.jpg");
    floorSpec = loadTexture("Content\\textures\\brick2_spec.png");
    floorNRM = loadTexture("Content\\textures\\brick2_NRM.png");

    // configure depth map FBO
    // -----------------------
    
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    // shader configuration
    // --------------------
    shader.Use();
    shader.setInt("shadowMap", 1);
    debugDepthQuad.Use();
    debugDepthQuad.setInt("depthMap", 0);

    LoadModelsScene1();
}

void DrawScene1() {
    drawScene = false;
    // render
       // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. render depth of scene to texture (from light's perspective)
    // --------------------------------------------------------------
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 0.1f, far_plane = 20.0f;
    lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
    lightView = glm::lookAt(vec3(2.0f, 10.0f, 3.0f) + camera.Position, vec3(0) + camera.Position, glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    // render scene from light's point of view
    simpleDepthShader.Use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderDirLightScene(simpleDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. render scene as normal using the generated depth/shadow map  
    // --------------------------------------------------------------

    drawScene = true;
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.Use();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    // set light uniforms
    shader.setVec3("viewPos", camera.Position);
    // directional light
    shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    shader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);

    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glActiveTexture(GL_TEXTURE0);
    renderDirLightScene(shader);


    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    debugDepthQuad.Use();
    debugDepthQuad.setFloat("near_plane", near_plane);
    debugDepthQuad.setFloat("far_plane", far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    //renderQuad();
}


Model table2;
Model room;
Model flower;
Model TV;
Model tumb;
unsigned int roomDiff;
unsigned int roomSpec;
unsigned int roomNRM;
void LoadModelsScene2() {
    table2 = Model("Content\\obj\\Table\\Wood_Table.obj", vec3(0,0,0));
    
    room = Model("Content\\obj\\room\\room2.obj", vec3(0, -0.05f, 0));
    room.Scale(vec3(2.0f));

    flower = Model("Content\\obj\\alocasia\\flower2.obj", vec3(-1.92f, 0, 3.14f));
    flower.Scale(vec3(0.002f, 0.002f, 0.002f));
    flower.Rotate(32, vec3(0, 1, 0));

    TV = Model("Content\\obj\\TV\\MyTV2.obj", vec3(2.24f, 0.56f, -2.48f));
    TV.Scale(vec3(0.2));
    TV.Rotate(-56, vec3(0, 1, 0));

    tumb = Model("Content\\obj\\tumb\\tumb.obj", vec3(-2.52,1.74,-2.1));
    tumb.Scale(vec3(0.3f));
    tumb.Rotate(38, vec3(0.1, 1, 0));

}
void DrawMyModelsScene2(Shader shader) {
    glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    shader.setInt("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
    room.Draw(shader, drawScene);
    shader.setInt("reverse_normals", 0); // and of course disable it
    glEnable(GL_CULL_FACE);


    table2.Draw(shader, drawScene);

    flower.Draw(shader, drawScene);

    glDisable(GL_CULL_FACE);
    TV.Draw(shader, drawScene);
    glEnable(GL_CULL_FACE);

    tumb.Draw(shader, drawScene);

    //moveModel(tumb, 0.1f);

}

unsigned int depthMapFBO2;
const unsigned int SHADOW_WIDTH2 = 1024, SHADOW_HEIGHT2 = 1024;
unsigned int depthCubemap;
glm::vec3 lightPos(0.0f, 2.0f, 0.0f);

Shader shader2;
Shader simpleDepthShader2;
Shader LampShad;
void initScene2() {
    LampShad = Shader("lampShader.vs", "lampShader.fs");
    shader2=Shader("point_shadows.vs", "point_shadows.fs");
    simpleDepthShader2=Shader("point_shadows_depth.vs", "point_shadows_depth.fs", "point_shadows_depth.gs");

    roomDiff = loadTexture("Content\\textures\\brick1.jpg");
    roomSpec = loadTexture("Content\\textures\\brick1_SPEC.jpg");
    roomNRM = loadTexture("Content\\textures\\brick1_NRM.jpg");

    
    
    glGenFramebuffers(1, &depthMapFBO2);
    // create depth cubemap texture
    
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH2, SHADOW_HEIGHT2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO2);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shader configuration
    // --------------------
    shader2.Use();
    shader2.setInt("diffuseTexture", 0);
    shader2.setInt("depthMap", 1);

    LoadModelsScene2();

}
void DrawScene2() {
    drawScene = false;
    lightPos.z = sin(glfwGetTime() * 0.5) * 3.0;

    // render
        // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 0. create depth cubemap transformation matrices
    // -----------------------------------------------
    float near_plane = 1.0f;
    float far_plane = 30.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH2 / (float)SHADOW_HEIGHT2, near_plane, far_plane);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    // 1. render scene to depth cubemap
    // --------------------------------
    glViewport(0, 0, SHADOW_WIDTH2, SHADOW_HEIGHT2);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO2);
    glClear(GL_DEPTH_BUFFER_BIT);
    simpleDepthShader2.Use();
    for (unsigned int i = 0; i < 6; ++i)
        simpleDepthShader2.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    simpleDepthShader2.setFloat("far_plane", far_plane);
    simpleDepthShader2.setVec3("lightPos", lightPos);
    renderPointLightScene(simpleDepthShader2);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    drawScene = true;
    // 2. render scene as normal 
    // -------------------------
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader2.Use();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shader2.setMat4("projection", projection);
    shader2.setMat4("view", view);

    // set lighting uniforms
    shader2.setVec3("pointLight.position", lightPos);
    shader2.setVec3("pointLight.ambient", vec3(0.3f));
    shader2.setVec3("pointLight.diffuse", vec3(0.8f));
    shader2.setVec3("pointLight.specular",vec3(0.3f));
    shader2.setFloat("pointLight.constant", 0.1f);
    shader2.setFloat("pointLight.linear", 0.009f);
    shader2.setFloat("pointLight.quadratic", 0.005f);


    shader2.setVec3("viewPos", camera.Position);
    shader2.setFloat("far_plane", far_plane);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    glActiveTexture(GL_TEXTURE0);
    renderPointLightScene(shader2);


    LampShad.Use();
    LampShad.setMat4("projection", projection);
    LampShad.setMat4("view", view);
    mat4 model(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, vec3(0.2f));
    LampShad.setMat4("model", model);
    LampShad.setVec3("lightColor", vec3(0.8f));
    renderCube();

}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    //Тут GLFW
    glfwInit();//инициализация glfw

    //Задается минимальная требуемая версия OpenGL (минор и мажор)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Установка профайла для которого создается контекст
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//Возможность изменение окна (TRUE/FALSE)

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Kursovaya_Shadows_Kharisov", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, Keyboards);//нажатия кнопок
    glfwSetCursorPosCallback(window, mouse_callback);//Движение мыши
    glfwSetScrollCallback(window, scroll_callback);//Колесико

    //ТУТ GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    //даем понять glew размеры окна glfw
    glViewport(0, 0, width, height);


    glEnable(GL_DEPTH_TEST);

    initScene1();
    initScene2();
   

    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        do_movement();

        switch (sceneNum )
        {
        case 1: {
            DrawScene1();
            break;
        }
        case 2: {
            DrawScene2();
            break;
        }
        }
        
       
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


// renders the 3D scene
// --------------------
void renderDirLightScene(const Shader& shader)
{
    // floor
    glDisable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(shader.Program, "material.diffuseMap1"),  2);
    glBindTexture(GL_TEXTURE_2D, floorDiff);

    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(shader.Program, "material.specularMap1"), 3);
    glBindTexture(GL_TEXTURE_2D, floorSpec);

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(shader.Program, "material.normalMap1"), 4);
    glBindTexture(GL_TEXTURE_2D, floorNRM);
    

    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glActiveTexture(GL_TEXTURE0);

    DrawMyModelsScene1(shader);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void renderPointLightScene(const Shader& shader)
{
    //// room cube
    //
    //glActiveTexture(GL_TEXTURE2);
    //glUniform1i(glGetUniformLocation(shader.Program, "material.diffuseMap1"), 2);
    //glBindTexture(GL_TEXTURE_2D, roomDiff);

    //glActiveTexture(GL_TEXTURE3);
    //glUniform1i(glGetUniformLocation(shader.Program, "material.specularMap1"), 3);
    //glBindTexture(GL_TEXTURE_2D, roomSpec);

    //glActiveTexture(GL_TEXTURE4);
    //glUniform1i(glGetUniformLocation(shader.Program, "material.normalMap1"), 4);
    //glBindTexture(GL_TEXTURE_2D, roomNRM);

    //glm::mat4 model = glm::mat4(1.0f);
    //model = glm::scale(model, glm::vec3(5.0f));
    //shader.setMat4("model", model);
    //glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    //shader.setInt("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
    //renderCube();
    //shader.setInt("reverse_normals", 0); // and of course disable it
    //glEnable(GL_CULL_FACE);
    //glActiveTexture(GL_TEXTURE0);
    //// cubes
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
    //model = glm::scale(model, glm::vec3(0.5f));
    //shader.setMat4("model", model);
    //renderCube();
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
    //model = glm::scale(model, glm::vec3(0.75f));
    //shader.setMat4("model", model);
    //renderCube();
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
    //model = glm::scale(model, glm::vec3(0.5f));
    //shader.setMat4("model", model);
    //renderCube();
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
    //model = glm::scale(model, glm::vec3(0.5f));
    //shader.setMat4("model", model);
    //renderCube();
    
    


    DrawMyModelsScene2(shader);
}

