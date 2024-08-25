/*
@author: Mateo Bodenlle Villarino
@author: Sara Castro López
*/

// Librerias
/*nclude <glad.h>
#include <glfw3.h>
#include <iostream>
#include "lecturaShader_0_9.h"
#include "esfera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"*/
#include "avion.h"
//#include <array>

#define RENDER_DISTANCE 250

//	Constante de tamaño del suelo (va de -FLOOR a +FLOOR)
float Ibase = 0.7;
float* ambientI = &Ibase;

struct TextData {
	//Material...
	float fontSize = 1.0f;
	int renderOptions;
};
TextData textData = {};

// Declaración de variables globales:
unsigned int VAO;
unsigned int VAOCuadrado;
unsigned int VAOCubo;
unsigned int VAOEsfera;

// Tamaño de la ventana
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;


unsigned int camera = 2; // Variable para la selección de la cámara

// Variables camara
glm::vec3 cameraPos = glm::vec3(4.0f, 4.0f, -12.0f); // posicion inicial de la camara
glm::vec3 cameraFront = glm::vec3(0.0f, -0.2f, 1.0f); // direccion de la camara
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // eje de referencia

// Variable shaders
GLuint shaderProgram;

//	Variables texturas
unsigned int pavimento;
unsigned int cesped;
unsigned int pistaCentral, pistaDerecha, pistaIzquierda;
unsigned int tex_grua;
unsigned int tex_grua_esfera;
unsigned int tex_thrust;
unsigned int textura_avion;
unsigned int tex_almacen;
unsigned int rojo;
unsigned int tex_bala;
unsigned int tex_edificio;


typedef struct {
	float px, py, pz;		// Posición inicial
	float esx, esy, esz;	// Escalado en los tres ejes
	unsigned int VAO;		// Lista de render
	GLsizei verts;			// Número de vértices
}Almacen;

typedef struct {
	float px, py, pz;		// Posición inicial
	float esx, esy, esz;	// Escalado en los tres ejes
	unsigned int VAO;		// Lista de render
	GLsizei verts;			// Número de vértices
}puntero;

typedef struct {
	glm::vec3 p;			// Posición 
	glm::vec3 v;			// Velocidad
	float esx, esy, esz;	// Escalado en los tres ejes
	unsigned int VAO;		// Lista de render
	GLsizei verts;			// Número de vértices
	int lado;				// Lado de la bala	(-1 izquierda, 1 derecha)
}Bala;
Almacen almacen = { 25.0f, 1.0f, 32.0f, 14.0f, 12.0f, 12.5f, 0, 36 };
Almacen almacen2 = { 20.0f, 1.0f, 6.0f, 14.0f, 12.0f, 12.5f, 0, 36 };
Almacen almacen3 = { 20.0f, 1.0f, 18.0f, 12.0f, 14.0f, 10.5f, 0, 36 };
Almacen almacen4 = { -20.0f, 2.0f, 14.0f, 10.0f, 11.0f, 16.5f, 0, 36 };

Almacen edificio1 = { 40.0f, 10.0f, 80.0f,	20.0f, 60.0f, 20.5f, 0, 36 };
Almacen edificio2 = { -30.0f, 20.0f, 90.0f,	16.0f, 80.0f, 16.5f, 0, 36 };
Almacen edificio3 = { 35.0f, 5.0f, 120.0f,	12.0f, 40.0f, 10.5f, 0, 36 };
Almacen edificio4 = { -50.0f, 12.0f, 104.0f,22.0f, 35.0f, 23.5f, 0, 36 };

//Variables globales de texto por pantalla
int fontHeight;
GLuint fontAtlasID;

struct Glyph
{
	glm::vec2 offset;
	glm::vec2 advance;
	glm::ivec2 textureCoords;
	glm::ivec2 size;
};

Glyph glyphs[127];


struct Transform
{
	glm::vec2 pos; // This is currently the Top Left!!
	glm::vec2 size;
	glm::ivec2 atlasOffset;
	glm::ivec2 spriteSize;
	int renderOptions; 
	int materialIdx;
	float layer;
	int padding;
};

// Prototipos de funciones externas
extern GLuint setShaders(const char* nVertx, const char* nFrag);

GLfloat ambientIntensity = 2.0f;

// Esta función se llama cada vez que se cambia el tamaño de la ventana
void resize(GLFWwindow* window, int width, int height) {

	// Cambiamos la anchura
	SCR_WIDTH = width;
	// Cambiamos la altura
	SCR_HEIGHT = height;
}

// Coloca la camara en vista exterior
void myCamaraExterior() {

	// Definimos el área de la ventana para el renderizado de la escena 3D
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	// Definimos la posicion y orientación de la camara
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); // Matriz de vista
	// Obtenemos la ubicación de la matriz de vista en el shader
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	// Establecemos el valor de la matriz de vista
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	// Definimos la proyeccion de la camara
	glm::mat4 projection; // matriz de proyeccion
	projection = glm::mat4(); // Se inicializa como la matriz identidad
	// Definimos la proyección de la cámara 
	projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, FLOOR * 2.5f);
	// Obtenemos la ubicación de la matriz de proyección en el shader
	unsigned int proyectionLoc = glGetUniformLocation(shaderProgram, "projection");
	// Establecemos el valor de la matriz de proyección
	glUniformMatrix4fv(proyectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

/*	Funcion de tercera persona
	Recibe como argumentos la posición de la grúa y el ángulo de giro*/
void myTerceraPersona(float posicionx, float posiciony, float posicionz, glm::vec3 direccion) {

	// Definimos el área de la ventana para el renderizado de la escena 3D
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	// Definimos la posicion y orientación de la camara
	glm::mat4 view = glm::lookAt(
		glm::vec3(posicionx, posiciony+5+direccion.y, posicionz)  - glm::normalize(direccion) * 9.0f, // posicion
		glm::vec3(posicionx , posiciony+1+direccion.y, posicionz) + glm::normalize(direccion) * 20.0f,			// a donde mira
		glm::vec3(0.0f, 1.0f, 0.0f));	// cameraup (eje de referencia)
	// Obtenemos la ubicación de la matriz de vista en el shader
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	// Establecemos el valor de la matriz de vista
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Definimos la proyeccion de la camara
	glm::mat4 projection; // matriz de proyeccion
	projection = glm::mat4(); // Se inicializa como la matriz identidad
	// Definimos la proyección de la cámara 
	projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,250.0f);
	// Obtenemos la ubicación de la matriz de proyección en el shader
	unsigned int proyectionLoc = glGetUniformLocation(shaderProgram, "projection");
	// Establecemos el valor de la matriz de proyección
	glUniformMatrix4fv(proyectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

/*	Función de primera persona
	Recibe como argumentos la posición de la grúa y el ángulo de giro*/
void myPrimeraPersona(float posicionx, float posiciony, float posicionz, float angulo) {

	// Definimos el área de la ventana para el renderizado de la escena 3D
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	// Definimos la posicion y orientación de la camara
	glm::mat4 view = glm::lookAt(
		glm::vec3(posicionx + 1 * sin(angulo * GRADOSTORADIANES), posiciony + 1, posicionz + 1 * cos(angulo * GRADOSTORADIANES)),
		glm::vec3(posicionx + 10 * sin(angulo * GRADOSTORADIANES), posiciony, posicionz + 10 * cos(angulo * GRADOSTORADIANES)),
		glm::vec3(0.0f, 1.0f, 0.0f));

	// Obtenemos la ubicación de la matriz de vista en el shader
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");

	// Establecemos el valor de la matriz de vista
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Definimos la proyeccion de la camara
	glm::mat4 projection; // matriz de proyeccion
	projection = glm::mat4(); // Se inicializa como la matriz identidad

	// Definimos la proyección de la cámara 
	projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);

	// Obtenemos la ubicación de la matriz de proyección en el shader
	unsigned int proyectionLoc = glGetUniformLocation(shaderProgram, "projection");

	// Establecemos el valor de la matriz de proyección
	glUniformMatrix4fv(proyectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}


int ladoDisparo = -1;
std::vector <Bala> balas;

//Void disparo
void dispararA() {

	ladoDisparo *= -1;
	Bala ba = {
		avion.p,
		avion.v + glm::normalize(avion.direccion) * glm::vec3(2.5),
		0.35, 0.35, 0.35, VAOCubo, 36,
		ladoDisparo };
	float coseno = cos(atan(1.3 / DISTANCIAAPUNTADO * ladoDisparo));
	float seno =   sin(atan(1.3 / DISTANCIAAPUNTADO * ladoDisparo));
	ba.v.x = ba.v.x * coseno - ba.v.z * seno;
	ba.v.z = ba.v.x * seno + ba.v.z * coseno;

	balas.push_back(ba);
}

//	Gestión de teclado de movimiento de la grúa y la cámara
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// Tecla ESCAPE para salir del programa
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// MOVER BASE
	if (key == GLFW_KEY_W) {  // W: acelerar
		motor(1);
	}
	if (key == GLFW_KEY_X) { // X: frenar
		motor(-1);
	}
	if (key == GLFW_KEY_D) { // D: giro derecha 
		guiñada(1);
	}
	if (key == GLFW_KEY_A) { // A: giro izquierda
		guiñada(-1);
	}
	if (key == GLFW_KEY_S) { // S: Frenar en seco
		freno();
	}
	if (key == GLFW_KEY_U) { // U: subir
			cabeceo(1);
		}
	if (key == GLFW_KEY_J) { // J: bajar
		cabeceo(-1);
	}
	if (key == GLFW_KEY_Q) { // Q: subir
		alabeo(1);
	}
	if (key == GLFW_KEY_E) { // E: bajar
		alabeo(-1);
	}
	if (key == GLFW_KEY_P){ // P: apagar el motor
		alternarMotor(false);
	}
	if (key == GLFW_KEY_O) { //O: encender el motor
		alternarMotor(true);
	}
	if (key == GLFW_KEY_H){// H: disparar ametralladora
		dispararA();
	}

	// CONTROL DE CAMARA
	if (key == GLFW_KEY_1) { // PRIMERA PERSONA
		camera = 1;
	}
	if (key == GLFW_KEY_2) { // VISTA EXTERIOR
		camera = 2;
	}
	if (key == GLFW_KEY_3) { // TERCERA PERSONA
		camera = 3;
	}

	// Cambiar al modo noche
	if (key == GLFW_KEY_N) {
		ambientIntensity = 0.5f;
		glClearColor(0.15f, 0.15f, 0.3f, 1.0f);// Establece el color de borrado (fondo)

	}
	// Cambiar al modo día
	if (key == GLFW_KEY_B) {
		ambientIntensity = 2.0f;
		glClearColor(0.5f, 0.7f, 1.0f, 1.0f);// Establece el color de borrado (fondo)
	}

	// MOVER CAMARA : solo para camara exterior
	if (camera == 2) {
		if (key == GLFW_KEY_UP) {  // arriba
			cameraFront += glm::vec3(0.0f, 0.01f, 0.0f);
		}
		if (key == GLFW_KEY_DOWN) { // abajo
			cameraFront -= glm::vec3(0.0f, 0.01f, 0.0f);
		}
		if (key == GLFW_KEY_RIGHT) { // derecha
			cameraFront -= glm::vec3(0.01f, 0.0f, 0.0f);
		}
		if (key == GLFW_KEY_LEFT) { // izquierda 
			cameraFront += glm::vec3(0.01f, 0.0f, 0.0f);
		}
	}
}

//	Función para dibujar cuadrados
void dibujarCuadrado() {

	// Identificadores de objetos para Vertex Buffer y Element Buffer
	unsigned int VBO;

	// Vértices del cuadrado
	float vertices[] = {
		// Triángulo 1			
		//	Posición			//	Color			//	Textura
		-0.5f, 0.0f, -0.5f,		1.0f, 1.0f, 1.0f,	0.0f,	0.0f,
		-0.5f, 0.0f, 0.5f,		1.0f, 1.0f, 1.0f,	1.0f,	0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f, 1.0f,	1.0f,	1.0f,
		// Triángulo 2			
		0.5f, 0.0f, 0.5f,		1.0f, 1.0f, 1.0f,	1.0f,	1.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f, 1.0f,	0.0f,	1.0f,
		-0.5f, 0.0f, 0.5f,		1.0f, 1.0f, 1.0f,	0.0f,	0.0f

	};

	// Generamos VAO, VBO y EBO
	glGenVertexArrays(1, &VAOCuadrado);
	glGenBuffers(1, &VBO);

	// Vinculamos VAO
	glBindVertexArray(VAOCuadrado);

	// Vinculamos VBO 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Cargamos datos de vértices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configuramos la información de posición del vértice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Configuramos la información de color del vértice
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Configuramos la información de textura del vértice
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Desactivamos VBO y VAO 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Eliminamos buffers de vértices
	glDeleteBuffers(1, &VBO);
}

//	Función para dibujar cubos
void dibujarCubo() {

	unsigned int VBO;

	// Cubo de la grúa
	float vertices[] = {
		// POSICIÓN           // TEXTURA      // NORMALES       
		// CARA INFERIOR		
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f  ,	  0.0f, 0.0f, -1.0f,
		0.5f, -0.5f,  0.5f,   1.0f, 0.0f  ,   0.0f, 0.0f, -1.0f,
		0.5f,  0.5f,  0.5f,   1.0f, 1.0f  ,   0.0f, 0.0f, -1.0f,

		0.5f,  0.5f,  0.5f,   1.0f, 1.0f  ,   0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f,  0.5f,   0.0f, 1.0f  ,   0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f  ,   0.0f, 0.0f, -1.0f,

		// CARA SUPERIOR
		-0.5f, 0.5f, 0.5f,    0.0f, 1.0f  ,   0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,     1.0f, 1.0f  ,   0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,    1.0f, 0.0f  ,   0.0f, 1.0f, 0.0f,

		0.5f, 0.5f, -0.5f,    1.0f, 0.0f  ,   0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,   0.0f, 0.0f  ,   0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,    0.0f, 1.0f  ,   0.0f, 1.0f, 0.0f,

		// CARA LATERAL IZQUIERDA
		/*-0.5f, -0.5f, -0.5f,  0.0f, 0.0f  ,   -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,   0.0f, 1.0f  ,   -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,    1.0f, 1.0f  ,   -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f  ,   -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,    1.0f, 1.0f  ,   -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,   1.0f, 0.0f  ,   -1.0f, 0.0f, 0.0f,
		*/
		// CARA LATERAL DERECHA
		0.5f, -0.5f, 0.5f,    0.0f, 1.0f  ,   -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  -0.5f,  0.0f, 0.0f  ,   -1.0f, 0.0f, 0.0f,
		0.5f, 0.5f,  -0.5f,   1.0f, 0.0f  ,   -1.0f, 0.0f, 0.0f,

		0.5f, -0.5f,  0.5f,   0.0f, 1.0f  ,   1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f,    1.0f, 0.0f  ,   1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f,     1.0f, 1.0f  ,   1.0f, 0.0f, 0.0f,
		
		// CARA INFERIOR
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f  ,   0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f,    1.0f, 1.0f  ,   0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,   0.0f, 1.0f  ,   0.0f, -1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f  ,   0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,   1.0f, 0.0f  ,   0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f,    1.0f, 1.0f  ,   0.0f, -1.0f, 0.0f,

		// CARA TRASERA
		-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f  ,   0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,   1.0f, 0.0f  ,   0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  -0.5f, 0.0f, 0.0f  ,   0.0f, 0.0f, 1.0f,

		0.5f, 0.5f,  -0.5f,   1.0f, 1.0f  ,   0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,   1.0f, 0.0f  ,   0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,   0.0f, 1.0f  ,   0.0f, 0.0f, 1.0f
	};

	// Generamos el VAO para el cubo
	glGenVertexArrays(1, &VAOCubo);

	// Generamos los VBO y EBO necesarios
	glGenBuffers(1, &VBO);

	// Enlazamos el VAO para el cubo
	glBindVertexArray(VAOCubo);

	// Enlazarmos el VBO 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Cargamos los datos de los vértices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vértices

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// Normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Limpiamos los buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desenlazamos el VAO y VBO
	glBindVertexArray(0);

	// Eliminar los VBO y EBO
	glDeleteBuffers(1, &VBO);
}


//	Función para dibujar esferas
void dibujarEsfera() {

	unsigned int VBO;
	glActiveTexture(tex_grua_esfera);
	glBindTexture(GL_TEXTURE_2D, tex_grua_esfera);

	// Generamos el VAO y lo hacemos activo
	glGenVertexArrays(1, &VAOEsfera);
	glBindVertexArray(VAOEsfera);

	// Generamos el VBO de vértices y lo hacemos activo
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_esfera), vertices_esfera, GL_STATIC_DRAW);

	// Especificamos el formato de los datos de las normales en el VBO
	// Posición
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// Color
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Especificamos el formato de los datos de los vértices en el VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(0);

	// Desenlazamos el VBO y VAO 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Borramos los buffers
	glDeleteBuffers(1, &VBO);

}
void dibujarThrust() {

	unsigned int VBO;
	glActiveTexture(tex_thrust);
	glBindTexture(GL_TEXTURE_2D, tex_thrust);

	// Generamos el VAO y lo hacemos activo
	glGenVertexArrays(1, &VAOEsfera);
	glBindVertexArray(VAOEsfera);

	// Generamos el VBO de vértices y lo hacemos activo
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_esfera), vertices_esfera, GL_STATIC_DRAW);

	// Especificamos el formato de los datos de las normales en el VBO
	// Posición
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// Color
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Especificamos el formato de los datos de los vértices en el VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(0);

	// Desenlazamos el VBO y VAO 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Borramos los buffers
	glDeleteBuffers(1, &VBO);
}


	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // No las usaremos por ahora

void dibujarAvion() {

	unsigned int VBO;
	glActiveTexture(tex_grua_esfera);
	glBindTexture(GL_TEXTURE_2D, tex_grua_esfera);

	// Generamos el VAO y lo hacemos activo
	glGenVertexArrays(1, &avion.VAO);
	glBindVertexArray(avion.VAO);

	// Generamos el VBO de vértices y lo hacemos activo
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Desenlazamos el VBO y VAO 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Borramos los buffers
	glDeleteBuffers(1, &VBO);
}

unsigned int cargarTextura(const char* texture_name) {

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//	Wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//	Filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//	Load image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texture_name, &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels == 3) {		// Si el formato de color es RGB
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else if (nrChannels == 4) {	// Si el fotmato de color es RGBA
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return texture;
}


void dibujarBalas(glm::mat4* transform, unsigned int transformLoc) {
	
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	std::vector<Bala> balas2;

    for (int i = 0; i < balas.size(); i++) {
	glActiveTexture(tex_bala);
	glBindTexture(GL_TEXTURE_2D, tex_bala);

		Bala b = balas.at(i);
        glm::vec3 posicion = b.p;
		b.p += b.v / glm::vec3(1);

        // Obtener la posición de la bala
		
	//printf("PI%d / %dUU %f %f %f, %f %f %f\n",i,balas.size(), b.p.x, b.p.y, b.p.z, b.v.x, b.v.y, b.v.z);
	*transform = glm::mat4();

	// Aplicamos una traslación a la posición del cubo
	*transform = glm::translate(*transform, glm::vec3(b.p.x, b.p.y, b.p.z));
	*transform = glm::translate(*transform, glm::vec3(b.lado*1.3, 0, 0));

	*transform = glm::scale(*transform, glm::vec3(b.esx, b.esy, b.esz));

	// Enviamos la matriz de transformaciones al shader y dibujamos el objeto
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	glBindVertexArray(b.VAO);
	glDrawArrays(GL_TRIANGLES, 0, b.verts);

	// Desenlazamos el VAO
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Desenlazamos el VAO
	glBindVertexArray(0);
	
	if (glm::distance(avion.p, b.p) < RENDER_DISTANCE) balas2.push_back(b);
	}
	balas = balas2;
}

	int aeropuertoMinX = -FLOOR / 3;  // Coord. X mínima del aeropuerto
	int aeropuertoMaxX = FLOOR / 3;   // Coord. X máxima del aeropuerto
	int aeropuertoMinZ = -FLOOR / 3;  // Coord. Z mínima del aeropuerto
	int aeropuertoMaxZ = FLOOR / 3;  // Coord. Z máxima del aeropuerto
	int pistaCentralMinX = -8;  // Coord. X mínima de la pista central
	int pistaCentralMaxX = 8;   // Coord. X máxima de la pista central
	int pistaIzquierdaX = 2;  // Coordenada X de la pista izquierda
	int pistaDerechaX = 6;   // Coordenada X de la pista derecha

void dibujarSuelo(glm::mat4* transform, unsigned int transformLoc) {

	// Dibujamos en modo GL_FILL (líneas con relleno)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// Dibujamos los cuadrados que forman el suelo
	for (int i = -FLOOR; i <= FLOOR; i += FLOOR_SCALE) {
		for (int j = -FLOOR * 4; j < FLOOR * 4; j += FLOOR_SCALE) {
			if (abs(avion.p.x - i) < RENDER_DISTANCE && abs(avion.p.z - j) < RENDER_DISTANCE) {
				// Si estamos dentro de los límites del aeropuerto, lo dibujamos
				if ((i >= aeropuertoMinX && i <= aeropuertoMaxX) && (j >= aeropuertoMinZ && j <= aeropuertoMaxZ)){
					// Dibujamos la pista central
					if ((i >= pistaCentralMinX && i <= pistaCentralMaxX)) {
						glActiveTexture(pistaCentral);
						glBindTexture(GL_TEXTURE_2D, pistaCentral);
					}
					/// Dibujamos la pista izquierda
					/*else if (i == pistaIzquierdaX) {
						glActiveTexture(pistaIzquierda);
						glBindTexture(GL_TEXTURE_2D, pistaIzquierda);
					}
					// Dibujamos la pista derecha
					else if (i == pistaDerechaX) {
						glActiveTexture(pistaDerecha);
						glBindTexture(GL_TEXTURE_2D, pistaDerecha);
					}*/
					// Dibujamos el pavimento del aeropuerto
					else {
						glActiveTexture(pavimento);
						glBindTexture(GL_TEXTURE_2D, pavimento);
					}
				}
				// Si estamos fuera de los límites del aeropuerto, dibujamos césped
				else {
					glActiveTexture(cesped);
					glBindTexture(GL_TEXTURE_2D, cesped);
				}
				// Inicializamos la matriz de transformaciones para el cubo actual
				*transform = glm::mat4();
				// Aplicamos una traslación
				*transform = glm::translate(*transform, glm::vec3(i, -0.5f, j));
				// Aplicamos una escala al cubo
				*transform = glm::scale(*transform, glm::vec3(FLOOR_SCALE, FLOOR_SCALE, FLOOR_SCALE));
				// Enviamos la matriz de transformaciones al shader
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
				// Enlazamos el VAO del suelo
				glBindVertexArray(VAOCuadrado);
				// Dibujamos el suelo con GL_LINES
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}
	}
	// Desenlazamos el VAO
	glBindVertexArray(0);
}



//	Función para dibujar los elementos de la grúa
void dibujarElementoAvion(objeto obj, glm::mat4* transform, unsigned int transformLoc, glm::mat4* stack) {

	//	Activamos y enlazamos la textura de las articulaciones
	glActiveTexture(textura_avion);
	glBindTexture(GL_TEXTURE_2D, textura_avion);
	glDisable(GL_CULL_FACE);

	// Obtenemos la matriz de transformaciones actual del stack y la actualizamos con la posición y la rotación de los brazos
	*transform = *stack;
	

   	*transform = glm::translate(*transform, glm::vec3(obj.p.x, obj.p.y, obj.p.z));

	//(*transform) *= glm::lookAt(glm::vec3(0.0f), obj.direccion * glm::vec3(1, 1, -1), glm::cross(glm::vec3(-avion.direccion.z, 0, avion.direccion.x),avion.direccion));
	//printf("Direccion: %f %f %f\n", obj.direccion.x, obj.direccion.y, obj.direccion.z);

	glm::vec3 avionUp = glm::cross(avion.direccion, normalize(glm::cross(glm::vec3(0, 1, 0), avion.direccion)));
	glm::vec3 mundoUp = glm::vec3(0, 1, 0);

	*transform = glm::rotate(*transform, (float)(obj.ang_alabeo * GRADOSTORADIANES), glm::normalize(obj.direccion));

	if (avion.p.y > 0.2) *transform = glm::translate(*transform, glm::vec3(0, -1.5 * (50 - abs(ang_flaps_guiñada))/50, 0));
	else *transform = glm::translate(*transform, glm::vec3(0, -1.5, 0));

    *transform = glm::rotate(*transform, atan2(obj.direccion.x, obj.direccion.z), mundoUp);
	*transform = glm::rotate(*transform, -asin(obj.direccion.y), glm::vec3(1, 0, 0));

	
	

	// Actualizamos el stack con la nueva matriz de transformaciones y aplicamos un escalado al elemento del brazo
	*stack = *transform;
	*transform = glm::scale(*transform, obj.escalado);

	// Enviamos la matriz de transformaciones al shader y dibujamos el objeto
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	glBindVertexArray(obj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, obj.verts);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Desenlazamos el VAO
	glBindVertexArray(0);
}

void dibujarPuntero(glm::mat4* transform, unsigned int transformLoc){
	// Dibujamos en modo GL_FILL (líneas con relleno)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// Dibujamos los cuadrados que forman el suelo
		glActiveTexture(rojo);
		glBindTexture(GL_TEXTURE_2D, rojo);


				*transform = glm::mat4();
				// Aplicamos una traslación
				*transform = glm::translate(*transform, avion.p + glm::normalize(avion.direccion) * glm::vec3(DISTANCIAAPUNTADO));
				// Aplicamos una escala al cubo
				*transform = glm::scale(*transform, glm::vec3(0.45, 0.45, 0.45));
				// Enviamos la matriz de transformaciones al shader
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
				// Enlazamos el VAO del suelo
				glBindVertexArray(VAOCubo);
				// Dibujamos el suelo con GL_LINES
				glDrawArrays(GL_TRIANGLES, 0, 36);
			
	
	// Desenlazamos el VAO
	glBindVertexArray(0);
}


//	Función para dibujar el almacén
void dibujarAlmacen(Almacen obj, glm::mat4* transform, unsigned int transformLoc) {
	
    // Comprobar si el almacén está dentro de la distancia de render respecto a la posición del avión
    glm::vec3 almacenPos = glm::vec3(almacen.px, almacen.py, almacen.pz); // Obtener la posición del almacén

    float distance = glm::distance(avion.p, almacenPos); // Calcular la distancia entre el avión y el almacén

    if (distance > RENDER_DISTANCE) 
		return;

	glActiveTexture(tex_almacen);
	glBindTexture(GL_TEXTURE_2D, tex_almacen);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	// Dibujamos en modo GL_FILL (relleno)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Inicializamos la matriz de transformaciones para el cubo actual
	*transform = glm::mat4();

	// Aplicamos una traslación a la posición del cubo
	*transform = glm::translate(*transform, glm::vec3(obj.px, obj.py, obj.pz));
	*transform = glm::scale(*transform, glm::vec3(obj.esx, obj.esy, obj.esz));

	// Enviamos la matriz de transformaciones al shader y dibujamos el objeto
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	glBindVertexArray(obj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, obj.verts);

	// Desenlazamos el VAO
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Desenlazamos el VAO
	glBindVertexArray(0);
}

void dibujarEdificio(Almacen obj, glm::mat4* transform, unsigned int transformLoc) {

	// Comprobar si el almacén está dentro de la distancia de render respecto a la posición del avión
	glm::vec3 almacenPos = glm::vec3(almacen.px, almacen.py, almacen.pz); // Obtener la posición del almacén

	float distance = glm::distance(avion.p, almacenPos); // Calcular la distancia entre el avión y el almacén

	if (distance > RENDER_DISTANCE)
		return;

	glActiveTexture(tex_edificio);
	glBindTexture(GL_TEXTURE_2D, tex_edificio);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	// Dibujamos en modo GL_FILL (relleno)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Inicializamos la matriz de transformaciones para el cubo actual
	*transform = glm::mat4();

	// Aplicamos una traslación a la posición del cubo
	*transform = glm::translate(*transform, glm::vec3(obj.px, obj.py, obj.pz));
	*transform = glm::scale(*transform, glm::vec3(obj.esx, obj.esy, obj.esz));

	// Enviamos la matriz de transformaciones al shader y dibujamos el objeto
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	glBindVertexArray(obj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, obj.verts);

	// Desenlazamos el VAO
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Desenlazamos el VAO
	glBindVertexArray(0);
}

//	Función que realiza el cálculo del punto donde se situa la luz
void calculoPLuz(glm::mat4 transform) {
	const float* pSource = (const float*)glm::value_ptr(transform);

	for (int i = 0; i < 16; ++i) {
		dArray[i] = pSource[i];
	}

	pluz.px = (float)dArray[12];
	pluz.py = (float)dArray[13];
	pluz.pz = (float)dArray[14];
}

//	Función de modificación de parametros del shader para la iluminación
void myIluminacion() {

	// COLOR DEL OBJETO
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLoc, 0.0, 1.0f, 1.0f);

	// Color de la luz
	unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightLoc, 0.5f, 0.5f, 0.5f);

	// Posición de la luminación
	unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3f(lightPosLoc, (float)pluz.px, (float)pluz.py + 3.4f, (float)pluz.pz);
	// Direccion de la luminación
	unsigned int lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
	glUniform3f(lightDirLoc, -avion.direccion.x, -avion.direccion.y, -avion.direccion.z);

	// Luz de ambiente
	// Obtener la ubicación de la variable uniforme en el shader
	GLuint ambientI = glGetUniformLocation(shaderProgram, "ambientI");

	// Establecer el valor de la luz de ambiente
	glUniform1f(ambientI, ambientIntensity);

}
// Función para inicializar openGL
void openGlInit() {

	// Inicializaciones
	glClearDepth(1.0f); // Establece el valor del z-buffer de limpieza
	glClearColor(0.5f, 0.7f, 1.0f, 1.0f);  // Establece el valor de limpieza del buffer de color
	glEnable(GL_DEPTH_TEST); // Habilita el z-buffer para las pruebas de profundidad
	glDisable(GL_CULL_FACE); // Habilita la ocultación de caras back
	//glCullFace(GL_BACK); // Especifica que OpenGL debe cullface en la parte posterior del polígono
	//const char* origenFuente = "ITC Avant Garde Gothic LT Condensed Book.ttf";
	//load_font(const_cast<char*>(origenFuente), 8);

}

// Funcion de lo que se dibuja por pantalla
void Display() {

	glm::mat4 transform = glm::mat4(); ; // Matriz de transformacion
	glm::mat4 stack = glm::mat4(1.0f); ; // Stack

	// Obtenemos la ubicación de la variable "transform" en el shader
	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
	// Obtenemos la ubicación de la variable "Colors" en el shader
	GLuint colorLoc = glGetUniformLocation(shaderProgram, "Colors");

	// Renderizado
	//glClearColor(0.15f, 0.15f, 0.3f, 1.0f);// Establece el color de borrado (fondo)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Dibujo del fondo

	
	// Cambiamos la vista de la cámara según el valor de la variable "camera"
	if (camera == 1) {
		myPrimeraPersona(avion.p.x, avion.p.y, avion.p.z,0 /*avion.ang_guiñada*/);
	}
	else if (camera == 2) {
		myCamaraExterior();
	}
	else if (camera == 3) {
		myTerceraPersona(avion.p.x, avion.p.y, avion.p.z, avion.direccion);
	}

	// Llamamos a la función de movimiento de la grúa
	movimiento();

	// Dibujamos el suelo
	dibujarSuelo(&transform, transformLoc);

	dibujarElementoAvion(avion, &transform, transformLoc, &stack);

	dibujarAlmacen(almacen, &transform, transformLoc);
	dibujarAlmacen(almacen2, &transform, transformLoc);
	dibujarAlmacen(almacen3, &transform, transformLoc);
	dibujarAlmacen(almacen4, &transform, transformLoc);
	dibujarPuntero(&transform, transformLoc);

	dibujarEdificio(edificio1, &transform, transformLoc);
	dibujarEdificio(edificio2, &transform, transformLoc);
	dibujarEdificio(edificio3, &transform, transformLoc);
	dibujarEdificio(edificio4, &transform, transformLoc);


	
	dibujarBalas(&transform, transformLoc);

	calculoPLuz(transform);
	myIluminacion();

}

// Funcion principal
int main()
{
	// Inicializamos GLFW
	glfwInit();

	// Establecemos las versiones de OpenGL que usaremos
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creamos la ventana con las especificaciones dadas
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Aviator.cg", NULL, NULL);

	// Verificamos si se pudo crear la ventana
	if (window == NULL)
	{
		printf("Eror al crear la ventana");
		glfwTerminate();
		return -1;
	}

	// Establecemos la ventana como la ventana actual de contexto
	glfwMakeContextCurrent(window);

	// Registra la función 'resize' como una devolución de llamada de redimensionamiento de ventana
	glfwSetFramebufferSizeCallback(window, resize);

	// Inicializamos Glad para cargar las extensiones de OpenGL
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		printf("Error con Glad");
		return -1;
	}

	// Configuramos el callback para manejar las teclas
	glfwSetKeyCallback(window, keyCallback);

	// Inicializamos OpenGL
	openGlInit();

	glm::mat4 transform = glm::mat4(1.0f); // Creamos una matriz de transformación 
	GLint trans1Loc = glGetUniformLocation(shaderProgram, "transform"); // Obtenemos la ubicación de la matriz de transformación en el shader

	// Seleccionamos el shader para su uso en el renderizado
	glUseProgram(shaderProgram);
	// Enviamos la matriz de transformación al shader
	glUniformMatrix4fv(trans1Loc, 1, GL_FALSE, glm::value_ptr(transform));
	// Cargamos los shaders 
	shaderProgram = setShaders("camera.vert", "camera.frag");

	unsigned int VBO;

	// Read our .obj file
	bool res = loadOBJ("resources//objects//avionT.obj", vertices, uvs, normals);
	avion.verts = vertices.size();

	almacen.VAO = VAOCubo;

	// Dibujamos los objetos
	dibujarCuadrado();
	dibujarAvion();
	dibujarThrust();
	dibujarCubo();
	almacen.VAO = VAOCubo;
	almacen2.VAO = VAOCubo;
	almacen3.VAO = VAOCubo;
	almacen4.VAO = VAOCubo;

	edificio1.VAO = VAOCubo;
	edificio2.VAO = VAOCubo;
	edificio3.VAO = VAOCubo;
	edificio4.VAO = VAOCubo;

	//	Cargamos texturas
	//	Cesped
	pavimento = cargarTextura("resources//textures//pavimento.jpg");

	//	Tierra
	cesped = cargarTextura("resources//textures//cesped.jpg");

	//	Negro
	tex_grua_esfera = cargarTextura("resources//textures//negro_metal.jpg");

	// Thrust
	tex_thrust = cargarTextura("resources//textures//thrust.jpg");

	//	Pista de despegue del avión
	pistaCentral = cargarTextura("resources//textures//pista.jpeg");
	pistaDerecha = cargarTextura("resources//textures//pistaDerecha.jpeg");
	pistaIzquierda = cargarTextura("resources//textures//pistaIzquierda.jpeg");
	textura_avion = cargarTextura("resources//textures//fuselajeR.jpg");
	tex_almacen = cargarTextura("resources//textures//almacen.jpg");
	rojo = cargarTextura("resources//textures//rojo.jpg");
	tex_bala = cargarTextura("resources//textures//disparo.png");
	tex_edificio = cargarTextura("resources//textures//ladrillo.jpg");

	// Grúa
	tex_grua = cargarTextura("resources//textures//amarillo_metal.jpg");


	// Se vincula el shader para su uso en el renderizado
	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

	// Lazo principal
	while (!glfwWindowShouldClose(window)) { // Hasta el cierre de la ventana

		// Renderizamos la escena
		Display();
		// Intercambiamos los buffers de la ventana para mostrar la escena
		glfwSwapBuffers(window);
		// Procesamos los eventos de ventana
		glfwPollEvents();
	}

	// Borramos el VAO
	glDeleteVertexArrays(1, &VAO);
	// Terminamos GLFW
	glfwTerminate();
}
