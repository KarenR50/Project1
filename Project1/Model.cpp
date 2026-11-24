#include"Model.h"
#include<algorithm>
#include<cctype>
#include<cstring>
#include<stdexcept>

Model::Model(const char* file)
{
	// Make a JSON object
	std::string text = get_file_contents(file);
	JSON = json::parse(text);

	// Get the binary data
	Model::file = file;
	data = getData();

	// Recorre todos los nodos raiz de la escena
	int sceneIdx = JSON.value("scene", 0);
	for (unsigned int i = 0; i < JSON["scenes"][sceneIdx]["nodes"].size(); i++)
		traverseNode(JSON["scenes"][sceneIdx]["nodes"][i]);
}

void Model::Draw(Shader& shader, Camera& camera, glm::mat4 matrix)
{
	// Go over all meshes and draw each one
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Mesh::Draw(shader, camera, matrix * matricesMeshes[i]);
	}
}

void Model::loadMesh(unsigned int indMesh)
{
	json primitives = JSON["meshes"][indMesh]["primitives"];

	// Una malla de OpenGL por cada primitiva
	for (unsigned int p = 0; p < primitives.size(); p++)
	{
		json prim = primitives[p];

		// Get all accessor indices
		unsigned int posAccInd = prim["attributes"]["POSITION"];
		unsigned int normalAccInd = prim["attributes"]["NORMAL"];
		unsigned int texAccInd = prim["attributes"]["TEXCOORD_0"];
		unsigned int indAccInd = prim["indices"];

		// Use accessor indices to get all vertices components
		std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
		std::vector<glm::vec3> positions = groupFloatsVec3(posVec);

		// Actualiza la caja del modelo con los vertices ya transformados
		glm::mat4 m = matricesMeshes[meshes.size()];
		for (unsigned int k = 0; k < positions.size(); k++)
		{
			glm::vec3 w = glm::vec3(m * glm::vec4(positions[k], 1.0f));
			boundsMin = glm::min(boundsMin, w);
			boundsMax = glm::max(boundsMax, w);
		}

		std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
		std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
		std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
		std::vector<glm::vec2> texUVs = groupFloatsVec2(texVec);

		// Combine all the vertex components and also get the indices and textures
		std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
		std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);

		// Texturas segun el material de esta primitiva
		int materialInd = prim.value("material", -1);
		std::vector<Texture> textures = getTextures(materialInd);

		// Combine the vertices, indices, and textures into a mesh
		meshes.push_back(Mesh(vertices, indices, textures));
	}
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
	// Current node
	json node = JSON["nodes"][nextNode];

	// Get translation if it exists
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end())
	{
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = (node["translation"][i]);
		translation = glm::make_vec3(transValues);
	}
	// Get quaternion if it exists
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		float rotValues[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotValues);
	}
	// Get scale if it exists
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = glm::make_vec3(scaleValues);
	}
	// Get matrix if it exists
	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = (node["matrix"][i]);
		matNode = glm::make_mat4(matValues);
	}

	// Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	// Use translation, rotation, and scale to change the initialized matrices
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	// Multiply all matrices together
	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	// Check if the node contains a mesh and if it does load it
	if (node.find("mesh") != node.end())
	{
		unsigned int meshInd = node["mesh"];
		unsigned int numPrims = JSON["meshes"][meshInd]["primitives"].size();

		// Una matriz por cada primitiva de la malla
		for (unsigned int p = 0; p < numPrims; p++)
		{
			translationsMeshes.push_back(translation);
			rotationsMeshes.push_back(rotation);
			scalesMeshes.push_back(scale);
			matricesMeshes.push_back(matNextNode);
		}

		loadMesh(meshInd);
	}

	// Check if the node has children, and if it does, apply this function to them with the matNextNode
	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matNextNode);
	}
}

std::vector<unsigned char> Model::getData()
{
	// Create a place to store the raw text, and get the uri of the .bin file
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];

	// Store raw text data into bytesText
	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	bytesText = get_file_contents((fileDirectory + uri).c_str());

	// Transform the raw text data into bytes and put them in a vector
	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

std::vector<float> Model::getFloats(json accessor)
{
	std::vector<float> floatVec;

	// Get properties from the accessor
	unsigned int buffViewInd = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	// Get properties from the bufferView
	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	// Interpret the type and store it into numPerVert
	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

	// Go over all the bytes in the data at the correct place using the properties from above
	unsigned int beginningOfData = byteOffset + accByteOffset;
	unsigned int lengthOfData = count * 4 * numPerVert;
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4)
	{
		unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}

	return floatVec;
}

std::vector<GLuint> Model::getIndices(json accessor)
{
	std::vector<GLuint> indices;

	// Get properties from the accessor
	unsigned int buffViewInd = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	// Get properties from the bufferView
	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	// Get indices with regards to their type: unsigned int, unsigned short, or short
	unsigned int beginningOfData = byteOffset + accByteOffset;
	if (componentType == 5125)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i += 4)
		{
			unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i += 2)
		{
			unsigned char bytes[] = { data[i], data[i + 1] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i += 2)
		{
			unsigned char bytes[] = { data[i], data[i + 1] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}

	return indices;
}

std::vector<Texture> Model::getTextures(int materialInd)
{
	std::vector<Texture> textures;
	if (materialInd < 0) return textures;

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	json material = JSON["materials"][materialInd];
	if (!material.contains("pbrMetallicRoughness")) return textures;
	json pbr = material["pbrMetallicRoughness"];

	const char* keys[2] = { "baseColorTexture", "metallicRoughnessTexture" };
	const char* types[2] = { "diffuse", "specular" };

	for (int k = 0; k < 2; k++)
	{
		if (!pbr.contains(keys[k])) continue;

		unsigned int texInd = pbr[keys[k]]["index"];
		unsigned int imgInd = JSON["textures"][texInd]["source"];
		std::string texPath = JSON["images"][imgInd]["uri"];

		// Reutiliza la textura si ya fue cargada (misma imagen y mismo tipo)
		std::string key = std::string(types[k]) + "|" + texPath;
		bool found = false;
		for (unsigned int j = 0; j < loadedTexName.size(); j++)
		{
			if (loadedTexName[j] == key)
			{
				textures.push_back(loadedTex[j]);
				found = true;
				break;
			}
		}

		if (!found)
		{
			Texture tex((fileDirectory + texPath).c_str(), types[k], 0);
			textures.push_back(tex);
			loadedTex.push_back(tex);
			loadedTexName.push_back(key);
		}
	}

	return textures;
}

std::vector<Vertex> Model::assembleVertices
(
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> texUVs
)
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back
		(
			Vertex
			{
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				texUVs[i]
			}
		);
	}
	return vertices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec)
{
	const unsigned int floatsPerVector = 2;

	std::vector<glm::vec2> vectors;
	for (unsigned int i = 0; i < floatVec.size(); i += floatsPerVector)
	{
		vectors.push_back(glm::vec2(0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++)
		{
			vectors.back()[j] = floatVec[i + j];
		}
	}
	return vectors;
}
std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec)
{
	const unsigned int floatsPerVector = 3;

	std::vector<glm::vec3> vectors;
	for (unsigned int i = 0; i < floatVec.size(); i += floatsPerVector)
	{
		vectors.push_back(glm::vec3(0, 0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++)
		{
			vectors.back()[j] = floatVec[i + j];
		}
	}
	return vectors;
}
std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec)
{
	const unsigned int floatsPerVector = 4;

	std::vector<glm::vec4> vectors;
	for (unsigned int i = 0; i < floatVec.size(); i += floatsPerVector)
	{
		vectors.push_back(glm::vec4(0, 0, 0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++)
		{
			vectors.back()[j] = floatVec[i + j];
		}
	}
	return vectors;
}