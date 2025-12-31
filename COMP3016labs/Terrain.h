//STD
#include <iostream>
#include <vector>

//GLAD
#include <glad/glad.h>

//LEARNOPENGL
#include <learnopengl/shader_m.h>

//GENERAL

#include "FastNoiseLite.h"

using namespace std;

class Terrain
{
public:

    Terrain(int chunkX, int chunkY); 
    ~Terrain();

    void Draw(Shader& shader);
    void generateMesh(int chunkX, int chunkY);

private:
    //VAO vertex attribute positions in correspondence to vertex attribute type
    enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 };
    //VAOs
    GLuint VAOs[NumVAOs];

    //Buffer types
    enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
    //Buffer objects
    GLuint Buffers[NumBuffers];

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

