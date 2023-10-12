#pragma once
#include <array>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tex;
  glm::vec3 worldPos;
  glm::vec3 originalPos;
};

struct Face{
  std::array<int, 3> vertexIndices;
  std::array<int, 3> normalIndices;
};

bool loadOBJ(
  const char *path,
  std::vector<glm::vec3> &out_vertices,
  std::vector<glm::vec3> &out_normals,
  std::vector<glm::vec3> &out_texcoords,
  std::vector<Face>& out_faces
);
