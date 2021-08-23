#include "rayTracer.hpp"

#include <cmath>
#include <fstream>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

const float PI = std::acos(-1.0f);

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
    // ensure rayDir is a unit vector
    float rayDirLength = std::sqrt(
            rayDir.x * rayDir.x
            + rayDir.y * rayDir.y
            + rayDir.z * rayDir.z);
    rayDir /= rayDirLength;

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

std::vector<unsigned char> Ex02::RT::renderGraySphere(
        unsigned int outputWidth,
        unsigned int outputHeight,
        float sphereRadius,
        int threadCount,
        glm::vec3 spherePos,
        glm::vec3 lightPos) {
    std::vector<unsigned char> grayscalePixels;
    grayscalePixels.resize(outputWidth * outputHeight);
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
                auto rayResult = Internal::rayToSphere(
                    rayPos, rayDir, spherePos, sphereRadius);
                if(rayResult) {
                    glm::vec3 toLight = lightPos - *rayResult;
                    glm::vec3 toLightCached = toLight;
                    toLight /= std::sqrt(
                        toLight.x * toLight.x
                        + toLight.y * toLight.y
                        + toLight.z * toLight.z);
                    glm::vec3 toLightPos = *rayResult + toLight;
                    auto collResult = Internal::rayToSphere(
                        toLightPos, toLight, spherePos, sphereRadius);
                    if(collResult) {
                        continue;
                    }

                    float dist = std::sqrt(
                        toLightCached.x * toLightCached.x
                        + toLightCached.y * toLightCached.y
                        + toLightCached.z * toLightCached.z);
                    if(dist < lightFalloffStart) {
                        grayscalePixels.at(i + j * outputWidth) = 255;
                    } else if(dist >= lightFalloffStart && dist <= lightFalloffEnd) {
                        grayscalePixels.at(i + j * outputWidth) =
                            (1.0f - (dist - lightFalloffStart)
                                    / (lightFalloffEnd - lightFalloffStart))
                                * 255.0f;
                    }
                }
            }
        }
    } else {
    }

    return grayscalePixels;
}

void Ex02::RT::writeGrayscaleToFile(
        const std::vector<unsigned char> &pixels,
        unsigned int outputWidth,
        std::string filename) {
    std::ofstream out(filename + ".pgm");
    out << "P2\n" << outputWidth << ' ' << pixels.size() / outputWidth << " 255"
        << '\n';

    for(unsigned int j = 0; j < pixels.size() / outputWidth; ++j) {
        for(unsigned int i = 0; i < outputWidth; ++i) {
            out << (int)pixels.at(i + j * outputWidth)
                << ' ';
        }
        out << '\n';
    }
}
