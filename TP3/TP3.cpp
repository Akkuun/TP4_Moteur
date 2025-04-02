// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/texture.hpp>

#include <common/Node_3D.hpp>
#include <common/Transform.hpp>
#include <common/SceneManager.hpp>
#include <common/Sphere.hpp>

bool isAboveThreshold = false;

float calculateDistance(const glm::vec3& point1, const glm::vec3& point2) {
    return glm::length(point1 - point2);
}
void updateLOD(Node_3D& sun, const glm::vec3& camera_position, GLuint programID) {
    float distance = calculateDistance(sun.getTransform().getPosition(), camera_position);

    sun.indices.clear();
    sun.indexed_vertices.clear();
    sun.indexed_uv.clear();

    if (distance < 5.0f) {
        std::cout << "High detail LOD" << std::endl;
        createHighDetailSphere(sun.indices, sun.indexed_vertices, sun.indexed_uv, sun.vertexbuffer, sun.elementbuffer, sun.uvs);
    } else if (distance < 15.0f) {
        std::cout << "Medium detail LOD" << std::endl;
        createMediumDetailSphere(sun.indices, sun.indexed_vertices, sun.indexed_uv, sun.vertexbuffer, sun.elementbuffer, sun.uvs);
    } else {
        std::cout << "Low detail LOD" << std::endl;
        createLowDetailSphere(sun.indices, sun.indexed_vertices, sun.indexed_uv, sun.vertexbuffer, sun.elementbuffer, sun.uvs);
    }

    // Rebind the VAO and update the buffer data
    glBindVertexArray(sun.VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, sun.vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sun.indexed_vertices.size() * sizeof(glm::vec3), &sun.indexed_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sun.elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sun.indices.size() * sizeof(unsigned short), &sun.indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, sun.uvs);
    glBufferData(GL_ARRAY_BUFFER, sun.indexed_uv.size() * sizeof(glm::vec2), &sun.indexed_uv[0], GL_STATIC_DRAW);

    sun.initBufferObject(programID); // Reinitialize the buffers with the new geometry
}

//fonction that create a sphere
void createSphere(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv){
    int nX = 40;
    int nY = 40;
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


void createRectangle(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_uv) {
    int width = 10;
    int height = 10;
    float maxHeight = 1.0f;

    // Generate vertices with random heights
    for (int i = 0; i <= width; ++i) {
        for (int j = 0; j <= height; ++j) {
            float x = static_cast<float>(i) - width / 2.0f;
            float z = static_cast<float>(j) - height / 2.0f;
            float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * maxHeight;
            indexed_vertices.push_back(glm::vec3(x, y, z));
            indexed_uv.push_back(glm::vec2(static_cast<float>(i) / width, static_cast<float>(j) / height));
        }
    }

    // Generate indices for the triangles
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            int topLeft = i * (height + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (height + 1);
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, -11.0f,  -0.1f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

// to process inputs
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float timeCst = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;


//MVP MATRIX
glm::mat4 modele = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

// rotations
float orbitalSpeedMoonEarth = 0.5f;
float orbitalSpeedEarthSun = 0.1f;
float orbitRadius = 1.0f; // Radius Earch Sun Orbit
float lunarOrbitRadius = 1.0f; // Radius Moon Earth Orbit

float tiltAngle = glm::radians(23.0f);

// SceneManager
SceneManager sceneManager;


//Objet principal
// NODES
Node_3D SUN;
Node_3D TERRAIN;


// Global variables for mouse control
float lastX = 1024.0f / 2.0;
float lastY = 768.0f / 2.0;
float yaw = -90.0f; // Yaw is initialized to -90.0 degrees to look along the z-axis
float pitch = 0.0f;
bool firstMouse = true;


/*******************************************************************************/


// Fonction pour vérifier si un point est dans un triangle en utilisant la méthode de Möller-Trumbore
bool isPointInTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    glm::vec3 v0 = c - a;
    glm::vec3 v1 = b - a;
    glm::vec3 v2 = p - a;

    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0) && (v >= 0) && (u + v < 1);
}

// Fonction pour obtenir la hauteur du terrain à un point donné
float getTerrainHeight(glm::vec3 point, Node_3D& terrain) {
    for (size_t i = 0; i < terrain.indices.size(); i += 3) {
        glm::vec3 a = terrain.indexed_vertices[terrain.indices[i]];
        glm::vec3 b = terrain.indexed_vertices[terrain.indices[i + 1]];
        glm::vec3 c = terrain.indexed_vertices[terrain.indices[i + 2]];

        if (isPointInTriangle(point, a, b, c)) {
            // Interpolation barycentrique
            glm::vec3 v0 = b - a;
            glm::vec3 v1 = c - a;
            glm::vec3 v2 = point - a;

            float d00 = glm::dot(v0, v0);
            float d01 = glm::dot(v0, v1);
            float d11 = glm::dot(v1, v1);
            float d20 = glm::dot(v2, v0);
            float d21 = glm::dot(v2, v1);

            float denom = d00 * d11 - d01 * d01;
            float v = (d11 * d20 - d01 * d21) / denom;
            float w = (d00 * d21 - d01 * d20) / denom;
            float u = 1.0f - v - w;

            return u * a.y + v * b.y + w * c.y;
        }
    }
    return 0.0f; // Default height if not found
}


// Fonction pour mettre à jour la position de l'objet


void updateObjectPosition(Node_3D& object, Node_3D& terrain, float offset, float lowerThreshold, float upperThreshold) {
    glm::vec3 position = object.getTransform().getPosition();
    float terrainHeight = getTerrainHeight(position, terrain);

    if (isAboveThreshold) {
        if (position.z < terrainHeight + lowerThreshold) {
            position.z = terrainHeight + offset;
            isAboveThreshold = false;
        }
    } else {
        if (position.z < terrainHeight + upperThreshold) {
            position.z = terrainHeight + offset;
            isAboveThreshold = true;
        }
    }

    object.transform.setPosition(position);
}

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "TP1 - GLFW", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.3f, 0.8f, 0.5f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "vertex_shader.glsl", "fragment_shader.glsl" );



    //geometry of sun
    createSphere(SUN.indices, SUN.indexed_vertices, SUN.indexed_uv);
    SUN.setCouleur(glm::vec3(0, 0, 1));
    //translate the sun to the top
    SUN.transform.translate(glm::vec3(0.0f, 0.0f, 1.0f));
    SUN.textureID = loadBMP_custom("data/sun.bmp");
    sceneManager.add(&SUN);

    //geometry of terrain
    createRectangle(TERRAIN.indices, TERRAIN.indexed_vertices, TERRAIN.indexed_uv);
    TERRAIN.setCouleur(glm::vec3(0.3f, 0.8f, 0.3f)); // Green color for terrain
    //rotate the terrain to 90°
    TERRAIN.transform.rotateTransform(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //scale the terrain
    TERRAIN.transform.scaleTransform(glm::vec3(2.0f, 1.0f, 1.0f));
    //translate the terrain to the bottom
    TERRAIN.transform.translate(glm::vec3(0.0f, 0.0f, -1.0f));
    TERRAIN.textureID = loadBMP_custom("data/grass.bmp");
    sceneManager.add(&TERRAIN);


    sceneManager.initScene(programID);

    int nbFrames = 0;

    //UPDATE LOOP
    do {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        timeCst += deltaTime;
        processInput(window);

        // Update LOD based on distance
        updateLOD(SUN, camera_position, programID);

        // Update the position of the object
        updateObjectPosition(SUN, TERRAIN, 0.01f, 0.1f, 0.2f);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        projection = glm::perspective(glm::radians(45.f), 4.0f/3.0f, 0.1f, 100.0f);
        view = glm::lookAt(camera_position, camera_target, camera_up);
        GLuint positionProjection = glGetUniformLocation(programID, "projection");
        GLuint positionView = glGetUniformLocation(programID, "view");
        glUniformMatrix4fv(positionProjection, 1 , GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(positionView, 1 , GL_FALSE,  glm::value_ptr(view));
        GLuint position = glGetUniformLocation(programID, "modele");
        glUniformMatrix4fv(position, 1, GL_FALSE, glm::value_ptr(modele));

        // Draw the sun
        sceneManager.drawScene(programID);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0 );

    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float moveSpeed = 2.5f * deltaTime;
    float rotationSpeed = 50.0f * deltaTime; // Rotation speed in degrees per second

    // Move SUN
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        SUN.transform.translate(glm::vec3(0.0f, moveSpeed, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        SUN.transform.translate(glm::vec3(0.0f, -moveSpeed, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        SUN.transform.translate(glm::vec3(-moveSpeed, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        SUN.transform.translate(glm::vec3(moveSpeed, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        SUN.transform.translate(glm::vec3(0.0f, 0.0f, moveSpeed));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        SUN.transform.translate(glm::vec3(0.0f, 0.0f, -moveSpeed));


  // Move camera freely
if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera_position += moveSpeed * camera_target;
if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera_position -= moveSpeed * camera_target;
if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camera_position -= glm::normalize(glm::cross(camera_target, camera_up)) * moveSpeed;
if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camera_position += glm::normalize(glm::cross(camera_target, camera_up)) * moveSpeed;
if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera_position += moveSpeed * camera_up;
if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera_position -= moveSpeed * camera_up;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


