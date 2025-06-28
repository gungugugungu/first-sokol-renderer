@vs vs
in vec4 position;   // Changed from vec4 to vec3

void main() {
    gl_Position = vec4(position);  // Explicitly construct vec4
}
@end

@fs fs
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
@end

@program simple vs fs