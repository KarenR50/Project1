#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
//glm para 3d
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
//para texturas
#include<stb/stb_image.h>

//incluir las clases 
#include"shaderClass.h"
#include"EBO.h"
#include"VAO.h"
#include"VBO.h"
#include"Texture.h"
const unsigned int width = 800;
const unsigned int height = 800;



GLfloat vertices[] =
{
	// Posición (X,Y,Z) | Color (R,G,B) | UV (U,V) | Normal (X,Y,Z) | Tangente (X,Y,Z)

	// Vértice 0
	-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,
	 0.8f,  0.3f, 0.02f,
	 0.0f,  0.0f,
	 0.0f,  0.0f, 1.0f,
	 1.0f,  0.0f, 0.0f,

	 // Vértice 1
	  0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,
	  0.8f,  0.3f, 0.02f,
	  1.0f,  0.0f,
	  0.0f,  0.0f, 1.0f,
	  1.0f,  0.0f, 0.0f,

	  // Vértice 2
	   0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,
	   1.0f,  0.6f, 0.32f,
	   0.5f,  1.0f,
	   0.0f,  0.0f, 1.0f,
	   1.0f,  0.0f, 0.0f,

	   // Vértice 3
	   -0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,
		0.9f,  0.45f, 0.17f,
		0.25f, 0.5f,
		0.0f,  0.0f, 1.0f,
		1.0f,  0.0f, 0.0f,

		// Vértice 4
		 0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,
		 0.9f,  0.45f, 0.17f,
		 0.75f, 0.5f,
		 0.0f,  0.0f, 1.0f,
		 1.0f,  0.0f, 0.0f,

		 // Vértice 5
		  0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,
		  0.8f,  0.3f, 0.02f,
		  0.5f, 0.0f,
		  0.0f,  0.0f, 1.0f,
		  1.0f,  0.0f, 0.0f
};
	



GLuint indices[] =
{
	0,3,5,  //triangulo abajo izq
	3,2,4,  //triangulo abajo der
	5,4,1  //triangulo arriba xdd
};



int main()
{

	glfwInit();



	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//decirle a GLFW que estamos usando CORE profile
	//Eso quiere decir que solo usaremos dunciones modernas xdd
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);




	GLFWwindow* window = glfwCreateWindow(width, height, "Project1", NULL, NULL);


	if (window == NULL)
	{
		std::cout << "Error al cear la ventana GLFW" << std::endl;

		return -1;
	}




	glfwMakeContextCurrent(window);


	gladLoadGL();



	glViewport(0, 0, width, height);



	//generar los objetos de shader usando los default.frag y .vert
	Shader shaderProgram("default.vert", "default.frag");


	VAO VAO1;
	VAO1.Bind();

	// Genera Vertex Buffer Object y conectarlo a los vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Genera Element Buffer Object y lo conecta a los indices
	EBO EBO1(indices, sizeof(indices));

	// Atributo 0: Posición
	VAO1.LinkAttrib(VBO1,0,3,GL_FLOAT,14 * sizeof(float),(void*)0);

	// Atributo 1: Color
	VAO1.LinkAttrib(VBO1,1,3,GL_FLOAT,14 * sizeof(float),(void*)(3 * sizeof(float)));

	// Atributo 2: Coordenadas de textura
	VAO1.LinkAttrib(VBO1,2,2,GL_FLOAT,14 * sizeof(float),(void*)(6 * sizeof(float)));

	// Atributo 3: Normal
	VAO1.LinkAttrib(
		VBO1,3,3,GL_FLOAT,14 * sizeof(float),(void*)(8 * sizeof(float)));

	// Atributo 4: Tangente
	VAO1.LinkAttrib(VBO1,4,3,GL_FLOAT,14 * sizeof(float),(void*)(11 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");


	// Texture
	
	shaderProgram.Activate();

	Texture gl_texture("../images/textura.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	gl_texture.texUnit(shaderProgram, "tex0", 0);

	//Normal Map
	Texture normalMap("../images/textura_normal.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
	normalMap.texUnit(shaderProgram, "normalMap", 1);
	

	normalMap.texUnit(shaderProgram, "normalMap", 1);
	

	float rotation = 0.0f;
	double prevTime = glfwGetTime();


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.191f, 0.104f, 0.500f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Activate();
	
		glActiveTexture(GL_TEXTURE0);
		gl_texture.Bind();

		glActiveTexture(GL_TEXTURE1);
		normalMap.Bind();

		double crntTime = glfwGetTime();

		if (crntTime - prevTime >= 1.0 / 60.0)
		{
			rotation += 0.25f;
			prevTime = crntTime;
		}


		//inicializar las matrices de modelo vista y proyeccion
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		//model = glm::mat4(1.0f);
		//model = glm::rotate(model,glm::radians(0.0f),glm::vec3(1.0f, 0.0f, 0.0f));

		model = glm::rotate(
			model,
			glm::radians(rotation),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		view = glm::translate(view, glm::vec3(0.0f, -0.2f, -2.0f));
		proj = glm::perspective(glm::radians(45.0f),(float)width / (float)height,0.1f,100.0f);

		int modeloc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modeloc, 1, GL_FALSE, glm::value_ptr(model));
		int viewloc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewloc, 1, GL_FALSE, glm::value_ptr(view));
		int projloc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projloc, 1, GL_FALSE, glm::value_ptr(proj));




		VAO1.Bind();
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//especificar primitiva, indices, tipo de dato de los
		// indices
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);

		//Gestionar todos los eventos de glfw
		glfwPollEvents();
	}


	// Borrar todos los objetos que hemos creadoo
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	
	gl_texture.Delete();
	shaderProgram.Delete();
	//borrar la ventana y terminar GLFW antes de terminar el programa
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}