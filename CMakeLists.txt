cmake_minimum_required(VERSION 3.27)
project(mattsquared_graphics)

set(CMAKE_CXX_STANDARD 17)

add_executable(mattsquared_graphics
        main.cpp
        glad.c
        Mesh3D.cpp
        Object3D.cpp
        ShaderProgram.cpp
        AssimpImport.cpp
        StbImage.cpp
        Animator.cpp
        Scene.cpp
        DynamicLight.cpp
        UserInput.cpp
        Vehicle.cpp
        BoneRotationAnimation.h
        MatrixUtils.cpp
        MyBone.cpp
        MyBone.h
)

find_package(SFML COMPONENTS system window REQUIRED)
find_package(GLM CONFIG REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(mattsquared_graphics ${ZLIB_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(mattsquared_graphics
        sfml-system sfml-window
        ${ASSIMP_LIBRARIES}
)