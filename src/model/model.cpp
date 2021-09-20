#include "model.h"

namespace Model {
Model::Model(std::string_view filename) {
    vertices = {
        {{-0.5, -0.5, 0.5}},  {{-0.5, 0.5, 0.5}},  {{0.5, 0.5, 0.5}},  {{0.5, -0.5, 0.5}},
        {{-0.5, -0.5, -0.5}}, {{-0.5, 0.5, -0.5}}, {{0.5, 0.5, -0.5}}, {{0.5, -0.5, -0.5}},
    };
    indices = {
        0, 3, 2, 0, 2, 1, 2, 3, 7, 2, 7, 6, 0, 4, 7, 0, 7, 3,
        1, 2, 6, 1, 6, 5, 4, 5, 6, 4, 6, 7, 0, 1, 5, 0, 5, 4,
    };

    vertex_buffer.Create();
    glNamedBufferData(vertex_buffer.handle, vertices.size() * sizeof(vertices[0]), vertices.data(),
                      GL_STATIC_DRAW);

    vertex_array_object.Create();
    glBindVertexArray(vertex_array_object.handle);

    glVertexArrayVertexBuffer(vertex_array_object.handle, 0, vertex_buffer.handle, 0,
                              sizeof(vertices[0]));
    constexpr GLuint PositionLocation = 0;
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribFormat(PositionLocation, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(PositionLocation, 0);

    index_buffer.Create();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(),
                 GL_STATIC_DRAW);
}
} // namespace Model
