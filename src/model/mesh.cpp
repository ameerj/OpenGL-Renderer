#include "mesh.h"

namespace Mesh {
Mesh::Mesh(const std::vector<Vertex>& vertices_, const std::vector<u32>& indices_)
    : vertices{vertices_}, indices{indices_} {
    vertex_buffer.Create();
    glNamedBufferData(vertex_buffer.handle, vertices.size() * sizeof(vertices[0]), vertices.data(),
                      GL_STATIC_DRAW);

    vertex_array_object.Create();
    glBindVertexArray(vertex_array_object.handle);

    index_buffer.Create();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(),
                 GL_STATIC_DRAW);
}

void Mesh::Render() const {
    glBindVertexArray(vertex_array_object.handle);
    glVertexArrayVertexBuffer(vertex_array_object.handle, 0, vertex_buffer.handle, 0,
                              sizeof(vertices[0]));
    constexpr GLuint PositionLocation = 0;
    constexpr GLuint TextureCoordsLocation = 1;
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribFormat(PositionLocation, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexAttribBinding(PositionLocation, 0);
    glEnableVertexAttribArray(TextureCoordsLocation);
    glVertexAttribFormat(TextureCoordsLocation, 2, GL_FLOAT, GL_FALSE,
                         offsetof(Vertex, texture_coords));
    glVertexAttribBinding(TextureCoordsLocation, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.handle);

    glDrawElements(GL_TRIANGLES, NumIndices(), GL_UNSIGNED_INT, 0);
}

} // namespace Mesh
