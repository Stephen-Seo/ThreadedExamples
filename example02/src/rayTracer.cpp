#include "rayTracer.hpp"

#include <cmath>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

const float PI = std::acos(-1.0f);

glm::vec3 Ex02::RT::Internal::defaultSpherePos() {
    // the default model matrix pushes everything back 2 units,
    // so it should be ok to define the sphere at location 0
    return glm::vec3{0.0f, 0.0f, 0.0f};
}

glm::vec3 Ex02::RT::Internal::defaultLightPos() {
    return glm::vec3{1.0f, 1.0f, 1.0f};
}

glm::mat4x4 Ex02::RT::Internal::defaultMVP() {
    glm::mat4x4 mvp = glm::perspective(
        PI / 2.0f,
        1.0f,
        EX02_RAY_TRACER_DEFAULT_NEAR_PLANE,
        EX02_RAY_TRACER_DEFAULT_FAR_PLANE);

    mvp *= glm::lookAt(
        glm::vec3{0.0f, 0.0f, 0.0f}, 
        glm::vec3{0.0f, 0.0f, -1.0f},
        glm::vec3{0.0f, 1.0f, 0.0f});

    // model pushes back by 2 units
    mvp *= glm::translate(
        glm::identity<glm::mat4x4>(),
        glm::vec3{0.0f, 0.0f, -2.0f});

    return mvp;
}

std::vector<unsigned char> Ex02::RT::renderGraySphere(
        unsigned int outputWidth,
        unsigned int outputHeight,
        float sphereRadius,
        int threadCount,
        glm::vec3 spherePos,
        glm::vec3 lightPos,
        glm::mat4x4 mvp) {
    std::vector<unsigned char> grayscalePixels;

    return grayscalePixels;
}
