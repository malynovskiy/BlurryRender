#pragma once
#include <vector>

using UINT = unsigned int;

constexpr UINT PositionAttrib = 3;
constexpr UINT PositionTextureAttrib = 5;
constexpr UINT PositionNormalTextureAttrib = 8;

constexpr UINT CubeVerticesAmount = 36;
constexpr UINT PlaneVerticesAmount = 6;

constexpr UINT PositionVertexAttribute = 0;
constexpr UINT NormalVertexAttribute = 1;
constexpr UINT TextureCoordVertexAttribute = 2;

class Primitive
{
public:
  enum VertexAttributeStructure
  {
    PositionTexture,
    PositionNormalTexture
  };

  UINT VAO, VBO;
  std::vector<float> vertices;
  Primitive() : vertices(0), VAO(0), VBO(0) {}
  Primitive(std::vector<float> Vertices, UINT vertexAttributes = PositionTexture) : vertices(Vertices), VAO(0), VBO(0)
  {
    setupMesh(vertexAttributes);
  }
  Primitive(const float *Vertices, UINT number, UINT vertexAttributes = PositionTexture)
    : vertices(Vertices, Vertices + number), VAO(0), VBO(0)
  {
    setupMesh(vertexAttributes);
  }

  virtual ~Primitive() = default;

private:
  virtual void setupMesh(UINT vertexAttributes)
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    const bool isNormals = vertexAttributes == PositionNormalTexture;
    const UINT vertexAttributesCount = !isNormals ? PositionTextureAttrib : PositionNormalTextureAttrib;

    // vertex Positions
    glEnableVertexAttribArray(PositionVertexAttribute);
    glVertexAttribPointer(
      PositionVertexAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * vertexAttributesCount, (void *)0);

    if (isNormals)
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
      (void *)((isNormals ? 6 : 3) * sizeof(float)));
    glBindVertexArray(0);
  }
};

class LightPrimitive : public Primitive
{
public:
  LightPrimitive() : Primitive() {}
  LightPrimitive(std::vector<float> Vertices) : Primitive(Vertices, PositionNormalTexture) {}
  LightPrimitive(const float *Vertices, UINT number) : Primitive(Vertices, number, PositionNormalTexture) {}

private:
  void setupMesh(UINT vertexAttributes) override
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

const float CubeVertices[CubeVerticesAmount * PositionNormalTextureAttrib] = {// clang-format off
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
