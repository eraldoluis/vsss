/*
 * MoveRLSimulator.cpp
 *
 *  Created on: 13/10/2014
 *      Author: paulo
 */

#include "MoveRLSimulator.hpp"

/**
 * Constructor for your simulator. Do any once off
 * initialisation. Read data files, allocate memory, stuff like that.
 * Can be left empty if need be.
 */
MoveRLSimulator::MoveRLSimulator(Visao& visao, Comunicacao& com,
		Strategy& estrategia, Robo& robo) :
	visao(visao), com(com), estrategia(estrategia), distTarget(50),
			angToTarget(0), distToTarget(0), angSpeed(0), spatialSpeed(0),
			robo(robo), reward(-1) {
	srand(time(NULL));
}

// Empty desctructor. Fill in if you need to deallocate stuff.
MoveRLSimulator::~MoveRLSimulator() {
}

void MoveRLSimulator::geraTarget() {
	// Gera primeiro target.
	target = robo.getCentroAtualRobo();
	// Sorteia um angulo.
	Vector2D v(distTarget, 0);
	v = v.rotateLeft(float((double(rand()) / RAND_MAX) * 2 * M_PI));
	target.x += v.x;
	target.y += v.y;
}

/**
 * Return the reward for the current state, or previous-state and action combination.
 * In the multi-agent case there is one reward per reward. Here we assume a single agent, so only
 * a single scalar reward. It's often easier to compute the reward during doAction() and save
 * it a class variable to return here.
 * @param rewards vector to put the rewards in.
 */
void MoveRLSimulator::getReward(Vector& rewards) {
	rewards[0] = reward;
}

/**
 * Fill in a description of the current state. Think of the
 * observation for a single agent as being a single column matrix of
 * dimension getObsRows()*1.
 *
 * An observation is sometimes refered to as a basis feature.
 *
 * It's up to you to define below in
 * getObsRows() how big the observation is. Making it really big
 * can degrade performance because more parameters are required.
 * Choose your observation features carefully.
 *
 * One tip is to try and normalise each element of your observation
 * to be between -1 and 1. Another tip is to make sure at least
 * one element of the observation is a constant 1.0 to provide
 * bias to the neural network approximator (don't worry if you don't
 * know what this means, just do it.)
 *
 * In an MDP setting there would typically be only one
 * element in the observation, the state-ID. This is not
 * appropriate for TemplateRLApp. Use a state basis feature
 * instead. If you want to use state-IDs for observations, edit
 * TemplateRLApp.cc to use TableLookup controllers instead of
 * NeuralNet controllers.
 *
 * @param obs The observation class to fill in and return
 */
void MoveRLSimulator::getObservation(Observation& obs) {

	// Retrieve the feature matrix from the observation.
	Matrix myObs = obs.getFeatures();

	myObs.clear();
	myObs(0, 0) = 1;
	myObs(1, 0) = angToTarget;
	myObs(2, 0) = distToTarget;
	myObs(3, 0) = angSpeed;
	myObs(4, 0) = spatialSpeed;

	// Note, don't do anything that affects the state of the
	// simulator. You never know if getObservation() will
	// be called more than once between doAction()

}

/**
 * Do the action passed in. Again, in the single agent case the action vector
 * will always be 1x1, and contain an integer valued action.
 * You should take this action and update the state of the simulator
 * accordingly. You can add state variables in MySimulator.hh
 * IMPORTANT: For this template I assume an infinite-horizon process.
 * This means the simulation does not terminate. If your problem
 * is finite-horizon (it has terminal state or goals), then it's your job
 * to reset the state to the initial state when you encounter a terminal
 * state here. By editing TemplateRLApp.cc you can choose GoalPOMDP as the
 * top level algorithm appropriate for episodic cases.
 * @param action The 1x1 vector containing the action to do.
 * @return 1 for goal state (ignored in TemplateRLApp).
 */
int MoveRLSimulator::doAction(Vector& action) {

	int a = (int) action[0];

	/* Altera estado do robo de acordo com a action. */
	int left = a / 3;
	int right = a % 3;

	if (left == 1)
		robo.diminuiVelocidadeMotorEsquerdo();
	else if (left == 2)
		robo.aumentaVelocidadeMotorEsquerdo();

	if (right == 1)
		robo.diminuiVelocidadeDireitoRobo();
	else if (right == 2)
		robo.aumentaVelocidadeDireitoRobo();

	robo.traduzirComandos();
	com.enviaDadosRobo(estrategia.getteam());

	// Captura novo estado (visao) e atualiza estado interno do simulador.
	atualizaEstado();

	// Centro do robô sendo treinado.
	const CvPoint& centroRobo = robo.getCentroAtualRobo();
	const CvPoint& frenteRobo = robo.getFrenteRobo();

	// Exibe target.
	cvCircle(visao.frame, target, 5, cvScalar(255, 0, 0), -1, 1, 0);
	cvCircle(visao.frame, centroRobo, 5, cvScalar(0, 255, 0), -1, 1, 0);

	// Desenha figura.
	cvShowImage("Color", visao.frame);

	reward = -1;

	bool onTarget = false;

	// Verifica se chegou no target.
	if (abs(target.x - centroRobo.x) < MoveRLSimulator::TOL && abs(target.y
			- centroRobo.y) < MoveRLSimulator::TOL) {
		// Sinaliza para getReward().
		onTarget = true;
		reward = 1;
		// Gera nova target.
		geraTarget();
	}

	// Limpa a imagem.
	visao.centroPontos.limpaTabelaCor();

	/* Condicao de parada (tecla esc) */
	if ((cvWaitKey(10) & 255) == 27) {
		robo.pararRobo();
		com.enviaDadosRobo(estrategia.getteam());
		printf("Esperando qualquer tecla para continuar.");
		if ((cvWaitKey(0) & 255) == 27)
			exit(1);
		atualizaEstado();
		geraTarget();
		atualizaEstado();
		return 0;
	}

	if (onTarget)
		return 1;
	return 0; // This indicates goal state not encountered.
	// Always return 0 for infinite horizon.
	// This actually won't be checked by TemplateRLApp
	// Unless you change which RLAlg is used.
}

void MoveRLSimulator::atualizaEstado() {
	// Captura frame e o processa.
	visao.captura();

	// Centro do robô sendo treinado.
	const CvPoint& centroRobo = robo.getCentroAtualRobo();
	const CvPoint& frenteRobo = robo.getFrenteRobo();

	// Atualiza estado (observation).
	double angToTargetNew = produtoEscalar(frenteRobo, centroRobo, target)
			- M_PI;
	double distToTargetNew =
			sqrt((centroRobo.x - target.x) * (centroRobo.x - target.x)
					+ (centroRobo.y - target.y) * (centroRobo.y - target.y));
	angSpeed = angToTargetNew - angToTarget;
	spatialSpeed = distToTargetNew - distToTarget;
	angToTarget = angToTargetNew;
	distToTarget = distToTargetNew;
}
