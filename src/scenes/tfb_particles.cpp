#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "tfb_particles.h"

namespace Scenes {
namespace {
float RandomNormalized() {
    float min = -1;
    float max = 1;
    float r = (float)rand() / (float)RAND_MAX;
    float result = min + r * (max - min);
    return result;
}

Vertex Random2DNormalized() {
    return {{RandomNormalized(), RandomNormalized(), 0.0}};
}
} // namespace

void TFBDemo::Init() {
    std::vector<Vertex> points(3000);
    std::generate(points.begin(), points.end(), Random2DNormalized);
    mesh_model.AddMesh(points);
    shader_program = Shaders::GetTfbShader();
    glUseProgram(shader_program.handle);

    glClearColor(0.25, 0.25, 0.25, 0.0);
}

void TFBDemo::Configure() {
    glEnable(GL_PROGRAM_POINT_SIZE);
}

void TFBDemo::Render() {
    mesh_model.Render(GL_POINTS);
}

} // namespace Scenes
