
attribute vec4 a_position;
uniform mat4 projectionMat;
varying vec2 uv;
varying vec3 viewSpace;
void main() {
    // This is just drawing a quad.
    // Pass the same output vertex position as the input
    gl_Position = a_position;
    viewSpace = (projectionMat*a_position).xyz;
    // Calculate the texture coordinate
    // The position here is a within a [-1, 1] square
    uv = (a_position.xy + 1.0) * 0.5;
}