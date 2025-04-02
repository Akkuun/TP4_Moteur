// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "Sphere.hpp"

void createPlane(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv, int nX, int nY){

    indices.clear();
    indexed_vertices.clear();
    indexed_uv.clear();

    for (int x = 0; x < nX; x++)
    {
        for (int y = 0; y < nY; y++)
        {
            indexed_vertices.push_back(glm::vec3(x, 0, y));
            indexed_uv.push_back(glm::vec2(float((float(x)/float(nX))),float(float(y)/float(nY))));
        }
    }

    for (int x = 0; x < nX - 1; x++){
        for (int y = 0; y < nY - 1; y++)
        {
            // Indices des sommets du carré actuel
            int p0 = x * nY + y;
            int p1 = p0 + 1;
            int p2 = (x + 1) * nY + y;
            int p3 = p2 + 1;

            indices.push_back(p0);
            indices.push_back(p2);
            indices.push_back(p1);

            indices.push_back(p1);
            indices.push_back(p2);
            indices.push_back(p3);

        }
    }


}



void createSphere(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv, int nX, int nY){

    float x, y, z, phi, theta;

    for(int i = 0; i < nX; i++){

        for(int j = 0; j < nY; j++){
            theta = float(i) * 2.f * M_PI/float(nX-1) - M_PI; // angle horizontal entre -pi et pi)
            phi = float(j) * M_PI/(nY-1) - M_PI/2.f; // angle vertical (entre -pi/2 et pi/2)
            x =  cos(theta) * cos(phi);
            y = sin(theta) * cos(phi);
            z = sin(phi);

            indexed_vertices.push_back(glm::vec3(x, y, z));
            float u = float(i) / float(nX-1); // coordonnée horizontale
            float v = float(j) / float(nY-1); // coordonnée verticale
            indexed_uv.push_back(glm::vec2(u, v));


        }
    }


    for(int i = 0; i < nX-1 ; i++){
        for (int j = 0; j < nY-1; j++){
            int p0 = i*nX + j; // point courant
            int p1 = i * nX + (j+1); // en dessous du point courant
            int p2 = (i+1) * nX + j;  // à droite de p0
            int p3 = (i+1) * nX + (j+1);  // en bas à droite

            indices.push_back(p0);
            indices.push_back(p2);
            indices.push_back(p1);

            indices.push_back(p1);
            indices.push_back(p2);
            indices.push_back(p3);
        }
    }
}