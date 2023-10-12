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
#include "fragment.h"
#include "framebuffer.h"
#include "ObjLoader.h"
#include "shaders.h"
#include "triangle.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Color currentColor;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Error: Failed to create SDL window: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Error: Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void setColor(const Color& color) {
    currentColor = color;
}

void render(const std::vector<glm::vec3>& VBO, const Uniforms& uniforms, std::function<Fragment(const Fragment&)> shaderFunction) {
    std::vector<Vertex> transformedVertices(VBO.size() / 2);
    for (size_t i = 0; i < VBO.size() / 2; ++i) {
        Vertex vertex = { VBO[i * 2], VBO[i * 2 + 1] };
        transformedVertices[i] = vertexShader(vertex, uniforms);
    }

    std::vector<std::vector<Vertex>> assembledVertices(transformedVertices.size() / 3);
    for (size_t i = 0; i < transformedVertices.size() / 3; ++i) {
        Vertex edge1 = transformedVertices[3 * i];
        Vertex edge2 = transformedVertices[3 * i + 1];
        Vertex edge3 = transformedVertices[3 * i + 2];

        assembledVertices[i] = { edge1, edge2, edge3 };
    }

    for (size_t i = 0; i < assembledVertices.size(); ++i) {
        std::vector<Fragment> rasterizedTriangle = triangle(
            assembledVertices[i][0],
            assembledVertices[i][1],
            assembledVertices[i][2]
        );

        for (const auto& fragment : rasterizedTriangle) {
            const Fragment& finalFragment = shaderFunction(fragment);
            point(finalFragment);  // Be aware of potential race conditions here
        }
    }
}

glm::mat4 createViewportMatrix(size_t screenWidth, size_t screenHeight) {
    glm::mat4 viewport = glm::mat4(1.0f);

    // Escala uniformemente el objeto en la pantalla
    float scale_factor = 0.5f; // Puedes ajustar el factor de escala según tus necesidades
    viewport = glm::scale(viewport, glm::vec3(scale_factor, scale_factor, 1.0f));

    // Ajusta la escala según el tamaño de la pantalla
    viewport = glm::scale(viewport, glm::vec3(screenWidth / 2.0f, screenHeight / 2.0f, 0.5f));

    // Translate
    viewport = glm::translate(viewport, glm::vec3(1.0f, 1.0f, 0.5f));

    return viewport;
}

bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec3>& out_normals, std::vector<Face>& out_faces){
    std::ifstream file(path);
    if (!file){
        std::cout << "Failed to open the file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)){
        std::istringstream iss(line);
        std::string lineHeader;
        iss >> lineHeader;

        if (lineHeader == "v"){
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            out_vertices.push_back(vertex);
        }
        else if (lineHeader == "vn"){
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            out_normals.push_back(normal);
        }
        else if (lineHeader == "f"){
            Face face;
            for (int i = 0; i < 3; ++i){
                std::string faceData;
                iss >> faceData;

                std::replace(faceData.begin(), faceData.end(), '/', ' ');

                std::istringstream faceDataIss(faceData);
                int temp; // for discarding texture indices
                faceDataIss >> face.vertexIndices[i] >> temp >> face.normalIndices[i];

                // obj indices are 1-based, so convert to 0-based
                face.vertexIndices[i]--;
                face.normalIndices[i]--;
            }
            out_faces.push_back(face);
        }
    }

    return true;
}