#ifndef EX02_RAY_TRACER_HPP
#define EX02_RAY_TRACER_HPP

#define EX02_RAY_TRACER_VIEW_RATIO 0.5f
#define EX02_RAY_TRACER_DEFAULT_NEAR_PLANE 0.2f
#define EX02_RAY_TRACER_DEFAULT_FAR_PLANE 4.0f
#define EX02_RAY_TRACER_COLL_INCREMENT 2.0f

#include <vector>
#include <optional>
#include <string>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>

namespace Ex02 {
namespace RT {

namespace Internal {
    glm::vec3 defaultSpherePos();
    glm::vec3 defaultLightPos();

    std::optional<glm::vec3> rayToSphere(
        glm::vec3 rayPos,
        glm::vec3 rayDir,
        glm::vec3 spherePos,
        float sphereRadius);

    float angleBetweenRays(
        glm::vec3 a,
        glm::vec3 b);
}

std::vector<unsigned char> renderGraySphere(
    unsigned int outputWidth,
    unsigned int outputHeight,
    float sphereRadius,
    int threadCount = 1,
    glm::vec3 spherePos = Internal::defaultSpherePos(),
    glm::vec3 lightPos = Internal::defaultLightPos()
);

void writeGrayscaleToFile(
    const std::vector<unsigned char> &pixels,
    unsigned int outputWidth,
    std::string filename);

} // namespace RT
} // namespace Ex02

#endif
