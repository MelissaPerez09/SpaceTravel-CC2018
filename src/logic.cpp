#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <cstring>
#include <algorithm>

#include "camera.h"
#include "color.h"
#include "ObjLoader.h"
#include "shaders.h"

const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 400;

Uint32 frameStart;
Uint32 frameTime;
int frameCount = 0;
int fps = 0;

SDL_Renderer* renderer;
std::array<double, WINDOW_WIDTH * WINDOW_HEIGHT> zbuffer;

enum ShaderId {
    gasPlanet, orbitalPlanet, misteryPlanet, star, rocoso, earth
};

struct Model {
    Uniforms uniforms;
    std::vector<Vertex> vertices;
    ShaderId i;
};

Color clearColor = {0, 0, 0, 255};
Color currentColor = {255, 255, 255, 255};
Color colorA(255, 0, 0, 255); // Red color
Color colorB(0, 255, 255, 255); // Green color
Color colorC(0, 24, 255, 255); // Blue color

glm::vec3 L = glm::vec3(0, 0, 200.0f); // Dirección de la luz en el espacio del ojo

Uniforms uniforms;

Color interpolateColor(const glm::vec3& barycentricCoord, const Color& colorA, const Color& colorB, const Color& colorC) {
    float u = barycentricCoord.x;
    float v = barycentricCoord.y;
    float w = barycentricCoord.z;

    // Realiza una interpolación lineal para cada componente del color
    uint8_t r = static_cast<uint8_t>(u * colorA.r + v * colorB.r + w * colorC.r);
    uint8_t g = static_cast<uint8_t>(u * colorA.g + v * colorB.g + w * colorC.g);
    uint8_t b = static_cast<uint8_t>(u * colorA.b + v * colorB.b + w * colorC.b);
    uint8_t a = static_cast<uint8_t>(u * colorA.a + v * colorB.a + w * colorC.a);

    return Color(r, g, b, a);
}

bool isBarycentricCoordInsideTriangle(const glm::vec3& barycentricCoord) {
    return barycentricCoord.x >= 0 && barycentricCoord.y >= 0 && barycentricCoord.z >= 0 &&
           barycentricCoord.x <= 1 && barycentricCoord.y <= 1 && barycentricCoord.z <= 1 &&
           glm::abs(1 - (barycentricCoord.x + barycentricCoord.y + barycentricCoord.z)) < 0.00001f;
}

glm::vec3 calculateBarycentricCoord(const glm::vec2& A, const glm::vec2& B, const glm::vec2& C, const glm::vec2& P) {
    float denominator = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);
    float u = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    float v = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    float w = 1 - u - v;
    return glm::vec3(u, v, w);
}

void render(const std::vector<Vertex>& vertexArray, const Uniforms& uniform, int id) {
    std::vector<Vertex> transformedVertexArray;
    for (const auto& vertex : vertexArray) {
        auto transformedVertex = vertexShader(vertex, uniform);
        transformedVertexArray.push_back(transformedVertex);
    }

    for (size_t i = 0; i < transformedVertexArray.size(); i += 3) {
        const Vertex& a = transformedVertexArray[i];
        const Vertex& b = transformedVertexArray[i + 1];
        const Vertex& c = transformedVertexArray[i + 2];

        glm::vec3 A = a.position;
        glm::vec3 B = b.position;
        glm::vec3 C = c.position;

        // Calculate the bounding box of the triangle
        int minX = static_cast<int>(std::min({A.x, B.x, C.x}));
        int minY = static_cast<int>(std::min({A.y, B.y, C.y}));
        int maxX = static_cast<int>(std::max({A.x, B.x, C.x}));
        int maxY = static_cast<int>(std::max({A.y, B.y, C.y}));

        // Iterate over each point in the bounding box
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                if (y<0 || x<0 || x>WINDOW_WIDTH || y>WINDOW_HEIGHT)
                    continue;

                glm::vec2 pixelPosition(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f); // Central point of the pixel
                glm::vec3 barycentricCoord = calculateBarycentricCoord(A, B, C, pixelPosition);

                if (isBarycentricCoordInsideTriangle(barycentricCoord)) {
                    Color g {0,0,0};
                    // Interpolate attributes (color, depth, etc.) using barycentric coordinates
                    Color interpolatedColor = interpolateColor(barycentricCoord, g, g, g);

                    // Calculate the depth (Z-coordinate) of the fragment using barycentric coordinates
                    float depth = barycentricCoord.x * A.z + barycentricCoord.y * B.z + barycentricCoord.z * C.z;

                    glm::vec3 normal = a.normal * barycentricCoord.x + b.normal * barycentricCoord.y+ c.normal * barycentricCoord.z;

                    float fragmentIntensity = glm::dot(normal, glm::vec3 (0,0.0f,1.0f));
                    if (fragmentIntensity<=0 ){
                        continue;
                    }
                    // Apply fragment shader to calculate final color with shading
                    Color finalColor = interpolatedColor * fragmentIntensity;

                    glm::vec3 original = a.original * barycentricCoord.x + b.original * barycentricCoord.y + c.original * barycentricCoord.z;
                    // Create a fragment with the position, interpolated attributes, and depth
                    Fragment fragment;
                    fragment.position = glm::ivec2(x, y);
                    fragment.color = finalColor;
                    fragment.z = depth;  // Set the depth of the fragment
                    fragment.original = original;


                    int index = y * WINDOW_WIDTH + x;
                    if (depth < zbuffer[index]) {
                        // Apply fragment shader to calculate final color
                        Color fragmentS;
                        // Draw the fragment using SDL_SetRenderDrawColor and SDL_RenderDrawPoint
                        //std::cout << "El valor de i es: " << id << std::endl;
                        switch (id) {
                            case gasPlanet:
                                fragmentS = gasPlanet1(fragment);
                                SDL_SetRenderDrawColor(renderer, fragmentS.r, fragmentS.g,fragmentS.b, fragmentS.a);
                                break;
                            case orbitalPlanet:
                                fragmentS = saturn(fragment);
                                SDL_SetRenderDrawColor(renderer, fragmentS.r, fragmentS.g, fragmentS.b, fragmentS.a);
                                break;
                            case misteryPlanet:
                                fragmentS = mystery(fragment);
                                SDL_SetRenderDrawColor(renderer, fragmentS.r, fragmentS.g,fragmentS.b, fragmentS.a);
                                break;
                            case star:
                                fragmentS = sun(fragment);
                                SDL_SetRenderDrawColor(renderer, fragmentS.r, fragmentS.g,fragmentS.b, fragmentS.a);
                                break;
                            case rocoso:
                                fragmentS = rocky(fragment);
                                SDL_SetRenderDrawColor(renderer, fragmentS.r, fragmentS.g,fragmentS.b, fragmentS.a);
                                break;
                            case earth:
                                fragmentS = tierra(fragment);
                                SDL_SetRenderDrawColor(renderer, fragmentS.r, fragmentS.g,fragmentS.b, fragmentS.a);
                                break;
                        }
                        SDL_RenderDrawPoint(renderer, x, WINDOW_HEIGHT-y);
                        newTime = 0.5f + 1.0f;
                        zbuffer[index] = depth;
                    }
                }
            }
        }
    }
}

glm::mat4 createViewportMatrix() {
    glm::mat4 viewport = glm::mat4(1.0f);
    viewport = glm::scale(viewport, glm::vec3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.5f));
    viewport = glm::translate(viewport, glm::vec3(1.0f, 1.0f, 0.5f));
    return viewport;
}

glm::mat4 createProjectionMatrix() {
    float fovInDegrees = 45.0f;
    float aspectRatio = WINDOW_WIDTH / WINDOW_HEIGHT;
    float nearClip = 0.1f;
    float farClip = 100.0f;
    return glm::perspective(glm::radians(fovInDegrees), aspectRatio, nearClip, farClip);
}

float a = 3.14f / 3.0f;

glm::mat4 createModelMatrixStars() {
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(10.0f, 10.0f, 10.0f));
    glm::mat4 rotation = glm::mat4(1.0f);
    return translation * scale * rotation;
}

glm::mat4 createModelMatrixEntity(glm::vec3 matrixTranslation, glm::vec3 matrixScale, glm::vec3 matrixRotation, float radianSpeed)  {
    glm::mat4 translation = glm::translate(glm::mat4(1), matrixTranslation);
    glm::mat4 scale = glm::scale(glm::mat4(1), matrixScale);
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians((a++)*radianSpeed), matrixRotation);
    return translation * scale * rotation;
}

glm::mat4 createModelMatrixEntityWithMoon(const glm::mat4& modelMatrix3) {
    // Aquí defines la transformación del "modelo 4" en relación con el "modelo 3"
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.3f, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.2f, 0.2f, 0.2f));
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians((a++) * 4), glm::vec3(0.0f, 1.0f, 0.0f));
    // Aplica la transformación del "modelo 4" en relación con el "modelo 3"
    return modelMatrix3 * translation * scale * rotation;
}

glm::mat4 createModelMatrix6(glm::vec3 cameraPosition3, glm::vec3 targetPosition3,glm::vec3 upVector3, float xRotate, float yRotate) {
    glm::mat4 translation = glm::translate(glm::mat4(1), (targetPosition3 - cameraPosition3) /7.0f + cameraPosition3 +upVector3*0.15f);
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.3f, 0.3f, 0.3f));
    glm::mat4 rotationX = glm::rotate(glm::mat4(1), glm::radians(xRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    //glm::mat4 rotationY = glm::rotate(glm::mat4(1), glm::radians((yRotate)+90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    return translation * scale * rotationX;
}