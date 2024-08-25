#pragma once
/*
@author: Mateo Bodenlle Villarino
@author: Sara Castro López
*/

//#include <glfw3.h> 
#include <iostream>
#include <fstream>  // Para std::ifstream
#include <sstream>  // Para std::istringstream
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "lecturaShader_0_9.h"
#include "esfera.h"
#include "calculos.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h" 

//librerías de texto en pantalla
/*#include <ft2build.h>
#include FT_FREETYPE_H*/

// CONSTANTES DEFINIDAS PARA EL PROGRAMA
#define FLOOR 1024
#define FLOOR_SCALE 8
#define GRADOSTORADIANES 0.0174532925 //	Constante de transformacion de grados a radianes
#define DISTANCIAAPUNTADO 150.0f		//Distancia a la que apuntan las armas, en la que se sitúa el puntero (giran las armas)

//Constantes físicas
#define GRAVITY -9.81/2.0f // Gravedad

#define friccion_suelo    2.0f // Frenado del suelo
#define friccion_aire    0.3f // Frenado del aire

#define resistencia_cabeceo 2.0f
#define resistencia_alabeo  1.0f
#define resistencia_guiñada 1.0f


// VARIABLES GLOBALES PARA PROGRAMAR EL FUNCIONAMIENTO DEL AVIÓN
float ang_flaps_guiñada;	// "intención" de giro de guiñado del avión
float ang_flaps_cabeceo;	// "intención" de giro de cabeceo del avión
float ang_flaps_alabeo;		// "intención" de giro de alabeo del avión
float potenciaMotor = 0;			// 0-100: aceleración del avión, en términos de potencia del motor, no es una aceleración cte...
bool encendido = false;				// Estado del motor del avión



// ESTRUCTURA PARA ALMACENAR LA INFORMACIÓN DEL MATERIAL MTL
struct Material {
	std::string name;
	std::string textureMap;
};

// Material por defecto del avion que está en el archivo mtl
Material defaultMaterial = { "Black", "Aircraft_Texture.png" };

// Estructura de un objeto
typedef struct {
	//float ang_guiñada;	    // Guiñada - Giro sobre el eje y
	//float ang_cabeceo;		// Cabeceo - Ángulo de giro eje x
	float ang_alabeo;		// Alabeo - Ángulo de giro eje z
	glm::vec3 direccion;		// Posición del objeto
	glm::vec3 escalado;	// Escalado en los tres ejes
	unsigned int VAO;		// Lista de render
	GLsizei verts;			// Número de vértices
	glm::vec3 p, v, a, f; //Posición, velocidad, aceleración y fuerzas en los 3 ejes 
	float masa;				// Masa del objeto
	Material mtl;			// Material del objeto sacada del archivo mtl
}objeto;

// Iluminación
typedef struct {
	float px, py, pz;
} puesto;

puesto pview = { 0.0f , 0.0f , 0.0f };
puesto pluz = { 0.0f , 1.0f , 0.0f };

//	Array para cálculo de posición de la luz
double dArray[16] = { 0.0f };

objeto avion = {
	0.0f,
	{0.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f},
	0, 39,
	glm::vec3{4.0f, 0.0f, 0.0f},
	glm::vec3{0.0f},
	glm::vec3{0.0f},
	glm::vec3{0.0f},
	100, defaultMaterial}; 

// baseA1 = esfera
/*objeto baseA1 = {0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.35f, 0.35f, 0.35f, 0, 1080};
// base1 = prisma rectangular = brazo inferior
objeto base1 = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.25f, 1.5f, 0.25f, 0, 36 };
// baseA2 = esfera 
objeto baseA2 = { 0.0f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f, 0, 1080 };
// base2 = prisma rectangular = brazo superior
objeto base2 = { 0.0f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.15f, 1.5f, 0.15f, 0, 36 };*/

//objeto avion = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.5f, 0, 36, 0 }; //todo modificar

bool loadOBJ(const char* path, std::vector < glm::vec3 >& out_vertices, std::vector < glm::vec2 >& out_uvs, std::vector < glm::vec3 >& out_normals) {
	
	printf("Cargando objeto");

	// Variables temporales en las que almacenaremos los contenidos del .obj 
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	// Abrimos el archivo
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	// Leemos el archivo y analizamos cada linea
	while (1) {
		char lineHeader[200];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		else {
			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				// Leemos toda la línea como una cadena
				char line[256]; // Tamaño suficiente para la línea
				fgets(line, sizeof(line), file);

				// Dividimos la línea en tokens usando strtok
				char* token = strtok(line, " ");

				// Variables temporales para almacenar los índices
				unsigned int vertexIndex, uvIndex, normalIndex;

				// Leemos los índices de vértice, textura y normal de cada vértice en la cara
				while (token != NULL) {
					// Saltamos el primer token, que es "f"
					if (strcmp(token, "f") != 0) {
						// Leemos los índices de vértice, textura y normal
						sscanf(token, "%d/%d/%d", &vertexIndex, &uvIndex, &normalIndex);

						// Añadimos los índices al vector correspondiente
						vertexIndices.push_back(vertexIndex);
						uvIndices.push_back(uvIndex);
						normalIndices.push_back(normalIndex);
					}
					token = strtok(NULL, " "); // Obtenemos el siguiente token
				}
			}
		}
	}


	// Para cada vértice de cada triángulo
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Vértices
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);

		// Textura
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		out_uvs.push_back(uv);

		// Normales
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[normalIndex - 1];
		out_normals.push_back(normal);

	}
	printf("CARGA DE OBJETO LISTA");
}

float cttGravedad = 0.0012f;
void gravedad() {
	avion.f.y += GRAVITY * cttGravedad * avion.masa;
}

void control_colisiones() {

	//Colisión contra el suelo
	if (avion.p.y < 0.0f) {
		avion.v.y = 0;
		avion.p.y = 0;
		//avion.f.y = 0;
		avion.ang_alabeo = 0;
		avion.direccion *= glm::vec3(1.0, 0.9, 1.0);
	}

}

float coefRozamiento = 0.1f;
void friccion() {
	int valor = ((avion.p.y == 0.0f) ? 1 : 0);
	
	if (glm::vec3(0.0f) != avion.v) {
		avion.f -= friccion_aire * avion.v +
			glm::vec3(avion.v.x, 0, avion.v.z) * (-avion.f.y) * (float)valor * friccion_suelo;
	
		avion.f -= (normalize(avion.v) - normalize(avion.direccion)) * glm::length(avion.v) * 1.0f * friccion_suelo;
	}
}


float cttResolucionGuiñada = 0.1f;
float angulo, seno, coseno;
void resolucionGuiñada() {
	angulo =  ang_flaps_guiñada * GRADOSTORADIANES * cttResolucionGuiñada * dot(avion.v, avion.direccion);
    seno = sin(angulo);
    coseno = cos(angulo);
    avion.direccion.x = avion.direccion.x * coseno - avion.direccion.z * seno;
    avion.direccion.z = avion.direccion.x * seno + avion.direccion.z * coseno;

	if (avion.p.y > 0.25f) avion.ang_alabeo += 0.20 * ang_flaps_guiñada*cos(avion.ang_alabeo * GRADOSTORADIANES) * glm::length(avion.v);
}


/** Cálculos pertinentes para determinar el cabeceo del avión en cada momento

*/
void resolucionCabeceo() {
	if (avion.p.y < 0.2) { if (normalize(avion.direccion).y < 0.2) avion.direccion.y += ang_flaps_cabeceo * GRADOSTORADIANES / resistencia_cabeceo * glm::length(avion.v);}
	//else avion.direccion.y += ang_flaps_cabeceo * GRADOSTORADIANES / resistencia_cabeceo * glm::length(avion.v);
	else avion.direccion.y = sin(asin(avion.direccion.y) + ang_flaps_cabeceo * GRADOSTORADIANES / resistencia_cabeceo * glm::length(avion.v));
}

void resolucionAlabeo() {
	if (avion.p.y > 0.2) avion.ang_alabeo -= 0.3 * ang_flaps_alabeo * glm::length(avion.v);

}

float cttResolucionMotor = 0.015f;
void resolucionMotor() {
	avion.f += potenciaMotor * avion.direccion * cttResolucionMotor;
}

void resolucionAceleracion() {
	avion.a = avion.f / avion.masa;
	if (avion.p.y == 0 && avion.a.y<0) avion.a *= glm::vec3(1, 0, 1);
}

void resolucionVelocidad() {
	avion.v += avion.a;

}

void resolucionPosicion() {
	avion.p += avion.v;
}



/**
Acción de intento de cabeceo del avión, signo puede ser 1 o -1 para indicar arriba o abajo.
Modifica los flaps de cabeceo
*/float cttCabeceo = 0.2f;
void cabeceo(int signo) {
	if (ang_flaps_cabeceo < 8 && ang_flaps_alabeo > -8)
		ang_flaps_cabeceo += signo * cttCabeceo;
}

/**
Acción de intento de guiñada del avión, signo puede ser 1 o -1 para indicar derecha o izquierda.
Modifica los flaps de guiñada
*/float cttGuiñada = 1.0f;
bool guiñando = false;
void guiñada(int signo) {
	guiñando = true;
	if (ang_flaps_guiñada < 35 && ang_flaps_guiñada > -35)
	ang_flaps_guiñada += signo * cttGuiñada;


	//avion.ang_alabeo += 0.5*signo;
}


/**
Acción de intento de alabeo del avión, signo puede ser 1 o -1 para indicar antihorario o antiohorario.
Modifica los flaps de alabeo
*/float cttAlabeo = 1.0;
void alabeo(int signo) {
	if (ang_flaps_alabeo < 35 && ang_flaps_alabeo > -35)
	ang_flaps_alabeo += signo * cttAlabeo;
}

/**
Acción de motor del avión, signo puede ser 1 o -1 para indicar aceleración o desaceleración. (el motor puede acelerar más o menos (ht 0, claro), pero nunca ir hacia atrás ni desacelerar
, solo se frena con el "rozamiento del aire"
Modifica los flaps de alabeo
*/float cttMotor = 1.0f;
void motor(int signo) {
	if (encendido && potenciaMotor < 100 && potenciaMotor > -20)
		potenciaMotor += signo * cttMotor;
}

void alternarMotor(bool estado) {
	encendido = estado;
	if (!estado) potenciaMotor = 0;
}

float cttFrenoSuelo = friccion_suelo * 5;
float cttFrenoAire =  friccion_aire  * 1;
bool frenando = false;
void freno() {
	frenando = true;
}
void resolucionFreno() {
	if (frenando) {
		potenciaMotor = 0;
		if (avion.p.y <= 0.01) {
			printf("Frenando\n");
			avion.f -= (avion.v * cttFrenoSuelo);
		}
		frenando = false;
	}
}
/**
* 
* Devuelve gradualmente los controles del avion (flaps) a su posición neutra
*/
void retornoAvion(){
	ang_flaps_alabeo *= 0.97;
	ang_flaps_cabeceo *= 0.90;
	if (!guiñando) ang_flaps_guiñada *= 0.95;
	else {
		guiñando = false;
		ang_flaps_guiñada *= 0.975;
	}
	ang_flaps_alabeo *= 0.95;
	if (ang_flaps_alabeo < 0.75 && ang_flaps_alabeo>-0.75){
		if (avion.ang_alabeo > 0) {
			while (avion.ang_alabeo > 360) avion.ang_alabeo -= 360;
			if (avion.ang_alabeo > 180) {
				avion.ang_alabeo = 360 - avion.ang_alabeo;
				avion.ang_alabeo *= -1;
			}
		}
	if (avion.ang_alabeo < 0) {
		while (avion.ang_alabeo < -360) avion.ang_alabeo += 360;
		if (avion.ang_alabeo < -180) {
			avion.ang_alabeo = -360 - avion.ang_alabeo;
			avion.ang_alabeo *= -1;
		}
	}

			avion.ang_alabeo *= 0.965;
	}
	if (avion.p.y == 0.00) avion.direccion.y *= 0.975;
	if (avion.p.y == 0.00 && avion.direccion.y<0) avion.direccion.y *= 0.90;
}

void resetFuerzas() {
	avion.f = glm::vec3(0.0f);

}

void movimiento() {
	resetFuerzas();

	gravedad();
	
	resolucionMotor();
	resolucionGuiñada();
	resolucionCabeceo();
	resolucionAlabeo();
	resolucionFreno();

	friccion();

	resolucionAceleracion();
	resolucionVelocidad();
	control_colisiones();
	resolucionPosicion();

	printf("m: %.0f, f: %.2f,%.2f,%.2f, a: %.2f,%.2f,%.2f, v: %.1f,%.1f,%.1f, p: %.0f,%.0f,%.0f, d: %.2f,%.2f,%.2f, afa: %.1f\n",
		    potenciaMotor,avion.f.x, avion.f.y, avion.f.z, avion.a.x, avion.a.y, avion.a.z, avion.v.x, avion.v.y, avion.v.z,avion.p.x, avion.p.y, avion.p.z,
			avion.direccion.x, avion.direccion.y, avion.direccion.z, ang_flaps_guiñada);
	retornoAvion();
}
