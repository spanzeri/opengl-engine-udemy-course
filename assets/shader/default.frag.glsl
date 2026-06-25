#version 430 core

in vec3 vColor;
in vec2 vUV;

out vec4 out_frag_color;

uniform float roughness;
uniform vec2 uvOffset;

layout(binding = 0) uniform sampler2D albedo;

void main()
{
    vec3 tex_color = texture(albedo, vUV + uvOffset).rgb;
    out_frag_color = vec4(tex_color, 1.0);
}

