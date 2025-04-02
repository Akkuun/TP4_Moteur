#ifndef SPHERE_HPP
#define SPHERE_HPP
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <GLFW/glfw3.h>
#include <GL/glew.h>
void createPlane(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv, int nX, int nY);
void createSphere(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv, int nX, int nY);
void createHighDetailSphere(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv, GLuint &vertexbuffer, GLuint &elementbuffer, GLuint &uvs);
void createMediumDetailSphere(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv, GLuint &vertexbuffer, GLuint &elementbuffer, GLuint &uvs);
void createLowDetailSphere(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv, GLuint &vertexbuffer, GLuint &elementbuffer, GLuint &uvs);

#endif // SPHERE_HPP