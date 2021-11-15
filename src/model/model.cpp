#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
            .normal =
                {
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z,
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
    return Mesh::Mesh(vertices, indices, mesh->mMaterialIndex);
}
} // namespace

void Model::ParseObjModel(const std::string& path) {
    auto default_scale = glm::vec3(0.25f, 0.25f, 0.25f);
    auto default_translate = glm::vec3(0.0f);
    auto default_rotate = glm::vec3(0.0f, 180.0f, 0.0f);
    ParseObjModel(path, default_scale, default_translate, default_rotate);
}

void Model::ParseObjModel(const std::string& path, const glm::vec3& scale,
                          const glm::vec3& translate, const glm::vec3& rotate) {
    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                    aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        fprintf(stderr, "ASSIMP error: %s\n ", importer.GetErrorString());
        return;
    }
    meshes = std::vector<Mesh::Mesh>{};
    textures = std::vector<Texture>{};

    // process ASSIMP's root node recursively
    ProcessAINode(scene->mRootNode, scene);
    LoadMaterials(scene);
    sampler.Create();
    sampler.DefaultConfiguration();

    model_matrix = glm::scale(model_matrix, scale);

    model_matrix = glm::rotate(model_matrix, glm::radians(rotate.x), glm::vec3(1, 0, 0));
    model_matrix = glm::rotate(model_matrix, glm::radians(rotate.y), glm::vec3(0, 1, 0));
    model_matrix = glm::rotate(model_matrix, glm::radians(rotate.z), glm::vec3(0, 0, 1));

    model_matrix = glm::translate(model_matrix, translate);
}

void Model::CreateBezierBuffers() {
    meshes = std::vector<Mesh::Mesh>{};
    textures = std::vector<Texture>{};
    const std::vector<Vertex> control_points{
        {{0.0, 0.0, 0.0}},  {{2.0, 0.0, 1.5}},  {{4.0, 0.0, 2.9}}, {{6.0, 0.0, 0.0}},
        {{0.0, 2.0, 1.1}},  {{2.0, 2.0, 3.9}},  {{4.0, 2.0, 3.1}}, {{6.0, 2.0, 0.7}},
        {{0.0, 4.0, -0.5}}, {{2.0, 4.0, 2.6}},  {{4.0, 4.0, 2.4}}, {{6.0, 4.0, 0.4}},
        {{0.0, 6.0, -0.5}}, {{2.0, 6.0, -0.1}}, {{4.0, 6.0, 1.3}}, {{6.0, 6.0, -0.2}},
    };
    AddMesh(control_points);

    model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) *
                   glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 1, 0)) *
                   glm::translate(glm::mat4(1.0f), glm::vec3(-3, -3, 0));
}

void Model::AddMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices,
                    u32 mat_idx) {
    meshes.push_back(Mesh::Mesh(vertices, indices, mat_idx));
}

void Model::Render(GLenum topology, bool with_texture) {
    if (with_texture) {
        RenderWithDiffuseTexture(topology);
    } else {
        RenderWithoutTextures(topology);
    }
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
        textures[i].UploadDataFromPath(texture_path);
    }
}

void Model::RenderWithDiffuseTexture(GLenum topology) {
    glActiveTexture(GL_TEXTURE0);
    glBindSampler(0, sampler.handle);
    for (size_t i = 0; i < meshes.size(); ++i) {
        const auto& mesh = meshes[i];
        if (!textures.empty()) {
            const auto texture_idx = mesh.material_index;
            glBindTexture(GL_TEXTURE_2D, textures.at(texture_idx).handle);
        }
        mesh.Render(topology);
    }
}

void Model::RenderWithoutTextures(GLenum topology) {
    for (const auto& mesh : meshes) {
        mesh.Render(topology);
    }
}
} // namespace Model
