#version 430 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 color;
layout (location=2) in vec2 uv;

out vec3 vColor;
out vec2 vUV;

layout(std140, binding = 0) uniform FrameData {
    mat4  view;
    mat4  projection;
    vec4  cameraPos;
    float time;
};

layout (location = 0) uniform mat4 uModel;

void main()
{
    gl_Position = projection * view * uModel * vec4(position, 1.0);
    vColor = color;
    vUV = uv;
}

