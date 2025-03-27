#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

// Include GLFW
#include <GLFW/glfw3.h>


extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Transform.hpp"



class Node_3D {
private : 
    int idObject; 
    Node_3D *parent;

public:
    // geometry
    std::vector<glm::vec3> indexed_vertices;
    std::vector<unsigned short> indices;
    std::vector<glm::vec2> indexed_uv;
    //default transform
    Transform transform = Transform(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
    glm::vec3 colorObject = glm::vec3(0.75, 0.5, 0.5);
    GLuint vertexbuffer, elementbuffer, VertexArrayID, uvs;

    //buffers
    GLuint transformationLoc, couleurLoc, textureID, textureLoc; 
 
    
    // empty constructor
    Node_3D() : parent(NULL){}
    
    int getIdObject() const { return idObject; }
    Node_3D* getParent() const { return parent; }
    std::vector<glm::vec3> getIndexedVertices() const { return indexed_vertices; }
    std::vector<unsigned short> getIndices() const { return indices; }
    Transform getTransform() const { return transform; }


    // Setters
    void setIdObject(int id) { idObject = id; }
    void setParent(Node_3D* p) { parent = p;}
    void setCouleur(glm::vec3 couleur){this->colorObject = couleur; }
    void setTransform(const Transform& t) { transform = t; }


    //used to init all the buffer object
    void initBufferObject(GLuint programID){

        transformationLoc = glGetUniformLocation(programID, "transformation");
        couleurLoc = glGetUniformLocation(programID, "colorObject");
        textureLoc = glGetUniformLocation(programID, "objectTexture"); 
        
        
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
        
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

        glGenBuffers(1, &uvs);
        glBindBuffer(GL_ARRAY_BUFFER, uvs);
        glBufferData(GL_ARRAY_BUFFER, indexed_uv.size() * sizeof(glm::vec2), &indexed_uv[0], GL_STATIC_DRAW);

    }

    // Get the transformation matrix of the parent
    glm::mat4 getTransfomParent(){
        if (parent == NULL){
            return transform.matrixReconst(); 
        }else{
            return parent->getTransfomParent() * transform.matrixReconst(); 
        }
    }

    void draw(){

        glm::mat4 parentMatrix = getTransfomParent();
        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(parentMatrix));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(textureLoc, 0);
        glUniform3fv(couleurLoc, 1, &colorObject[0]);
        glEnableVertexAttribArray(0); // layout = 0
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );
        glEnableVertexAttribArray(1); // layout = 1
        glBindBuffer(GL_ARRAY_BUFFER, uvs);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        // Draw the triangles !
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    indices.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }



};


#endif