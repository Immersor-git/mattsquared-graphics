#include "Scene.h"
#include "AssimpImport.h"
#include "ShaderProgram.h"
#include "Texture.h"



Scene Scene::jeep() {
    auto jeep = assimpLoad("../models/mil_jeep_fbx/mil_jeep.fbx", true);
    jeep.move(glm::vec3(0, -1.2, 0));
    jeep.grow(glm::vec3(0.004, 0.004, 0.004));

    std::vector<Object3D> objects;
    objects.push_back(std::move(jeep));
    Animator animJeep;
    animJeep.addAnimation(std::make_unique<RotationAnimation>(objects[0], 10, glm::vec3(0, 5.5, 0)));
    std::vector<Animator> animators;
    animators.push_back(std::move(animJeep));

    return Scene {
            ShaderProgram::phongLighting(),
            std::move(objects),
            std::move(animators)
    };
}

/**
 * @brief Constructs a scene of a tiger sitting in a boat, where the tiger is the child object
 * of the boat.
 * @return
 */
Scene Scene::lifeOfPi() {
    // This scene is more complicated; it has child objects, as well as animators.
    auto boat = assimpLoad("../models/boat/boat.fbx", true);
    boat.move(glm::vec3(0, -0.7, 0));
    boat.grow(glm::vec3(0.01, 0.01, 0.01));
    auto tiger = assimpLoad("../models/tiger/scene.gltf", true);
    tiger.move(glm::vec3(0, -5, 10));
    boat.addChild(std::move(tiger));

    // Because boat and tiger are local variables, they will be destroyed when this
    // function terminates. To prevent that, we need to move them into a vector, and then
    // move that vector as part of the return value.
    std::vector<Object3D> objects;
    objects.push_back(std::move(boat));

    // We want these animations to referenced the *moved* objects, which are no longer
    // in the variables named "tiger" and "boat". "boat" is now in the "objects" list at
    // index 0, and "tiger" is the index-1 child of the boat.
    Animator animBoat;
    animBoat.addAnimation(std::make_unique<RotationAnimation>(objects[0], 10, glm::vec3(0, 6.28, 0)));
    Animator animTiger;
    animTiger.addAnimation(std::make_unique<RotationAnimation>(objects[0].getChild(1), 10, glm::vec3(0, 0, 6.28)));

    // The Animators will be destroyed when leaving this function, so we move them into
    // a list to be returned.
    std::vector<Animator> animators;
    animators.push_back(std::move(animBoat));
    animators.push_back(std::move(animTiger));

    // Transfer ownership of the objects and animators back to the main.
    return Scene {
            ShaderProgram::phongLighting(),
            std::move(objects),
            std::move(animators)
    };
}

/**
 * @brief Constructs a scene of the textured Stanford bunny.
 */
Scene bunny() {
    auto bunny = assimpLoad("../models/bunny_textured.obj", true);
    bunny.grow(glm::vec3(9, 9, 9));
    bunny.move(glm::vec3(0.2, -1, 0));

    return Scene {
            ShaderProgram::phongLighting(),
            {bunny}
    };
}

/**
 * @brief  Demonstrates loading a square, oriented as the "floor", with a manually-specified texture
 * that does not come from Assimp.
 * @return
 */
Scene marbleSquare() {
    std::vector<Texture> textures = {
            Texture::loadTexture("../models/White_marble_03/Textures_4K/white_marble_03_4k_baseColor.tga", "baseTexture"),
    };

    auto mesh = Mesh3D::square(textures);
    auto square = Object3D(std::vector<Mesh3D>{mesh});
    square.grow(glm::vec3(5, 5, 5));
    square.rotate(glm::vec3(-3.14159 / 4, 0, 0));
    return Scene{
            ShaderProgram::phongLighting(),
            {square}
    };
}
