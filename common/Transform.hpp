
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Transform {
    private:
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 rotation;

    public:
        Transform() : position(glm::vec3(0)), scale(glm::vec3(1.0)), rotation(glm::vec3(0)) {}
        Transform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation) : position(position), scale(scale), rotation(rotation) {}

        glm::vec3 getPosition() const { return position; }
        glm::vec3 getScale() const { return scale; }
        glm::vec3 getRotation() const { return rotation; }

        void setPosition(glm::vec3 p) { position = p; }
        void setScale(glm::vec3 s) { scale = s; }
        void setRotation(glm::vec3 r) { rotation = r; }

        void translate(glm::vec3 translationValue) {
            position += translationValue;
        }

        void scaleTransform(glm::vec3 scaleValue) {
            scale += scaleValue;
             
        }
        void rotateTransform(float angle, glm::vec3 axis) {
            rotation += angle * axis; 
        }

        glm::mat4 matrixReconst() {
            glm::mat4 matScale = glm::scale(glm::mat4(1.0), scale);
            glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
            glm::mat4 matRotationX = glm::rotate(glm::mat4(1.0), rotation.x, glm::vec3(1.0, 0, 0));
            glm::mat4 matRotationY = glm::rotate(glm::mat4(1.0), rotation.y, glm::vec3(0, 1, 0));
            glm::mat4 matRotationZ = glm::rotate(glm::mat4(1.0), rotation.z, glm::vec3(0, 0, 1));

            return matScale * matTranslate * matRotationX * matRotationY * matRotationZ;
        }
};
#endif
