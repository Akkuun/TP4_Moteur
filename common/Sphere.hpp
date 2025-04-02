#ifndef SPHERE_HPP
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void createPlane(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv, int nX, int nY);
void createSphere(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv, int nX, int nY);
#endif
