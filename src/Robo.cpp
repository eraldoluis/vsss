#include "Robo.hpp"

#include "Funcoes.hpp"
#include "Vector.hpp"

int Robo::xInferior;
int Robo::yInferior;
int Robo::xSuperior;
int Robo::ySuperior;

/* Construtor */
Motor::Motor() {
	velocidade = 1;
	sentido = 0;
}

Robo::Robo() :
		CentroCor() {
	frames = 0;
	comandoAnteriorRobo = 0;
	roboPerdido = false;
	mira = chute = false;
	segundoPonto = false;
	volta = false;
	countCommands = 0;
	qtdAnguloIgual = 0;
	qtdFrameParado = 0;
	qtdvolta = 0;
	comandoAnteriorRobo = 0;
	roboPerdido = false;
	roboPreso = false;
	frames = 0;
	liberar = 0;
	flag = 0;
}

Robo::Robo(int id, int time, int corRobo) {
	Robo();
	setIdRobo(id);
	setCorTimeRobo(time);
	setCorRobo(corRobo);
}

int Robo::procuraRobo(TabelaCor* centrosPontos, IplImage* frame) {
	for (CentroCor* corAtual = centrosPontos->tabelaCores[this->getCorTime()];
			corAtual != NULL; corAtual = corAtual->prox) {
		for (CentroCor* corPar = centrosPontos->tabelaCores[this->getCorRobo()];
				corPar != NULL; corPar = corPar->prox) {
			CvPoint2D32f tamanhoVect;
			tamanhoVect.x = corAtual->ponto.x - corPar->ponto.x;
			tamanhoVect.y = corAtual->ponto.y - corPar->ponto.y;
			float tamanhoFinal = tamanhoVetor(tamanhoVect);

			if (tamanhoFinal > DISTANCIA_MINIMA
					&& tamanhoFinal < DISTANCIA_MAXIMA) {
				CvPoint pontoAuxiliar, er;

				pontoAuxiliar.x = (corAtual->ponto.x + corPar->ponto.x) / 2;
				pontoAuxiliar.y = (corAtual->ponto.y + corPar->ponto.y) / 2;

				//VERIFICAR POSICAO
				if (((this->getCentroAtualRobo().x - pontoAuxiliar.x)
						* (this->getCentroAtualRobo().x - pontoAuxiliar.x)
						+ (this->getCentroAtualRobo().y - pontoAuxiliar.y)
								* (this->getCentroAtualRobo().y
										- pontoAuxiliar.y) <= 15)) {
					this->qtdFrameParado++;
				} else
					this->qtdFrameParado = 0;

				this->setcentroAtualRobo(pontoAuxiliar);

				er = gira(corAtual->ponto, this->getCentroAtualRobo(), -45); //110

				pontoAuxiliar.x = (er.x - (this->getCentroAtualRobo()).x);
				pontoAuxiliar.y = (er.y - (this->getCentroAtualRobo()).y);
				pontoAuxiliar.x *= 3;
				pontoAuxiliar.y *= 3;
				pontoAuxiliar.x += (this->getCentroAtualRobo()).x;
				pontoAuxiliar.y += (this->getCentroAtualRobo()).y;

				this->setLadoRobo(pontoAuxiliar);

				er = gira(corAtual->ponto, this->getCentroAtualRobo(), 45); //110

				pontoAuxiliar.x = (er.x - (this->getCentroAtualRobo()).x);
				pontoAuxiliar.y = (er.y - (this->getCentroAtualRobo()).y);
				pontoAuxiliar.x *= 3;
				pontoAuxiliar.y *= 3;
				pontoAuxiliar.x += (this->getCentroAtualRobo()).x;
				pontoAuxiliar.y += (this->getCentroAtualRobo()).y;

				this->setFenteRobo(pontoAuxiliar);

				er = gira(corAtual->ponto, this->getCentroAtualRobo(), -135); //110

				pontoAuxiliar.x = (er.x - (this->getCentroAtualRobo()).x);
				pontoAuxiliar.y = (er.y - (this->getCentroAtualRobo()).y);
				pontoAuxiliar.x *= 3;
				pontoAuxiliar.y *= 3;
				pontoAuxiliar.x += (this->getCentroAtualRobo()).x;
				pontoAuxiliar.y += (this->getCentroAtualRobo()).y;

				this->costas = pontoAuxiliar;

				cvLine(frame, this->getCentroAtualRobo(), this->getLadoRobo(),
						cvScalar(0, 0, 255), 1, 8, 0); //VERMELHO LADO
				cvLine(frame, this->getCentroAtualRobo(), this->getFrenteRobo(),
						cvScalar(0, 255, 0), 1, 8, 0); //VERDE FRENTE
				cvLine(frame, this->getCentroAtualRobo(), this->costas,
						cvScalar(255, 0, 0), 1, 8, 0); //AZUL COSTAS

				char texto[3];

				if (this->getCorRobo() == COR_ROBO_UM)
					strcpy(texto, "#1");
				else if (this->getCorRobo() == COR_ROBO_DOIS)
					strcpy(texto, "#2");
				else if (this->getCorRobo() == COR_ROBO_TRES)
					strcpy(texto, "#3");

				CvFont font;
				cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.35, 0.35, 0, 1,
				CV_AA);
				cvPutText(frame, texto,
						cvPoint(this->getCentroAtualRobo().x + 25,
								this->getCentroAtualRobo().y + 25), &font,
						cvScalar(255, 255, 255, 0));

				return 1;
			}
		}
	}

	return 0;
}

float Robo::distancia(CvPoint pontoUm, CvPoint pontoDois) {
	CvPoint2D32f auxDistancia;
	auxDistancia.x = pontoUm.x - pontoDois.x;
	auxDistancia.y = pontoUm.y - pontoDois.y;
	return tamanhoVetor(auxDistancia);
}

float Robo::distanciaPara(CvPoint ponto) {
	CvPoint2D32f auxDistancia;
	auxDistancia.x = centroAtualRobo.x - ponto.x;
	auxDistancia.y = centroAtualRobo.y - ponto.y;
	return tamanhoVetor(auxDistancia);
}

int Robo::getVelocidade(float distancia) {
	int velocidade;

	velocidade = (70 / (6.4 * 51)) * (sqrt(distancia));

	return (int) velocidade;
}

int Robo::getVelocidadeAngular(float angulo, int distancia) {
	int velocidade;

	if (angulo < 15.0f)
		return 0;

	//velocidade = (180 / 34.05) * sqrt(angulo);
	int div = 34.05; //(-0.8 * distancia) + 80;
	velocidade = (180 / div) * sqrt(angulo);
	return (int) velocidade / 20;
}

int Robo::getSentidoRodas(CvPoint alvo, bool goleiro) {
	if (goleiro) {
		if (distancia(alvo, frenteRobo) < distancia(alvo, costas))
			return FRENTE;
		else
			return TRAS;
	}
	return FRENTE;
}

float Robo::getErroAngulo(CvPoint alvo) {
	float cos = produtoEscalar(this->frenteRobo, this->centroAtualRobo, alvo);
	float angulo = acos(cos) * (180 / M_PI);

	return angulo;
}

void Robo::chuteGoleiro() {

}

void Robo::pararRobo(bool goleiro) {
	this->motorEsquerdoRobo.velocidade = 0;
	this->motorDireitoRobo.velocidade = 0;
	roboPreso = false;

	setMotorRobo();

}

CvPoint Robo::getPontoRef(CvPoint alvo, CvPoint direcao) {
	Vector2D vec1(alvo.x - centroAtualRobo.x, alvo.y - centroAtualRobo.y);
	Vector2D vec2(direcao.x - alvo.x, direcao.y - alvo.y);

	float gama = Vector2D::angle(vec1, vec2);

	Vector2D ref;

	ref = vec1.rotateLeft(gama);

	CvPoint ponto;
	ponto.x = centroAtualRobo.x + ref.x;
	ponto.y = centroAtualRobo.y + ref.y;

	return ponto;
}

void Robo::evitarColisao(CvPoint* ponto) {
	if (ponto->x < Robo::xInferior)
		ponto->x = Robo::xInferior;
	else if (ponto->x > Robo::xSuperior)
		ponto->x = Robo::xSuperior;

	if (ponto->y < Robo::yInferior)
		ponto->y = Robo::yInferior;
	else if (ponto->y > Robo::ySuperior)
		ponto->y = Robo::ySuperior;
}

CvPoint Robo::calculaPontoRef(CvPoint alvo, CvPoint ladoBola, CvPoint bola,
		CvPoint centroGolAdversario, CvPoint metadeCampo) {

	CvPoint ponto;
	float angleDiff = getAngleDiff(alvo, bola);

	if (angleDiff > (M_PI / 2))
		ponto = getPontoRef(alvo, centroGolAdversario);
	else
		ponto = ladoBola;

	return ponto;
}

float Robo::getAngleDiff(CvPoint alvo, CvPoint bola) {
	Vector2D vec1(alvo.x - bola.x, alvo.y - bola.y);
	Vector2D vec2(bola.x - centroAtualRobo.x, bola.y - centroAtualRobo.y);

	return fabs(Vector2D::angle(vec1, vec2));
}

void Robo::verificaRoboPreso() {
	if (motorDireitoRobo.velocidade > 0 || motorEsquerdoRobo.velocidade > 0) {
		if (frames > 10 || roboPreso) {
			roboPreso = true;
		}
	}
}

int Robo::liberarRobo(CvPoint bola, CvPoint golAdversario, bool goleiro) {
	liberar++;
	if (liberar < 7) {

		motorDireitoRobo.sentido = TRAS;
		motorEsquerdoRobo.sentido = TRAS;

		float cossenoLado = produtoEscalar(ladoRobo, centroAtualRobo, bola);

		if (goleiro) {
			if (cossenoLado > 0) {
				this->motorEsquerdoRobo.velocidade = 2;
				this->motorDireitoRobo.velocidade = 1;
			} else {
				this->motorEsquerdoRobo.velocidade = 1;
				this->motorDireitoRobo.velocidade = 2;
			}

			return 0;
		}

		if (cossenoLado > 0) {
			this->motorEsquerdoRobo.velocidade = 4;
			this->motorDireitoRobo.velocidade = 2;
		} else {
			this->motorEsquerdoRobo.velocidade = 2;
			this->motorDireitoRobo.velocidade = 4;
		}

		return 0;
	}

	frames = 0;
	roboPreso = false;
	liberar = 0;

	return 0;
}

bool Robo::foraDaArea(CvPoint trave) {
	int distanciaMax = 40;

	if (abs(centroAtualRobo.x - trave.x) > distanciaMax)
		return true;

	return false;
}

bool Robo::seguirPonto(CvPoint metadeCampoBaixo, CvPoint alvo, CvPoint trave,
		CvPoint golAdversario, bool goleiro, bool zagueiro, float angulo) {

	int distMaxima = goleiro == true ? 30 : 40;

	if (roboPerdido) {
		//printf("perdido\n");
		pararRobo();
		return false;
	}

	float dist;

	printf("centro.x %d\n", metadeCampoBaixo.x);
	printf("centro.y %d\n", metadeCampoBaixo.y);

	dist = distancia(centroAtualRobo, alvo);

	verificaRoboPreso();

	if (roboPreso) {
		liberarRobo(alvo, golAdversario);
		setMotorRobo();
		return false;
	}

	if (dist > distMaxima) {

		int sentido = getSentidoRodas(alvo, false);
		this->motorDireitoRobo.sentido = sentido;
		this->motorEsquerdoRobo.sentido = sentido;

		int velocidade = getVelocidade(distancia(centroAtualRobo, alvo));
		this->motorEsquerdoRobo.velocidade = velocidade;
		this->motorDireitoRobo.velocidade = velocidade;

		float erroAngulo = getErroAngulo(alvo);

		float cossenoLado = produtoEscalar(ladoRobo, centroAtualRobo, alvo);

		if (cossenoLado > 0) {
			this->motorEsquerdoRobo.velocidade -= (getVelocidadeAngular(
					erroAngulo, dist) / 2);
			this->motorDireitoRobo.velocidade += getVelocidadeAngular(
					erroAngulo, dist);
		} else {
			this->motorDireitoRobo.velocidade -= (getVelocidadeAngular(
					erroAngulo, dist) / 2);
			this->motorEsquerdoRobo.velocidade += getVelocidadeAngular(
					erroAngulo, dist);
		}

		setMotorRobo();

		return false;
	}

	frames = 0;
	return true;
}

bool Robo::andarReto(CvPoint alvo, bool goleiro, bool foraDoGol,
		bool zagueiro) {
	verificaRoboPreso();

	CvPoint pontoAleatorio;

	int distMax = goleiro ? 30 : 40;
	int dist = distanciaPara(alvo);

	int sentido = getSentidoRodas(alvo, goleiro);

	if (dist > distMax) {
		if (goleiro) {
			this->motorEsquerdoRobo.velocidade = 4;
			this->motorDireitoRobo.velocidade = 4;
		} else {
			if (roboPreso) {
				liberarRobo(alvo, pontoAleatorio);
				setMotorRobo();
				return false;
			}

			this->motorEsquerdoRobo.velocidade = 1;
			this->motorDireitoRobo.velocidade = 1;
		}

		this->motorDireitoRobo.sentido = sentido;
		this->motorEsquerdoRobo.sentido = sentido;

		setMotorRobo();
		return false;
	}

	if (goleiro) {
		if (roboPreso && foraDoGol) {
			liberarRobo(alvo, pontoAleatorio, true);
			setMotorRobo();
			return false;
		}
	}

	return true;
}

bool Robo::alinharGoleiro(float angulo, CvPoint alvo, CvPoint trave,
		bool foraDoGol, bool fundoDoGol) {

	//float angulo = getErroAngulo(trave);

	verificaRoboPreso();

	if (angulo > 20) {

		float cossenoLado = produtoEscalar(ladoRobo, centroAtualRobo, trave);
		int sentido = getSentidoRodas(alvo, true);
		this->motorEsquerdoRobo.sentido = sentido;
		this->motorDireitoRobo.sentido = sentido;

		if (sentido == FRENTE) {
			if (cossenoLado > 0) {
				this->motorEsquerdoRobo.velocidade = 0; //velocidadeAng;
				this->motorDireitoRobo.velocidade = 1;
			} else {
				this->motorEsquerdoRobo.velocidade = 1;
				this->motorDireitoRobo.velocidade = 0; //velocidadeAng;
			}
		} else if (sentido == TRAS) {
			if (cossenoLado > 0) {
				this->motorEsquerdoRobo.velocidade = 0; //velocidadeAng;
				this->motorDireitoRobo.velocidade = 1;
			} else {
				this->motorEsquerdoRobo.velocidade = 1;
				this->motorDireitoRobo.velocidade = 0; //velocidadeAng;
			}
		}

		setMotorRobo();

		if (roboPreso && (foraDoGol || fundoDoGol)) {
			liberarRobo(alvo, trave, true);
			setMotorRobo();
			printf("preso alinhar\n");
			return false;
		}

		flag = 0;
		return false;
	}
	return true;
}

bool Robo::alinhar(CvPoint bola, CvPoint golAdversario) {
	verificaRoboPreso();
	CvPoint pontoAleatorio;

	if (roboPreso) {
		liberarRobo(bola, pontoAleatorio);
		setMotorRobo();
		return false;
	}

	float angulo = getErroAngulo(golAdversario);

	if (angulo > 32) {
		float cossenoLado = produtoEscalar(ladoRobo, centroAtualRobo, bola);
		if (cossenoLado > 0) {
			this->motorEsquerdoRobo.velocidade = 0; //;
			this->motorDireitoRobo.velocidade = 1;
		} else {
			this->motorEsquerdoRobo.velocidade = 1;
			this->motorDireitoRobo.velocidade = 0; //velocidadeAng;
		}

		setMotorRobo();
		return false;
	}

	return true;
}

void Robo::chutar() {
	this->motorEsquerdoRobo.velocidade = 4;
	this->motorDireitoRobo.velocidade = 4;
	printf("chute\n");
	setMotorRobo();
}

bool Robo::emMovimento() {
	return this->motorDireitoRobo.velocidade
			|| this->motorEsquerdoRobo.velocidade;
}

void Robo::traduzirComandos() {
	int codigoMotorEsquerdoRobo = 0, codigoMotorDireitoRobo = 0;
	this->motor = 0;

	if (this->motorDireitoRobo.velocidade != 0)
		codigoMotorDireitoRobo = 5 * this->motorDireitoRobo.sentido
				+ this->motorDireitoRobo.velocidade;

	if (this->motorEsquerdoRobo.velocidade != 0)
		codigoMotorEsquerdoRobo = 5 * this->motorEsquerdoRobo.sentido
				+ this->motorEsquerdoRobo.velocidade;

	this->motor = codigoMotorDireitoRobo * 11 + codigoMotorEsquerdoRobo;

}
