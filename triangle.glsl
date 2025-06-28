@vs vs
in vec4 position;
in vec4 aColor;

out vec4 color;

void main() {
    gl_Position = vec4(position);
    color = aColor;
}
@end

@fs fs
out vec4 FragColor;

in vec4 color;

void main() {
    FragColor = vec4(color.xyz, 1.0f);
}
@end

@program simple vs fs