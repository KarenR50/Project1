#version 330 core

//Posicion, coordenadas
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;


// Genera el color para el sombreador de fragmentos
out vec3 color;
// Controla la escala de los vertices
uniform float scale;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	
// Muestra las posiciones/coordenadas de todos los vértices
	gl_Position = proj * view * model *vec4(aPos, 1.0);
	// Asigna los colores de los datos de vértice a "color"
	color = aColor;
}