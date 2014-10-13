/*
 * Visao.hpp
 *
 *  Created on: Oct 13, 2014
 *      Author: eraldo
 *
 * Algoritmo de visão computacional que reconhece o campo de jogo, os jogadores
 * e a bola. Todos estes objetos são representados dentro desta classe.
 */

#ifndef VISAO_HPP_
#define VISAO_HPP_

#include <opencv2/opencv.hpp>

#include "Configuracoes.hpp"
#include "PontosCampo.hpp"
#include "TabelaCor.hpp"
#include "Robo.hpp"
#include "Cor.hpp"
#include "Funcoes.hpp"

/*
 * Controla o acesso a câmera e realiza o reconhecimento dos objetos no campo.
 */
class Visao {

public:

	CvCapture* capture;
	IplImage* frame;
	IplImage* hsv_frame;

	PontosCampo centroCampo;
	TabelaCor centroPontos;

	Robo ROBO_UM;
	Robo ROBO_DOIS;
	Robo ROBO_TRES;

	Cor* cores;

	Visao() :
			ROBO_UM(0, COR_TIME_UM, COR_ROBO_UM), ROBO_DOIS(1, COR_TIME_UM,
			COR_ROBO_DOIS), ROBO_TRES(2, COR_TIME_UM, COR_ROBO_TRES), capture(
					NULL), frame(NULL), hsv_frame(NULL), cores(NULL) {
	}

	/*
	 * Método responsável por reconhecer os jogadores e a bola.
	 */
	void reconhece() {
		uchar* ptr;
		int a, b, a_max, b_max;

		// Percorre toda a imagem.
		for (int y = 0; y < hsv_frame->height; y++) {
			for (int x = 0; x < hsv_frame->width; x++) {
				// Cor do pixel (x, y).
				ptr = cvPtr2D(hsv_frame, y, x, NULL);

				int i = cores->verificaHSVCor(ptr[0], ptr[1], ptr[2]);

				if (i != -1) {
					if (centroPontos.verificaTabelaCor(x, y)) {
						if ((x > 0 && (x < (hsv_frame->width - 3)))
								&& (y > 0 && (y < (hsv_frame->height - 3)))) {
							if (pixelReal(hsv_frame, i, x, y, cores)) {
								int somax, somay, total;
								somax = somay = 0;
								total = 0;
								b = (y - (2 * RAIO) > 0) ? y - (2 * RAIO) : 0;
								a_max = (x + (2 * RAIO) < hsv_frame->width) ?
										x + (2 * RAIO) : hsv_frame->width - 1;
								b_max = (y + (2 * RAIO) < hsv_frame->height) ?
										y + (2 * RAIO) : hsv_frame->height - 1;

								for (; b < b_max; b++) {
									a = (x - (2 * RAIO) > 0) ?
											x - (2 * RAIO) : 0;
									for (; a < a_max; a++) {
										ptr = cvPtr2D(hsv_frame, b, a, NULL);

										if (cores->verificaCor(ptr[0], ptr[1],
												ptr[2], i)) {
											if (centroPontos.verificaTabelaCor(
													a, b)) {
												somax += a;
												somay += b;
												total++;
											}
										}
									}
								}

								if (total > 50) {
									CvPoint target;
									target.x = somax / total;
									target.y = somay / total;
									centroPontos.adicinaTabelaCor(target, i);
								}
								break;
							} //fim pixelReal
						}
					}
				}
			}
		}

		/* Verifica se os robos estao perdidos */
		if (ROBO_UM.procuraRobo(centroPontos, frame) == 0) {
			ROBO_UM.setRoboPerdido(true);
			//printf("robo #1 perdido\n");
		} else
			ROBO_UM.setRoboPerdido(false);

		if (ROBO_DOIS.procuraRobo(centroPontos, frame) == 0) {
			ROBO_DOIS.setRoboPerdido(true);
			//printf("robo #2 perdido\n");
		} else
			ROBO_DOIS.setRoboPerdido(false);

		if (ROBO_TRES.procuraRobo(centroPontos, frame) == 0) {
			ROBO_TRES.setRoboPerdido(true);
			//printf("robo #3 perdido\n");
		} else
			ROBO_TRES.setRoboPerdido(false);
	}

	/*
	 * Lê os valores das cores de um arquivo.
	 */
	bool leValoresHSV(const char* nomeArquivo) {
		FILE* arquivo = fopen(nomeArquivo, "r");
		if (arquivo == NULL)
			return false;

		int number[8] = { 0 };
		int cont = 0;
		int numero;
		int HSVMin[3];
		int HSVMax[3];
		bool first = true;

		while (!feof(arquivo)) {
			fscanf(arquivo, "%d", &numero);

			// Skip one character.
			fgetc(arquivo);

			if (cont == 3)
				for (int i = 0; i < 3; i++)
					HSVMin[i] = number[i];
			else if (cont >= 7) {
				for (int i = 0; i < 3; i++)
					HSVMax[i] = number[i + 4];
				if (first) {
					cores = new Cor(HSVMin, HSVMax, numero);
					first = false;
				} else
					cores->adicionaCor(HSVMin, HSVMax, numero);
			} else
				number[cont] = numero;
			cont++;
			if (cont == 8)
				cont = 0;
		}

		fclose(arquivo);

		return true;
	}

	/*
	 * Lê pontos extremos do campo do arquivo dado.
	 */
	bool lePontosDoCampo(const char* nomeArquivo) {
		// Pontos do campo.
		FILE* pontosCampo = fopen(nomeArquivo, "r");
		if (pontosCampo == NULL)
			return false;
		centroCampo.lePontosCampo(pontosCampo);
		fclose(pontosCampo);
		return true;
	}

	/*
	 * Inicializa visão.
	 */
	bool inicializa() {
		// Inicialização da camera.
		capture = cvCaptureFromCAM(NUMERO_CAMERA);
		// Captura frame atual em RGB.
		frame = cvQueryFrame(capture);
		// Converte o frame em formato HSV.
		hsv_frame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
		return (frame != NULL && hsv_frame != NULL);
	}

	bool captura() {
		// Captura frame atual em RGB.
		frame = cvQueryFrame(capture);
		if (frame == NULL)
			return false;
		// Converte o frame em formato HSV.
		cvCvtColor(frame, hsv_frame, CV_BGR2HSV);
		// Reconhece elementos no frame.
		reconhece();
		return true;
	}

};

#endif /* VISAO_HPP_ */
