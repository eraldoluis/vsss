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
#include <iostream>

#include <PGBasics.hh>
#include <NeuralNetBatch.hh>
#include <LSTDQController.hh>
#include <OLPomdp.hh>
#include <GOALPomdp.hh>
#include <eGreedyPolicy.hh>

#include "MoveRLSimulator.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace std;

/* Main do software */
int main(int argc, char* argv[]) {

	// Probability of a random action
	double explorationProb = 0.1;

	/*
	 * LSPI Specific settings
	 */

	/*
	 * For NAC, start with 0.8. Always stay less than
	 * 1.0 Increase slowly only if you're not getting a
	 * good solution. If you're getting no improvement
	 * at all start by increasing the step size below.
	 * For LSPI, start with 0.0 and try values up to an
	 * including 1.0
	 */
	double lambda = 0.8;

	/*
	 * Natural actor-critic specific settings
	 */

	/*
	 * Start small and grow by 10x until things
	 * become unstable during optimisation. If
	 * nothing at all seems to change, this value
	 * might need to be much bigger.
	 * If things improve quickly at the start, then
	 * the reward suddenly gets much worse, make
	 * this smaller.
	 */
	double stepSize = 0.01;

	// Total max optimization steps, 0=unbounded
	int maxSteps = 0;
	// Total max wall clock time, 0=unbounded
	int maxTime = 0;

	/*
	 * Steps per epoch (progress print outs/LSTDQ steps)
	 * Increase this if youre getting too much information.
	 * Decrease this if things are very slow, but not
	 * too small for LSPI otherwise it will
	 * increase variance.
	 */
	int epochSteps = 1000;

	bool useUI = false;
	int raioRobo = 30;
	int toleranciaAlvo = 10;
	int tamArena = 600;
	double discountFactor = 0.9;
	int velMaxMotor = 1;
	double passoMotor;
	string outParams;

	int opt;
	po::options_description desc("Opções");
	desc.add_options()("help,h", "exibe ajuda")
	//
	("expProb", po::value<double>(&explorationProb)->default_value(0.1),
			"eGreedy exploration probability")
	//
	("lambda", po::value<double>(&lambda)->default_value(0.8),
			"eligibility trace")
	//
	("alpha", po::value<double>(&stepSize)->default_value(0.01),
			"learning step size")
	//
	("maxSteps", po::value<int>(&maxSteps)->default_value(0),
			"Maximum training steps (0 = unbounded)")
	//
	("maxTime", po::value<int>(&maxTime)->default_value(0),
			"Maximum training time in seconds (0 = unbounded)")
	//
	("epoch", po::value<int>(&epochSteps)->default_value(1000),
			"Number of steps in one epoch")
	//
	("ui", po::value<bool>(&useUI)->default_value(false),
			"Exibe janela de interface")
	//
	("raioRobo", po::value<int>(&raioRobo)->default_value(30), "Raio do robô")
	//
	("tolAlvo", po::value<int>(&toleranciaAlvo)->default_value(10),
			"Tolerância para alcançar o alvo")
	//
	("tamArena", po::value<int>(&tamArena)->default_value(600),
			"Tamanho da arena (largura e comprimento")
	//
	("discFactor", po::value<double>(&discountFactor)->default_value(1.0),
			"discount factor for rewards (to form the expected return)")
	//
	("velMaxMotor", po::value<int>(&velMaxMotor)->default_value(1),
			"Velocidade máxima dos motores")
	//
	("passoMotor", po::value<double>(&passoMotor)->default_value(1.0 / 8),
			"tamanho do passo dos motores")
	//
	("outParams", po::value<string>(&outParams),
			"arquivo para salvar parâmetros da melhor política")
			//
			;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << "\n";
		return 0;
	}

	// STEP 1. Create simulator
	MoveRLSimulator* moveSimulator = new MoveRLSimulator(useUI, raioRobo,
			toleranciaAlvo, tamArena, discountFactor, velMaxMotor, passoMotor);

	// STEP 2. Create linear approximator (see documentation to change this to a multi-layer perceptron)
	Approximator* neuralNet = new NeuralNetBatch(moveSimulator->getObsRows(),
			moveSimulator->getNumActions());

	// STEP 3. Create a controller (implement different algorithms here)
	Controller* controller;
	// Create Natural Actor-Critic controller
	// controller = new NACTransform(new BasicController(neuralNet), mySimulator->getDiscountFactor());
	// Create LSPI controller
	controller = new LSTDQController(neuralNet,
			new eGreedyPolicy(explorationProb),
			moveSimulator->getDiscountFactor());

	// STEP 4. Link all together with an algorithm
	RLAlg* alg = new GOALPomdp(controller, moveSimulator, lambda, stepSize);

	// Indicate to automatically save the parameters whenever performance improves
	if (vm.count("outParams"))
		alg->saveBest((char*) outParams.c_str());

	// STEP 5. Learn!
	alg->learn(epochSteps, maxTime, maxSteps);

	delete alg;
	delete controller;
	delete neuralNet;
	delete moveSimulator;

	return 0;
}
