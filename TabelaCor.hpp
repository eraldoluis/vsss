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

#ifndef __TabelaCor_cpp
#define __TabelaCor_cpp

class TabelaCor {
public:
	CentroCor* tabelaCores[NUM_CORES];

	TabelaCor() {
		for (int i = 0; i < NUM_CORES; i++)
			this->tabelaCores[i] = NULL;
	}

	void adicinaTabelaCor(CvPoint novo, int cor) {
		CentroCor* aux = this->tabelaCores[cor];
		if (aux == NULL) {
			aux = new CentroCor(novo, cor);
			this->tabelaCores[cor] = aux;
		} else
			aux->adicionaNovoCentroCor(novo.x, novo.y, cor);
	}

	bool verificaTabelaCor(int x, int y) {
		for (int i = 0; i < NUM_CORES; i++)
			if (this->tabelaCores[i] != NULL)
				if (this->tabelaCores[i]->verificaCentroCor(x, y) == false)
					return false;
		return true;
	}

	bool existeTabelaCor() {
		for (int i = 0; i < NUM_CORES; i++)
			if (this->tabelaCores[i] != NULL)
				return true;
		return false;
	}

	void limpaTabelaCor() {
		for (int i = 0; i < NUM_CORES; i++) {
			free(this->tabelaCores[i]);
			this->tabelaCores[i] = NULL;
		}
	}

};

#endif //__tabela_cor_cpp
