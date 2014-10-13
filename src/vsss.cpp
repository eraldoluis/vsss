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
	Strategy TIME_UM(visao);

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

	if (primeiroTempo) {
		TIME_UM.setcentroGol(visao.centroCampo.devolvePontoPontosCampo(8),
				visao.centroCampo.devolvePontoPontosCampo(11)); //VERMELHO
		TIME_UM.setcentroGolAdversario(
				visao.centroCampo.devolvePontoPontosCampo(2),
				visao.centroCampo.devolvePontoPontosCampo(5)); //AZUL
	} else {
		TIME_UM.setcentroGol(visao.centroCampo.devolvePontoPontosCampo(2),
				visao.centroCampo.devolvePontoPontosCampo(5)); //VERMELHO
		TIME_UM.setcentroGolAdversario(
				visao.centroCampo.devolvePontoPontosCampo(8),
				visao.centroCampo.devolvePontoPontosCampo(11)); //AZUL
	}

	/* Define os pontos de centro do campo, metade de cima, metade de baixo */
	TIME_UM.metadeCampoCima.x = (visao.centroCampo.devolvePontoPontosCampo(1).x
			- visao.centroCampo.devolvePontoPontosCampo(0).x) / 2
			+ visao.centroCampo.devolvePontoPontosCampo(0).x;
	TIME_UM.metadeCampoCima.y = (visao.centroCampo.devolvePontoPontosCampo(1).y
			- visao.centroCampo.devolvePontoPontosCampo(0).y) / 2
			+ visao.centroCampo.devolvePontoPontosCampo(0).y;

	TIME_UM.metadeCampoBaixo.x = (visao.centroCampo.devolvePontoPontosCampo(6).x
			- visao.centroCampo.devolvePontoPontosCampo(7).x) / 2
			+ visao.centroCampo.devolvePontoPontosCampo(7).x;
	TIME_UM.metadeCampoBaixo.y = (visao.centroCampo.devolvePontoPontosCampo(6).y
			- visao.centroCampo.devolvePontoPontosCampo(7).y) / 2
			+ visao.centroCampo.devolvePontoPontosCampo(7).y;

	TIME_UM.metadeTudo.x = (TIME_UM.metadeCampoCima.x
			- TIME_UM.metadeCampoBaixo.x) / 2 + TIME_UM.metadeCampoBaixo.x;
	TIME_UM.metadeTudo.y = (TIME_UM.metadeCampoCima.y
			- TIME_UM.metadeCampoBaixo.y) / 2 + TIME_UM.metadeCampoBaixo.y;

	TIME_UM.calculaPontoAtrasGol();
	TIME_UM.getteam()[1]->chutar();

	/* Programa executando */
	while (1) {

		if (!visao.captura()) {
			fprintf(stderr, "ERROR: frame is null...\n");
			break;
		}

		/* Chama a estrategia caso a bola nao esteja perdida */
		if ((visao.centroPontos.tabelaCores[4] != NULL)) {
			/* Seta os pontos da bola */
			TIME_UM.setBola(visao.centroPontos.tabelaCores[4]->ponto);

			TIME_UM.run(visao.frame, primeiroTempo);

			// Envia novos comandos para os robôs.
			com.enviaDadosRobo(TIME_UM.getteam());

			/* Exibe o ponto da bola */
			cvCircle(visao.frame, visao.centroPontos.tabelaCores[4]->ponto, 0,
					cvScalar(255, 255, 0), 10, 1, 0);

		} else {
			// Envia novos comandos para os robôs.
			com.enviaDadosRobo(TIME_UM.getteam());
			// Para todos os robôs (100 vezes!?).
			for (int i = 0; i < 100; i++)
				TIME_UM.parar();
		}

		/* Exibe os pontos na imagem */
		cvCircle(visao.frame, TIME_UM.getCentroGol(), 0, cvScalar(0, 0, 0), 10, 1, 0);
		cvCircle(visao.frame, TIME_UM.getCentroGolAdversario(), 0,
				cvScalar(0, 255, 255), 10, 1, 0);

		cvCircle(visao.frame, TIME_UM.metadeCampoCima, 0, cvScalar(0, 255, 255), 10,
				1, 0);
		cvCircle(visao.frame, TIME_UM.metadeCampoBaixo, 0, cvScalar(0, 255, 255), 10,
				1, 0);
		cvCircle(visao.frame, TIME_UM.metadeTudo, 0, cvScalar(0, 255, 255), 10, 1, 0);

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

	// STEP 1. Create simulator
	MoveRLSimulator* moveSimulator = new MoveRLSimulator(visao);

	// STEP 2. Create linear approximator (see documentation to change this to a multi-layer perceptron)
	Approximator* neuralNet = new NeuralNetBatch(moveSimulator->getObsRows(),
			moveSimulator->getNumActions());

	// STEP 3. Create a controller (implement different algorithms here)
	Controller* controller;
	//    if (ALGORITHM == NAC) {
	// Create Natural Actor-Critic controller
	//      controller = new NACTransform(new BasicController(neuralNet), mySimulator->getDiscountFactor());
	//   }
	//    else {
	// Create alternative LSPI controller
	controller = new LSTDQController(neuralNet,
			new eGreedyPolicy(EXPLORATION_PROB),
			moveSimulator->getDiscountFactor());
	//    }

	// STEP 4. Link all together with an algorithm
	RLAlg* alg = new GOALPomdp(controller, moveSimulator, LAMBDA, STEP_SIZE);

	// Indicate to automatically save the parameters whenever performance improves
	if (argc > 1)
		alg->saveBest(argv[1]);

	// STEP 5. Learn!
	alg->learn(EPOCH_STEPS, MAX_TIME, MAX_STEPS);

	// Para todos os robôs.
	TIME_UM.parar();
	sleep(1);

	// Finaliza thread de comunicação.
	com.finaliza();

	return 0;
}
