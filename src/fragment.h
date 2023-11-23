#pragma once
#include <glm/glm.hpp>
#include "color.h"

// Representa el resultado del shader de fragmentos
struct FragColor {
    Color color;  // Color del fragmento
    double z;     // Valor de profundidad (en lugar del z-buffer)
};


struct Fragment {
    glm::ivec2 position; // X and Y coordinates of the pixel (in screen space)
    Color color;
    float z;
    glm::vec3 original;
    // Other interpolated attributes (e.g., color, texture coordinates, normals) can be added here
};
