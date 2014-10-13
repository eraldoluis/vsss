#include "CentroCor.hpp"

/* Construtor */
CentroCor::CentroCor() {
	prox = NULL;
}

CentroCor::CentroCor(CvPoint novo, int idCor) {
	this->ponto.x = novo.x;
	this->ponto.y = novo.y;
	this->quantidade = 1;
	this->prox = NULL;
	this->idCor = idCor;
}

CentroCor::CentroCor(int x, int y, int idCor) {
	this->ponto.x = x;
	this->ponto.y = y;
	this->quantidade = 1;
	this->prox = NULL;
	this->idCor = idCor;
}

void CentroCor::adicionaNovoCentroCor(int x, int y, int idCor) {
	CentroCor* novoPonto = new CentroCor(x, y, idCor);
	CentroCor* aux;
	aux = this;

	while (aux->prox != NULL)
		aux = aux->prox;

	aux->prox = novoPonto;
	novoPonto->prox = NULL;

	this->quantidade++;
}

bool CentroCor::verificaCentroCor(int x, int y) {
	CentroCor* aux;
	aux = this;

	while (aux != NULL) {
		if (((x - aux->ponto.x) * (x - aux->ponto.x)
				+ (y - aux->ponto.y) * (y - aux->ponto.y) <= RAIO_DISTANCIA))
			return false;
		aux = aux->prox;
	}
	return true;
}

/* Destrutor */
CentroCor::~CentroCor() {
	delete this->prox;
}
