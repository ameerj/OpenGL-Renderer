#pragma once
#include <string>

#include <glm/mat4x4.hpp>
#include "mesh.h"

struct aiNode;
struct aiScene;

namespace Model {
class Model {
public:
    Model() = default;
    ~Model() = default;

    void ParseObjModel(const std::string& path);

    void ParseObjModel(const std::string& path, glm::vec3& scale, glm::vec3& translate,
                       float rotate);

    void CreateBezierBuffers();

    void AddMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices = {},
                 u32 mat_idx = 0);

    void Render(GLenum topology);

    glm::mat4 ModelMatrix() {
        return model_matrix;
    }

private:
    void ProcessAINode(aiNode* node, const aiScene* scene);

    void LoadMaterials(const aiScene* scene);

    std::vector<Mesh::Mesh> meshes;
    std::vector<Texture> textures;
    Sampler sampler;
    glm::mat4 model_matrix{};
};

} // namespace Model
