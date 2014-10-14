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

#ifndef __CentroCor_cpp
#define __CentroCor_cpp

#include <opencv2/opencv.hpp>

#include "Configuracoes.hpp"

class CentroCor {
public:
	CentroCor* prox;
	CvPoint ponto;
	int quantidade;
	int idCor;

	/* Construtor */
	CentroCor();

	CentroCor(CvPoint novo, int idCor);

	CentroCor(int x, int y, int idCor);

	void adicionaNovoCentroCor(int x, int y, int idCor);

	bool verificaCentroCor(int x, int y);

	/* Destrutor */
	~CentroCor();
};

#endif //__CentroCor_cpp
