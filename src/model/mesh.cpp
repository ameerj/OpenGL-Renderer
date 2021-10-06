#include "mesh.h"

namespace Mesh {
Mesh::Mesh(const std::vector<Vertex>& vertices_, const std::vector<u32>& indices_, u32 mat_idx)
    : vertices{vertices_}, indices{indices_}, material_index{mat_idx} {
    if (!vertices.empty()) {
        vertex_buffer.Create();
        glNamedBufferData(vertex_buffer.handle, vertices.size() * sizeof(vertices[0]),
                          vertices.data(), GL_STATIC_DRAW);
        vertex_array_object.Create();
        glBindVertexArray(vertex_array_object.handle);
    }
    if (!indices.empty()) {
        index_buffer.Create();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.handle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(),
                     GL_STATIC_DRAW);
    }
}

void Mesh::Render(GLenum topology) const {
    glBindVertexArray(vertex_array_object.handle);
    glVertexArrayVertexBuffer(vertex_array_object.handle, 0, vertex_buffer.handle, 0,
                              sizeof(vertices[0]));
    constexpr GLuint PositionLocation = 0;
    constexpr GLuint NormalLocation = 1;
    constexpr GLuint TextureCoordsLocation = 2;

    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribFormat(PositionLocation, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexAttribBinding(PositionLocation, 0);

    glEnableVertexAttribArray(NormalLocation);
    glVertexAttribFormat(NormalLocation, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexAttribBinding(NormalLocation, 0);

    glEnableVertexAttribArray(TextureCoordsLocation);
    glVertexAttribFormat(TextureCoordsLocation, 2, GL_FLOAT, GL_FALSE,
                         offsetof(Vertex, texture_coords));
    glVertexAttribBinding(TextureCoordsLocation, 0);

    if (index_buffer.handle) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.handle);
        glDrawElements(topology, NumIndices(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(topology, 0, vertices.size());
    }
}

} // namespace Mesh
