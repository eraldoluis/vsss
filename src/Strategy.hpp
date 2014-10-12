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

#ifndef __Strategy_hpp
#define __Strategy_hpp

class Strategy {
private:
	int priority[10];
	robo* team[3];

	//BOLA
	CvPoint bolaAnterior, bolaAtual;
	CvPoint previsaoBola;

	//GOL
	CvPoint centroGol;
	CvPoint centroGolAdversario;
	CvPoint atrasGolAdversario;
	CvPoint atrasGol;

public:
	CvPoint metadeCampoCima;
	CvPoint metadeCampoBaixo;
	CvPoint metadeTudo;
	bool primeiroPonto;
	int flag;

	CvPoint traveEsquerda;
	CvPoint traveDireita;

	CvPoint A1, A2, A3, A4;

	/* Construtor */
	Strategy() {
	}

	Strategy(robo* um, robo* dois, robo* tres) {
		/* coloca a configuração do robo 1 na posição 0 do vetor */
		team[0] = um;
		team[1] = dois;
		team[2] = tres;
		primeiroPonto = false;
		flag = 0;
	}

	void setExtremidades() {
		robo::xInferior = centroGol.x;
		robo::yInferior = metadeCampoBaixo.y;

		robo::xSuperior = centroGolAdversario.x;
		robo::ySuperior = metadeCampoCima.y;
	}

	CvPoint getBolaAnterior() {
		return this->bolaAnterior;
	}

	CvPoint getBolaAtual() {
		return this->bolaAtual;
	}

	CvPoint getPrevisaoBola() {
		return this->previsaoBola;
	}

	/* Os centros dos gols são calculados pela média da distâncias do x e y das traves */
	CvPoint getCentroGol() {
		return this->centroGol;
	}

	CvPoint getCentroGolAdversario() {
		return this->centroGolAdversario;
	}

	CvPoint getAtrasGolAdversario() {
		return this->atrasGolAdversario;
	}

	CvPoint getTraveDireita() {
		return this->traveDireita;
	}

	CvPoint getTraveEsquerda() {
		return this->traveEsquerda;
	}

	robo* getRoboUm() {
		return this->team[0];
	}

	robo* getRoboDois() {
		return this->team[1];
	}

	robo* getRoboTres() {
		return this->team[2];
	}

	void setCentroGolAdversario(CvPoint ponto) {
		this->centroGolAdversario = ponto;
	}

	void setCentroGol(CvPoint ponto) {
		this->centroGol = ponto;
	}

	void setAtrasGolAdversario(CvPoint ponto) {
		this->atrasGolAdversario = ponto;
	}

	void setBolaAnterior(CvPoint ponto) {
		this->bolaAnterior = ponto;
	}

	void setBolaAtual(CvPoint ponto) {
		this->bolaAtual = ponto;
	}

	void setPrevisaoBola(CvPoint ponto) {
		this->previsaoBola = ponto;
	}

	void setTraveDireita(CvPoint ponto) {
		this->traveDireita = ponto;
	}

	void setTraveEsquerda(CvPoint ponto) {
		this->traveEsquerda = ponto;
	}

	/* Calcula ponto atras do gol */
	void calculaPontoAtrasGol() {
		/* if e else para parametrizar o fundo do gol para o primeiro e segundo tempo */
		if (centroGolAdversario.x > metadeTudo.x) {
			atrasGolAdversario.x = centroGolAdversario.x + 40;
			atrasGol.x = centroGol.x - 40;
		} else {
			atrasGolAdversario.x = centroGolAdversario.x - 40;
			atrasGol.x = centroGol.x + 40;
		}
		atrasGolAdversario.y = centroGolAdversario.y;
		atrasGol.y = centroGol.y;
	}

	/* Calcula o cosseno do angulo dos vetores direção e objetivo por meio do produto escalar */
	float produtoEscalar2(CvPoint direcao, CvPoint centro, CvPoint objetivo) {
		CvPoint2D32f vetordirecao;
		CvPoint2D32f vetorobjetivo;
		float tam;

		/* tamanho do vetor direção do robo na direção x e y */
		vetordirecao.x = centro.x - direcao.x;
		vetordirecao.y = centro.y - direcao.y;

		/* módulo do vetor direção */
		tam = tamanhoVetor(vetordirecao);

		/* o vetor direção recebe seu vetor unitário */
		vetordirecao.x = vetordirecao.x / tam;
		vetordirecao.y = vetordirecao.y / tam;

		/* o vetor objetivo recebe seu vetor unitário */
		vetorobjetivo.x = centro.x - objetivo.x;
		vetorobjetivo.y = centro.y - objetivo.y;

		tam = tamanhoVetor(vetorobjetivo);

		vetorobjetivo.x = vetorobjetivo.x / tam;
		vetorobjetivo.y = vetorobjetivo.y / tam;

		/* faz o produto escalar entre os dois vetores e obtém-se o coseno do angulo entre os vetores */
		return vetordirecao.x * vetorobjetivo.x + vetordirecao.y
				* vetorobjetivo.y;
	}

	/* transforma senos e cosenos em ângulos */
	float senCosToAngle(float sen, float cos) {
		if (sen >= 0 && sen <= 1 && cos >= 1 && cos >= 0)
			return sen * 90;
		else if (cos <= 0 && cos >= -1 && sen >= 0 && sen <= 1)
			return (sen * 90) + 90;
		else if (sen <= 0 && sen >= -1 && cos >= -1 && cos <= 0)
			return (sen * 90) * (-1) + 180;
		else // if (cos >= 0 and cos <= 1 && sen >= -1 && sen <= 0)
			return (cos * 90) + 270;
	}

	/* this->getbolaAtual(),this->getbolaAnterior(), distancia */
	CvPoint execPrevisaoBola(CvPoint objeto, CvPoint destino,
			int distanciaPixel) {

		float a, b;
		/* bola anterior - bola atual no sentido x e y */
		int deltay = destino.y - objeto.y;
		int deltax = destino.x - objeto.x;
		CvPoint ponto1, ponto2;

		/* evita a divisão por zero */
		if (deltax == 0)
			deltax = 1;

		/* calcula o coeficiente angular da reta */
		if (deltax != 0 && deltay != 0)
			a = (float) deltay / deltax;
		else if (deltax != 0)
			a = 1;

		/* calcula o coeficiente linear */
		b = (objeto.y - (a * (objeto.x)));

		/* bola indo no sentido da esquerda para direita, cima ou baixo */
		if ((deltax >= 0 && deltay > 0) || (deltax > 0 && deltay <= 0)) {
			float distancia;
			CvPoint2D32f auxDistancia;
			int value = 0;
			/* tenta prever a direção que a bola irá */
			int recebey = a * (objeto.x - value) + b; //sinal -
			int recebex = (recebey - b) / a;

			/* ve a distancia entre o ponto futuro e o atual */
			auxDistancia.x = recebex - objeto.x;
			auxDistancia.y = recebey - objeto.y;
			distancia = tamanhoVetor(auxDistancia);

			/* Ajusta a distância da previsão para ficar igual ao que foi lido achando que a bola continuará no mesmo sentido e velocidade */
			while (distancia < distanciaPixel) {
				/* faz um recalculo da previsão */
				value++;
				recebey = a * (objeto.x - value) + b; //sinal -
				recebex = (recebey - b) / a;

				auxDistancia.x = recebex - objeto.x;
				auxDistancia.y = recebey - objeto.y;
				distancia = tamanhoVetor(auxDistancia);
			}
			/* gera o ponto futuro */
			ponto1 = cvPoint(recebex, recebey);
			return ponto1;

			/* bola indo no sentido da esquerda para direita, cima ou baixo, idem anterior */
		} else if ((deltax < 0 && deltay >= 0) || (deltax <= 0 && deltay < 0)) {
			float distancia;
			CvPoint2D32f auxDistancia;
			int value = 0;
			int recebey = a * (objeto.x + value) + b; //sinal +
			int recebex = (recebey - b) / a;

			auxDistancia.x = recebex - objeto.x;
			auxDistancia.y = recebey - objeto.y;
			distancia = tamanhoVetor(auxDistancia);

			while (distancia < distanciaPixel) {
				value++;
				recebey = a * (objeto.x + value) + b; //sinal +
				recebex = (recebey - b) / a;

				auxDistancia.x = recebex - objeto.x;
				auxDistancia.y = recebey - objeto.y;
				distancia = tamanhoVetor(auxDistancia);
			}

			ponto1 = cvPoint(recebex, recebey);

			return ponto1;
		}
	}

	/* calcula a distância de dois pontos */
	float distancia(CvPoint pontoUm, CvPoint pontoDois) {
		CvPoint2D32f auxDistancia;
		auxDistancia.x = pontoUm.x - pontoDois.x;
		auxDistancia.y = pontoUm.y - pontoDois.y;
		return tamanhoVetor(auxDistancia);
	}

	robo** getteam() {
		return team;
	}

	void setBola(CvPoint bola) {
		this->setBolaAnterior(this->getBolaAtual());
		this->setBolaAtual(bola);
	}

	void setcentroGol(CvPoint dir, CvPoint esq) {
		setCentroGol(cvPoint(esq.x + (dir.x - esq.x) / 2, esq.y + (dir.y
				- esq.y) / 2));
		setTraveDireita(dir);
		setTraveEsquerda(esq);
	}

	void setcentroGolAdversario(CvPoint dir, CvPoint esq) {
		setCentroGolAdversario(cvPoint(esq.x + (dir.x - esq.x) / 2, esq.y
				+ (dir.y - esq.y) / 2));
	}

	bool pontoForaDoCampo(CvPoint ponto) {
		if (ponto.y >= metadeCampoBaixo.y)
			return true;
		if (ponto.y <= metadeCampoCima.y)
			return true;

		return false;
	}

	/* calcula um ponto atrás da bola para ele alinhar e chutar a bola em direção ao centro do gol adversário */
	CvPoint atrasBola(int pts) {
		cv::Point ponto;

		ponto.x = previsaoBola.x - atrasGolAdversario.x;
		ponto.y = previsaoBola.y - atrasGolAdversario.y;

		Vector2D vec(ponto.x, ponto.y);

		vec.normalize(pts);

		ponto.x = previsaoBola.x + vec.x;
		ponto.y = previsaoBola.y + vec.y;

		//if (pontoForaDoCampo(ponto))
		//return previsao_bola;

		return ponto;
	}

	/* retorna um ponto do lado da bola para o jogador ir para se preparar para o chute */
	CvPoint ladoBola(int pts, robo* robo) {
		cv::Point ponto;
		CvPoint bola = bolaAtual;

		/* rota que o robo faz para nao bater na borda */
		if (((metadeTudo.y + metadeCampoBaixo.y) / 2 < bola.y)
				|| ((metadeTudo.y + metadeCampoCima.y) / 2 < bola.y)) {
			if (robo->getCentroAtualRobo().y > bola.y)
				ponto.y = bola.y + pts;
			else
				ponto.y = bola.y - pts;
		} else if (bola.y > metadeTudo.y)
			ponto.y = bola.y - pts;
		else
			ponto.y = bola.y + pts;

		ponto.x = bola.x;

		return ponto;

	}

	/* verifica se o robo está dentro do gol */
	bool fundoDoGol(robo* goleiro, bool primeiroTempo) {
		if (primeiroTempo) {
			if (goleiro->getFrenteRobo().x < atrasGol.x || goleiro->costas.x
					< atrasGol.x)
				return true;
			else if (goleiro->getFrenteRobo().x > atrasGol.x
					|| goleiro->costas.x > atrasGol.x)
				return true;
			return false;
		}
	}

	/* verifica se o robo está fora da linha do gol */
	bool foraDoGol(robo* goleiro, bool primeiroTempo) {
		if (primeiroTempo) {
			if (goleiro->getCentroAtualRobo().y < traveEsquerda.y
					|| goleiro->getCentroAtualRobo().y > traveDireita.y)
				return true;
		} else {
			if (goleiro->getCentroAtualRobo().y < traveDireita.y
					|| goleiro->getCentroAtualRobo().y > traveEsquerda.y)
				return true;
		}

		return false;
	}

	/* calcula o angulo entre a trave e a frente ou costas do goleiro */
	float getAnguloTrave(robo* goleiro, CvPoint alvo, CvPoint* trave,
			bool primeiroTempo) {
		float cosseno;

		if (!primeiroTempo) {
			/* verifica se o centro y do robo é menor que o do objetivo ou maior e
			 * dependendo da direção da frente do robo ele vai para frente ou para trás */
			if (goleiro->getCentroAtualRobo().y < alvo.y) {
				if (goleiro->getSentidoRodas(alvo, true) == FRENTE) {
					cosseno = produtoEscalar2(goleiro->getFrenteRobo(),
							goleiro->getCentroAtualRobo(), traveEsquerda);
				} else {
					cosseno = produtoEscalar2(goleiro->costas,
							goleiro->getCentroAtualRobo(), traveEsquerda);
				}
				trave->x = traveEsquerda.x;
				trave->y = traveEsquerda.y;
			} else {
				if (goleiro->getSentidoRodas(alvo, true) == FRENTE) {
					cosseno = produtoEscalar2(goleiro->getFrenteRobo(),
							goleiro->getCentroAtualRobo(), traveDireita);
				} else {
					cosseno = produtoEscalar2(goleiro->costas,
							goleiro->getCentroAtualRobo(), traveDireita);
				}
				trave->x = traveDireita.x;
				trave->y = traveDireita.y;
			}
		} else {
			if (goleiro->getCentroAtualRobo().y < alvo.y) {
				if (goleiro->getSentidoRodas(alvo, true) == FRENTE) {
					cosseno = produtoEscalar2(goleiro->getFrenteRobo(),
							goleiro->getCentroAtualRobo(), traveDireita);
				} else {
					cosseno = produtoEscalar2(goleiro->costas,
							goleiro->getCentroAtualRobo(), traveDireita);
				}
				trave->x = traveDireita.x;
				trave->y = traveDireita.y;
			} else {
				if (goleiro->getSentidoRodas(alvo, true) == FRENTE) {
					cosseno = produtoEscalar2(goleiro->getFrenteRobo(),
							goleiro->getCentroAtualRobo(), traveEsquerda);
				} else {
					cosseno = produtoEscalar2(goleiro->costas,
							goleiro->getCentroAtualRobo(), traveEsquerda);
				}
				trave->x = traveEsquerda.x;
				trave->y = traveEsquerda.y;
			}
		}

		return acos(cosseno) * (180 / PI);
	}

	void moveRoboGoleiro(robo* goleiro, CvPoint destino, bool primeiroTempo) {
		CvPoint trave;
		float angulo = getAnguloTrave(goleiro, destino, &trave, primeiroTempo);
		bool fora = foraDoGol(goleiro, primeiroTempo);
		bool fundo = fundoDoGol(goleiro, primeiroTempo);

		if (goleiro->alinharGoleiro(angulo, destino, trave, fora, fundo)) {
			if (goleiro->andarReto(destino, true, fora)) {
				goleiro->pararRobo(true);
			}
		}

	}
	void moveAtacante(robo* atacante, CvPoint pontoGoleiro, bool bolaNoAtaque,
			bool primeiroTempo, IplImage* frame) {

		if (!bolaNoAtaque) {
			if (atacante->seguirPonto(metadeCampoCima, calculaPontoRef(atacante, primeiroTempo),
					traveDireita, centroGolAdversario)) {
				if (posicionadoAtrasDaBola(atacante)) {
					if (atacante->alinhar(bolaAtual, centroGolAdversario)) {
						//printf("chute\n");
						atacante->chutar();
					}
				} else {
					atacante->andarReto(atrasBola(60));
				}
			}
		} else {
			atacante->pararRobo();
		}
	}

	void moveZagueiro(robo* zagueiro, CvPoint destino, bool bolaNaDefesa,
			bool primeiroTempo, IplImage* frame) {

		if (!bolaNaDefesa) {
			if (zagueiro->seguirPonto(metadeCampoCima, calculaPontoRef(zagueiro, primeiroTempo),
					traveDireita, centroGolAdversario)) {
				if (posicionadoAtrasDaBola(zagueiro)) {
					if (zagueiro->alinhar(bolaAtual, centroGolAdversario)) {
						zagueiro->chutar();
					}
				}
			}
		} else {
			// acompanha a bola, faz um movimento parecido com o do goleiro
			CvPoint pontoZagueiro = calculaPontoZagueiro(destino);
			if (zagueiro->seguirPonto(metadeCampoCima, pontoZagueiro, traveDireita, metadeTudo,
					false, true)) {
				zagueiro->pararRobo();
			}
		}

	}

	/* ponto quanto na frente o zagueiro deve ficar do goleiro */
	CvPoint calculaPontoZagueiro(CvPoint ponto) {
		if (centroGol.x < metadeTudo.x)
			ponto.x = centroGol.x + 150;
		else
			ponto.x = centroGol.x - 150;

		return ponto;
	}

	/* pega um ponto de referência do lado da bola para fazer o alinhamento e chutar */
	CvPoint calculaPontoRef(robo* robo, bool primeiroTempo) {
		CvPoint bola = bolaAtual;

		if (!primeiroTempo) {
			if (centroGolAdversario.x < metadeTudo.x) {
				if (robo->getFrenteRobo().x < bola.x)
					return ladoBola(60, robo);
			} else if (centroGolAdversario.x > metadeTudo.x) {
				if (robo->getFrenteRobo().x > bola.x)
					return ladoBola(60, robo);
			}
		} else {
			if (centroGolAdversario.x > metadeTudo.x) {
				if (robo->getFrenteRobo().x > bola.x)
					return ladoBola(60, robo);
			} else if (centroGolAdversario.x > metadeTudo.x) {
				if (robo->getFrenteRobo().x > bola.x)
					return ladoBola(60, robo);
			}
		}

		//return atrasBola(50);

		return atrasBola(80);
	}

	/* calcula o Ponto na linha do gol que o goleiro deve ficar */
	CvPoint calculaPontoGoleiro(robo* goleiro, bool primeiroTempo) {
		CvPoint ponto;
		CvPoint bola;
		int m;

		if (goleiro->distanciaPara(bolaAtual) < 50)
			bola = bolaAtual;
		else
			bola = previsaoBola;

		if (!primeiroTempo) {
			/* bola em direção do gol */
			if (bola.y < traveEsquerda.y && bola.y > traveDireita.y) {
				/* se as traves estão meio tortas faz a variação no eixo x caso contrário é 1 */
				int div = abs(traveEsquerda.x - traveDireita.x) > 0 ? abs(
						traveEsquerda.x - traveDireita.x) : 1;
				/* calcula um coeficiente angular da reta que passa pelas traves */
				m = (traveEsquerda.y - traveDireita.y) / div;
				/* calcula o x e y do ponto que goleiro deve ficar nessa reta */
				ponto.x
						= ((bola.y - traveEsquerda.y + m * traveEsquerda.x) / m);
				ponto.y = bola.y;
				return ponto;
				/* bola esta depois da trave esquerda, entao o robo não passa desse ponto */
			} else if (bola.y < traveEsquerda.y) {
				ponto.x = traveDireita.x;
				ponto.y = traveDireita.y;
				return ponto;//traveDireita;
				/* bola esta depois da trave direita, entao o robo não passa desse ponto */
			} else {
				ponto.x = traveEsquerda.x;
				ponto.y = traveEsquerda.y;
				return ponto; // traveEsquerda;
			}

			/* mesma coisa só que para o segundo tempo */
		} else {
			if (bola.y > traveEsquerda.y && bola.y < traveDireita.y) {
				int div = abs(traveEsquerda.x - traveDireita.x) > 0 ? abs(
						traveEsquerda.x - traveDireita.x) : 1;
				m = (traveEsquerda.y - traveDireita.y) / div;
				ponto.x
						= ((bola.y - traveEsquerda.y + m * traveEsquerda.x) / m);
				ponto.y = bola.y;
				return ponto;
			} else if (bola.y > traveEsquerda.y) {
				ponto.x = traveDireita.x;
				ponto.y = traveDireita.y;
				return ponto;//traveDireita;
			} else {
				ponto.x = traveEsquerda.x;
				ponto.y = traveEsquerda.y;
				return ponto; // traveEsquerda;

			}
		}
	}

	/* verifica se o robo está atras da bola */
	bool posicionadoAtrasDaBola(robo* robo) {
		CvPoint bola = bolaAtual;

		/* if 1tempo else 2tempo */
		if (centroGol.x > metadeTudo.x) {
			if (robo->getCentroAtualRobo().x > bola.x)
				return true;
		} else if (centroGol.x < metadeTudo.x) {
			if (robo->getCentroAtualRobo().x < bola.x)
				return true;
		}

		return false;
	}

	bool bolaNoAtaque() {
		int limite;
		CvPoint bola = previsaoBola;

		if (metadeTudo.x < centroGol.x && bola.x > metadeTudo.x + 20)
			return true;
		else if (metadeTudo.x > centroGol.x && bola.x < metadeTudo.x - 20)
			return true;

		return false;
	}

	void run(IplImage* frame, bool primeiroTempo, Comunicacao* com) {
		bool ataque;
		float distancia;
		CvPoint2D32f auxDistancia;

		/* pega a componente x e y do vetor direção da bola */
		auxDistancia.x = this->getBolaAtual().x - this->getBolaAnterior().x;
		auxDistancia.y = this->getBolaAtual().y - this->getBolaAnterior().y;
		/* Retorna a distância da bola anterior em pixels, como é muito pequena aumenta essa distancia */
		distancia = tamanhoVetor(auxDistancia) * 20; //20

		/* se a bola está devagar ou rápida */
		if (distancia <= 50) //50
			this->setPrevisaoBola(this->getBolaAtual());
		else
			//this->setprevisao_bola(this->getbolaatual());
			this->setPrevisaoBola(this->execPrevisaoBola(this->getBolaAtual(),
					this->getBolaAnterior(), distancia));

		int goleiro = 2;
		int atacante = 0;
		int zagueiro = 1;

		// Antes de tudo, calcular os pontos em que cada jogador ficará
		CvPoint pontoGoleiro =
				calculaPontoGoleiro(team[goleiro], primeiroTempo);
		CvPoint pz = calculaPontoZagueiro(pontoGoleiro);
		cvCircle(frame, pz, 0, cvScalar(0, 255, 0), 10, 1, 0);
		//printf("pontoGoleiro = %d %d\n", pontoGoleiro.x, pontoGoleiro.y);
		//cvCircle(frame, atrasGolAdversario, 0, cvScalar(0, 255, 0), 10, 1, 0);
		//cvCircle(frame, atrasGol, 0, cvScalar(0, 255, 0), 10, 1, 0);

		// Estado do jogo
		ataque = bolaNoAtaque();

		// Mover goleiro
		moveRoboGoleiro(team[goleiro], pontoGoleiro, primeiroTempo);

		// Mover atacante
		moveAtacante(team[atacante], pontoGoleiro, ataque, primeiroTempo, frame);

		//Mover o zagueiro
		moveZagueiro(team[zagueiro], pontoGoleiro, !ataque, primeiroTempo,
				frame);
	}

	/* Para os robos */
	void parar() {
		team[0]->pararRobo();
		team[1]->pararRobo();
		team[2]->pararRobo();
	}

/*	void bolaNaBorda(){

	}*/
};

#endif //Strategy_hpp
