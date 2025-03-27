#version 330 core


layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec2 uvbuffer;

// MVP matrix
uniform mat4 transformation;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 modele;

// Height map
uniform sampler2D heightMap;

out float height; 
out vec2 UV;

void main(){
        UV = uvbuffer;
        gl_Position = projection * view * modele * transformation * vec4(vertices_position_modelspace, 1.0);

}
