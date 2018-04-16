#ifndef MESH_H
#define MESH_H

#include <vector>
#include "glm/glm.hpp"

#include "texture.h"

struct Vertex {
  glm::vec4 pos;
  glm::vec4 color;
  glm::vec2 texCoords;
  glm::vec4 nor;
};

struct Mesh {

  std::vector<Vertex> mVertices;
  std::vector<GLuint> mIndices;
  std::vector<Texture> mTextures;
  
  std::string name;
  glm::mat4 absTransf;
  glm::mat4 relTransf;
  Mesh* parent = nullptr;
  Mesh** children = nullptr;
  size_t childrenCount = 0;

  GLuint mVAO;
  GLuint mVBO;
  GLuint mEBO;
  
};

#endif
