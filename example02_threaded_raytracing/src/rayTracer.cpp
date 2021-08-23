#include "rayTracer.hpp"

#include <cmath>
#include <fstream>

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

glm::vec3 Ex02::RT::Internal::defaultSpherePos() {
    return glm::vec3{0.0f, 0.0f, -2.5f};
}

glm::vec3 Ex02::RT::Internal::defaultLightPos() {
    return glm::vec3{4.0f, 4.0f, 0.0f};
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

std::optional<glm::vec3> Ex02::RT::Internal::rayToSphere(
        glm::vec3 rayPos,
        glm::vec3 rayDir,
        glm::vec3 spherePos,
        float sphereRadius) {
    // check if there is collision
    glm::vec3 tempVec = rayPos - spherePos;
    float temp =
        rayDir.x * tempVec.x
        + rayDir.y * tempVec.y
        + rayDir.z * tempVec.z;
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
            rayDir.x * tempVec.x
            + rayDir.y * tempVec.y
            + rayDir.z * tempVec.z;
        float dist = -temp - std::sqrt(delta);
        float dist2 = -temp + std::sqrt(delta);
        float min = dist > dist2 ? dist2 : dist;
        float max = dist > dist2 ? dist : dist2;
        if(min < 0.0f) {
            if(max < 0.0f) {
                return {};
            } else {
                return {rayPos + rayDir * max};
            }
        } else {
            return {rayPos + rayDir * min};
        }
    }
}

float Ex02::RT::Internal::angleBetweenRays(glm::vec3 a, glm::vec3 b) {
    float dot = a.x * b.x + a.y * b.y + a.z * b.z;
    float amag = std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    float bmag = std::sqrt(b.x * b.x + b.y * b.y + b.z * b.z);

    return std::acos(dot / amag / bmag);
}

Ex02::RT::Internal::RTSVisibleType Ex02::RT::Internal::rayToSphereVisible(
        glm::vec3 rayPos,
        glm::vec3 rayDir,
        glm::vec3 spherePos,
        float sphereRadius,
        glm::vec3 lightPos) {
    glm::vec3 rayDirUnit = rayDir / std::sqrt(
        rayDir.x * rayDir.x
        + rayDir.y * rayDir.y
        + rayDir.z * rayDir.z);

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
        int threadCount,
        glm::vec3 spherePos,
        glm::vec3 lightPos) {
    Image image(outputWidth, outputHeight);
    glm::vec3 rayPos{0.0f, 0.0f, 0.0f};
    float lightFalloffStart = 4.5f;
    float lightFalloffEnd = 7.0f;
    if(threadCount == 1) {
        for(unsigned int j = 0; j < outputHeight; ++j) {
            float offsetY = ((float)j + 0.5f - ((float)outputHeight / 2.0f));
            for(unsigned int i = 0; i < outputWidth; ++i) {
                float offsetX = ((float)i + 0.5f - ((float)outputWidth / 2.0f));
                glm::vec3 rayDir = glm::vec3{
                    offsetX, offsetY, -(float)outputHeight * EX02_RAY_TRACER_VIEW_RATIO};
                auto rayResult = Internal::rayToSphereVisible(
                    rayPos, rayDir,
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
    } else {
    }

    return image;
}
