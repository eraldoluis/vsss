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

class CentroCor {
public:
	CentroCor* prox;
	CvPoint ponto;
	int quantidade;
	int idCor;

	/* Construtor */
	CentroCor() {
	}

	CentroCor(CvPoint novo, int idCor) {
		this->ponto.x = novo.x;
		this->ponto.y = novo.y;
		this->quantidade = 1;
		this->prox = NULL;
		this->idCor = idCor;
	}

	CentroCor(int x, int y, int idCor) {
		this->ponto.x = x;
		this->ponto.y = y;
		this->quantidade = 1;
		this->prox = NULL;
		this->idCor = idCor;
	}

	void adicionaNovoCentroCor(int x, int y, int idCor) {
		CentroCor* novoPonto = new CentroCor(x, y, idCor);
		CentroCor* aux;
		aux = this;

		while (aux->prox != NULL)
			aux = aux->prox;

		aux->prox = novoPonto;
		novoPonto->prox = NULL;

		this->quantidade++;
	}

	bool verificaCentroCor(int x, int y) {
		CentroCor* aux;
		aux = this;

		while (aux != NULL) {
			if (((x - aux->ponto.x) * (x - aux->ponto.x) + (y - aux->ponto.y)
					* (y - aux->ponto.y) <= RAIO_DISTANCIA))
				return false;
			aux = aux->prox;
		}
		return true;
	}

	/* Destrutor */
	~CentroCor() {
		free(this->prox);
		delete this;
	}
};

#endif //__CentroCor_cpp
