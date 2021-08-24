#include "rayTracer.hpp"

#include <cmath>
#include <fstream>
#include <array>
#include <thread>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

const float PI = std::acos(-1.0f);

Ex02::RT::Pixel::Pixel() :
    r(0),
    g(0),
    b(0)
{}

Ex02::RT::Image::Image(unsigned int width, unsigned int height) :
    width(width)
{
    data.resize(width * height);
}

Ex02::RT::Pixel& Ex02::RT::Image::getPixel(
        unsigned int x, unsigned int y) {
    return data.at(x + y * width);
}

const Ex02::RT::Pixel& Ex02::RT::Image::getPixel(
        unsigned int x, unsigned int y) const {
    return data.at(x + y * width);
}

void Ex02::RT::Image::writeToFile(const std::string &filename) const {
    std::ofstream out(filename + ".ppm");
    out << "P3\n" << width << ' ' << data.size() / width << " 255"
        << '\n';

    for(unsigned int j = 0; j < data.size() / width; ++j) {
        for(unsigned int i = 0; i < width; ++i) {
            out << (int)data.at(i + j * width).r << ' '
                << (int)data.at(i + j * width).g << ' '
                << (int)data.at(i + j * width).b << ' ';
        }
        out << '\n';
    }
}

/*
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
    mvp = glm::translate(
        mvp,
        glm::vec3{0.0f, 0.0f, 2.0f});

    return mvp;
}
*/

Ex02::RT::Internal::LightSource::LightSource() :
    pos{0.0f, 0.0f, 0.0f},
    falloffStart(2.0f),
    falloffEnd(7.0f),
    color{1.0f, 1.0f, 1.0f}
{}

void Ex02::RT::Internal::LightSource::applyLight(
        glm::vec3 pos, Pixel &pixelOut) const {
    pos = this->pos - pos;
    float dist = std::sqrt(
        pos.x * pos.x
        + pos.y * pos.y
        + pos.z * pos.z);
    if(dist < falloffStart) {
        const auto applyColor = [] (auto *color, unsigned char *out) {
            unsigned int temp = (unsigned int)*out
                + (unsigned int)(*color * 255.0f);
            if(temp > 255) {
                *out = 255;
            } else {
                *out = temp;
            }
        };
        applyColor(&color.x, &pixelOut.r);
        applyColor(&color.y, &pixelOut.g);
        applyColor(&color.z, &pixelOut.b);
    } else if(dist >= falloffStart && dist <= falloffEnd) {
        const auto applyFalloffColor = [] (auto *color, unsigned char *out, float f) {
            unsigned int temp = (unsigned int)*out
                + (unsigned int)(*color * 255.0f * f);
            if(temp > 255) {
                *out = 255;
            } else {
                *out = temp;
            }
        };
        float f = (1.0f - (dist - falloffStart) / (falloffEnd - falloffStart));
        applyFalloffColor(&color.x, &pixelOut.r, f);
        applyFalloffColor(&color.y, &pixelOut.g, f);
        applyFalloffColor(&color.z, &pixelOut.b, f);
    }
}

void Ex02::RT::Internal::LightSource::applyLight(
        glm::vec3 pos, Pixel &pixelOut, std::mutex* mutex) const {
    pos = this->pos - pos;
    float dist = std::sqrt(
        pos.x * pos.x
        + pos.y * pos.y
        + pos.z * pos.z);
    if(dist < falloffStart) {
        const auto applyColor = [] (auto *color, unsigned char *out) {
            unsigned int temp = (unsigned int)*out
                + (unsigned int)(*color * 255.0f);
            if(temp > 255) {
                *out = 255;
            } else {
                *out = temp;
            }
        };
        if(mutex) {
            std::lock_guard<std::mutex> lock(*mutex);
            applyColor(&color.x, &pixelOut.r);
            applyColor(&color.y, &pixelOut.g);
            applyColor(&color.z, &pixelOut.b);
        } else {
            applyColor(&color.x, &pixelOut.r);
            applyColor(&color.y, &pixelOut.g);
            applyColor(&color.z, &pixelOut.b);
        }
    } else if(dist >= falloffStart && dist <= falloffEnd) {
        const auto applyFalloffColor = [] (auto *color, unsigned char *out, float f) {
            unsigned int temp = (unsigned int)*out
                + (unsigned int)(*color * 255.0f * f);
            if(temp > 255) {
                *out = 255;
            } else {
                *out = temp;
            }
        };
        float f = (1.0f - (dist - falloffStart) / (falloffEnd - falloffStart));
        if(mutex) {
            std::lock_guard<std::mutex> lock(*mutex);
            applyFalloffColor(&color.x, &pixelOut.r, f);
            applyFalloffColor(&color.y, &pixelOut.g, f);
            applyFalloffColor(&color.z, &pixelOut.b, f);
        } else {
            applyFalloffColor(&color.x, &pixelOut.r, f);
            applyFalloffColor(&color.y, &pixelOut.g, f);
            applyFalloffColor(&color.z, &pixelOut.b, f);
        }
    }
}

Ex02::RT::Internal::Sphere::Sphere() :
    pos{0.0f, 0.0f, 0.0f},
    radius(2.5f)
{}

std::optional<glm::vec3> Ex02::RT::Internal::Sphere::rayToSphere(
        glm::vec3 rayPos, glm::vec3 rayDirUnit) const {
    return Ex02::RT::Internal::rayToSphere(rayPos, rayDirUnit, pos, radius);
}

Ex02::RT::Internal::RTSVisibleType Ex02::RT::Internal::Sphere::rayToSphereVisible(
        glm::vec3 rayPos, glm::vec3 rayDirUnit,
        const LightSource &light) const {
    return Ex02::RT::Internal::rayToSphereVisible(
        rayPos, rayDirUnit, pos, radius, light.pos);
}

std::optional<glm::vec3> Ex02::RT::Internal::rayToSphere(
        glm::vec3 rayPos,
        glm::vec3 rayDirUnit,
        glm::vec3 spherePos,
        float sphereRadius) {
    // check if there is collision
    glm::vec3 tempVec = rayPos - spherePos;
    float temp =
        rayDirUnit.x * tempVec.x
        + rayDirUnit.y * tempVec.y
        + rayDirUnit.z * tempVec.z;
    float delta = temp * temp;
    temp =
        tempVec.x * tempVec.x
        + tempVec.y * tempVec.y
        + tempVec.z * tempVec.z
        - sphereRadius * sphereRadius;
    delta -= temp;

    if(delta < 0.0f) {
        return {};
    } else {
        temp =
            rayDirUnit.x * tempVec.x
            + rayDirUnit.y * tempVec.y
            + rayDirUnit.z * tempVec.z;
        float dist = -temp - std::sqrt(delta);
        float dist2 = -temp + std::sqrt(delta);
        float min = dist > dist2 ? dist2 : dist;
        float max = dist > dist2 ? dist : dist2;
        if(min < 0.0f) {
            if(max < 0.0f) {
                return {};
            } else {
                return {rayPos + rayDirUnit * max};
            }
        } else {
            return {rayPos + rayDirUnit * min};
        }
    }
}

float Ex02::RT::Internal::angleBetweenRays(glm::vec3 a, glm::vec3 b) {
    float dot = a.x * b.x + a.y * b.y + a.z * b.z;
    float amag = std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    float bmag = std::sqrt(b.x * b.x + b.y * b.y + b.z * b.z);

    return std::acos(dot / amag / bmag);
}

float Ex02::RT::Internal::distBetweenPositions(glm::vec3 a, glm::vec3 b) {
    a = a - b;
    return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Ex02::RT::Internal::RTSVisibleType Ex02::RT::Internal::rayToSphereVisible(
        glm::vec3 rayPos,
        glm::vec3 rayDirUnit,
        glm::vec3 spherePos,
        float sphereRadius,
        glm::vec3 lightPos) {
    auto collPos = rayToSphere(rayPos, rayDirUnit, spherePos, sphereRadius);
    if(collPos) {
        glm::vec3 toLight = lightPos - *collPos;
        glm::vec3 toLightUnit = toLight / std::sqrt(
            toLight.x * toLight.x
            + toLight.y * toLight.y
            + toLight.z * toLight.z);
        glm::vec3 toLightPos = *collPos + toLight / 3.0f;
        auto collResult = Internal::rayToSphere(
            toLightPos, toLightUnit, spherePos, sphereRadius);
        if(collResult) {
            return {};
        } else {
            return {{*collPos, toLight}};
        }
    } else {
        return {};
    }
}

Ex02::RT::Image Ex02::RT::renderGraySphere(
        unsigned int outputWidth,
        unsigned int outputHeight,
        unsigned int threadCount) {
    const glm::vec3 spherePos{0.0f, 0.0f, -2.5f};
    const glm::vec3 lightPos{4.0f, 4.0f, 0.0f};
    Image image(outputWidth, outputHeight);
    const glm::vec3 rayPos{0.0f, 0.0f, 0.0f};
    const float lightFalloffStart = 4.5f;
    const float lightFalloffEnd = 7.0f;
//    if(threadCount <= 1) {
        for(unsigned int j = 0; j < outputHeight; ++j) {
            float offsetY = ((float)j + 0.5f - ((float)outputHeight / 2.0f));
            for(unsigned int i = 0; i < outputWidth; ++i) {
                float offsetX = ((float)i + 0.5f - ((float)outputWidth / 2.0f));
                glm::vec3 rayDir{
                    offsetX,
                    offsetY,
                    -(float)outputHeight * EX02_RAY_TRACER_VIEW_RATIO};
                glm::vec3 rayDirUnit = rayDir / std::sqrt(
                    rayDir.x * rayDir.x
                    + rayDir.y * rayDir.y
                    + rayDir.z * rayDir.z);
                auto rayResult = Internal::rayToSphereVisible(
                    rayPos, rayDirUnit,
                    spherePos, EX02_RAY_TRACER_GRAY_SPHERE_RADIUS,
                    lightPos);
                if(rayResult) {
                    glm::vec3 *toLight = &std::get<1>(rayResult.value());
                    float dist = std::sqrt(
                        toLight->x * toLight->x
                        + toLight->y * toLight->y
                        + toLight->z * toLight->z);
                    if(dist < lightFalloffStart) {
                        image.getPixel(i, j).r = 255;
                        image.getPixel(i, j).g = 255;
                        image.getPixel(i, j).b = 255;
                    } else if(dist >= lightFalloffStart && dist <= lightFalloffEnd) {
                        image.getPixel(i, j).r =
                            (1.0f - (dist - lightFalloffStart)
                                / (lightFalloffEnd - lightFalloffStart))
                            * 255.0f;
                        image.getPixel(i, j).g = image.getPixel(i, j).r;
                        image.getPixel(i, j).b = image.getPixel(i, j).r;
                    }
                }
            }
        }
//    } else {
//    }

    return image;
}

Ex02::RT::Image Ex02::RT::renderColorsWithSpheres(
        unsigned int outputWidth,
        unsigned int outputHeight,
        unsigned int threadCount) {
    Image image(outputWidth, outputHeight);
    const glm::vec3 rayPos{0.0f, 0.0f, 0.0f};
    std::array<Internal::Sphere, 7> spheres;
    std::array<Internal::LightSource, 3> lights;

    spheres[0].pos.x = 2.0f;
    spheres[0].pos.y = -2.0f;
    spheres[0].pos.z = -4.5f;
    spheres[0].radius = 1.0f;

    spheres[1].pos.x = -2.0f;
    spheres[1].pos.y = 2.0f;
    spheres[1].pos.z = -4.5f;
    spheres[1].radius = 1.0f;

    spheres[2].pos.x = 0.0f;
    spheres[2].pos.y = 0.0f;
    spheres[2].pos.z = -6.0f;
    spheres[2].radius = 2.0f;

    spheres[3].pos.x = 2.0f;
    spheres[3].pos.y = 2.0f;
    spheres[3].pos.z = -2.5;
    spheres[3].radius = 1.0f;

    spheres[4].pos.x = -2.0f;
    spheres[4].pos.y = -2.0f;
    spheres[4].pos.z = -2.5;
    spheres[4].radius = 1.0f;

    spheres[5].pos.x = -0.7f;
    spheres[5].pos.y = -0.7f;
    spheres[5].pos.z = -4.0f;
    spheres[5].radius = -0.7f;

    spheres[6].pos.x = 0.7f;
    spheres[6].pos.y = 0.7f;
    spheres[6].pos.z = -4.0f;
    spheres[6].radius = -0.7f;

    lights[0].color.r = 1.0f;
    lights[0].color.g = 0.0f;
    lights[0].color.b = 0.0f;
    lights[0].pos.x = 0.0f;
    lights[0].pos.y = -1.0f;
    lights[0].pos.z = 0.0f;
    lights[0].falloffStart = 3.0f;
    lights[0].falloffEnd = 7.0f;

    lights[1].color.r = 0.0f;
    lights[1].color.g = 1.0f;
    lights[1].color.b = 0.0f;
    lights[1].pos.x = std::cos(PI / 3.0f);
    lights[1].pos.y = std::sin(PI / 3.0f);
    lights[1].pos.z = 0.0f;
    lights[1].falloffStart = 3.0f;
    lights[1].falloffEnd = 7.0f;

    lights[2].color.r = 0.0f;
    lights[2].color.g = 0.0f;
    lights[2].color.b = 1.0f;
    lights[2].pos.x = std::cos(PI * 2.0 / 3.0f);
    lights[2].pos.y = std::sin(PI * 2.0 / 3.0f);
    lights[2].pos.z = 0.0f;
    lights[2].falloffStart = 3.0f;
    lights[2].falloffEnd = 7.0f;

    const auto yIteration = [&spheres, &lights, &image, outputWidth, outputHeight, rayPos] (unsigned int j, std::mutex *mutex) {
        float offsetY = ((float)j + 0.5f - ((float)outputHeight / 2.0f));
        for(unsigned int i = 0; i < outputWidth; ++i) {
            float offsetX = ((float)i + 0.5f - ((float)outputWidth / 2.0f));
            glm::vec3 rayDir{
                offsetX,
                offsetY,
                -(float)outputHeight * EX02_RAY_TRACER_VIEW_RATIO};
            glm::vec3 rayDirUnit = rayDir / std::sqrt(
                rayDir.x * rayDir.x
                + rayDir.y * rayDir.y
                + rayDir.z * rayDir.z);

            // cast ray to all spheres, finding closest result
            std::optional<std::tuple<glm::vec3, float, unsigned int>> closestResult;
            for(unsigned int idx = 0; idx < spheres.size(); ++idx) {
                auto result = spheres[idx].rayToSphere(rayPos, rayDirUnit);
                if(result) {
                    float dist = Internal::distBetweenPositions(
                        rayPos, spheres[idx].pos);
                    if(closestResult) {
                        if(dist < std::get<1>(*closestResult)) {
                            closestResult = {{*result, dist, idx}};
                        }
                    } else {
                        closestResult = {{*result, dist, idx}};
                    }
                }
            }

            if(!closestResult) {
                continue;
            }

            // cast ray to each light checking if colliding with other
            // spheres
            for(const auto &light : lights) {
                glm::vec3 toLight = light.pos - std::get<0>(*closestResult);
                glm::vec3 toLightUnit = toLight / std::sqrt(
                    toLight.x * toLight.x
                    + toLight.y * toLight.y
                    + toLight.z * toLight.z);
                bool isBlocked = false;
                for(unsigned int idx = 0; idx < spheres.size(); ++idx) {
                    if(idx == std::get<2>(*closestResult)) {
                        continue;
                    }
                    auto result = spheres[idx].rayToSphere(
                        std::get<0>(*closestResult),
                        toLightUnit);
                    if(result) {
                        isBlocked = true;
                        break;
                    }
                }
                if(isBlocked) {
                    continue;
                }

                // at this point, it is known that no spheres blocks ray
                // to light
                light.applyLight(
                    std::get<0>(*closestResult),
                    image.getPixel(i, j),
                    mutex);
            }
        }
    };

    if(threadCount <= 1) {
        for(unsigned int j = 0; j < outputHeight; ++j) {
            yIteration(j, nullptr);
        }
    } else {
        std::vector<std::thread> threads;
        std::mutex mutex;
        unsigned int range = outputHeight / threadCount;
        for(unsigned int threadIdx = 0; threadIdx < threadCount; ++threadIdx) {
            unsigned int start = range * threadIdx;
            unsigned int end = range * (threadIdx + 1);
            if(threadIdx + 1 == threadCount) {
                end = outputHeight;
            }
            threads.emplace_back(std::thread([&yIteration] (unsigned int start, unsigned int end, std::mutex *mutex) {
                for(unsigned int y = start; y < end; ++y) {
                    yIteration(y, mutex);
                }
            },
                start,
                end,
                &mutex));
        }
        for(std::thread &thread : threads) {
            thread.join();
        }
    }

    return image;
}
