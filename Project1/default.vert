#version 330 core

//Posicion, coordenadas
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;
// Texture Coordinates
layout (location = 2) in vec2 aTex;

// Genera el color para el sombreador de fragmentos
out vec3 color;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;
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
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;
}