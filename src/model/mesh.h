#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../common/common_types.h"
#include "../resources/resources.h"

struct Vertex {
    glm::vec3 position;
    glm::vec2 texture_coords;
};

namespace Mesh {
class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices);

    void Render() const;

    size_t NumIndices() const {
        return indices.size();
    }

    std::vector<Vertex> vertices;
    std::vector<u32> indices;

private:
    VAO vertex_array_object;
    Buffer vertex_buffer;
    Buffer index_buffer;
};
} // namespace Mesh
