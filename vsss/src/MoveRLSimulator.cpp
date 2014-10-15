/*
 * MoveRLSimulator.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: eraldo
 */

#include "MoveRLSimulator.hpp"

#include <cmath>

int RoboSim::velMaxMotor = 1;
double RoboSim::passoMotor = 1.0/8;

/*******************************************************************************
 * Auxiliary functions.
 ******************************************************************************/

/**
 * Rotaciona ponto dado ao redor da origem (0,0).
 */
template<class T>
Point_<T>& rot(Point_<T>& ponto, float ang) {
	float c = cosf(ang);
	float s = sinf(ang);
	ponto.x = ponto.x * c - ponto.y * s;
	ponto.y = ponto.y * c + ponto.x * s;
	return ponto;
}

/**
 * Rotaciona ponto dado ao redor do centro dado.
 */
template<class T>
Point_<T>& rot(Point_<T>& ponto, const Point_<T>& centro, float ang) {
	float c = cosf(ang);
	float s = sinf(ang);
	ponto.x = (ponto.x - centro.x) * c - (ponto.y - centro.y) * s + centro.x;
	ponto.y = (ponto.y - centro.y) * c + (ponto.x - centro.x) * s + centro.y;
	return ponto;
}

/*******************************************************************************
 * RoboSim methods.
 ******************************************************************************/
RoboSim::RoboSim(int raio, const Point2f& centro, const Rect& limiteCentro) :
		centro(centro), frente(centro.x, centro.y + raio), raio(raio), velAngL(
				0), velAngR(0), limiteCentro(limiteCentro) {
	// Rotaciona frente aleatoriamente.
	rot(frente, centro, float(rand() * 2 * M_PI / RAND_MAX));
}

void RoboSim::move(float passo) {
	Point2f frente = this->frente;
	Point2f centro = this->centro;

	// Ponto de tração do motor esquerdo.
	Point2f pontoLeft;
	pontoLeft.x = -(frente.y - centro.y) + centro.x;
	pontoLeft.y = frente.x - centro.x + centro.y;
	// Ponto de tração do motor direito.
	Point2f pontoRight;
	pontoRight.x = frente.y - centro.y + centro.x;
	pontoRight.y = -(frente.x - centro.x) + centro.y;
	// Move frente e centro de acordo com motor esquerdo.
	rot(frente, pontoRight, (-1) * RoboSim::passoMotor * velAngL * passo);
	rot(centro, pontoRight, (-1) * RoboSim::passoMotor * velAngL * passo);
	// Move frente e centro de acordo com motor direito.
	rot(frente, pontoLeft, RoboSim::passoMotor * velAngR * passo);
	rot(centro, pontoLeft, RoboSim::passoMotor * velAngR * passo);

	// Verifica limites.
	if (limiteCentro.contains(centro)) {
		frente -= centro;
		float l = norm(frente);
		frente *= (raio / l);
		frente += centro;
		this->frente = frente;
		this->centro = centro;
	}
}

/*******************************************************************************
 * MoveRLSimulator methods.
 ******************************************************************************/
MoveRLSimulator::MoveRLSimulator(bool useUI, int raioRobo, int toleranciaAlvo,
		int tamArena, double discountFactor, int velMaxMotor, double passoMotor) :
		distTarget(50), angToTarget(0), distToTarget(0), angSpeed(0), spatialSpeed(
				0), reward(0), frame(tamArena, tamArena,
		CV_8UC3), useUI(useUI), toleranciaAlvo(toleranciaAlvo), arena(0, 0,
				tamArena, tamArena), discountFactor(discountFactor) {
	srand(time(NULL));

	RoboSim::velMaxMotor = velMaxMotor;
	RoboSim::passoMotor = passoMotor;

	limiteCentroRobo = arena;
	limiteCentroRobo.x += robo.getRaio();
	limiteCentroRobo.y += robo.getRaio();
	limiteCentroRobo.width -= 2 * robo.getRaio();
	limiteCentroRobo.height -= 2 * robo.getRaio();

	// Posiciona robo no centro da arena.
	robo = RoboSim(raioRobo, Point2f(tamArena / 2, tamArena / 2),
			limiteCentroRobo);
	// Gera primeiro target.
	geraTarget();

	if (useUI) {
		// Cria uma janela para exibir o frame.
		cvNamedWindow("Color", CV_WINDOW_AUTOSIZE);
	}
}

MoveRLSimulator::~MoveRLSimulator() {
}

void MoveRLSimulator::geraTarget() {
	do {
		// Gera primeiro target.
		target.x = robo.getCentro().x;
		target.y = robo.getCentro().y;
		// Sorteia um angulo.
		Vector2D v(distTarget, 0);
		v = v.rotateLeft(float((double(rand()) / RAND_MAX) * 2 * M_PI));
		target.x += v.x;
		target.y += v.y;
	} while (!limiteCentroRobo.contains(target));
}

void MoveRLSimulator::getReward(libpg::Vector& rewards) {
	rewards[0] = reward;
}

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

int MoveRLSimulator::doAction(libpg::Vector& action) {

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

	if (useUI) {

		// Limpa frame.
		rectangle(frame, arena, cvScalar(0, 0, 0), -1, 1, 0);
		// Exibe limite de movimento do robô.
		rectangle(frame, limiteCentroRobo, cvScalar(255, 255, 255), 1, 1, 0);
		// Exibe target.
		circle(frame, target, 5, cvScalar(0, 255, 0), -1, 1, 0);
		// Exibe tolerância do target.
		rectangle(frame,
				Rect(target.x - toleranciaAlvo, target.y - toleranciaAlvo,
						2 * toleranciaAlvo, 2 * toleranciaAlvo),
				cvScalar(255, 255, 255), 1, 1, 0);
		// Exibe centro do robô.
		circle(frame, centroRobo, 5, cvScalar(255, 0, 0), -1, 1, 0);
		// Exibe frente e lados do robô.
		line(frame, centroRobo, frenteRobo, cvScalar(255, 0, 0), 3, 1, 0);
		line(frame, centroRobo,
				Point(-(frenteRobo.y - centroRobo.y) + centroRobo.x,
						frenteRobo.x - centroRobo.x + centroRobo.y),
				cvScalar(255, 0, 0), 3, 1, 0);
		line(frame, centroRobo,
				Point(frenteRobo.y - centroRobo.y + centroRobo.x,
						-(frenteRobo.x - centroRobo.x) + centroRobo.y),
				cvScalar(255, 0, 0), 3, 1, 0);

		// Desenha figura.
		imshow("Color", frame);
	}

	// Verifica se chegou no target.
	if (abs(target.x - centroRobo.x) < toleranciaAlvo
			&& abs(target.y - centroRobo.y) < toleranciaAlvo) {
		reward = 1.0;

		if (useUI) {
			/* Condicao de parada (tecla esc) */
			if (cvWaitKey(500) > 0) {
				if ((cvWaitKey() & 255) == 27)
					exit(1);
			}
		}

		// Gera nova target.
		geraTarget();

		// Sinaliza final do episódio.
		return 1;
	}

	if (useUI) {
		if (cvWaitKey(50) > 0) {
			if ((cvWaitKey() & 255) == 27)
				exit(1);
		}
	}

	reward = -1.0;
	return 0; // This indicates goal state not encountered.
	// Always return 0 for infinite horizon.
	// This actually won't be checked by TemplateRLApp
	// Unless you change which RLAlg is used.
}

void MoveRLSimulator::atualizaEstado() {
	// Captura frame e o processa.
	robo.move();

	/*
	 * Atualiza estado (observation).
	 */

	// Ângulo entre frente do robô e o target (temporário).
	Point2f vecFrente = robo.getFrente() - robo.getCentro();
	Point2f vecTarget = target - robo.getCentro();
	double angToTargetNew = atan2f(vecTarget.y, vecTarget.x)
			- atan2f(vecFrente.y, vecFrente.x);
	// Distância para o target (temporário).
	double distToTargetNew = norm(robo.getCentro() - target);
	// Velocidade angular do robô.
	angSpeed = angToTargetNew - angToTarget;
	// Velocidade linear.
	spatialSpeed = distToTargetNew - distToTarget;

	// Atualiza valores definitivos.
	angToTarget = angToTargetNew;
	distToTarget = distToTargetNew;
}

int MoveRLSimulator::getObsRows() {
	return 5;
}

int MoveRLSimulator::getNumActions() {
	return 9;
}

double MoveRLSimulator::getDiscountFactor() {
	return 0.9;
}

int MoveRLSimulator::getRewardDim() {
	return 1;
}

int MoveRLSimulator::getAgents() {
	return 1;
}

int MoveRLSimulator::getMaxEpisodeLength() {
	return 0;
}

int MoveRLSimulator::getObsCols() {
	return getAgents();
}

int MoveRLSimulator::getActionDim() {
	return getAgents();
}
