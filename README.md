

# Proyecto de Computación Gráfica - OpenGL y C++

## Descripción

Este proyecto consiste en el desarrollo de una escena tridimensional
interactiva mediante OpenGL y el lenguaje de programación C++.

Se implementa la carga de modelos 3D en formato glTF con sus
respectivas texturas, un sistema de iluminación, una cámara de libre
movimiento y la animación de una esfera mediante una curva de Bézier
cúbica.

El proyecto integra diferentes conceptos de computación gráfica,
como el procesamiento de vértices, las transformaciones geométricas,
el mapeado de texturas, los shaders y la interacción con el usuario.

## Funcionalidades

- Carga de modelos 3D en formato glTF.
- Aplicación de texturas y mapas de normales.
- Iluminación ambiental, difusa y especular.
- Cámara interactiva mediante teclado y mouse.
- Transformaciones mediante matrices de modelo, vista y proyección.
- Generación de geometría mediante código.
- Implementación de una curva de Bézier cúbica.
- Animación de una esfera a través de la curva.
- Eventos de teclado para controlar la animación.
- Renderizado en tiempo real.

## Tecnologías y librerías

| Tecnología | Descripción |
C++ | Lenguaje de programación principal. 
OpenGL 3.3 | API para el renderizado gráfico. 
GLFW | Creación de ventanas, contexto OpenGL y eventos de entrada. 
GLAD | Carga de las funciones de OpenGL. 
GLM | Operaciones matemáticas, vectores y matrices. 
stb_image | Carga de imágenes y texturas. 
GLSL | Lenguaje de programación de shaders. 
Visual Studio | Entorno de desarrollo. 
vcpkg | Gestor de dependencias de C++. 

## Estructura del proyecto

El proyecto se encuentra organizado mediante clases y archivos
de encabezado (.h) e implementación (.cpp), con el propósito
de separar las responsabilidades de cada componente.


## Sistema de iluminación

La iluminación de la escena se compone de tres componentes:

- Ambiental: proporciona una iluminación base a los objetos.
- Difusa: depende de la orientación de la superficie respecto
  a la fuente de luz.
- Especular: genera reflejos que dependen de la posición de
  la luz y del observador.

También se utiliza normal mapping para representar detalles
en las superficies sin incrementar la cantidad de polígonos.

## Animación mediante curva de Bézier

Se implementa una curva de Bézier cúbica definida por cuatro
puntos de control. A partir de esta curva se calcula la posición
de una esfera en función de un parámetro t comprendido entre
0 y 1.

Al presionar la barra espaciadora, la esfera inicia su recorrido
a través de la trayectoria establecida.

## Controles

| Entrada | Acción |
| W | Desplazamiento hacia adelante. |
| S | Desplazamiento hacia atrás. |
| A | Desplazamiento hacia la izquierda. |
| D | Desplazamiento hacia la derecha. |
| Mouse | Cambio de orientación de la cámara. |
| Espacio | Activar la animación de la esfera. |




## Autores

- Karen Tatiana Rodríguez Pedraza


## Información académica
- Universidad Militar Nueva Granada.
- Ingeniería Multimedia.
- Asignatura: Computación Gráfica.
- Año: 2026.
