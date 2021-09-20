#pragma once
#include <string_view>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../common/common_types.h"
#include "../resources/resources.h"

namespace Model {
struct Vertex {
    glm::vec3 position;
};

class Model {
public:
    Model() = default;
    Model(std::string_view filename);

    size_t NumIndices() {
        return indices.size();
    }

    std::vector<Vertex> vertices;
    std::vector<u32> indices;

private:
    VAO vertex_array_object;
    Buffer vertex_buffer;
    Buffer index_buffer;
};

} // namespace Model
