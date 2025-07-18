@ctype mat4 HMM_Mat4

@vs vs
in vec3 aPos;
in vec2 aTexCoord;

out vec2 TexCoord;

layout(binding = 0) uniform vs_params {
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main() {
    // note that we read the multiplication from right to left
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
@end

@fs fs
out vec4 FragColor;

in vec2 TexCoord;

layout(binding = 0) uniform texture2D _texture1;
layout(binding = 0) uniform sampler texture1_smp;
#define texture1 sampler2D(_texture1, texture1_smp)
layout(binding = 1) uniform texture2D _texture2;
layout(binding = 1) uniform sampler texture2_smp;
#define texture2 sampler2D(_texture2, texture2_smp)

void main() {
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);
}
@end

@program simple vs fs