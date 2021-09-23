#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "model.h"

namespace Model {
namespace {
Mesh::Mesh ProcessMesh(const aiMesh* const mesh, const aiScene* const scene) {
    std::vector<Vertex> vertices(mesh->mNumVertices);
    std::vector<u32> indices;

    for (u32 i = 0; i < mesh->mNumVertices; i++) {
        const Vertex vertex{
            .position =
                {
                    mesh->mVertices[i].x,
                    mesh->mVertices[i].y,
                    mesh->mVertices[i].z,
                },
        };

        vertices[i] = vertex;
    }
    for (u32 i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (u32 j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    return Mesh::Mesh(vertices, indices);
}
} // namespace

Model::Model(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                    aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        fprintf(stderr, "ASSIMP error: %s\n ", importer.GetErrorString());
        return;
    }

    // process ASSIMP's root node recursively
    ProcessAINode(scene->mRootNode, scene);
}

void Model::ProcessAINode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        const aiMesh* const mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessAINode(node->mChildren[i], scene);
    }
}

void Model::Render() {
    for (const auto& mesh : meshes) {
        mesh.Render();
    }
}
} // namespace Model
