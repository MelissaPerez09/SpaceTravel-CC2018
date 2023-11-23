#pragma once
#include "glm/glm.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <array>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "glm/gtc/matrix_transform.hpp"

struct Face {
    std::array<int, 3> vertexIndices;
    std::array<int, 3> normalIndices;
};

bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec3>& out_normals, std::vector<Face>& out_faces)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cout << "Failed to open the file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string lineHeader;
        iss >> lineHeader;

        if (lineHeader == "v")
        {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            out_vertices.push_back(vertex);
        }
        else if (lineHeader == "vn")
        {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            out_normals.push_back(normal);
        }
        else if (lineHeader == "f")
        {
            Face face;
            for (int i = 0; i < 3; ++i)
            {
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

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 original;
};

std::vector<Vertex> setupVertexArray(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<Face>& faces){
    std::vector<Vertex> vertexArray;


    float scale = 1.0f;

    for (const auto& face : faces){
        glm::vec3 vertexPosition1 = vertices[face.vertexIndices[0]];
        glm::vec3 vertexPosition2 = vertices[face.vertexIndices[1]];
        glm::vec3 vertexPosition3 = vertices[face.vertexIndices[2]];

        glm::vec3 normalPosition1 = normals[face.normalIndices[0]];
        glm::vec3 normalPosition2 = normals[face.normalIndices[1]];
        glm::vec3 normalPosition3 = normals[face.normalIndices[2]];

        // Add the vertex positions to the vertex array
        vertexArray.push_back(Vertex {vertexPosition1, normalPosition1});
        vertexArray.push_back(Vertex {vertexPosition2, normalPosition2});
        vertexArray.push_back(Vertex {vertexPosition3, normalPosition3});


    }

    return vertexArray;
}
