#pragma once
#include "Animation.h"
#include "Mesh3D.h"
#include "MatrixUtils.h"
#include <iostream>

class BoneRotationAnimation : public Animation {
private:
    BoneInfo& m_bone;
    glm::vec3 m_rotationPerSecond;
    ShaderProgram& m_shader;

void applyAnimation(float_t dt) override {
    // Calculate the rotation for this tick
    glm::vec3 tickRotation = m_rotationPerSecond * dt;
    // Convert the rotation from degrees to radians
    tickRotation = glm::radians(tickRotation);
    // Create a rotation matrix
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), tickRotation.z, glm::vec3(0.0f, 0.0f, 10.0f));
    m_bone.finalTransformation = rotationMatrix * m_bone.finalTransformation;

    // Print rotation per second
    //std::cout << "Rotation per second: " << m_rotationPerSecond.x << ", " << m_rotationPerSecond.y << ", " << m_rotationPerSecond.z << std::endl;

    // Print the tickRotation and boneMatrix values
    //std::cout << "Tick rotation: " << tickRotation.x << ", " << tickRotation.y << ", " << tickRotation.z << std::endl;
//    std::cout << "Transforming bone: " << m_bone.name << std::endl;
//    std::cout << "Bone matrix: " << std::endl;
//    printMat4(m_bone.finalTransformation);
}

public:
BoneRotationAnimation(const BoneInfo& bone, Object3D& obj, float_t duration, const glm::vec3& totalRotation, ShaderProgram& shader):
    Animation(obj, duration),
    m_bone(const_cast<BoneInfo &>(bone)),
    m_rotationPerSecond(totalRotation / duration),
    m_shader(shader)
    {}
};