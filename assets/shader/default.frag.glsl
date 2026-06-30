#version 430 core

in vec3 vColor;
in vec2 vUV;
in vec3 vFragPosition;
in vec3 vNormal;

out vec4 out_frag_color;

struct Light {
    vec3 color;
    vec3 position;
};
uniform Light uLight;

layout(std140, binding = 0) uniform FrameData {
    mat4  view;
    mat4  projection;
    vec4  cameraPos;
    float time;
};

uniform float roughness;
uniform vec2 uvOffset;

layout(binding = 0) uniform sampler2D albedo;

void main()
{
    // Diffuse
    vec3 light_dir = normalize(uLight.position - vFragPosition);
    float diffuse_amount = max(0, dot(light_dir, vNormal));
    vec3 diffuse = diffuse_amount * uLight.color;

    // Specular
    vec3 view_dir = normalize(cameraPos.xyz - vFragPosition);
    vec3 reflected_dir = reflect(-light_dir, vNormal);
    float specular_amount = pow(max(0, dot(view_dir, reflected_dir)), 32.0);
    float specular_strength = 0.5;
    vec3 specular = specular_strength * specular_amount * uLight.color;

    // Ambient
    vec3 ambient = 0.02 * uLight.color;

    vec3 tex_color = texture(albedo, vUV + uvOffset).rgb;
    out_frag_color = vec4(tex_color * (ambient + diffuse + specular), 1.0);
}

