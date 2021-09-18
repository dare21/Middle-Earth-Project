#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <cstdlib>
#include <cmath>

#define NAZGULS 9

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);
void moveRing(Camera_Movement direction);
bool ringDestroyed();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool blinn = false;
bool blinnKeyPressed = false;

// camera
Camera camera(glm::vec3(8.0f, 3.0f, 10.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 ringPosition(-6.0f, 0.2f, -5.6f);
glm::vec3 pyramidPosition(5.15f, -0.5f, 4.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Middle-Earth", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Shaders
    // -------------------------
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader lightshowShader("resources/shaders/lightshow.vs", "resources/shaders/lightshow.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float pyramidVertices[] = {
         // positions         // normals           // texture coords
         1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,    //A
         0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,    //B
        -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,    //C
         0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,    //D
         0.0f,  0.0f,  2.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,	   //E
    };
    unsigned int pyramidIndices[] = {
         0, 1, 3,	//ABD
         1, 2, 3,	//BDC
         0, 1, 4,	//ABE
         0, 3, 4,	//ADE
         2, 3, 4,	//CDE
         1, 2, 4,	//BCE
    };

    float floorVertices[] = {
        // positions          // normals          // texture coords
         0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  1.0f,  1.0f,  // top right
         0.5f, -0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  1.0f,  0.0f,  // bottom right
        -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,  // bottom left
        -0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f,  1.0f   // top left
    };
    unsigned int floorIndices[] = {
         0, 1, 3,  // first Triangle
         1, 2, 3   // second Triangle
    };

    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // Cube
    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Pyramid
    unsigned int pyramidVAO, pyramidVBO, pyramidEBO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glGenBuffers(1, &pyramidEBO);

    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // Floor
    unsigned int floorVAO, floorVBO, floorEBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // Skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);


    // loading textures into shaders
    // -------------

    // lightshow textures
    unsigned int ringSpecularRed = loadTexture(FileSystem::getPath("resources/objects/ring/Ring_Light.fw.png").c_str());
    unsigned int ringSpecularShine = loadTexture(FileSystem::getPath("resources/objects/ring/Ring_Mesh.fw.png_specular.jpg").c_str());

    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/stone.jpg").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/lava.jpg").c_str());
    lightshowShader.use();
    lightshowShader.setInt("material.diffuse", 0);
    lightshowShader.setInt("material.specular", 1);
    lightshowShader.setInt("material.specular2", 2);

    //pyramid textures
    unsigned int pyramidDiffuseMap = diffuseMap;   //loadTexture(FileSystem::getPath("resources/textures/stone.jpg").c_str());
    unsigned int pyramidSpecularMap = specularMap;   //loadTexture(FileSystem::getPath("resources/textures/lava.jpg").c_str());

    // floor textures
    unsigned int floorDiffuseMap = loadTexture(FileSystem::getPath("resources/textures/middle_earth.jpg").c_str());
    unsigned int floorSpecularMap = specularMap;   //loadTexture(FileSystem::getPath("resources/textures/lava.jpg").c_str());

    // skybox textures
    vector<std::string> skyboxSides = {
        FileSystem::getPath("resources/textures/mordor_skybox/hot_rt.png"),
        FileSystem::getPath("resources/textures/mordor_skybox/hot_lf.png"),
        FileSystem::getPath("resources/textures/mordor_skybox/hot_up.png"),
        FileSystem::getPath("resources/textures/mordor_skybox/hot_dn.png"),
        FileSystem::getPath("resources/textures/mordor_skybox/hot_bk.png"),
        FileSystem::getPath("resources/textures/mordor_skybox/hot_ft.png")
    };
    unsigned int cubemapTexture = loadCubemap(skyboxSides);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    Model ring(FileSystem::getPath("resources/objects/ring/The One Ring.FBX"));
    ring.SetShaderTextureNamePrefix("material.");


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        auto currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float time = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // lightshowShader
        // -----------
        lightshowShader.use();
        lightshowShader.setVec3("viewPos", camera.Position);
        lightshowShader.setFloat("material.shininess", 16.0f);
        lightshowShader.setInt("blinn", blinn);

        // directional light setup
        lightshowShader.setVec3("dirLight.direction", 1.0f, -0.5f, 0.0f);
        lightshowShader.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.01f);
        lightshowShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightshowShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        // point light setup
        lightshowShader.setVec3("pointLight.position", ringPosition);
        lightshowShader.setVec3("pointLight.ambient", 0.01f, 0.01f, 0.01f);
        lightshowShader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightshowShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        lightshowShader.setFloat("pointLight.constant", 1.0f);
        lightshowShader.setFloat("pointLight.linear", 0.05f);
        lightshowShader.setFloat("pointLight.quadratic", 0.012f);

        // spotlight setup
        lightshowShader.setVec3("spotLight.position", camera.Position);
        lightshowShader.setVec3("spotLight.direction", camera.Front);
        lightshowShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightshowShader.setVec3("spotLight.diffuse", 0.7f, 0.7f, 0.7f);
        lightshowShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightshowShader.setFloat("spotLight.constant", 1.0f);
        lightshowShader.setFloat("spotLight.linear", 0.05);
        lightshowShader.setFloat("spotLight.quadratic", 0.012);
        lightshowShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.5f)));
        lightshowShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(13.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightshowShader.setMat4("projection", projection);
        lightshowShader.setMat4("view", view);

        glm::mat4 model(1.0f);

        // cubes
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(cubeVAO);

        float angleDifference = M_PI_2 / 8;
        float angle = -M_PI_4/2;
        float radius = 2.5;
        float x = 1;

        for (unsigned int i = 1; i <= NAZGULS; i++)
        {
            // a few needed variables
            float trigConstant = time * x * (float)i + (float)i;    // trial and error, this formula shows good results
            float c = 4.5f * (sin(trigConstant) - 1.0f);    // there was an idea about this formula but i lost it...
            glm::vec3 nazgulDirection(-1.0f + radius*cos(angle), -0.6f, -0.5f + radius * sin(angle));   // sets nazguls in an arch around the tower
            glm::vec3 nazgulLevitate(0, 0.3f*cos(time), 0); // nazguls move up and down, simulating flying
            glm::vec3 nazgulPosition = pyramidPosition - nazgulDirection + nazgulLevitate;
            glm::vec3 moving(x, 0.0f, 1 - x);   // nazguls will move in a straight line, each by his own directional vector
            glm::vec3 advancedMoving(0.1f * cos(time), 0, 0.1f * cos(time));    // nazguls move a bit wiggly, not in a straight line anymore
            float deep = i > 3 && i < 8 ? 2.0f : 1.0f;  // middle nazguls should go further
            glm::vec3 translateVector = nazgulPosition + c*(deep*moving + advancedMoving);

            // world transformation
            model = glm::mat4(1.0f);
            model = glm::translate(model, translateVector);
            model = glm::scale(model, glm::vec3(0.3f));
            lightshowShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            angle += angleDifference;
            x -= 1.0f / NAZGULS;
        }


        // pyramid
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, pyramidPosition);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0, 0.0f));
        model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.8f));
        lightshowShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pyramidDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pyramidSpecularMap);

        // render pyramid
        glBindVertexArray(pyramidVAO);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, nullptr);


        // ring
        // material properties
        lightshowShader.setFloat("material.shininess", 2.0f);

        // world transformations
        model = glm::mat4(1.0f);
        model = glm::translate(model, ringPosition);
        model = glm::rotate(model, time, glm::vec3(1.0f));
        /* uncomment for faster rotation */
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f));
        model = glm::scale(model, glm::vec3(0.005f));
        lightshowShader.setMat4("model", model);

        // bind ring textures
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ringSpecularRed);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ringSpecularShine);

        // render ring
        ring.Draw(lightshowShader);

        // unbind ring texture
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);


        // floor
        // light properties
        lightshowShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // material properties
        lightshowShader.setFloat("material.shininess", 256.0f);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.51f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(25.0f));
        lightshowShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floorSpecularMap);

        // render floor
        glBindVertexArray(floorVAO);
        glEnable(GL_CULL_FACE);     // floor won't be visible if looked from bellow
        glCullFace(GL_BACK);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glDisable(GL_CULL_FACE);


        // skybox shader setup
        // -----------
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // render skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteBuffers(1, &pyramidEBO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteBuffers(1, &floorEBO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        moveRing(FORWARD);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        moveRing(BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        moveRing(LEFT);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        moveRing(RIGHT);

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        moveRing(UP);
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        moveRing(DOWN);

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
        cout << "Blinn-Phong" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void moveRing(Camera_Movement direction)
{
    float velocity = 2.5f * deltaTime;
    glm::vec3 yLock(1.0f, 0.0f, 1.0f);
    glm::vec3 yMove(0.0f, 1.0f, 0.0f);

    if (direction == FORWARD)
        ringPosition += camera.Front * velocity * yLock;
    if (direction == BACKWARD)
        ringPosition -= camera.Front * velocity * yLock;
    if (direction == LEFT)
        ringPosition -= camera.Right * velocity * yLock;
    if (direction == RIGHT)
        ringPosition += camera.Right * velocity * yLock;

    if (direction == UP)
        ringPosition += velocity * yMove;
    if (direction == DOWN)
        ringPosition -= velocity * yMove;

    if (ringPosition.y < 0.0f)
        ringPosition.y = 0.0f;
    else if (ringPosition.y > 3.0f)
        ringPosition.y = 3.0f;

    if (ringDestroyed()) {
        cout << "The Ring has been destroyed!" << endl;
        exit(EXIT_SUCCESS);
    }
}

bool ringDestroyed()
{
    glm::vec3 difference = abs(pyramidPosition - ringPosition);
    glm::vec3 criticalArea = glm::vec3(0.1f);
    return difference.x < criticalArea.x && difference.z < criticalArea.z;
}
