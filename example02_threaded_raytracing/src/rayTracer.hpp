#ifndef EX02_RAY_TRACER_HPP
#define EX02_RAY_TRACER_HPP

constexpr float EX02_RAY_TRACER_VIEW_RATIO = 0.5F;
constexpr float EX02_RAY_TRACER_DEFAULT_NEAR_PLANE = 0.2F;
constexpr float EX02_RAY_TRACER_DEFAULT_FAR_PLANE = 4.0F;
constexpr float EX02_RAY_TRACER_COLL_INCREMENT = 2.0F;
constexpr float EX02_RAY_TRACER_GRAY_SPHERE_RADIUS = 1.5F;

#include <optional>
#include <string>
#include <tuple>
#include <vector>

#ifdef __MINGW32__
# include "/usr/include/glm/mat4x4.hpp"
# include "/usr/include/glm/matrix.hpp"
# include "/usr/include/glm/vec3.hpp"
#else
# include <glm/mat4x4.hpp>
# include <glm/matrix.hpp>
# include <glm/vec3.hpp>
#endif

namespace Ex02::RT {

struct Pixel {
  Pixel();

  unsigned char r, g, b;
};

class Image {
public:
  Image(unsigned int width, unsigned int height);

  Pixel &getPixel(unsigned int x, unsigned int y);
  const Pixel &getPixel(unsigned int x, unsigned int y) const;

  // returns actual output filename
  std::string writeToFile(const std::string &filename) const;

private:
  unsigned int width;
  std::vector<Pixel> data;
};

namespace Internal {
  using RTSVisibleType = std::optional<std::tuple<glm::vec3, glm::vec3>>;

  struct LightSource {
    LightSource();

    glm::vec3 pos;
    float falloffStart;
    float falloffEnd;
    glm::vec3 color;

    void applyLight(glm::vec3 pos, Pixel &pixelOut) const;
  };

  struct Sphere {
    Sphere();

    glm::vec3 pos;
    float radius;

    std::optional<glm::vec3> rayToSphere(glm::vec3 rayPos,
                                         glm::vec3 rayDirUnit) const;

    RTSVisibleType rayToSphereVisible(glm::vec3 rayPos, glm::vec3 rayDirUnit,
                                      const LightSource &light) const;
  };

  // returns pos of collision
  std::optional<glm::vec3> rayToSphere(glm::vec3 rayPos, glm::vec3 rayDirUnit,
                                       glm::vec3 spherePos, float sphereRadius);

  float angleBetweenRays(glm::vec3 a, glm::vec3 b);

  float distBetweenPositions(glm::vec3 a, glm::vec3 b);

  // first vec3 is result from rayToSphere(), second is ray to light source
  RTSVisibleType rayToSphereVisible(glm::vec3 rayPos, glm::vec3 rayDirUnit,
                                    glm::vec3 spherePos, float sphereRadius,
                                    glm::vec3 lightPos);
} // namespace Internal

Image renderGraySphere(unsigned int outputWidth, unsigned int outputHeight,
                       unsigned int threadCount = 1);

Image renderColorsWithSpheres(unsigned int outputWidth,
                              unsigned int outputHeight,
                              unsigned int threadCount = 1);

} // namespace Ex02::RT

#endif
