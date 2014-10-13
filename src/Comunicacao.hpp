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

#include "Robo.hpp"

class Comunicacao: public Robo {
private:
	int fd;
	unsigned char* byte;
	unsigned char* copia;

public:
	// Flag que indica quando enviar novo pacote.
	static bool enviarPacote;

	/* Construtor */
	Comunicacao();

	void abrePortaSerialComunicacao();

	void configuraComunicacao();

	void enviaDadosRobo(Robo** time);

	int getFileDescriptor();

};

#endif //__Comunicacao_cpp
