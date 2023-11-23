#pragma once
#include "glm/glm.hpp"
#include "fragment.h"
#include "color.h"
#include "FastNoise.h"

struct Uniforms {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 viewport;
};

float newTime = 0.5f;

void printMatrix(const glm::mat4& myMatrix) {
    // Imprimir los valores de la matriz
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << myMatrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// Función para imprimir un vec4
void printVec4(const glm::vec4& vector) {
    std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
}

// Función para imprimir un vec3
void printVec3(const glm::vec3& vector) {
    std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
}


Vertex vertexShader(const Vertex& vertex, const Uniforms& uniforms) {
    glm::vec4 transformedVertex = uniforms.viewport * uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);
    glm::vec3 vertexRedux;
    vertexRedux.x = transformedVertex.x / transformedVertex.w;
    vertexRedux.y = transformedVertex.y / transformedVertex.w;
    vertexRedux.z = transformedVertex.z / transformedVertex.w;
    Color fragmentColor(255, 0, 0, 255);
    glm::vec3 normal = glm::normalize(glm::mat3(uniforms.model) * vertex.normal);
    Fragment fragment;
    fragment.position = glm::ivec2(transformedVertex.x, transformedVertex.y);
    fragment.color = fragmentColor;
    return Vertex {vertexRedux, normal, vertex.position};
}

Color tierra(Fragment& fragment) {
    glm::vec2 fragmentCoords(fragment.original.x, fragment.original.y);

    Color bandColor1 = Color(0, 200, 50); 
    Color bandColor2 = Color(66, 135, 245); 

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    float ox = 3000.0f;
    float zoom = 500.0f;

    float noiseValue = noise.GetNoise((fragment.original.x + ox) * zoom, (fragment.original.y) * zoom);

    float t = (noiseValue + 1.0f) / 2.0f;

    Color bandColor = bandColor1 * (1.0f - t) + bandColor2 * t;

    fragment.color = bandColor;
    return fragment.color;
}

Color gasPlanet1(Fragment& fragment) {
    // Obtaining the fragment coordinates in 2D space
    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    // Define the colors for the gaseous planet
    Color mainColor = Color(204, 255, 255);
    Color secondColor = Color(0, 153, 153);
    
    // Configuration of the noise
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    
    float offsetX = 800.0f;
    float offsetY = 200.0f;
    float scale = 100.0f;
    
    // Generate noise value
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.5f;
    noiseValue = glm::smoothstep(0.0f, 1.0f, noiseValue);
    
    secondColor = secondColor * (1.0f - noiseValue) + mainColor * noiseValue;
    
    // Adding sinusoidal variation
    float frequency = 20.0;
    float amplitude = 0.1;
    float sinValue = glm::sin(uv.y * frequency) * amplitude;
    secondColor = secondColor + Color(sinValue, sinValue, sinValue);
    
    // Applying color to the fragment
    fragment.color = secondColor;
    
    return fragment.color;
}

Color saturn(Fragment& fragment) {
    glm::vec2 fragmentCoords(fragment.original.x, fragment.original.y);

    Color scolor2 = Color(207, 146, 37); 
    Color scolor1 = Color(59, 41, 9); 

    FastNoiseLite noise;

    float oy = 3000.0f;
    float zoom = 5000.0f;

    float noiseValue = abs(noise.GetNoise((fragment.original.x), (fragment.original.y + oy) * zoom, fragment.original.z));

    Color tmpColor = (noiseValue < 0.1f) ? scolor1 : scolor2;

    fragment.color = tmpColor * fragment.z;
    return fragment.color;
}

Color rocky(Fragment& fragment) {
    glm::vec2 fragmentCoords(fragment.original.x, fragment.original.y);

    Color rock1 = Color(207, 37, 37);
    Color rock2 = Color(79, 13, 13); 

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetSeed(1337);
    noise.SetFrequency(0.05f); 
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(6);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);

    float ox = 3000.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    float noiseValue = noise.GetNoise((fragment.original.x + ox) * zoom, (fragment.original.y + oy) * zoom);

    float t = (noiseValue + 1.0f) / 2.0f; 

    Color rockColor = rock1 * (1.0f - t) + rock2 * t;

    fragment.color = rockColor;
    return fragment.color;
}

Color sun(Fragment& fragment) {
    glm::vec2 fragmentCoords(fragment.original.x, fragment.original.y);

    Color mys1 = Color(255, 225, 0); 
    Color mys2 = Color(255, 125, 0);  

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(4321); 
    noise.SetFrequency(0.02f);
    noise.SetFractalType(FastNoiseLite::FractalType_DomainWarpProgressive);
    noise.SetFractalOctaves(6);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);

    float ox = 3000.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    float noiseValue = abs(noise.GetNoise((fragment.original.x + ox) * zoom, (fragment.original.y + oy) * zoom));
    float threshold = 0.2f;
    float gradient = (noiseValue - threshold) / (1.0f - threshold);
    fragment.color = mys1 * (1.0f - gradient) + mys2 * gradient;
    return fragment.color;
}

Color mystery(Fragment& fragment) {
    glm::vec2 fragmentCoords(fragment.original.x, fragment.original.y);

    Color mys1 = Color(121, 27, 204); 
    Color mys2 = Color(121, 27, 204);  

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(4321); 
    noise.SetFrequency(0.02f);
    noise.SetFractalType(FastNoiseLite::FractalType_DomainWarpProgressive);
    noise.SetFractalOctaves(6);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);

    float ox = 3000.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    float noiseValue = abs(noise.GetNoise((fragment.original.x + ox) * zoom, (fragment.original.y + oy) * zoom));
    float threshold = 0.2f;
    float gradient = (noiseValue - threshold) / (1.0f - threshold);
    fragment.color = mys1 * (1.0f - gradient) + mys2 * gradient * zoom;
    return fragment.color;
}