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

#ifndef __Cor_cpp
#define __Cor_cpp

class Cor {
public:
	int HSVMin[3];
	int HSVMax[3];
	int idCor;
	Cor* prox;

	/* Construtor */
	Cor() {
	}

	/* Cria o objeto cor */
	Cor(int HSVMin[3], int HSVMax[3], int idCor) {
		for (int i = 0; i < 3; i++) {
			this->HSVMin[i] = HSVMin[i];
			this->HSVMax[i] = HSVMax[i];
		}
		this->idCor = idCor;
		this->prox = NULL;
	}

	/* Adiciona a cor  */
	void adicionaCor(int HSVMin[3], int HSVMax[3], int idCor) {
		Cor* novaCor = new Cor(HSVMin, HSVMax, idCor);
		Cor* aux;
		aux = this;
		while (aux->prox != NULL)
			aux = aux->prox;
		aux->prox = novaCor;
		novaCor->prox = NULL;
	}

	/* 	Verifica se a cor do pixel atual corresponde a alguma das cores mapeadas
	 Caso seja, retorna o id respectivo a desta cor
	 */
	int verificaHSVCor(int h, int s, int v) {
		Cor* aux;
		for (aux = this; aux != NULL; aux = aux->prox)
			if (h >= aux->HSVMin[0] && s >= aux->HSVMin[1] && v
					>= aux->HSVMin[2])
				if (h <= aux->HSVMax[0] && s <= aux->HSVMax[1] && v
						<= aux->HSVMax[2])
					return aux->idCor;
		return -1;
	}

	/* 	Verifica se a cor do pixel atual corresponde a alguma das cores mapeadas
	 Verifica também se o id da cor é o mesmo, e responde se os valores correspondem
	 */
	bool verificaCor(int h, int s, int v, int idCor) {
		Cor* aux;
		for (aux = this; aux != NULL; aux = aux->prox)
			if (h >= aux->HSVMin[0] && s >= aux->HSVMin[1] && v
					>= aux->HSVMin[2])
				if (h <= aux->HSVMax[0] && s <= aux->HSVMax[1] && v
						<= aux->HSVMax[2])
					if (aux->idCor == idCor)
						return true;
		return false;
	}

	/* Destrutor */
	~Cor() {
		free(this->prox);
		delete this;
	}
};

#endif //__cor_cppF
