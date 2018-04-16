#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Model {

public:
  Model();
  ~Model();

  std::vector<Mesh*> mMeshes;
  Mesh* find(std::string name);
  void rotateHorizontally(float angle);
  void rotateVertically(float angle);
  void loadFromFilename(std::string filename);
  void resetModelMatrix();
  glm::mat4 getModelMatrix();
  
private:
  std::string mCurrentFolder;
  glm::mat4 mModelMatrix;
  Mesh* root;
  Mesh* loadMesh(const aiMesh*, const aiScene*scene, Mesh *parent);
  void applyTransform(Mesh* mesh, glm::mat4 &rot);
  void _applyTransform(Mesh* mesh, glm::mat4 mat);
  void applyTransform(aiMesh* mesh, const aiMatrix4x4& mat);
  void applyTransformCascade(Mesh* mesh, glm::mat4 &mat);
  void computeAbsoluteTransform( aiNode* pcNode );
  void iterateNode(const aiNode*, const aiScene*, Mesh *parent, glm::mat4 tf);
  std::vector<Texture> getTextures(const aiMaterial *material, TextureType textureType);
};

#endif
