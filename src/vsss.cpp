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

/* Libs */
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <unistd.h>
#include <list>
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include "Configuracoes.h"

using namespace std;

FILE* arquivo;
FILE* pontosCampo;

#ifndef __Cor_cpp
#include "Cor.cpp"
#endif

Cor* cores = NULL;

#ifndef __CentroCor_cpp
#include "CentroCor.cpp"
#endif

#ifndef __TabelaCor_cpp
#include "TabelaCor.cpp"
#endif

#ifndef __PontosCampo_cpp
#include "PontosCampo.cpp"
#endif

#ifndef __Funcoes_cpp
#include "Funcoes.cpp"
#endif

#ifndef VECTOR_H_
#include "Vector.cpp"
#endif

#ifndef __robo_cpp
#include "robo.cpp"
#endif

#ifndef __Comunicacao_cpp
#include "Comunicacao.cpp"
#endif

#ifndef __Strategy_cpp
#include "Strategy.cpp"
#endif

IplImage* frame;

PontosCampo* centroCampo = new PontosCampo();
TabelaCor* centroPontos = new TabelaCor();

robo* ROBO_UM = new robo(0, COR_TIME_UM, COR_ROBO_UM);
robo* ROBO_DOIS = new robo(1, COR_TIME_UM, COR_ROBO_DOIS);
robo* ROBO_TRES = new robo(2, COR_TIME_UM, COR_ROBO_TRES);

Comunicacao* COMRobo = new Comunicacao();
Strategy* TIME_UM = new Strategy(ROBO_UM, ROBO_DOIS, ROBO_TRES);

bool rodando;

/* Envia os dados para os robos */
void* enviarDados(void* args) {
	/*Inicialização da comunicação via porta serial */
	COMRobo->abrePortaSerialComunicacao();
	COMRobo->configuraComunicacao();

	/* Envia dados enquanto o programa estiver rodando */
	while (rodando) {
		COMRobo->enviaDadosRobo(TIME_UM->getteam());
		usleep(9000);
	}

	pthread_exit(0);
}

/* Função responsável por encontrar os robôs na imagem */
void color(IplImage* color) {
	uchar* ptr;
	int a, b, a_max, b_max;

	/*Percorre a imagem toda */
	for (int y = 0; y < color->height; y++)
		for (int x = 0; x < color->width; x++) {
			/* Retorna as cores de um determinado pixel (x, y) da imagem */
			ptr = cvPtr2D(color, y, x, NULL);

			int i = cores->verificaHSVCor(ptr[0], ptr[1], ptr[2]);

			if (i != -1) {
				if (centroPontos->verificaTabelaCor(x, y)) {
					if ((x > 0 && (x < (color->width - 3))) && (y > 0 && (y
							< (color->height - 3)))) {
						if (pixelReal(color, i, x, y)) {
							int somax, somay, total;
							somax = somay = 0;
							total = 0;
							b = (y - (2 * RAIO) > 0) ? y - (2 * RAIO) : 0;
							a_max = (x + (2 * RAIO) < color->width) ? x + (2
									* RAIO) : color->width - 1;
							b_max = (y + (2 * RAIO) < color->height) ? y + (2
									* RAIO) : color->height - 1;

							for (; b < b_max; b++) {
								a = (x - (2 * RAIO) > 0) ? x - (2 * RAIO) : 0;
								for (; a < a_max; a++) {
									ptr = cvPtr2D(color, b, a, NULL);

									if (cores->verificaCor(ptr[0], ptr[1],
											ptr[2], i)) {
										if (centroPontos->verificaTabelaCor(a,
												b)) {
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
								centroPontos->adicinaTabelaCor(target, i);
							}
							break;
						}//fim pixelReal				
					}
				}
			}
		}

	/* Verifica se os robos estao perdidos */
	if (ROBO_UM->procuraRobo(centroPontos, frame) == 0) {
		ROBO_UM->setRoboPerdido(true);
		//printf("robo #1 perdido\n");
	} else
		ROBO_UM->setRoboPerdido(false);

	if (ROBO_DOIS->procuraRobo(centroPontos, frame) == 0) {
		ROBO_DOIS->setRoboPerdido(true);
		//printf("robo #2 perdido\n");
	} else
		ROBO_DOIS->setRoboPerdido(false);

	if (ROBO_TRES->procuraRobo(centroPontos, frame) == 0) {
		ROBO_TRES->setRoboPerdido(true);
		//printf("robo #3 perdido\n");
	} else
		ROBO_TRES->setRoboPerdido(false);
}

/* Main do software */
int main(int argc, char* argv[]) {

	rodando = true;
	bool primeiroTempo;
	sched_param param;
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	/* Valores do HSV */
	arquivo = fopen("ARQUIVOSINFORMACAO/hsv.arff", "r");
	if (arquivo == NULL) {
		printf("ERROR: FILE HSV NOT OPEN\n");
		exit(1);
	}
	guardaValoresHSV();//Guarda valores de HSV


	/* Pontos do campo */
	pontosCampo = fopen("ARQUIVOSINFORMACAO/marc.txt", "r");
	if (pontosCampo == NULL) {
		printf("ERROR: FILE CAMPO NOT OPEN\n");
		exit(1);
	}
	centroCampo->guardaPontosCampo(pontosCampo);

	/* Inicializacao da camera */
	CvCapture* capture = cvCaptureFromCAM(NUMERO_CAMERA);
	/* Captura frame atual em RGB */
	frame = cvQueryFrame(capture);
	/* Verifica se conseguiu pegar o frame */
	if (!frame) {
		fprintf(stderr, "ERROR: frame is null...\n");
		getchar();
		exit(0);
	}

	/* Converte o frame em formato HSV */
	IplImage* hsv_frame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
	/* Cria uma janela para exibir o frame */
	cvNamedWindow("Color", CV_WINDOW_AUTOSIZE);

	/* Thread de comunicacao */
	int ret = pthread_create(&tid, &attr, enviarDados, 0);
	if (ret != 0) {
		printf("erro ao criar thread de enviar dados\n");
		return 1;
	} else
		printf("Thread de comunicação criada\n");

	/* Define o lado que sera defesa e ataque definido pelo argumento [1] */
	primeiroTempo = atoi(argv[1]) == 1 ? 1 : 0;
	if (primeiroTempo) {
		TIME_UM->setcentroGol(centroCampo->devolvePontoPontosCampo(8),
				centroCampo->devolvePontoPontosCampo(11));//VERMELHO
		TIME_UM->setcentroGolAdversario(
				centroCampo->devolvePontoPontosCampo(2),
				centroCampo->devolvePontoPontosCampo(5));//AZUL
	} else {
		TIME_UM->setcentroGol(centroCampo->devolvePontoPontosCampo(2),
				centroCampo->devolvePontoPontosCampo(5));//VERMELHO
		TIME_UM->setcentroGolAdversario(
				centroCampo->devolvePontoPontosCampo(8),
				centroCampo->devolvePontoPontosCampo(11));//AZUL
	}

	/* Define os pontos de centro do campo, metade de cima, metade de baixo */
	TIME_UM->metadeCampoCima.x = (centroCampo->devolvePontoPontosCampo(1).x
			- centroCampo->devolvePontoPontosCampo(0).x) / 2
			+ centroCampo->devolvePontoPontosCampo(0).x;
	TIME_UM->metadeCampoCima.y = (centroCampo->devolvePontoPontosCampo(1).y
			- centroCampo->devolvePontoPontosCampo(0).y) / 2
			+ centroCampo->devolvePontoPontosCampo(0).y;

	TIME_UM->metadeCampoBaixo.x = (centroCampo->devolvePontoPontosCampo(6).x
			- centroCampo->devolvePontoPontosCampo(7).x) / 2
			+ centroCampo->devolvePontoPontosCampo(7).x;
	TIME_UM->metadeCampoBaixo.y = (centroCampo->devolvePontoPontosCampo(6).y
			- centroCampo->devolvePontoPontosCampo(7).y) / 2
			+ centroCampo->devolvePontoPontosCampo(7).y;

	TIME_UM->metadeTudo.x = (TIME_UM->metadeCampoCima.x
			- TIME_UM->metadeCampoBaixo.x) / 2 + TIME_UM->metadeCampoBaixo.x;
	TIME_UM->metadeTudo.y = (TIME_UM->metadeCampoCima.y
			- TIME_UM->metadeCampoBaixo.y) / 2 + TIME_UM->metadeCampoBaixo.y;

	TIME_UM->calculaPontoAtrasGol();
	TIME_UM->getteam()[1]->chutar();

	/* Programa executando */
	while (1) {

		/* Captura frame atual em RGB */
		frame = cvQueryFrame(capture);
		/* Verifica se conseguiu pegar o frame */
		if (!frame) {
			fprintf(stderr, "ERROR: frame is null...\n");
			getchar();
			break;
		}

		/* Converte o frame em formato HSV */
		cvCvtColor(frame, hsv_frame, CV_BGR2HSV);

		/* Responsável por encontrar os robôs na imagem */

		color(hsv_frame);

		/* Chama a estrategia caso a bola nao esteja perdida */
		if ((centroPontos->tabelaCores[4] != NULL)) {
			/* Seta os pontos da bola */
			TIME_UM->setBola(centroPontos->tabelaCores[4]->ponto);

			TIME_UM->run(frame, primeiroTempo, COMRobo);


			/* Autoriza enviar os comandos */
			Comunicacao::enviarPacote = true;

			/* Exibe o ponto da bola */
			cvCircle(frame, centroPontos->tabelaCores[4]->ponto, 0, cvScalar(255,
					255, 0), 10, 1, 0);

		}else{
			/* Autoriza enviar os comandos */
			Comunicacao::enviarPacote = true;
			for (int i=0; i<100; i++)
				TIME_UM->parar();
		}

		/* Exibe os pontos na imagem */
		cvCircle(frame, TIME_UM->getCentroGol(), 0, cvScalar(0, 0, 0), 10, 1, 0);
		cvCircle(frame, TIME_UM->getCentroGolAdversario(), 0, cvScalar(0, 255,
				255), 10, 1, 0);

		cvCircle(frame, TIME_UM->metadeCampoCima, 0, cvScalar(0, 255, 255), 10,
				1, 0);
		cvCircle(frame, TIME_UM->metadeCampoBaixo, 0, cvScalar(0, 255, 255),
				10, 1, 0);
		cvCircle(frame, TIME_UM->metadeTudo, 0, cvScalar(0, 255, 255), 10, 1, 0);

		cvCircle(frame, centroCampo->devolvePontoPontosCampo(2), 0, cvScalar(
				255, 0, 0), 10, 1, 0);
		cvCircle(frame, centroCampo->devolvePontoPontosCampo(5), 0, cvScalar(
				255, 0, 0), 10, 1, 0);

		cvCircle(frame, centroCampo->devolvePontoPontosCampo(8), 0, cvScalar(0,
				0, 255), 10, 1, 0);
		cvCircle(frame, centroCampo->devolvePontoPontosCampo(11), 0, cvScalar(
				0, 0, 255), 10, 1, 0);

		/* Exibe o frame atual na janela */
		cvShowImage("Color", frame);

		/* Limpa a imagem */
		centroPontos->limpaTabelaCor();

		/* Condicao de parada (tecla esc) */
		if ((cvWaitKey(10) & 255) == 27)
			break;
	}

	/* Encerra a comunicacao e finaliza o software */
	TIME_UM->parar();
	sleep(1);
	rodando = false;
	pthread_join(tid, 0);
	return 0;
}
