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

#ifndef __Caminho_h
#define __Caminho_h

class Caminho {
private:
	CvPoint from;
	CvPoint to;

public:

	/* Construtor */
	Caminho() {
	}

	Caminho(CvPoint from, CvPoint to) {
		this->from = from;
		this->to = to;
	}

	float getDistanciaCaminho() {
		CvPoint2D32f auxDistancia;

		auxDistancia.x = from.x - to.x;
		auxDistancia.y = from.y - to.y;

		return tamanhoVetor(auxDistancia);
	}

	list<CvPoint>* getCaminho() {
		list<CvPoint>* caminho = new list<CvPoint> ();
		while (!(this->getDistanciaCaminho() < 40.0f)) {
			CvPoint novoPonto = this->getProximoPontoCaminho();
			caminho->push_back(novoPonto);
			this->from = novoPonto;
		}
		return caminho;
	}

	CvPoint getProximoPontoCaminho() {
		float menorDistancia, distancia;
		CvPoint novoEstado;

		for (int i = 0; i < 70; i++) {
			if (this->getDistanciaCaminho() < 40.0f)
				break;

			menorDistancia = abs(from.x - to.x) + abs((from.y + 1) - to.y);
			novoEstado.x = from.x;
			novoEstado.y = from.y + 1;

			distancia = abs((from.x + 1) - to.x) + abs((from.y + 1) - to.y);
			if (distancia < menorDistancia) {
				atualizaEstadoCaminho(&novoEstado, from.x + 1, from.y + 1);
				menorDistancia = distancia;
			}

			distancia = abs((from.x - 1) - to.x) + abs((from.y + 1) - to.y);
			if (distancia < menorDistancia) {
				atualizaEstadoCaminho(&novoEstado, from.x - 1, from.y + 1);
				menorDistancia = distancia;
			}

			distancia = abs((from.x - 1) - to.x) + abs(from.y - to.y);
			if (distancia < menorDistancia) {
				atualizaEstadoCaminho(&novoEstado, from.x - 1, from.y);
				menorDistancia = distancia;
			}

			distancia = abs((from.x + 1) - to.x) + abs(from.y - to.y);
			if (distancia < menorDistancia) {
				atualizaEstadoCaminho(&novoEstado, from.x + 1, from.y);
				menorDistancia = distancia;
			}

			distancia = abs(from.x - to.x) + abs((from.y - 1) - to.y);
			if (distancia < menorDistancia) {
				atualizaEstadoCaminho(&novoEstado, from.x, from.y - 1);
				menorDistancia = distancia;
			}

			distancia = abs((from.x + 1) - to.x) + abs((from.y - 1) - to.y);
			if (distancia < menorDistancia) {
				atualizaEstadoCaminho(&novoEstado, from.x + 1, from.y - 1);
				menorDistancia = distancia;
			}

			distancia = abs((from.x - 1) - to.x) + abs((from.y - 1) - to.y);
			if (distancia < menorDistancia) {
				atualizaEstadoCaminho(&novoEstado, from.x - 1, from.y - 1);
				menorDistancia = distancia;
			}
			this->from = novoEstado;
		}
		return novoEstado;
	}

	void atualizaEstadoCaminho(CvPoint* novoEstado, int x, int y) {
		novoEstado->x = x;
		novoEstado->y = y;
	}

};

#endif
