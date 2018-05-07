#include "model.h"
#include "mesh.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <QtDebug>
#include <QString>

Model::Model()
{
    joints = new Joints; // as on model
    joints->elbow = 0.0;
    joints->waist = 0.0;
    joints->shoulder = 2.5;
    joints->neck = 11.4;
    joints->flippers = 175.0;
}

glm::mat4 makeRotMat(float degree, int axis) {
    switch (axis) {
    case 1:
        return glm::rotate(glm::mat4(1.0f), glm::radians(degree), glm::vec3(1.0f,0.0f,0.0f));
    case 2:
        return glm::rotate(glm::mat4(1.0f), glm::radians(degree), glm::vec3(0.0f,1.0f,0.0f));
    case 3:
    default:
        return glm::rotate(glm::mat4(1.0f), glm::radians(degree), glm::vec3(0.0f,0.0f,1.0f));
    }
}

Model::~Model()
{
    delete joints;
}

void Model::rotateFlippers(float angle)
{
    qDebug() << "angle = " << angle << " res = " << angle - joints->flippers;
    applyTransform(flippers, makeRotMat(angle - joints->flippers, 1));
    joints->flippers = angle;
}

void Model::rotateWaist(float angle)
{
    applyTransform(sea, makeRotMat(-(angle - joints->waist), 3));
    joints->waist = angle;
}

void Model::rotateShoulder(float angle)
{
    applyTransform(sea, makeRotMat(angle - joints->shoulder, 1));
    joints->shoulder = angle;
}

void Model::rotateElbow(float angle)
{
    applyTransform(ena, makeRotMat(angle - joints->elbow, 1));
    joints->elbow = angle;
}

void Model::rotateNeck(float angle)
{
    applyTransform(head, makeRotMat(angle - joints->neck, 1));
    joints->neck = angle;
}

Mesh* _find(std::string name, Mesh* start) {
    if (start == nullptr || start->name == name) return start;
    Mesh* res = nullptr;
    for (size_t i = 0; i < start->childrenCount; ++i) {
        res = _find(name, start->children[i]);
        if (res != nullptr) return res;
    }
    return nullptr;
}

Mesh *Model::find(std::string name)
{
    return _find(name, root);
}

static inline glm::mat4 aiToGlm(const aiMatrix4x4 &m) {
    return glm::transpose(glm::make_mat4(&m.a1));
}

glm::vec4 aiToGlm(aiVector3D v) {
    return glm::vec4(v[0], v[1], v[2], 1.0f);
}

aiVector3D glmToAi(glm::vec4 v) {
    return aiVector3D(v.x, v.y, v.z);
}

void Model::loadFromFilename(std::string filename)
{ 
  Assimp::Importer import;
//  import.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE,1);
//  import.SetPropertyInteger(AI_CONFIG_PP_PTV_KEEP_HIERARCHY,1);
//  const aiScene* scene = import.ReadFile(filename, aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_PreTransformVertices | aiProcess_Triangulate | aiProcess_FlipUVs);
  const aiScene* scene = import.ReadFile(filename, aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_Triangulate);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
  {
    std::cout << "Error at loading model: " << import.GetErrorString() << "\n";
    return;
  }

  mCurrentFolder = filename.substr(0, filename.find_last_of("/"));

//  computeAbsoluteTransform(scene->mRootNode);
  iterateNode(scene->mRootNode, scene, nullptr, glm::mat4(1.0f));
  root = mMeshes[0];
  flippers = find("flippers");
  base = find("base");
  sea = find("s-e-arm");
  ena = find("e-n-arm");
  head = find("head");

//  rotateWaist(30);
//  rotateFlippers(30);
//  rotateShoulder(-30);
//  rotateFlippers(-10);
//  rotateNeck(20);
//  rotateShoulder(5);
//  rotateWaist(-10);
//  rotateNeck(-10);
//  rotateFlippers(10);
//  rotateShoulder(10);
//  rotateWaist(10);
//  rotateNeck(10);
//  rotateShoulder(40);
//  rotateFlippers(-50);
//  rotateElbow(-30);
}

void Model::iterateNode(const aiNode *node, const aiScene *scene, Mesh* parent, glm::mat4 tf)
{

    if (node->mNumMeshes > 1) qDebug() << "fuck! " << node->mNumMeshes << " in node";
    if (parent != nullptr) {
        parent->childrenCount += node->mNumMeshes;
        if (parent->children == nullptr) parent->children = new Mesh*[2];
    }
    Mesh *mptr = nullptr;
    tf = tf * aiToGlm(node->mTransformation);
  for (size_t i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    mptr = loadMesh(mesh, scene, parent);
    _applyTransform(mptr, tf);
    mptr->absTransf = tf;
    mptr->relTransf = aiToGlm(node->mTransformation);
    if (parent != nullptr) parent->children[(parent->childrenCount - node->mNumMeshes) + i] = mptr;
    if (root == nullptr && parent == nullptr) root = mptr;
    mMeshes.emplace_back(mptr);
  }
  for (size_t i = 0; i < node->mNumChildren; i++) {//recursion problem!
    iterateNode(node->mChildren[i], scene, mptr, tf);
  }
}

Mesh* Model::loadMesh(const aiMesh *asMesh, const aiScene *scene, Mesh* parent)
{
  std::vector<Vertex> vertices;
//    qDebug() << "loaded " << asMesh->mName.C_Str() << " transformed " << transforms.size();
  for (size_t i = 0; i < asMesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec4 vect;

    vect.x = asMesh->mVertices[i].x;
    vect.y = asMesh->mVertices[i].y;
    vect.z = asMesh->mVertices[i].z;
    vect.w = 1.0f;
    vertex.pos = vect;

    vect.x = asMesh->mNormals[i].x;
    vect.y = asMesh->mNormals[i].y;
    vect.z = asMesh->mNormals[i].z;
    vect.w = 0.0f;
    vertex.nor = vect;

    if (asMesh->mColors[0]) {
      glm::vec4 vect4;
      vect4.x = asMesh->mColors[0][i].r;
      vect4.y = asMesh->mColors[0][i].g;
      vect4.z = asMesh->mColors[0][i].b;
      vect4.w = asMesh->mColors[0][i].a;

      vertex.color = vect4;
    } else {
      vertex.color = glm::vec4(1.0f);
    }

    if (asMesh->mTextureCoords[0]) {
      glm::vec2 vect2;
      vect2.x = asMesh->mTextureCoords[0][i].x;
      vect2.y = asMesh->mTextureCoords[0][i].y;
      vertex.texCoords = vect2;
    } else {
      vertex.texCoords = glm::vec2(0.0f, 0.0f);
    }  
    
    vertices.push_back(vertex);
  }

  std::vector<GLuint> indices;

  for (size_t i = 0; i < asMesh->mNumFaces; i++) {
    aiFace face = asMesh->mFaces[i];
    for (size_t j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  std::vector<Texture> textures;

//  if (asMesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[asMesh->mMaterialIndex];
    std::vector<Texture> diffuseMaps = getTextures(material, DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = getTextures(material, SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//  }
  
  

  Mesh* mesh = new Mesh;
  mesh->mVertices = vertices;
  mesh->mIndices = indices;
  mesh->mTextures = textures;
  mesh->name = std::string(asMesh->mName.C_Str());
  mesh->parent = parent;

  return mesh;
  
}

void Model::applyTransform(Mesh *mesh, const glm::mat4 &rot)
{
    if (rot != glm::mat4(1.0f)) {
        glm::mat4 mat = mesh->absTransf * rot * glm::inverse(mesh->absTransf);
        glm::mat4 mWorldIT = glm::transpose(glm::inverse(mat));
        #pragma omp parallel for
        for (auto it = mesh->mVertices.begin(); it < mesh->mVertices.end(); ++it) {
            it->pos = mat * it->pos;
            it->nor = glm::normalize(mWorldIT * it->nor);
        }
        if (mesh->parent) {
            mesh->absTransf = mesh->absTransf * rot;
//            mesh->relTransf = glm::inverse(mesh->parent->absTransf * glm::inverse(mesh->absTransf));
        }
        for (size_t i = 0; i < mesh->childrenCount; ++i) {
            applyTransformCascade(mesh->children[i], mat);
        }
    }
}

void Model::_applyTransform(Mesh *mesh, const glm::mat4 &mat)
{
    if (mat != glm::mat4(1.0f)) {
        glm::mat4 mWorldIT = glm::transpose(glm::inverse(mat));
        #pragma omp parallel for
        for (auto it = mesh->mVertices.begin(); it < mesh->mVertices.end(); ++it) {
            it->pos = mat * it->pos;
            it->nor = glm::normalize(mWorldIT * it->nor);
        }
    }
}

void Model::applyTransformCascade(Mesh* mesh, const glm::mat4 &mat) {

    glm::mat4 mWorldIT = glm::transpose(glm::inverse(mat));
    #pragma omp parallel for
    for (auto it = mesh->mVertices.begin(); it < mesh->mVertices.end(); ++it) {
        it->pos = mat * it->pos;
        it->nor = glm::normalize(mWorldIT * it->nor);
    }
    mesh->absTransf = mesh->parent->absTransf * mesh->relTransf;
//    mesh->relTransf = glm::inverse(mesh->parent->absTransf * glm::inverse(mesh->absTransf));



//    mesh->relTransf = glm::inverse(rot) * mesh->relTransf;

//    mesh->absTransf = mesh->absTransf * rot;
//    mesh->relTransf = rot * mesh->relTransf * glm::inverse(rot);
//    mesh->absTransf = mesh->parent->absTransf * mesh->relTransf * rot;

//    mesh->relTransf = rot * mesh->relTransf;
//    mesh->absTransf = mesh->parent->absTransf * mesh->relTransf;


    for (size_t i = 0; i < mesh->childrenCount; ++i) {
        applyTransformCascade(mesh->children[i], mat);
    }

}

void Model::applyTransform(aiMesh* mesh, const aiMatrix4x4& mat)
{
//    glm::mat4 mat1 = aiToGlm(mat);
    // Check whether we need to transform the coordinates at all
    if (!mat.IsIdentity()) {

        if (mesh->HasPositions()) {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                mesh->mVertices[i] = mat * mesh->mVertices[i];
//                mesh->mVertices[i] = glmToAi(mat1 * aiToGlm(mesh->mVertices[i]));
            }
        }
        if (mesh->HasNormals() || mesh->HasTangentsAndBitangents()) {
            aiMatrix4x4 mWorldIT = mat;
            mWorldIT.Inverse().Transpose();

            // TODO: implement Inverse() for aiMatrix3x3
            aiMatrix3x3 m = aiMatrix3x3(mWorldIT);

            if (mesh->HasNormals()) {
                for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                    mesh->mNormals[i] = (m * mesh->mNormals[i]).Normalize();
                }
            }
            if (mesh->HasTangentsAndBitangents()) {
                for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                    mesh->mTangents[i]   = (m * mesh->mTangents[i]).Normalize();
                    mesh->mBitangents[i] = (m * mesh->mBitangents[i]).Normalize();
                }
            }
        }
    }
}

void Model::computeAbsoluteTransform(aiNode* pcNode)
{
    if (pcNode->mParent)    {
        pcNode->mTransformation = pcNode->mParent->mTransformation*pcNode->mTransformation;
    }

    for (unsigned int i = 0;i < pcNode->mNumChildren;++i)   {
        computeAbsoluteTransform(pcNode->mChildren[i]);
    }
}

//void Model::calcVertices(Mesh* m, glm::mat4 mat) {
////    mat = mat * m->relTransf;
//    mat = m->relTransf;
//    glm::mat4 mWorldIT = glm::transpose(glm::inverse(mat));
//    m->mVertices.reserve(m->relVertices.size());
//    for (Vertex v : m->relVertices) {
////            v.pos = mat * v.pos;
////            v.nor = glm::normalize(mWorldIT * v.nor);
//        m->mVertices.emplace_back(v);
//    }
//    for (size_t i = 0; i < m->childrenCount; ++i) {
//        calcVertices(m->children[i], mat);
//    }
//}

std::vector<Texture> Model::getTextures(const aiMaterial *material, TextureType textureType)
{
  aiTextureType type;
  switch (textureType) {
  case DIFFUSE:
    type = aiTextureType_DIFFUSE;
    break;
  case SPECULAR:
    type = aiTextureType_SPECULAR;
    break;
  }

  std::vector<Texture> textures;
  for (size_t i = 0; i < material->GetTextureCount(type); i++) {
    aiString aiStr;
    material->GetTexture(type, i, &aiStr);

    // aiString and std string no compatible. We convert aiString to c string then convert to std string.
    std::string path = mCurrentFolder + "/" + std::string(aiStr.C_Str());
    Texture texture(path.c_str(), textureType);
    textures.push_back(texture);
    
  }

  return textures;
}  

void Model::rotateHorizontally(float angle)
{
  mModelMatrix = glm::rotate(mModelMatrix, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Model::rotateVertically(float angle)
{
  mModelMatrix = glm::rotate(mModelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));  
}

void Model::resetModelMatrix()
{
    mModelMatrix = glm::mat4();
}

glm::mat4 Model::getModelMatrix()
{
    return mModelMatrix;
}
