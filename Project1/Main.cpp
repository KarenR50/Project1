
#include<filesystem>
namespace fs = std::filesystem;


#include"Model.h"
#include<cmath>

const unsigned int width = 800;
const unsigned int height = 800;

// Punto de una curva de Bezier cubica para t en [0, 1]
glm::vec3 bezier(const glm::vec3& p0, const glm::vec3& p1,
	const glm::vec3& p2, const glm::vec3& p3, float t)
{
	float u = 1.0f - t;
	return u * u * u * p0 + 3.0f * u * u * t * p1 + 3.0f * u * t * t * p2 + t * t * t * p3;
}

// Genera una esfera (vertices e indices)
void buildSphere(float r, int stacks, int sectors,
	std::vector<Vertex>& verts, std::vector<GLuint>& idx)
{
	const float PI = 3.14159265f;
	for (int i = 0; i <= stacks; i++)
	{
		float phi = PI * i / stacks;
		for (int j = 0; j <= sectors; j++)
		{
			float theta = 2.0f * PI * j / sectors;
			glm::vec3 n(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));
			verts.push_back(Vertex{ n * r, n, glm::vec3(1.0f), glm::vec2((float)j / sectors, (float)i / stacks) });
		}
	}
	for (int i = 0; i < stacks; i++)
	{
		for (int j = 0; j < sectors; j++)
		{
			GLuint a = i * (sectors + 1) + j;
			GLuint b = a + sectors + 1;
			idx.push_back(a); idx.push_back(b); idx.push_back(a + 1);
			idx.push_back(a + 1); idx.push_back(b); idx.push_back(b + 1);
		}
	}
}


int main()
{
	// Initialize GLFW
	glfwInit();

	// OpenGL 3.3 core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "window"
	GLFWwindow* window = glfwCreateWindow(width, height, "window", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // activa V-Sync

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	glViewport(0, 0, width, height);


	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 2.0f, 25.0f));


	// Ruta del modelo
	std::string modelPath = "C:/Users/karen/Documents/proyectosOpenGL/Project1/gltf/jungleblaster.gltf";

	// Cargar modelo
	Model model(modelPath.c_str());

	// Shader plano para la curva y la esfera
	Shader lightShader("light.vert", "light.frag");

	// --- Dimensiones del modelo ---
	glm::vec3 center = (model.boundsMin + model.boundsMax) * 0.5f;
	glm::vec3 size = model.boundsMax - model.boundsMin;
	std::cout << "min: " << model.boundsMin.x << ", " << model.boundsMin.y << ", " << model.boundsMin.z << std::endl;
	std::cout << "max: " << model.boundsMax.x << ", " << model.boundsMax.y << ", " << model.boundsMax.z << std::endl;

	// Camara lateral (mira el arma desde el costado, en direccion -X)
	camera.Orientation = glm::vec3(-1.0f, 0.0f, 0.0f);
	camera.Position = center + glm::vec3(glm::length(size) * 1.2f, 0.0f, 0.0f);

	// Direccion del cañon: el lado mas largo del modelo
	glm::vec3 dir = (size.x > size.z) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
	

	float offsetY = 0.23f;                     // sube o baja el punto de salida
	float range = glm::length(size) * 1.5f;    // alcance horizontal de la bala
	float drop = glm::length(size) * 0.8f;     // cuanto cae al final (mas = cae mas fuerte)

	// Boca del canon: extremo del modelo en la direccion dir
	glm::vec3 muzzle = center + dir * glm::dot(glm::abs(dir), size * 0.5f);
	muzzle.y += offsetY * size.y;

	// Puntos de control: recta al inicio y caida parabolica al final
	glm::vec3 down(0.0f, -1.0f, 0.0f);
	glm::vec3 P0 = muzzle;
	glm::vec3 P1 = muzzle + dir * (range / 3.0f);
	glm::vec3 P2 = muzzle + dir * (range * 2.0f / 3.0f) + down * (drop / 3.0f);
	glm::vec3 P3 = muzzle + dir * range + down * drop;

	// Curva
	std::vector<Vertex> curveVerts;
	const int segments = 100;
	for (int i = 0; i <= segments; i++)
	{
		glm::vec3 p = bezier(P0, P1, P2, P3, (float)i / segments);
		curveVerts.push_back(Vertex{ p, glm::vec3(0, 1, 0), glm::vec3(1.0f), glm::vec2(0.0f) });
	}
	VAO curveVAO;
	curveVAO.Bind();
	VBO curveVBO(curveVerts);
	curveVAO.LinkAttrib(curveVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	curveVAO.Unbind();
	curveVBO.Unbind();

	// Esfera proporcional al modelo
	std::vector<Vertex> sphereVerts;
	std::vector<GLuint> sphereIdx;
	std::vector<Texture> noTextures;
	buildSphere(glm::length(size) * 0.02f, 24, 24, sphereVerts, sphereIdx);
	Mesh sphere(sphereVerts, sphereIdx, noTextures);

	// Control del disparo
	bool disparando = false;
	bool espacioAnterior = false;
	float inicioDisparo = 0.0f;


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs
		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 1000.0f);

		// Detectar la tecla espacio
		bool espacioActual = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

		if (espacioActual && !espacioAnterior && !disparando)
		{
			disparando = true;
			inicioDisparo = (float)glfwGetTime();
		}

		espacioAnterior = espacioActual;


		// Draw a model
		model.Draw(shaderProgram, camera, glm::mat4(1.0f));

		// --- Curva ---
		lightShader.Activate();
		camera.Matrix(lightShader, "camMatrix");
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), 1.0f, 0.9f, 0.2f, 1.0f);
		curveVAO.Bind();
		glDrawArrays(GL_LINE_STRIP, 0, segments + 1);

		// --- Bala: se dispara con la tecla espacio ---
		float duracion = 2.0f;

		if (disparando)
		{
			float tiempo = (float)glfwGetTime() - inicioDisparo;

			float t = tiempo / duracion;

			if (t >= 1.0f)
			{
				t = 1.0f;
				disparando = false;
			}

			glm::vec3 pos = bezier(P0, P1, P2, P3, t);

			glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"),1.0f, 0.3f, 0.3f, 1.0f);

			sphere.Draw(lightShader,camera,glm::translate(glm::mat4(1.0f), pos));
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// Delete all the objects we've created
	shaderProgram.Delete();
	lightShader.Delete();
	curveVAO.Delete();
	curveVBO.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}