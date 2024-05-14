#include <iostream>
#include "Mesh3D.h"
#include <glad/glad.h>
#include "MatrixUtils.h"
#include "MyBone.h"

using std::vector;
using sf::Vector2u;
using glm::mat4;
using glm::vec4;

// Static variable to keep track of the next bone index
unsigned int Mesh3D::globalBoneIndex = 0;
std::vector<std::string> Mesh3D::shaderBones;

void Vertex3D::addBoneData(unsigned int boneID, float weight)
{
    int minWeightIndex = -1;
    float minWeight = 0.0f;

    // Find an empty slot or the slot with the smallest weight
    for (int i = 0; i < 4; i++)
    {
        if (boneData.weights[i] == 0.0f)
        {
            minWeightIndex = i;
            break;
        }
        else if (boneData.weights[i] < minWeight || minWeightIndex == -1)
        {
            minWeightIndex = i;
            minWeight = boneData.weights[i];
        }
    }

    // If this bone has a higher weight than the currently stored bone with the smallest weight,
    // replace the stored bone with this one
    if (weight > minWeight)
    {
        boneData.IDs[minWeightIndex] = boneID;
        boneData.weights[minWeightIndex] = weight;
    }
}

Mesh3D::Mesh3D(std::vector<Vertex3D>&& vertices, std::vector<uint32_t>&& faces,
	Texture texture) 
	: Mesh3D(std::move(vertices), std::move(faces), std::vector<Texture>{texture}) {
}

Mesh3D::Mesh3D(std::vector<Vertex3D>&& vertices, std::vector<uint32_t>&& faces, std::vector<Texture>&& textures)
 : m_vertexCount(vertices.size()), m_faceCount(faces.size()), m_textures(textures) {

	// Generate a vertex array object on the GPU.
	glGenVertexArrays(1, &m_vao);
	// "Bind" the newly-generated vao, which makes future functions operate on that specific object.
	glBindVertexArray(m_vao);

	// Generate a vertex buffer object on the GPU.
	uint32_t vbo;
	glGenBuffers(1, &vbo);

	// "Bind" the newly-generated vbo, which makes future functions operate on that specific object.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// This vbo is now associated with m_vao.
	// Copy the contents of the vertices list to the buffer that lives on the GPU.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), &vertices[0], GL_STATIC_DRAW);

	// Inform OpenGL how to interpret the buffer. Each vertex now has TWO attributes; a position and a color.
	// Atrribute 0 is position: 3 contiguous floats (x/y/z)...
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex3D), 0);
	glEnableVertexAttribArray(0);

	// Attribute 1 is normal (nx, ny, nz): 3 contiguous floats, starting 12 bytes after the beginning of the vertex.
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex3D), (void*)12);
	glEnableVertexAttribArray(1);

	// Attribute 2 is texture coordinates (u, v): 2 contiguous floats, starting 24 bytes after the beginning of the vertex.
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex3D), (void*)24);
	glEnableVertexAttribArray(2);

    // Attribute 3 is BoneIDs: 4 contiguous integers, starting after the texture coordinates in the Vertex3D structure.
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex3D), (void*)offsetof(Vertex3D, boneData.IDs));
    glEnableVertexAttribArray(3);

    // Attribute 4 is Weights: 4 contiguous floats, starting after the BoneIDs in the Vertex3D structure.
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, boneData.weights));
    glEnableVertexAttribArray(4);

	// Generate a second buffer, to store the indices of each triangle in the mesh.
	uint32_t ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(uint32_t), &faces[0], GL_STATIC_DRAW);

	// Unbind the vertex array, so no one else can accidentally mess with it.
	glBindVertexArray(0);
}

void Mesh3D::addTexture(Texture texture)
{
	m_textures.push_back(texture);
}

void Mesh3D::render(sf::Window& window, ShaderProgram& program) const {
	// Activate the mesh's vertex array.
	glBindVertexArray(m_vao);
	for (auto i = 0; i < m_textures.size(); i++) {
		program.setUniform(m_textures[i].samplerName, i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].textureId);
	}

    // Pass the bone transformation matrices to the shader
    for (const auto & i : boneInfos)
    {
        // Rigth now we only care about MyBone::tireBoneNames
        if (std::find(MyBone::tireBoneNames.begin(), MyBone::tireBoneNames.end(), i->name) == MyBone::tireBoneNames.end())
            continue;

//        std::cout << "Passing '" << i->name << "' to shader" << std::endl;
//
//        // Print the gBones array index and value
//        std::cout << "gBones[" << globalBoneIndex << "]:" << std::endl;
//        printMat4(i->finalTransformation);

        // If i->name is in MyBone::tireBoneNames
        if (std::find(MyBone::tireBoneNames.begin(), MyBone::tireBoneNames.end(), i->name) != MyBone::tireBoneNames.end())
        {
            // Find the iterator of shaderBones that corresponds to the current bone
            auto it = std::find(shaderBones.begin(), shaderBones.end(), i->name);
            if (it != shaderBones.end()) {
                // Calculate the index
                int index = std::distance(shaderBones.begin(), it);
                std::string name = "gBones[" + std::to_string(index) + "]";
                program.setUniform(name, i->finalTransformation);
                ++globalBoneIndex;
            }
        }
    }

	// Draw the vertex array, using its "element buffer" to identify the faces.
	glDrawElements(GL_TRIANGLES, m_faceCount, GL_UNSIGNED_INT, nullptr);
	// Deactivate the mesh's vertex array and texture.
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Mesh3D Mesh3D::square(const std::vector<Texture> &textures) {
	return Mesh3D(
		{ 
		  { 0.5, 0.5, 0, 0, 0, 1, 1, 0 },    // TR
		  { 0.5, -0.5, 0, 0, 0, 1, 1, 1 },   // BR
		  { -0.5, -0.5, 0, 0, 0, 1, 0, 1 },  // BL
		  { -0.5, 0.5, 0, 0, 0, 1, 0, 0 },   // TL
		}, 
		{ 
			2, 1, 3,
			3, 1, 0,
		},
		std::vector<Texture>(textures)
	);
}

Mesh3D Mesh3D::triangle(Texture texture) {
	return Mesh3D(
		{ { -0.5, -0.5, 0., 0, 0, 1, 0., 1. },
		  { -0.5, 0.5, 0, 0, 0, 1, 0., 0. },
		  { 0.5, 0.5, 0, 0, 0, 1, 1, 0 } },
		{ 2, 1, 0 },
		texture
	);
}

Mesh3D Mesh3D::cube(Texture texture) {
	std::vector<Vertex3D> verts = {
		///*BUR*/{ 0.5, 0.5, -0.5,  0, 0},
		///*BUL*/{ -0.5, 0.5, -0.5, 0, 0},
		///*BLL*/{ -0.5, -0.5, -0.5, 1.0, 0 },
		///*BLR*/{ 0.5, -0.5, -0.5, 0, 1.0},
		///*FUR*/{ 0.5, 0.5, 0.5, 1.0, 0},
		///*FUL*/{-0.5, 0.5, 0.5, 1.0, 1.0},
		///*FLL*/{-0.5, -0.5, 0.5, 0, 1.0},
		///*FLR*/{0.5, -0.5, 0.5, 1.0, 1.0}
	};
	std::vector<uint32_t> tris = {
		0, 1, 2,
		0, 2, 3,
		4, 0, 3,
		4, 3, 7,
		5, 4, 7,
		5, 7, 6,
		1, 5, 6,
		1, 6, 2,
		4, 5, 1,
		4, 1, 0,
		2, 6, 7,
		2, 7, 3
	};

	return Mesh3D({}, {}, texture);
}
