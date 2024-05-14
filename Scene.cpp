#include "Scene.h"
#include "AssimpImport.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "BoneRotationAnimation.h"
#include "MyBone.h"

void animateObjectAndChildren(Object3D& object, ShaderProgram& shader, const std::vector<std::string>& boneNames, std::vector<Animator>& animators) {
    for (auto& mesh : object.getMeshes()) {
for (const auto& boneName : boneNames) {
    auto it = std::find_if(mesh.boneInfos.begin(), mesh.boneInfos.end(), [&boneName](const std::shared_ptr<BoneInfo>& bone) {
        return bone->name == boneName;
    });

    // If the bone is found
    if (it != mesh.boneInfos.end()) {
        // Create a BoneRotationAnimation for the bone
        auto boneRotationAnimation = std::make_unique<BoneRotationAnimation>(**it, object, 3.0f, glm::vec3(0.0f, 0.0f, 30.0f), shader);

        // Create an animator and add the animation to it
        Animator animator;
        animator.addAnimation(std::move(boneRotationAnimation));

        // Add the animator to the list
        animators.push_back(std::move(animator));
    }
}
    }

    // Recursively animate children
    for (size_t i = 0; i < object.numberOfChildren(); ++i) {
        animateObjectAndChildren(object.getChild(i), shader, boneNames, animators);
    }
}

Scene Scene::jeep() {
    auto jeep = assimpLoad("../models/mil_jeep_fbx/mil_jeep.fbx", true);
    jeep.move(glm::vec3(0, -1.2, 0));
    jeep.grow(glm::vec3(0.004, 0.004, 0.004));

    std::vector<Texture> groundtex = {Texture::loadTexture("../models/concrete.png","baseTexture")};
    //std::vector<Mesh3D> ground = {Mesh3D::square(groundtex)};
    Object3D map = Object3D({Mesh3D::square(groundtex)});//assimpLoad("../models/racetrack/arena.obj", true);
    map.grow(glm::vec3(100,100,100));
    map.move(glm::vec3(0,-1.2,0));
    map.rotate(glm::vec3(glm::radians(-90.0f),0,0));

    auto lightSource = assimpLoad("../models/tiger/scene.gltf", true);
    lightSource.setPosition(glm::vec3(-2,1,0));
    lightSource.grow(glm::vec3(0.01,0.01,0.01));

    glm::mat4 dirLight = glm::mat4(
            0.5,-0.1 ,0,0, //Position (Direction for directional lights);
            1,1,1,0, //Color
            1,1,10,0, //LightType, Range, Cuttoff Angle (For spotlights)
            0,0,0,0 //LookAt (For spotlights)
    );

    glm::mat4 pointLight = glm::mat4(
            -5,0,0,0, //Position
            1,0,0,0, //Color
            2,25,0,0, //LightType, Range, Cuttoff Angle (For spotlights)
            0,0,0,0 //LookAt (For spotlights)
    );

    glm::mat4 spotLight = glm::mat4(
            1,0,5,0, //Position
            1,1,0.5,0, //Color
            3,70,50,0, //LightType, Range, Cuttoff Angle (For spotlights)
            0,0,1,0 //LookAt (For spotlights)
    );

    glm::mat4 pointLight2 = glm::mat4(
            5,0,0,0, //Position
            0,1,0,0, //Color
            2,25,0,0, //LightType, Range, Cuttoff Angle (For spotlights)
            0,0,0,0 //LookAt (For spotlights)
    );

    ShaderProgram shader = ShaderProgram::phongLighting();

    std::vector<Animator> animators;
    //animateObjectAndChildren(jeep, shader, MyBone::tireBoneNames, animators);

    std::vector<Object3D> objects;
    objects.push_back(std::move(jeep));
    objects.push_back(std::move(lightSource));
    objects.push_back(std::move(map));
    std::vector<DynamicLight> lights;
    lights.push_back(std::move(DynamicLight(dirLight)));
    lights.push_back(std::move(DynamicLight(pointLight)));
    lights.push_back(std::move(DynamicLight(spotLight)));
    lights.push_back(std::move(DynamicLight(pointLight2)));

    return Scene {
            std::move(shader),
            std::move(objects),
            std::move(animators),
            std::move(lights)
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
            std::move(animators),
            {}
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
