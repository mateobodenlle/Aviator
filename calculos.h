#pragma once
#include <math.h>
//Librer�a para c�lculos internos de avi�n, fundamentalmente interpolaciones o aprimaciones de funciones m�s complejas

float sinMacl(float ar) {
	//Calcula el seno de un n�mero a en radianes
	//Usa la serie de Maclaurin 
	//Se puede mejorar con la serie de Taylor
	float res = 0;
	for (int i = 0; i < 3; i++) {
		res += pow(-1, i) * pow(ar, 2 * i + 1) / tgamma(2 * i + 1+1); 
	}
	return res;	
}

float cosMacl(float ar) {
	//Calcula el coseno de un n�mero a en radianes
	//Usa la serie de Maclaurin
	//Se puede mejorar con la serie de Taylor
	float res = 0;
	for (int i = 0; i < 3; i++) {
		res += pow(-1, i) * pow(ar, 2 * i) / tgamma(2 * i + 1);
	}
	return res;
}