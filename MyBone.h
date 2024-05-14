#ifndef MATTSQUARED_GRAPHICS_MYBONE_H
#define MATTSQUARED_GRAPHICS_MYBONE_H

#include <string>
#include <glm/glm.hpp>
#include <assimp/mesh.h>
#include <glm/gtc/type_ptr.hpp>

class MyBone {
public:
    std::string m_name;
    unsigned int m_numWeights;
    std::vector<aiVertexWeight> m_weights;
    glm::mat4 m_offsetMatrix;  // Use glm::mat4 instead of aiMatrix4x4

    static std::vector<std::string> tireBoneNames;

    // Constructor that takes an aiBone and copies its data
    explicit MyBone(const aiBone* aiBone);
};


#endif //MATTSQUARED_GRAPHICS_MYBONE_H
