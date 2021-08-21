#ifndef EX02_RAY_TRACER_HPP
#define EX02_RAY_TRACER_HPP

#define EX02_RAY_TRACER_DEFAULT_NEAR_PLANE 0.2f
#define EX02_RAY_TRACER_DEFAULT_FAR_PLANE 4.0f
#define EX02_RAY_TRACER_COLL_INCREMENT 2.0f

#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>

namespace Ex02 {
namespace RT {

namespace Internal {
    glm::vec3 defaultSpherePos();
    glm::vec3 defaultLightPos();
    glm::mat4x4 defaultMVP();
}

std::vector<unsigned char> renderGraySphere(
    unsigned int outputWidth,
    unsigned int outputHeight,
    float sphereRadius,
    int threadCount = 1,
    glm::vec3 spherePos = Internal::defaultSpherePos(),
    glm::vec3 lightPos = Internal::defaultLightPos(),
    glm::mat4x4 mvp = Internal::defaultMVP()
);

} // namespace RT
} // namespace Ex02

#endif
