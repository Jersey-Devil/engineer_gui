#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "../robot_control/joints.h"
#include <vector>
#include <string>
#include <omp.h>
#include <QObject>

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
  void rotateFlippers(float angle);
  void rotateWaist(float angle);
  void rotateShoulder(float angle);
  void rotateElbow(float angle);
  void rotateNeck(float angle);
  void rotateHorizontally(float angle);
  void rotateVertically(float angle);
  void loadFromFilename(std::string filename);
  void resetModelMatrix();
  glm::mat4 getModelMatrix();

private:
  Mesh* base;
  Mesh* flippers;
  Mesh* sea;
  Mesh* ena;
  Mesh* head;
  Mesh* find(std::string name);
  std::string mCurrentFolder;
  glm::mat4 mModelMatrix;
  Joints* joints;
  Mesh* root;
  Mesh* loadMesh(const aiMesh*, const aiScene*scene, Mesh *parent);
  void applyTransform(Mesh* mesh, const glm::mat4 &rot);
  void _applyTransform(Mesh* mesh, const glm::mat4 &mat);
  void applyTransform(aiMesh* mesh, const aiMatrix4x4& mat);
  void applyTransformCascade(Mesh* mesh, const glm::mat4 &mat);
  void computeAbsoluteTransform( aiNode* pcNode );
  void iterateNode(const aiNode*, const aiScene*, Mesh *parent, glm::mat4 tf);
  std::vector<Texture> getTextures(const aiMaterial *material, TextureType textureType);
};

#endif
