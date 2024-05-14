#include "MyBone.h"

MyBone::MyBone(const aiBone* aiBone) {
    m_name = aiBone->mName.C_Str();
    m_numWeights = aiBone->mNumWeights;
    m_weights.assign(aiBone->mWeights, aiBone->mWeights + aiBone->mNumWeights);
    // Convert aiMatrix4x4 to glm::mat4
    m_offsetMatrix = glm::make_mat4(&aiBone->mOffsetMatrix.a1);
}

std::vector<std::string> MyBone::tireBoneNames = {
        "B_R_Tire_3",
        "F_R_Tire_3",
        "F_L_Tire_3",
        "B_L_Tire_3"
};