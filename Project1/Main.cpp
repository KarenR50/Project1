#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

//incluir las clases 
#include"shaderClass.h"
#include"EBO.h"
#include"VAO.h"
#include"VBO.h"



//coordenadas de los vertices 
GLfloat vertices[] =
{
	-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, //esquina inferior izquierda
	0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, //esquina inferior derecha
	0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f, // esquina superior
	//triangulo interno
	-0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, //izq
	0.5f / 2 , 0.5f * float(sqrt(3)) / 6, 0.0f, //der
	0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f, //bajo
};


//indices para el orden de los vertices
GLuint indices[] =
{
	0,3,5,  //triangulo abajo izq
	3,2,4,  //triangulo abajo der
	5,4,1  //triangulo arriba xdd
};



int main()
{
	//inicar GLFW
	glfwInit();


	//decir que version de openGL estamos usando
	//en este caso openGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//decirle a GLFW que estamos usando CORE profile
	//Eso quiere decir que solo usaremos dunciones modernas xdd
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	


	//Crear un objeto  ventana GLFW de 8000x8000 llamada 1Project1
	GLFWwindow* window = glfwCreateWindow(800, 800, "Project1", NULL, NULL);
	
	//error por si la ventana falla en crearse 
	if (window == NULL)
	{
		std::cout << "Error al cear la ventana GLFW" << std::endl;

		return -1;
	}



	//Introducir la ventana al contexto actual
	glfwMakeContextCurrent(window);

	//Cargar glad 
	gladLoadGL();


	//Especificar el viewport de opneGL en la ventana
	//En este caso el viewport va desde x=0, y=0 hasta x=800, y=800
	glViewport(0, 0, 800, 800);



	//generar los objetos de shader usando los default.frag y .vert
	Shader shaderProgram("default.vert", "default.frag");

	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Genera Vertex Buffer Object y conectarlo a los vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Genera Element Buffer Object y lo conecta a los indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO to VAO
	VAO1.LinkVBO(VBO1, 0);
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.764f, 0.694f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		shaderProgram.Activate();
		VAO1.Bind();
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//especificar primitiva, indices, tipo de dato de los
		// indices y el index de los indices
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);

	//Gestionar todos los eventos de glfw
		glfwPollEvents();
	}


	// Borrar todos los objetos que hemos creadoo
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();


	//borrar la ventana y terminar GLFW antes de terminar el programa
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}