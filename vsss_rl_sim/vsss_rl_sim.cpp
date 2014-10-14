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

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <PGBasics.hh>
#include <NeuralNetBatch.hh>
#include <LSTDQController.hh>
#include <OLPomdp.hh>
#include <GOALPomdp.hh>
#include <eGreedyPolicy.hh>

#include "MoveRLSimulator.hpp"

/* Main do software */
int main(int argc, char* argv[]) {

	// STEP 1. Create simulator
	MoveRLSimulator* moveSimulator = new MoveRLSimulator();

	moveSimulator->geraTarget();

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

	return 0;
}
