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
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

#include <unistd.h>
#include <list>
#include <fcntl.h>
#include <termios.h>

#include "Configuracoes.hpp"
#include "Cor.hpp"
#include "CentroCor.hpp"
#include "TabelaCor.hpp"
#include "PontosCampo.hpp"
#include "Funcoes.hpp"
#include "Vector.hpp"
#include "Robo.hpp"
#include "Comunicacao.hpp"
#include "Strategy.hpp"
#include "MoveRLSimulator.hpp"
#include "Visao.hpp"

using namespace std;

/* Main do software */
int main(int argc, char* argv[]) {

	bool primeiroTempo;

	// Objeto que realiza o reconhecimento dos jogadores e da bola.
	Visao visao;

	// Estratégia.
	Strategy estrategia(visao);

	// Valores do HSV.
	if (!visao.leValoresHSV("conf/hsv.arff")) {
		fprintf(stderr, "Erro ao ler valores do HSV!");
		return 1;
	}

	// Lê pontos extremos do campo.
	if (!visao.lePontosDoCampo("conf/marc.txt")) {
		fprintf(stderr, "Erro ao ler pontos extremos do campo!");
		return 1;
	}

	// Inicializa visão.
	if (!visao.inicializa()) {
		fprintf(stderr, "ERROR: frame is null...\n");
		return 1;
	}

	// Cria uma janela para exibir o frame.
	cvNamedWindow("Color", CV_WINDOW_AUTOSIZE);

	// Thread de comunicacao.
	Comunicacao com(PORTA_SERIAL);
	if (com.inicia() != 0) {
		printf("erro ao criar thread de enviar dados\n");
		return 1;
	} else
		printf("Thread de comunicação criada\n");

	/* Define o lado que sera defesa e ataque definido pelo argumento [1] */
	primeiroTempo = true;
	if (argc > 1)
		primeiroTempo = (atoi(argv[1]) == 1);

	// Inicializa estratégia.
	estrategia.inicializa(primeiroTempo);

	/* Programa executando */
	while (1) {

		// Captura e processa novo frame.
		if (!visao.captura()) {
			fprintf(stderr, "ERROR: frame is null...\n");
			break;
		}

		/* Chama a estrategia caso a bola nao esteja perdida */
		if ((visao.centroPontos.tabelaCores[4] != NULL)) {
			/* Seta os pontos da bola */
			estrategia.setBola(visao.centroPontos.tabelaCores[4]->ponto);

			estrategia.run(visao.frame, primeiroTempo);

			// Envia novos comandos para os robôs.
			com.enviaDadosRobo(estrategia.getteam());

			/* Exibe o ponto da bola */
			cvCircle(visao.frame, visao.centroPontos.tabelaCores[4]->ponto, 0,
					cvScalar(255, 255, 0), 10, 1, 0);

		} else {
			// Envia novos comandos para os robôs.
			com.enviaDadosRobo(estrategia.getteam());
			// Para todos os robôs (100 vezes!?).
			for (int i = 0; i < 100; i++)
				estrategia.parar();
		}

		/* Exibe os pontos na imagem */
		cvCircle(visao.frame, estrategia.getCentroGol(), 0, cvScalar(0, 0, 0), 10,
				1, 0);
		cvCircle(visao.frame, estrategia.getCentroGolAdversario(), 0,
				cvScalar(0, 255, 255), 10, 1, 0);

		cvCircle(visao.frame, estrategia.metadeCampoCima, 0, cvScalar(0, 255, 255),
				10, 1, 0);
		cvCircle(visao.frame, estrategia.metadeCampoBaixo, 0,
				cvScalar(0, 255, 255), 10, 1, 0);
		cvCircle(visao.frame, estrategia.metadeTudo, 0, cvScalar(0, 255, 255), 10,
				1, 0);

		cvCircle(visao.frame, visao.centroCampo.devolvePontoPontosCampo(2), 0,
				cvScalar(255, 0, 0), 10, 1, 0);
		cvCircle(visao.frame, visao.centroCampo.devolvePontoPontosCampo(5), 0,
				cvScalar(255, 0, 0), 10, 1, 0);

		cvCircle(visao.frame, visao.centroCampo.devolvePontoPontosCampo(8), 0,
				cvScalar(0, 0, 255), 10, 1, 0);
		cvCircle(visao.frame, visao.centroCampo.devolvePontoPontosCampo(11), 0,
				cvScalar(0, 0, 255), 10, 1, 0);

		/* Exibe o frame atual na janela */
		cvShowImage("Color", visao.frame);

		/* Limpa a imagem */
		visao.centroPontos.limpaTabelaCor();

		/* Condicao de parada (tecla esc) */
		if ((cvWaitKey(10) & 255) == 27)
			break;
	}

//	// STEP 1. Create simulator
//	MoveRLSimulator* moveSimulator = new MoveRLSimulator(visao);
//
//	// STEP 2. Create linear approximator (see documentation to change this to a multi-layer perceptron)
//	Approximator* neuralNet = new NeuralNetBatch(moveSimulator->getObsRows(),
//			moveSimulator->getNumActions());
//
//	// STEP 3. Create a controller (implement different algorithms here)
//	Controller* controller;
//	// Create Natural Actor-Critic controller
//	// controller = new NACTransform(new BasicController(neuralNet), mySimulator->getDiscountFactor());
//	// Create LSPI controller
//	controller = new LSTDQController(neuralNet,
//			new eGreedyPolicy(EXPLORATION_PROB),
//			moveSimulator->getDiscountFactor());
//
//	// STEP 4. Link all together with an algorithm
//	RLAlg* alg = new GOALPomdp(controller, moveSimulator, LAMBDA, STEP_SIZE);
//
//	// Indicate to automatically save the parameters whenever performance improves
//	if (argc > 1)
//		alg->saveBest(argv[1]);
//
//	// STEP 5. Learn!
//	alg->learn(EPOCH_STEPS, MAX_TIME, MAX_STEPS);

	// Para todos os robôs.
	estrategia.parar();
	sleep(1);

	// Finaliza thread de comunicação.
	com.finaliza();

	return 0;
}
