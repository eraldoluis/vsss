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
#include "Visao.hpp"
#include "Comunicacao.hpp"
#include "Strategy.hpp"

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
/*
 * Steps per epoch (progress print outs/LSTDQ steps)
 * Increase this if youre getting too much information.
 * Decrease this if things are very slow, but not
 * too small for LSPI otherwise it will
 * increase variance.
 */
#define EPOCH_STEPS 100

/*
 * Simulador de Reinforcemente Learning que gera pontos para o robô perseguir.
 */
class MoveRLSimulator: public libpg::Simulator {

private:

	/*
	 * Tolerância para considerar que o robô chegou no target.
	 */
	static const int TOL = 20;

public:

	Visao& visao;
	Comunicacao& com;
	Strategy& estrategia;

	int distTarget;

	// Estado anterior.
	double angToTarget;
	double distToTarget;
	double angSpeed;
	double spatialSpeed;

	CvPoint target;

	Robo& robo;

	double reward;

	/**
	 * Constructor for your simulator. Do any once off
	 * initialisation. Read data files, allocate memory, stuff like that.
	 * Can be left empty if need be.
	 */
	MoveRLSimulator(Visao& visao, Comunicacao& com, Strategy& estrategia, Robo& robo);

	// Empty desctructor. Fill in if you need to deallocate stuff.
	virtual ~MoveRLSimulator();

	void geraTarget();

	/**
	 * Return the reward for the current state, or previous-state and action combination.
	 * In the multi-agent case there is one reward per reward. Here we assume a single agent, so only
	 * a single scalar reward. It's often easier to compute the reward during doAction() and save
	 * it a class variable to return here.
	 * @param rewards vector to put the rewards in.
	 */
	void getReward(Vector& rewards);

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
	void getObservation(Observation& obs);

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
	int doAction(Vector& action);

	void atualizaEstado();

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
