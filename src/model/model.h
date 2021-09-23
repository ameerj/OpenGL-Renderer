#pragma once
#include <string>

#include "mesh.h"

struct aiNode;
struct aiScene;

namespace Model {
class Model {
public:
    Model() = default;
    Model(const std::string& path);

    void Render();

private:
    void ProcessAINode(aiNode* node, const aiScene* scene);

    void LoadMaterials(const aiScene* scene);

    std::vector<Mesh::Mesh> meshes;
    std::vector<Texture> textures;
    std::vector<Sampler> samplers;
    Sampler sampler;
};

} // namespace Model
