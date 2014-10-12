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

#ifndef __PontosCampo_cpp
#define __PontosCampo_cpp

#include <cstdio>

class PontosCampo {
public:
	PontosCampo* prox;
	CvPoint ponto;

	/* Construtor */
	PontosCampo() {
	}

	PontosCampo(CvPoint novo) {
		this->ponto.x = novo.x;
		this->ponto.y = novo.y;
		this->prox = NULL;
	}

	PontosCampo(int x, int y) {
		this->ponto.x = x;
		this->ponto.y = y;
		this->prox = NULL;
	}

	void substituiPontoPontosCampo(int x, int y) {
		this->ponto.x = x;
		this->ponto.y = y;
		this->prox = NULL;
	}

	void adicionaPontoPontosCampo(int x, int y) {
		PontosCampo* novocentro = new PontosCampo(x, y);
		PontosCampo* aux;
		aux = this;

		while (aux->prox != NULL)
			aux = aux->prox;

		aux->prox = novocentro;
		novocentro->prox = NULL;

	}

	void guardaPontosCampo(FILE* pontosCampo) {
		int cont = 0;
		int pontoX, pontoY;

		for (int i = 0; i < 24 && cont < 12; i++, cont++) {
			fscanf(pontosCampo, "%d", &pontoX);
			fscanf(pontosCampo, "%d", &pontoY);
			if (i == 8 || i == 11)
				pontoX += 5;
			if (i == 2 || i == 5)
				pontoX -= 5;

			if (i == 0)
				this->substituiPontoPontosCampo(pontoX, pontoY);
			else
				this->adicionaPontoPontosCampo(pontoX, pontoY);
		}
	}

	CvPoint devolvePontoPontosCampo(int num) {
		int cont = 0;
		PontosCampo* aux;
		aux = this;

		while (aux != NULL && cont < num) {
			aux = aux->prox;
			cont++;
		}
		return aux->ponto;
	}

	/* Destrutor */
	~PontosCampo() {
		free(this->prox);
		delete this;
	}
};

#endif //__centroList_cpp
