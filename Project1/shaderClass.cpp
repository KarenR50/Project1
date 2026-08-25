
#include "shaderClass.h"

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//crear el objeto vertex shader y obtener su referencia
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//adjuntar el recurso de vertex shader al objeto de vertex shader???
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//compilar el vertex shader a codigo de maquina
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//crear el objeto shader program y obtener su referencia 
	ID = glCreateProgram();
	//adjuntar los shaders  vertex y frafment al Shader program 
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	//envolver todos los shaders juntos en el Shader program
	glLinkProgram(ID);

	//borrar los ahora inutiles Shaders vertex y fragment 
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void Shader::Activate()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}