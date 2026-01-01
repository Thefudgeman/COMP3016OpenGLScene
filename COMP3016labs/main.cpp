//STD
#include <iostream>

//GLAD
#include <glad/glad.h>

//GLM
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/type_ptr.hpp> //Access to the value_ptr

//ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//LEARNOPENGL
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

//GENERAL
#include "main.h"
//#include "Terrain.h"

#include "FastNoiseLite.h"

using namespace std;
using namespace glm;

//Window
int windowWidth;
int windowHeight;



//Transformations
//Relative position within world space
vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
//The direction of travel
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

//Camera sidways rotation
float cameraYaw = -90.0f;
//Camera vertical rotation
float cameraPitch = 0.0f;
//Determines if first entry of mouse into window
bool mouseFirstEntry = true;
//Positions of camera from given last frame
float cameraLastXPos = 800.0f / 2.0f;
float cameraLastYPos = 600.0f / 2.0f;


//Model-View-Projection Matrix
mat4 mvp;
mat4 model;
mat4 view;
mat4 projection;

//Time
//Time change
float deltaTime = 0.0f;
//Last value of time change
float lastFrame = 0.0f;

int main()
{
    //Initialisation of GLFW
    glfwInit();
    //Initialisation of 'GLFWwindow' object
    windowWidth = 1280;
    windowHeight = 720;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Lab5", NULL, NULL);

    //Checks if window has been successfully instantiated
    if (window == NULL)
    {
        cout << "GLFW Window did not instantiate\n";
        glfwTerminate();
        return -1;
    }

    //Sets cursor to automatically bind to window & hides cursor pointer
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Binds OpenGL to window
    glfwMakeContextCurrent(window);

    //Initialisation of GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD failed to initialise\n";
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

  //  list<Shader> shaders;
    //Loading of shaders
    Shader Shaders("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    Shaders.use();

    Shaders.setVec3("light.position", vec3(0.0f, 1.0f, 0.0f));
    Shaders.setVec3("light.ambient", vec3(0.4f));
    Shaders.setVec3("light.diffuse", vec3(0.8f));
    Shaders.setVec3("light.specular", vec3(0.2f));

    Shaders.setVec3("viewPos", cameraPosition);
  //  shaders.push_back(Shaders);
    
    Shader TerrainShaders("shaders/terrainVertexShader.vert", "shaders/terrainFragShader.frag");
  //  shaders.push_back(TerrainShaders);
    TerrainShaders.use();

    TerrainShaders.setVec3("light.position", vec3(3.0f, 1.0f, 3.0f));
    TerrainShaders.setVec3("light.ambient", vec3(0.4f));
    TerrainShaders.setVec3("light.diffuse", vec3(0.8f));
    TerrainShaders.setVec3("light.specular", vec3(0.0f));

    TerrainShaders.setVec3("viewPos", cameraPosition);
    // Light properties
   // SetLight(shaders, 10.0f, 40.0f, 10.0f);


    Model Rock("media/rock/Rock07-Base.obj");
    Model Tree("media/tree/palm.obj");
    Model ForestTree("media/tree2/Small_Pine.obj");

    //Sets the viewport size within the window to match the window size of 1280x720
    glViewport(0, 0, 1280, 720);

    //Sets the framebuffer_size_callback() function as the callback for the window resizing event
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Sets the mouse_callback() function as the callback for the mouse movement event
    glfwSetCursorPosCallback(window, mouse_callback);

    Terrain Mountains(0, 0);
    Terrain Desert(1, 0);
    Terrain Forest(0, 1);
    //Model matrix
    model = mat4(1.0f);
    //Scaling to zoom in
    model = scale(model, vec3(0.025f, 0.025f, 0.025f)); //rock
    //Looking straight forward
    model = rotate(model, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
    //Elevation to look upon terrain
    model = translate(model, vec3(0.0f, -2.f, -1.5f));

    //Projection matrix
    projection = perspective(radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);



    //Render loop
    while (glfwWindowShouldClose(window) == false)
    {
        //Time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Input
        ProcessUserInput(window); //Takes user input

        //Rendering
        glClearColor(0.25f, 0.0f, 1.0f, 1.0f); //Colour to display on cleared window
        glClear(GL_COLOR_BUFFER_BIT); //Clears the colour buffer
        glClear(GL_DEPTH_BUFFER_BIT); //Might need

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        //Transformations & Drawing
        //Viewer orientation
        view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
        model = mat4(1.0f);
        TerrainShaders.use();
        SetMatrices(TerrainShaders);

        //Drawing
        Mountains.Draw(TerrainShaders);
        Desert.Draw(TerrainShaders);
        Forest.Draw(TerrainShaders);
        //Rock (reorient MVP back to starting values)
        Shaders.use();
        model = mat4(1.0f);
        model = translate(model, vec3(0.1f, 0.1f, 0.1f));
        model = scale(model, vec3(0.001f, 0.001f, 0.001f));
        SetMatrices(Shaders);
        Rock.Draw(Shaders);

        //Tree (changes MVP in relation to past values)
        glActiveTexture(GL_TEXTURE0);

        model = mat4(1.0f);
        model = translate(model, vec3(-1.0f, 0.0f, 1.0f));
        model = scale(model, vec3(0.1f, 0.1f, 0.1f));
        SetMatrices(Shaders);
        Tree.Draw(Shaders);

        glActiveTexture(GL_TEXTURE0);

        DrawModel(Shaders, model, ForestTree, Forest, 1.0f, 0.0f);
        DrawModel(Shaders, model, ForestTree, Forest, 1.0f, 1.0f);
        DrawModel(Shaders, model, ForestTree, Forest, 3.0f, 5.0f);

        //Refreshing
        glfwSwapBuffers(window); //Swaps the colour buffer
        glfwPollEvents(); //Queries all GLFW events
    }

    //Safely terminates GLFW
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Resizes window based on contemporary width & height values
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //Initially no last positions, so sets last positions to current positions
    if (mouseFirstEntry)
    {
        cameraLastXPos = (float)xpos;
        cameraLastYPos = (float)ypos;
        mouseFirstEntry = false;
    }

    //Sets values for change in position since last frame to current frame
    float xOffset = (float)xpos - cameraLastXPos;
    float yOffset = cameraLastYPos - (float)ypos;

    //Sets last positions to current positions for next frame
    cameraLastXPos = (float)xpos;
    cameraLastYPos = (float)ypos;

    //Moderates the change in position based on sensitivity value
    const float sensitivity = 0.025f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    //Adjusts yaw & pitch values against changes in positions
    cameraYaw += xOffset;
    cameraPitch += yOffset;

    //Prevents turning up & down beyond 90 degrees to look backwards
    if (cameraPitch > 89.0f)
    {
        cameraPitch = 89.0f;
    }
    else if (cameraPitch < -89.0f)
    {
        cameraPitch = -89.0f;
    }

    //Modification of direction vector based on mouse turning
    vec3 direction;
    direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
    direction.y = sin(radians(cameraPitch));
    direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
    cameraFront = normalize(direction);
}

void ProcessUserInput(GLFWwindow* WindowIn)
{
    //Closes window on 'exit' key press
    if (glfwGetKey(WindowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(WindowIn, true);
    }

    //Extent to which to move in one instance
    const float movementSpeed = 1.0f * deltaTime;
    //WASD controls
    if (glfwGetKey(WindowIn, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPosition += movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPosition -= movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
}

void SetMatrices(Shader& ShaderProgramIn)
{
    ShaderProgramIn.setMat4("model", model);
    ShaderProgramIn.setMat4("view", view);
    ShaderProgramIn.setMat4("projection", projection);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    ShaderProgramIn.setMat3("normalMatrix", normalMatrix);
}

void DrawModel(Shader& Shaders, mat4& model, Model& object, Terrain& terrain,float x, float z)
{
    model = mat4(1.0f);
    model = translate(model, vec3(x + (0.0625f * 128.0f * terrain.getChunkX()), terrain.getHeight(x + (0.0625f * 128.0f * terrain.getChunkX()), z+ (0.0625f * 128.0f * terrain.getChunkZ())) - 0.02, z + (0.0625f*128.0f * terrain.getChunkZ())));
    model = scale(model, vec3(0.2f, 0.2f, 0.2f));
    SetMatrices(Shaders);

    object.Draw(Shaders);
}

