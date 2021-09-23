#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "../resources/resources.h"
#include "model.h"

namespace Model {
namespace {
Mesh::Mesh ProcessMesh(const aiMesh* const mesh, const aiScene* const scene) {
    std::vector<Vertex> vertices(mesh->mNumVertices);
    std::vector<u32> indices;

    for (u32 i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{
            .position =
                {
                    mesh->mVertices[i].x,
                    mesh->mVertices[i].y,
                    mesh->mVertices[i].z,
                },
            .texture_coords{},
        };
        if (mesh->mTextureCoords[0]) {
            vertex.texture_coords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        }
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
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        fprintf(stderr, "ASSIMP error: %s\n ", importer.GetErrorString());
        return;
    }
    // process ASSIMP's root node recursively
    ProcessAINode(scene->mRootNode, scene);
    LoadMaterials(scene);
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

void Model::LoadMaterials(const aiScene* scene) {
    textures.resize(scene->mNumMaterials);
    samplers.resize(scene->mNumMaterials);
    for (size_t i = 0; i < scene->mNumMaterials; i++) {
        const aiMaterial* const material = scene->mMaterials[i];
        if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0) {
            continue;
        }
        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS) {
            continue;
        }
        const auto idx = std::string(path.data).rfind("\\");
        const auto filename = std::string(path.data).substr(idx + 1);
        const auto texture_path = std::string("../res/textures/") + filename;

        textures[i].Create();
        samplers[i].Create();
        textures[i].UploadDataFromPath(texture_path);
        samplers[i].DefaultConfiguration();
    }
}

void Model::Render() {
    for (size_t i = 0; i < meshes.size(); ++i) {
        glBindTexture(GL_TEXTURE_2D, textures.at(i).handle);
        glBindSampler(0, samplers.at(i).handle);
        meshes[i].Render();
    }
}
} // namespace Model
