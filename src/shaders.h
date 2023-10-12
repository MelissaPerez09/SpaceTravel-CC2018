#pragma once
#include <glm/glm.hpp>
#include "noise.h"
#include "color.h"
#include "fragment.h"

struct Uniforms {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 viewport;
};

// Shader de vértices
Vertex vertexShader(const Vertex& vertex, const Uniforms& uniforms) {
    // Transforma el vértice por las matrices model, view y projection
    glm::vec4 viewSpaceVertex = uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);
    glm::vec4 clipSpaceVertex = uniforms.projection * viewSpaceVertex;

    // Realiza la perspectiva divide
    glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

    // Aplica el transform de viewport
    glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);

    // Calcula la normal transformada (en este ejemplo, no se realiza normalización)
    glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;

    // Devuelve el vértice transformado
    return Vertex{
        glm::vec3(screenVertex),
        transformedNormal
    };
}

// Shader de fragmentos
Fragment fragmentShader(const Fragment& fragment) {
    Fragment modifiedFragment = fragment;
    modifiedFragment.color = modifiedFragment.color * modifiedFragment.intensity;
    
    return modifiedFragment;
}

Fragment star(const Fragment& fragment) {
    Color color;

    glm::vec3 solecito = glm::vec3(237.0f, 219.0f, 173.0f);

    float stripePattern = glm::abs(glm::cos(fragment.originalPos.y * 40.0f)) * 17.0f;
    float secondStripePattern = glm::abs(glm::cos(fragment.originalPos.y * 25.0f)) * 25.0f;

    glm::vec3 finalColor = solecito + glm::vec3(stripePattern) - glm::vec3(secondStripePattern);

    color = Color(static_cast<uint8_t>(finalColor.r), static_cast<uint8_t>(finalColor.g), static_cast<uint8_t>(finalColor.b));

    float glow = 1.0f + 1.1f * sin(0.9f);
    Color finalColorWithGlow = color * fragment.intensity * glow;

    Fragment modifiedFragment = fragment;
    modifiedFragment.setColor(finalColorWithGlow);

    return modifiedFragment;
}

Fragment misteryPlanet(const Fragment& fragment) {
    Color color;

    glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 orange = glm::vec3(1.0f, 0.5f, 0.0f);
    glm::vec3 red = glm::vec3(0.6f, 0.0f, 0.0f);

    float frequency = 10.0f;
    float amplitude = 0.2f;

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0, fragment.originalPos.y * 2.0 - 1.0);

    float distanceFromCenter = glm::length(uv);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    float reliefNoise = noiseGenerator.GetNoise(uv.x * frequency, uv.y * frequency) * amplitude;

    yellow *= 0.7f - distanceFromCenter;
    orange *= 0.8f - distanceFromCenter;
    red *= 0.3f - distanceFromCenter;

    glm::vec3 terrainColor = (yellow + orange + red) * glm::vec3(0.5f, 0.2f, 0.0f);

    float elevation = distanceFromCenter + reliefNoise;

    float edgeSmoothness = 0.1f;
    float softEdge = 1.0f - glm::smoothstep(1.0f - edgeSmoothness, 1.0f, elevation);

    glm::vec3 backgroundColor = glm::vec3(0.0f, 0.0f, 0.1f);
    glm::vec3 finalColor = glm::mix(backgroundColor, terrainColor, softEdge);

    finalColor *= fragment.intensity * 1.5f;

    color = Color(static_cast<uint8_t>(finalColor.r * 255), static_cast<uint8_t>(finalColor.g * 255), static_cast<uint8_t>(finalColor.b * 255));

    Fragment modifiedFragment = fragment;
    modifiedFragment.setColor(color);

    return modifiedFragment;
}

Fragment gasPlanet(const Fragment& fragment) {
    Color color;

    glm::vec3 mainColor = glm::vec3(204.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
    glm::vec3 secondColor = glm::vec3(0.0f/255.0f, 153.0f/255.0f, 153.0f/255.0f);
    glm::vec3 thirdColor = glm::vec3(204.0f/255.0f, 255.0f/255.0f, 229.0f/255.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0 , fragment.originalPos.y * 2.0 - 1.0);

    float frequency = 20.0; 
    float amplitude = 0.1; 

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

    float offsetX = 800.0f;
    float offsetY = 200.0f;
    float scale = 100.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.5f;

    noiseValue = glm::smoothstep(0.0f, 1.0f, noiseValue);

    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    float sinValue = glm::sin(uv.y * frequency) * amplitude;

    secondColor = secondColor + glm::vec3(sinValue);

    color = Color(static_cast<uint8_t>(secondColor.x * 255), static_cast<uint8_t>(secondColor.y * 255), static_cast<uint8_t>(secondColor.z * 255));

    color = color * fragment.intensity;

    Fragment modifiedFragment = fragment;
    modifiedFragment.setColor(color);

    return modifiedFragment;
}

Fragment orbitalPlanet(const Fragment& fragment) {
    Color color;

    glm::vec3 mainColor = glm::vec3(5.0f, 1.0f, 0.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0, fragment.originalPos.y * 2.0 - 1.0);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

    float offsetX = 8000.0f;
    float offsetY = 10000.0f;
    float scale = 1000.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);

    noiseValue = glm::smoothstep(0.0f, 1.0f, noiseValue);

    glm::vec3 sunColor = glm::mix(mainColor, mainColor * 1.5f, noiseValue);

    sunColor *= fragment.intensity;

    color = Color(static_cast<uint8_t>(sunColor.r * 255), static_cast<uint8_t>(sunColor.g * 255), static_cast<uint8_t>(sunColor.b * 255));

    Fragment modifiedFragment = fragment;
    modifiedFragment.setColor(color);

    return modifiedFragment;
}

Fragment rocoso(const Fragment& fragment) {
    Color color;

    glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 orange = glm::vec3(1.0f, 0.5f, 0.0f);
    glm::vec3 red = glm::vec3(6.0f, 0.0f, 0.0f);

    float noise = glm::fract(sin(glm::dot(glm::vec2(fragment.originalPos.x, fragment.originalPos.y), glm::vec2(12.9898, 78.233))) * 43758.5453) * 0.5f + 0.5f;

    float distanceFromCenter = glm::length(fragment.originalPos);
    yellow *= 0.5f - distanceFromCenter;
    orange *= 0.6f - distanceFromCenter;
    red *= 0.2f - distanceFromCenter;

    glm::vec3 tmpColor = (yellow + orange + red) * glm::vec3(0.4f, 0.1f, 0.0f);

    color = Color(static_cast<uint8_t>(tmpColor.r * 255 * noise), static_cast<uint8_t>(tmpColor.g * 255 * noise), static_cast<uint8_t>(tmpColor.b * 255 * noise));

    float glow = 1.0f + 1.1f * sin(0.9f);
    Color finalColor = color * fragment.intensity * glow;

    Fragment modifiedFragment = fragment;
    modifiedFragment.setColor(finalColor);

    return modifiedFragment;
}

Fragment star2(const Fragment& fragment) {
    Color color;

    glm::vec3 white = glm::vec3(1.0f, 1.0f, 0.0f);  
    glm::vec3 yellow = glm::vec3(1.0f, 0.8f, 0.0f); 
    glm::vec3 orange = glm::vec3(0.0f, 1.5f, 1.0f); 

    // Frecuencia y amplitud para el brillo
    float frequency = 0.5f;
    float amplitude = 0.3f;

    // Distancia desde el centro de la estrella
    float distanceFromCenter = glm::length(fragment.originalPos);

    // Genera un valor de ruido para el brillo
    float noise = glm::fract(sin(glm::dot(glm::vec2(fragment.originalPos.x, fragment.originalPos.y), glm::vec2(12.9898, 78.233))) * 43758.5453) * 0.5f + 0.5f;

    // Modifica los colores basados en la distancia desde el centro
    white *= 1.0f - distanceFromCenter;
    yellow *= 0.8f - distanceFromCenter;
    orange *= 0.6f - distanceFromCenter;

    // Mezcla de colores para la estrella
    glm::vec3 starColor = (white + yellow + orange) * glm::vec3(0.5f, 0.2f, 0.0f);

    // Calcula el brillo basado en el valor de ruido
    float glow = 0.9f + noise * amplitude;

    // Aplica el brillo a la estrella
    starColor *= glow;

    // Configura el color del fragmento
    color = Color(static_cast<uint8_t>(starColor.r * 255), static_cast<uint8_t>(starColor.g * 255), static_cast<uint8_t>(starColor.b * 255));

    // Aplica la intensidad de la luz
    color.r *= fragment.intensity;
    color.g *= fragment.intensity;
    color.b *= fragment.intensity;

    // Usa setColor para establecer el color del fragmento
    Fragment modifiedFragment = fragment;
    modifiedFragment.setColor(color);

    return modifiedFragment;
}
