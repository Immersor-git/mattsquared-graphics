#version 330
// A vertex shader for rendering vertices with normal vectors and texture coordinates,
// which creates outputs needed for a Phong reflection fragment shader.
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 vTexCoord;
layout (location=3) in ivec4 BoneIDs;
layout (location=4) in vec4 Weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 gBones[100]; // Array of bone transformation matrices

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragWorldPos;

void main() {
    mat4 BoneTransform = mat4(1.0); // start with identity matrix
    float totalWeight = 0.0;

    totalWeight += Weights[0];
    BoneTransform += gBones[BoneIDs[0]] * Weights[0];

    totalWeight += Weights[1];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];

    totalWeight += Weights[2];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];

    totalWeight += Weights[3];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];

    if (totalWeight > 0.0) {
        BoneTransform /= totalWeight; // normalize to get the weighted average
    } else {
        BoneTransform = mat4(1.0); // no bone influences the vertex, use identity matrix
    }

    vec4 PosL = BoneTransform * vec4(vPosition, 1.0);

    // Transform the position to clip space.
    gl_Position = projection * view * model * PosL;
    TexCoord = vTexCoord;
    Normal = mat3(transpose(inverse(model))) * vNormal;

    FragWorldPos = vec3(model * vec4(vPosition, 1.0));
}