#pragma once
#include <vector>

namespace
{
constexpr uint32_t PositionAttrib = 3;
constexpr uint32_t PositionTextureAttrib = 5;
constexpr uint32_t PositionNormalTextureAttrib = 8;

constexpr uint32_t CubeVerticesAmount = 36;
constexpr uint32_t PlaneVerticesAmount = 6;

constexpr uint32_t PositionVertexAttribute = 0;
constexpr uint32_t NormalVertexAttribute = 1;
constexpr uint32_t TextureCoordVertexAttribute = 2;
}// namespace

class Primitive
{
public:
  enum VertexAttributeStructure
  {
    PositionTexture,
    PositionNormalTexture
  };

  Primitive() : vertices(0), VAO(0), VBO(0) {}
  Primitive(const float *Vertices, uint32_t number, VertexAttributeStructure vertexAttributes = PositionTexture)
    : vertices(Vertices, Vertices + number), VAO(0), VBO(0)
  {
    setupMesh(vertexAttributes);
  }

  virtual ~Primitive() = default;

private:
  virtual void setupMesh(VertexAttributeStructure vertexAttributes)
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    const bool containNormals = vertexAttributes == PositionNormalTexture;
    const uint32_t vertexAttributesCount = !containNormals ? PositionTextureAttrib : PositionNormalTextureAttrib;

    // vertex Positions
    glEnableVertexAttribArray(PositionVertexAttribute);
    glVertexAttribPointer(
      PositionVertexAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * vertexAttributesCount, (void *)0);

    if (containNormals)
    {
      glEnableVertexAttribArray(NormalVertexAttribute);
      glVertexAttribPointer(NormalVertexAttribute,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * vertexAttributesCount,
        (void *)(3 * sizeof(float)));
    }

    // vertex Texture Coords
    glEnableVertexAttribArray(TextureCoordVertexAttribute);
    glVertexAttribPointer(TextureCoordVertexAttribute,
      2,
      GL_FLOAT,
      GL_FALSE,
      sizeof(float) * vertexAttributesCount,
      (void *)((containNormals ? 6 : 3) * sizeof(float)));
    glBindVertexArray(0);
  }

public:
  uint32_t VAO, VBO;
  std::vector<float> vertices;
};

class LightPrimitive : public Primitive
{
public:
  LightPrimitive() : Primitive() {}
  LightPrimitive(const float *Vertices, uint32_t number) : Primitive(Vertices, number, PositionNormalTexture) {}

private:
  void setupMesh(VertexAttributeStructure vertexAttributes) override
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // vertex Positions
    glEnableVertexAttribArray(PositionVertexAttribute);
    // TODO: We are using 8 vertex attributes here but actually we need only 3 for light, should be fixed later
    glVertexAttribPointer(
      PositionVertexAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * PositionNormalTextureAttrib, (void *)0);
    glBindVertexArray(0);
  }
};

constexpr float CubeVertices[CubeVerticesAmount * PositionNormalTextureAttrib] = {// clang-format off
    // positions           // normal             // texture Coords
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
                          
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
                          
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
                          
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
                          
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
                          
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f
  };// clang-format on

constexpr float PlaneVertices[PlaneVerticesAmount * PositionNormalTextureAttrib] = {// clang-format off
    // positions          // normal             // texture Coords 
     5.0f, -0.5f,  5.0f,  0.0f, 1.0f,  0.0f,   2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 1.0f,  0.0f,   0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 1.0f,  0.0f,   0.0f, 2.0f,

     5.0f, -0.5f,  5.0f,  0.0f, 1.0f,  0.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 1.0f,  0.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,  0.0f, 1.0f,  0.0f,  2.0f, 2.0f
  };// clang-format on

constexpr float QuadVertices[PlaneVerticesAmount * PositionTextureAttrib] = {// clang-format off
    // positions        // texCoords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f
};// clang-format on
