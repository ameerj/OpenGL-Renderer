#pragma once
#include <string>

#include "mesh.h"

struct aiNode;
struct aiScene;

namespace Model {
class Model {
public:
    Model() = default;
    ~Model() = default;

    void ParseObjModel(const std::string& path);

    void CreateBezierBuffers();

    void AddMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices = {},
                 u32 mat_idx = 0);

    void Render(GLenum topology);

private:
    void ProcessAINode(aiNode* node, const aiScene* scene);

    void LoadMaterials(const aiScene* scene);

    std::vector<Mesh::Mesh> meshes;
    std::vector<Texture> textures;
    Sampler sampler;
};

} // namespace Model
