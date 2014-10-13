/*
 * Strategy.cpp
 *
 *  Created on: Oct 12, 2014
 *      Author: eraldo
 */

#include "Strategy.hpp"

void Strategy::calculaPontoAtrasGol() {
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

float Strategy::produtoEscalar2(CvPoint direcao, CvPoint centro,
		CvPoint objetivo) {
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
	return vetordirecao.x * vetorobjetivo.x + vetordirecao.y * vetorobjetivo.y;
}

float Strategy::senCosToAngle(float sen, float cos) {
	if (sen >= 0 && sen <= 1 && cos >= 1 && cos >= 0)
		return sen * 90;
	else if (cos <= 0 && cos >= -1 && sen >= 0 && sen <= 1)
		return (sen * 90) + 90;
	else if (sen <= 0 && sen >= -1 && cos >= -1 && cos <= 0)
		return (sen * 90) * (-1) + 180;
	else
		// if (cos >= 0 and cos <= 1 && sen >= -1 && sen <= 0)
		return (cos * 90) + 270;
}

CvPoint Strategy::execPrevisaoBola(CvPoint objeto, CvPoint destino,
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
