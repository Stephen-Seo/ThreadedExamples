#ifndef EX02_RAY_TRACER_HPP
#define EX02_RAY_TRACER_HPP

#define EX02_RAY_TRACER_VIEW_RATIO 0.5f
#define EX02_RAY_TRACER_DEFAULT_NEAR_PLANE 0.2f
#define EX02_RAY_TRACER_DEFAULT_FAR_PLANE 4.0f
#define EX02_RAY_TRACER_COLL_INCREMENT 2.0f
#define EX02_RAY_TRACER_GRAY_SPHERE_RADIUS 1.5f

#include <vector>
#include <optional>
#include <string>
#include <tuple>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>

namespace Ex02 {
namespace RT {

struct Pixel {
    Pixel();

    unsigned char r,g,b;
};

class Image {
public:
    Image(unsigned int width, unsigned int height);

    Pixel& getPixel(unsigned int x, unsigned int y);
    const Pixel& getPixel(unsigned int x, unsigned int y) const;

    void writeToFile(const std::string &filename) const;

private:
    unsigned int width;
    std::vector<Pixel> data;
};

namespace Internal {

    glm::vec3 defaultSpherePos();
    glm::vec3 defaultLightPos();

    // returns pos of collision
    std::optional<glm::vec3> rayToSphere(
        glm::vec3 rayPos,
        glm::vec3 rayDir,
        glm::vec3 spherePos,
        float sphereRadius);

    float angleBetweenRays(
        glm::vec3 a,
        glm::vec3 b);

    // first vec3 is result from rayToSphere(), second is ray to light source
    typedef std::optional<std::tuple<glm::vec3, glm::vec3>> RTSVisibleType;
    RTSVisibleType rayToSphereVisible(
        glm::vec3 rayPos,
        glm::vec3 rayDir,
        glm::vec3 spherePos,
        float sphereRadius,
        glm::vec3 lightPos);
}

Image renderGraySphere(
    unsigned int outputWidth,
    unsigned int outputHeight,
    int threadCount = 1,
    glm::vec3 spherePos = Internal::defaultSpherePos(),
    glm::vec3 lightPos = Internal::defaultLightPos()
);

} // namespace RT
} // namespace Ex02

#endif
