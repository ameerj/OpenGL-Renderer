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

    void ProcessAINode(aiNode* node, const aiScene* scene);

    void Render();

private:
    std::vector<Mesh::Mesh> meshes;
};

} // namespace Model
