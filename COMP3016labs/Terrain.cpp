#include "Terrain.h"
#include "FastNoiseLite.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Terrain parameters
static constexpr int RENDER_DISTANCE = 128;
static constexpr int MAP_SIZE = RENDER_DISTANCE * RENDER_DISTANCE;
const int squaresRow = RENDER_DISTANCE - 1;
//Two triangles per square to form a 1x1 chunk
const int trianglesPerSquare = 2;
//Amount of triangles on map
const int trianglesGrid = squaresRow * squaresRow * trianglesPerSquare;

using namespace glm;
using namespace std;

Terrain::Terrain(int ChunkX, int ChunkZ)
{
    //Setting noise type to Perlin
    TerrainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //Sets the noise scale
    TerrainNoise.SetFrequency(0.05f);
    int terrainSeed = 1;
    //Sets seed for noise
    TerrainNoise.SetSeed(terrainSeed);
    setChunkX(ChunkX);
    setChunkZ(ChunkZ);
    generateMesh();
}

Terrain::~Terrain()
{

}
void Terrain::setChunkX(int ChunkX)
{
    chunkX = ChunkX;
}
void Terrain::setChunkZ(int ChunkZ)
{
    chunkZ = ChunkZ;
}
int Terrain::getChunkX()
{
    return chunkX;
}
int Terrain::getChunkZ()
{
    return chunkZ;
}
void Terrain::generateMesh()
{

    struct Vertex { vec3 pos, colour, normal; };

    //Generation of height map vertices
    Vertex terrainVertices[MAP_SIZE];

    const float vertexSpacing = 0.0625f;
    float chunkOffsetX = chunkX * (RENDER_DISTANCE - 1) * vertexSpacing;
    float chunkOffsetZ = chunkZ * (RENDER_DISTANCE - 1) * vertexSpacing;

    //Terrain vertice index
    int i = 0;
    //Using x & y nested for loop in order to apply noise 2-dimensionally
    for (int y = 0; y < RENDER_DISTANCE; y++)
    {
        for (int x = 0; x < RENDER_DISTANCE; x++, i++)
        {
            float noiseX = x + chunkX * (RENDER_DISTANCE - 1);
            float noiseY = y + chunkZ * (RENDER_DISTANCE - 1);
            float biomeValue = TerrainNoise.GetNoise((float)x, (float)y);
            float height = TerrainNoise.GetNoise(noiseX, noiseY);

            //Setting of height from 2D noise value at respective x & y coordinate
            if (chunkX == 1 || chunkZ == 1 || x>120 ||y>120)
            {
                terrainVertices[i].pos = vec3(x * vertexSpacing + chunkOffsetX, height/2.2f, y * vertexSpacing + chunkOffsetZ);
            }
            else
            {
                terrainVertices[i].pos = vec3(x * vertexSpacing + chunkOffsetX, height, y * vertexSpacing + chunkOffsetZ);

            }

     

            if (chunkX == 0 && chunkZ == 0) //mountains
            {
                terrainVertices[i].colour = vec3(0.5f, 0.5f, 0.5f);
                // Snow
                if (height >= (0.5f / 8.0f))
                {
                    terrainVertices[i].colour = vec3(1.0f);
                }
            }
            else if (chunkX == 1 && chunkZ == 0) //desert
            {
                terrainVertices[i].colour = vec3(1.0f, 1.0f, 0.5f);
            }
            else //forest
            {
                terrainVertices[i].colour = vec3(0.29f, 0.4f, 0.25f);
                // Water
                if (height <= -0.4f)
                {
                    terrainVertices[i].colour = vec3(0.0f, 0.25f, 0.39f);
                }
            }

            terrainVertices[i].normal = vec3(0.0f);
        }
    }

    GLuint terrainIndices[trianglesGrid][3];

    //Generation of map indices in the form of chunks (1x1 right angle triangle squares)
    int index = 0;
    for (int y = 0; y < RENDER_DISTANCE - 1; y++)
    {
        for (int x = 0; x < RENDER_DISTANCE - 1; x++)
        {
            terrainIndices[index][0] = y * RENDER_DISTANCE + x;
            terrainIndices[index][1] = y * RENDER_DISTANCE + x + RENDER_DISTANCE;
            terrainIndices[index][2] = y * RENDER_DISTANCE + x + 1;
            index++;

            terrainIndices[index][0] = y * RENDER_DISTANCE + x + 1;
            terrainIndices[index][1] = y * RENDER_DISTANCE + x + RENDER_DISTANCE;
            terrainIndices[index][2] = y * RENDER_DISTANCE + x + RENDER_DISTANCE + 1;
            index++;
        }
    }

    //calculate normals
    for (int i = 0; i < trianglesGrid; i++)
    {
        int i0 = terrainIndices[i][0];
        int i1 = terrainIndices[i][1];
        int i2 = terrainIndices[i][2];

        vec3 vert0 = terrainVertices[i0].pos;
        vec3 vert1 = terrainVertices[i1].pos;
        vec3 vert2 = terrainVertices[i2].pos;

        vec3 normal = normalize(cross(vert1 - vert0, vert2 - vert0));

        terrainVertices[i0].normal += normal;
        terrainVertices[i1].normal += normal;
        terrainVertices[i2].normal += normal;
    }

    for (int i = 0; i < MAP_SIZE; i++)
    {
        terrainVertices[i].normal = normalize(terrainVertices[i].normal);
    }

    //Sets index of VAO
    glGenVertexArrays(NumVAOs, VAOs);
    //Binds VAO to a buffer
    glBindVertexArray(VAOs[0]);
    //Sets indexes of all required buffer objects
    glGenBuffers(NumBuffers, Buffers);
    //glGenBuffers(1, &EBO);

    //Binds vertex object to array buffer
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
    //Allocates buffer memory for the vertices of the 'Triangles' buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

    //Binding & allocation for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrainIndices), terrainIndices, GL_STATIC_DRAW);


    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Colour
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
    glEnableVertexAttribArray(1);

    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



void Terrain::Draw(Shader& shader)
{
    shader.use();
    glBindVertexArray(VAOs[0]);
    glDrawElements(GL_TRIANGLES, trianglesGrid * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

float Terrain::getHeight(float localX, float localZ)
{
    const float vertexSpacing = 0.0625f;

    float worldX = localX / vertexSpacing;
    float worldZ = localZ / vertexSpacing;

    float height = TerrainNoise.GetNoise(worldX, worldZ);

    if (chunkX == 1 || chunkZ == 1 ||
        fmod(worldX, RENDER_DISTANCE) > 120 ||
        fmod(worldZ, RENDER_DISTANCE) > 120)
    {
        height /= 2.2f;
    }

    return height;
}
