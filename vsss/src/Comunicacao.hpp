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

#ifndef __Comunicacao_cpp
#define __Comunicacao_cpp

#include <string>
#include <pthread.h>

#include "Robo.hpp"

/**
 * Envia para todos os robôs do time as velocidades de cada motor. Coordena uma
 * thread que envia os comandos atuais a cada 9 milisegundos. Este envio é
 * constante, mesmo que as velocidades dos motores não tenham sido alteradas,
 * para evitar que a conexão com os robôs caia.
 */
class Comunicacao: public Robo {
private:
	/*
	 * ID da thread.
	 */
	pthread_t tid;

	pthread_mutex_t bufferLock;

	/*
	 * File descriptor da porta de comunicação.
	 */
	int fd;

	/*
	 * Buffer de envio.
	 */
	unsigned char byte[4];

	/*
	 * Endereço do dispositivo que representa a porta de comunicação.
	 */
	std::string porta;

	/*
	 * Flag para indicar a finalização da thread.
	 */
	bool finalizada;

	/*
	 * Abre porta de comunicação com os robôs.
	 */
	void abrePortaSerialComunicacao();

	/**
	 * Configura parâmetros de comunicação.
	 */
	void configuraComunicacao();

	/*
	 * Loop principal da thread.
	 */
	void loop();

	/*
	 * Função ponte.
	 */
	friend void* bridge(void* args);

public:
	/* Construtor */
	Comunicacao(const char* porta);

	~Comunicacao();

	/*
	 * Inicia thread de comunicação.
	 */
	int inicia();

	/*
	 * Preenche o buffer com os dados do time enviado. No próximo envio, estes
	 * dados serão usados.
	 */
	void enviaDadosRobo(Robo** time);

	/*
	 * Sinaliza thread de comunicação para finalizar e espera até que seja
	 * finalizada.
	 */
	void finaliza();

	/*
	 * Retorna o identificador da porta de comunicação aberta.
	 */
	int getFileDescriptor();

};

inline
int Comunicacao::getFileDescriptor() {
	return fd;
}

#endif //__Comunicacao_cpp
