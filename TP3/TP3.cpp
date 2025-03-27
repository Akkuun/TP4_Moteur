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
    // Define vertices for a rectangle
    indexed_vertices = {
            glm::vec3(-5.0f, 0.0f, -5.0f),
            glm::vec3(5.0f, 0.0f, -5.0f),
            glm::vec3(5.0f, 0.0f, 5.0f),
            glm::vec3(-5.0f, 0.0f, 5.0f)
    };

    // Define UV coordinates
    indexed_uv = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f)
    };

    // Define indices for two triangles
    indices = { 0, 1, 2, 2, 3, 0 };
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


/*******************************************************************************/


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
    do{
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        timeCst += deltaTime;
        processInput(window);

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

        //draw the sun
        sceneManager.drawScene(programID);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
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
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


