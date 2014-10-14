/*
 * MoveRLSimulator.hpp
 *
 *  Created on: Oct 13, 2014
 *      Author: eraldo
 */

#ifndef MOVERLSIMULATOR_HPP_
#define MOVERLSIMULATOR_HPP_

#include <opencv2/opencv.hpp>

#include <PGBasics.hh>
#include <Simulator.hh>

#include <cstdlib>
#include <cmath>
#include <time.h>

#include "Vector.hpp"
#include "Funcoes.hpp"
#include "Configuracoes.hpp"

using namespace libpg;

/*
 * LSPI Specific settings
 */

// Probability of a random action
#define EXPLORATION_PROB 0.1
/*
 * For NAC, start with 0.8. Always stay less than
 * 1.0 Increase slowly only if you're not getting a
 * good solution. If you're getting no improvement
 * at all start by increasing the step size below.
 * For LSPI, start with 0.0 and try values up to an
 * including 1.0
 */
#define LAMBDA 0.8

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
#define STEP_SIZE 0.01

// Total max optimisation steps, 0=unbounded
#define MAX_STEPS 0
// Total max wall clock time, 0=unbounded
#define MAX_TIME 0

#define MIN_X 0
#define MAX_X 640
#define MIN_Y 0
#define MAX_Y 480

/*
 * Steps per epoch (progress print outs/LSTDQ steps)
 * Increase this if youre getting too much information.
 * Decrease this if things are very slow, but not
 * too small for LSPI otherwise it will
 * increase variance.
 */
#define EPOCH_STEPS 1000

// Tamanho do passo da velocidade angular.
#define VEL_ANG_PASSO (M_PI/8)

// Número de velocidades angulares positivas (e negativas).
#define MAX_VEL_ANG 5

/**
 * Rotaciona ponto dado ao redor da origem (0,0).
 */
CvPoint2D32f& rot(CvPoint2D32f& ponto, float ang) {
	float c = cosf(ang);
	float s = sinf(ang);
	ponto.x = ponto.x * c - ponto.y * s;
	ponto.y = ponto.y * c + ponto.x * s;
	return ponto;
}

/**
 * Rotaciona ponto dado ao redor do centro dado.
 */
CvPoint2D32f& rot(CvPoint2D32f& ponto, const CvPoint2D32f& centro, float ang) {
	float c = cosf(ang);
	float s = sinf(ang);
	ponto.x = (ponto.x - centro.x) * c - (ponto.y - centro.y) * s + centro.x;
	ponto.y = (ponto.y - centro.y) * c + (ponto.x - centro.x) * s + centro.y;
	return ponto;
}

/**
 * Simula um robô.
 */
class RoboSim {
private:
	// Ponto central do robô.
	CvPoint2D32f centro;

	// Ponto à frente do robô.
	CvPoint2D32f frente;

	// Raio do robô.
	int raio;

	// Multiplicador da velocidade angular do motor esquerdo.
	int velAngL;

	// Multiplicador da velocidade angular do motor direito.
	int velAngR;

public:
	/**
	 * Construtor padrão.
	 */
	RoboSim(int raio) :
			raio(raio), velAngL(0), velAngR(0) {
		centro.x = centro.y = 100;
		frente.x = 100;
		frente.y = 100 + raio;
	}

	/**
	 * Atualiza posição do robô de acordo com as velocidades de seus motores e
	 * o passo dado.
	 */
	void move(float passo = 1.0) {
		CvPoint2D32f frente = this->frente;
		CvPoint2D32f centro = this->centro;

		// Ponto de tração do motor esquerdo.
		CvPoint2D32f pontoLeft;
		pontoLeft.x = -(frente.y - centro.y) + centro.x;
		pontoLeft.y = frente.x - centro.x + centro.y;
		// Ponto de tração do motor direito.
		CvPoint2D32f pontoRight;
		pontoRight.x = frente.y - centro.y + centro.x;
		pontoRight.y = -(frente.x - centro.x) + centro.y;
		// Move frente e centro de acordo com motor esquerdo.
		rot(frente, pontoRight, (-1) * VEL_ANG_PASSO * velAngL * passo);
		rot(centro, pontoRight, (-1) * VEL_ANG_PASSO * velAngL * passo);
		// Move frente e centro de acordo com motor direito.
		rot(frente, pontoLeft, VEL_ANG_PASSO * velAngR * passo);
		rot(centro, pontoLeft, VEL_ANG_PASSO * velAngR * passo);

		// Verifica limites.
		if (frente.x >= MIN_X + raio && frente.x <= MAX_X - raio
				&& frente.y >= MIN_Y + raio && frente.y <= MAX_Y - raio
				&& centro.x >= MIN_X + raio && centro.x <= MAX_X - raio
				&& centro.y >= MIN_Y + raio && centro.y <= MAX_Y - raio) {
			frente.x -= centro.x;
			frente.y -= centro.y;
			float l = sqrt(frente.x * frente.x + frente.y * frente.y);
			frente.x *= (raio / l);
			frente.y *= (raio / l);
			frente.x += centro.x;
			frente.y += centro.y;
			this->frente = frente;
			this->centro = centro;

		}
	}

	bool incVelRight() {
		if (velAngR < MAX_VEL_ANG) {
			++velAngR;
			return true;
		}

		return false;
	}

	bool decVelRight() {
		if (velAngR > -MAX_VEL_ANG) {
			--velAngR;
			return true;
		}

		return false;
	}

	bool incVelLeft() {
		if (velAngL < MAX_VEL_ANG) {
			++velAngL;
			return true;
		}

		return false;
	}

	bool decVelLeft() {
		if (velAngL > -MAX_VEL_ANG) {
			--velAngL;
			return true;
		}

		return false;
	}

	const CvPoint2D32f& getCentro() {
		return centro;
	}

	const CvPoint2D32f& getFrente() {
		return frente;
	}

};

/*
 * Simulador de Reinforcemente Learning que gera pontos para o robô perseguir.
 */
class MoveRLSimulator: public libpg::Simulator {

private:

	/*
	 * Tolerância para considerar que o robô chegou no target.
	 */
	static const int TOL = 10;

public:

	int distTarget;

// Estado anterior.
	double angToTarget;
	double distToTarget;
	double angSpeed;
	double spatialSpeed;

	CvPoint target;

	RoboSim robo;

	double reward;

	CvMat* frame;

	/**
	 * Constructor for your simulator. Do any once off
	 * initialisation. Read data files, allocate memory, stuff like that.
	 * Can be left empty if need be.
	 */
	MoveRLSimulator() :
			distTarget(50), angToTarget(0), distToTarget(0), angSpeed(0), spatialSpeed(
					0), robo(30), reward(0) {
		srand(time(NULL));
		// Cria uma janela para exibir o frame.
		cvNamedWindow("Color", CV_WINDOW_AUTOSIZE);
		// Captura frame atual em RGB.
		frame = cvCreateMat(480, 640, CV_8UC3);
	}

	// Empty desctructor. Fill in if you need to deallocate stuff.
	virtual ~MoveRLSimulator() {
		free(frame);
	}

	void geraTarget() {
		// Gera primeiro target.
		target.x = robo.getCentro().x;
		target.y = robo.getCentro().y;
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
	void getReward(Vector& rewards) {
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
	void getObservation(Observation& obs) {

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
	int doAction(Vector& action) {

		int a = (int) action[0];

		/* Altera estado do robo de acordo com a action. */
		int left = a / 3;
		int right = a % 3;

		if (left == 1)
			robo.decVelLeft();
		else if (left == 2)
			robo.incVelLeft();

		if (right == 1)
			robo.decVelRight();
		else if (right == 2)
			robo.incVelRight();

		// Captura novo estado (visao) e atualiza estado interno do simulador.
		atualizaEstado();

		// Centro e frente do robô sendo treinado.
		CvPoint centroRobo;
		centroRobo.x = robo.getCentro().x;
		centroRobo.y = robo.getCentro().y;
		CvPoint frenteRobo;
		frenteRobo.x = robo.getFrente().x;
		frenteRobo.y = robo.getFrente().y;

		// Exibe target.
		CvPoint um, dois;
		um.x = um.y = 0;
		dois.x = 640;
		dois.y = 480;
		cvRectangle(frame, um, dois, cvScalar(0, 0, 0), -1, 1, 0);
		cvCircle(frame, target, 5, cvScalar(0, 255, 0), -1, 1, 0);
		cvCircle(frame, centroRobo, 5, cvScalar(255, 0, 0), -1, 1, 0);
		cvLine(frame, centroRobo, frenteRobo, cvScalar(255, 0, 0), 3, 1, 0);

		// Desenha figura.
		cvShowImage("Color", frame);

		// Verifica se chegou no target.
		if (abs(target.x - centroRobo.x) < MoveRLSimulator::TOL
				&& abs(target.y == centroRobo.y) < MoveRLSimulator::TOL) {
			reward = 1.0;

			/* Condicao de parada (tecla esc) */
			if (cvWaitKey(500) > 0) {
				if ((cvWaitKey() & 255) == 27)
					exit(1);
			}

			// Gera nova target.
			geraTarget();

			// Sinaliza final do episódio.
			return 1;
		}

		if (cvWaitKey(50) > 0) {
			if ((cvWaitKey() & 255) == 27)
				exit(1);
		}

		reward = -1.0;
		return 0; // This indicates goal state not encountered.
		// Always return 0 for infinite horizon.
		// This actually won't be checked by TemplateRLApp
		// Unless you change which RLAlg is used.
	}

	void atualizaEstado() {
		// Captura frame e o processa.
		robo.move();

		// Centro e frente do robô sendo treinado.
		CvPoint centroRobo;
		centroRobo.x = robo.getCentro().x;
		centroRobo.y = robo.getCentro().y;
		CvPoint frenteRobo;
		frenteRobo.x = robo.getFrente().x;
		frenteRobo.y = robo.getFrente().y;

		// Atualiza estado (observation).
		double angToTargetNew = produtoEscalar(frenteRobo, centroRobo,
				target) - M_PI;
		double distToTargetNew = sqrt(
				(centroRobo.x - target.x) * (centroRobo.x - target.x)
						+ (centroRobo.y - target.y)
								* (centroRobo.y - target.y));
		angSpeed = angToTargetNew - angToTarget;
		spatialSpeed = distToTargetNew - distToTarget;
		angToTarget = angToTargetNew;
		distToTarget = distToTargetNew;
	}

	/**
	 * This defines how many elements there are in your observation vector.
	 * This can be as big as you want. Minimum 1 element.
	 *
	 * If you in an MDP setting, you can just say getObsRows=1 and you
	 * put the state-id in that element. But see warning above.... you
	 * need to edit TempateRLApp.cc to do this properly.
	 */
	int getObsRows() {
		return 5;
	}

	/**
	 * Return the number of actions the simulator knows about.
	 * The controller will only produce actionID's between
	 * 0 and getNumActions()-1
	 */
	int getNumActions() {
		return 9;
	}

	/**
	 * The discount factor for this problem. If you don't know what a discount factor is
	 * then read Barto and Sutton "Reinforcement Learning: An introduction", 1998.
	 * Note this is not part of the standard simulator interface, but many RL people
	 * expect the discount factor to be defined as part of the problem. For infinite
	 * horizon settings (assumed by TemplateRLApp) this needs to be between [0,1).
	 * Do not choose 1.0!
	 */
	double getDiscountFactor() {
		return 0.9;

	}

	/***************************************************************
	 * You don't need to edit any lower than this line for the
	 * single-agent case.
	 ***************************************************************/

	/**
	 * Get the dimensionality of the reward.
	 * Hard wired to 1 for single agent case.
	 * Only different if multi-agents and each
	 * agent generates its own local reward.
	 */
	int getRewardDim() {
		return 1;
	}

	/**
	 * Get the total number of agents. Hard wired to 1 here.
	 */
	int getAgents() {
		return 1;
	}

	/**
	 * Maximum episode length in steps. For planning mode. Only used
	 * by GOALPomdp at the moment.
	 * Do not change.
	 */
	int getMaxEpisodeLength() {
		return 0;
	}

	/**
	 * Get the number of columns in the observation matrix Typically
	 * use multiple columns to provide a different observation to each
	 * agent, so the number of columns is the same as getAgents()
	 */
	int getObsCols() {
		return getAgents();
	}

	/**
	 * Get the total action dimensionality. Often, but not
	 *  necessarily, the same as the number of agents.
	 */
	int getActionDim() {
		return getAgents();
	}

};

#endif /* MOVERLSIMULATOR_HPP_ */
