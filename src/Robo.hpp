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

#ifndef __robo_hpp
#define __robo_hpp

#include <cstdio>
#include <opencv2/opencv.hpp>

#include "CentroCor.hpp"
#include "TabelaCor.hpp"

class Motor {
private:
	int velocidade;
	int sentido;

	friend class Robo;
	friend class Strategy;

public:
	/* Construtor */
	Motor();
};

class Robo: public CentroCor {
private:
	CvPoint frenteRobo;
	CvPoint centroAtualRobo;
	CvPoint ladoRobo;
	CvPoint centroAnteriorRobo;

	unsigned char motor;
	int frames;
	int liberar;
	int comandoAnteriorRobo;

	int corTime;
	int corRobo;
	int idRobo;

	double tempoAnteriorRobo;
	double tempoAtualRobo;

	Motor motorDireitoRobo;
	Motor motorEsquerdoRobo;

	bool roboPerdido;
	bool roboPreso;

public:
	bool mira, chute;
	int countCommands;
	bool segundoPonto;
	int flag;

	int qtdAnguloIgual;
	int qtdFrameParado;
	int qtdvolta;

	bool volta;
	CvPoint pontochute;
	CvPoint costas;

	static int xInferior;
	static int yInferior;
	static int xSuperior;
	static int ySuperior;

	Robo();

	Robo(int id, int time, int corRobo);

	unsigned char getMotorRobo();

	CvPoint getFrenteRobo();

	CvPoint getCentroAtualRobo();

	CvPoint getLadoRobo();

	int getCorTime();

	int getCorRobo();

	bool getRoboPerdido();

	//Setters
	void setIdRobo(int idRobo);

	void setFenteRobo(CvPoint ponto);

	void setcentroAtualRobo(CvPoint ponto);

	void setCorTimeRobo(int corTime);

	void setCorRobo(int corRobo);

	void setLadoRobo(CvPoint ladoRobo);

	void setMotorRobo();

	void setTempoAnteriorRobo(double tempoAnterior);

	void setTempoAtualRobo(double tempoAtual);

	void setRoboPerdido(bool roboPerdido);

	int procuraRobo(TabelaCor* centrosPontos, IplImage* frame);

	float distancia(CvPoint pontoUm, CvPoint pontoDois);

	float distanciaPara(CvPoint ponto);

	int getVelocidade(float distancia);

	int getVelocidadeAngular(float angulo, int distancia = 0);

	int getSentidoRodas(CvPoint alvo, bool goleiro = false);

	float getErroAngulo(CvPoint alvo);

	void chuteGoleiro();

	void pararRobo(bool goleiro = false);

	CvPoint getPontoRef(CvPoint alvo, CvPoint direcao);

	void evitarColisao(CvPoint* ponto);

	CvPoint calculaPontoRef(CvPoint alvo, CvPoint ladoBola, CvPoint bola,
			CvPoint centroGolAdversario, CvPoint metadeCampo);

	float getAngleDiff(CvPoint alvo, CvPoint bola);

	void verificaRoboPreso();

	int liberarRobo(CvPoint bola, CvPoint golAdversario, bool goleiro = false);

	bool foraDaArea(CvPoint trave);

	bool seguirPonto(CvPoint metadeCampoBaixo, CvPoint alvo, CvPoint trave,
			CvPoint golAdversario, bool goleiro = false, bool zagueiro = false,
			float angulo = 0);

	bool andarReto(CvPoint alvo, bool goleiro = false, bool foraDoGol = false,
			bool zagueiro = false);

	bool alinharGoleiro(float angulo, CvPoint alvo, CvPoint trave,
			bool foraDoGol = false, bool fundoDoGol = false);

	bool alinhar(CvPoint bola, CvPoint golAdversario);

	void chutar();

	bool emMovimento();

	void traduzirComandos();

};

#endif //__robo_hpp
