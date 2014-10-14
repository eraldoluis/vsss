//[]-----------------------------------------------[]
//| 	Very Small Size Soccer software				|
//|		Version 2014.0                              |
//|		Laboratório de Inteligencia Artificial		|
//|		Universidade Federal de Mato Grosso do Sul	|
//|													|
//|		Autores:	Eduardo Zárate Guerreiro Max,	|
//|					Paulo Eduardo Suriano Tiene,	|
//|					Adolfo Marangoni Hengling,		|
//|					Pedro Augusto O. Silva,			|
//|					Luiz Henrique Claudino Silva	|
//[]-----------------------------------------------[]

#ifndef __Funcoes_cpp
#define __Funcoes_cpp

#include <cstdio>
#include <opencv2/opencv.hpp>

#include "Cor.hpp"

/* Le os valores do arquivo hsv.aff e cria um objeto para cada cor */
Cor* guardaValoresHSV(FILE* arquivo, Cor* cores);

CvPoint gira(CvPoint entrada, CvPoint meio, double angulo);

bool pixelReal(IplImage* color, int valueCor, int x, int y, Cor* cores);

inline
float tamanhoVetor(CvPoint2D32f vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y);
}

float produtoEscalar(CvPoint direcao, CvPoint centro, CvPoint objetivo);

#endif //__Funcoes_cpp
