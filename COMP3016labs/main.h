#pragma once
//GLAD
#include <glad/glad.h>

//GLM
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/type_ptr.hpp> //Access to the value_ptr

#include <GLFW/glfw3.h>
#include "FastNoiseLite.h"
#include "Terrain.h"

//Called on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//Called on mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//Processes user input on a particular window
void ProcessUserInput(GLFWwindow* WindowIn);

//Sets the model-view-projection matrix
void SetMatrices(Shader& ShaderProgramIn);

void DrawModel(Shader& Shaders, glm::mat4& model, Model& object, Terrain& terrain, float x, float z, float scaleSize);
GLuint program;