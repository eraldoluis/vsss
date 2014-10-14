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

#include <PGBasics.hh>
#include <NeuralNetBatch.hh>
#include <LSTDQController.hh>
#include <OLPomdp.hh>
#include <GOALPomdp.hh>
#include <eGreedyPolicy.hh>

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
#include "MoveRLInterface.hpp"
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
	if (!visao.leValoresHSV("../vsss/conf/hsv.arff")) {
		fprintf(stderr, "Erro ao ler valores do HSV!");
		return 1;
	}

	// Lê pontos extremos do campo.
	if (!visao.lePontosDoCampo("../vsss/conf/marc.txt")) {
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

	Robo& robo = visao.ROBO_TRES;

	// STEP 1. Create simulator
	MoveRLInterface* moveSimulator = new MoveRLInterface(visao, com, estrategia, robo);

	// Gera estado inicial e primeiro target.
	printf("Esperando qualquer tecla continuar.");
	cvWaitKey(0);
	moveSimulator->atualizaEstado();
	moveSimulator->geraTarget();
	moveSimulator->atualizaEstado();

	// STEP 2. Create linear approximator (see documentation to change this to a multi-layer perceptron)
	Approximator* neuralNet = new NeuralNetBatch(moveSimulator->getObsRows(),
			moveSimulator->getNumActions());

	// STEP 3. Create a controller (implement different algorithms here)
	Controller* controller;
	// Create Natural Actor-Critic controller
	// controller = new NACTransform(new BasicController(neuralNet), mySimulator->getDiscountFactor());
	// Create LSPI controller
	controller = new LSTDQController(neuralNet,
			new eGreedyPolicy(EXPLORATION_PROB),
			moveSimulator->getDiscountFactor());

	// STEP 4. Link all together with an algorithm
	RLAlg* alg = new GOALPomdp(controller, moveSimulator, LAMBDA, STEP_SIZE);

	// Indicate to automatically save the parameters whenever performance improves
	if (argc > 1)
		alg->saveBest(argv[1]);

	// STEP 5. Learn!
	alg->learn(EPOCH_STEPS, MAX_TIME, MAX_STEPS);

	delete alg;
	delete controller;
	delete neuralNet;
	delete moveSimulator;

	// Para todos os robôs.
	estrategia.parar();
	sleep(1);

	// Finaliza thread de comunicação.
	com.finaliza();

	return 0;
}
