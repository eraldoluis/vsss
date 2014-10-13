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

#include "Comunicacao.hpp"

#include <fcntl.h>
#include <termios.h>

/*
 * Função ponte que apenas chama o método loop do objeto de comunicação. No
 * final, finaliza a thread.
 */
void* bridge(void* args) {
	((Comunicacao*) args)->loop();
	pthread_exit(0);
}

Comunicacao::Comunicacao(const char* porta) :
		porta(porta), fd(-1), finalizada(false) {
}

void Comunicacao::loop() {
	// Envia dados enquanto o programa estiver rodando.
	while (!finalizada) {
		write(this->fd, byte, 4);
		usleep(9000);
	}
}

int Comunicacao::inicia() {
	// Inicialização da comunicação via porta serial.
	abrePortaSerialComunicacao();
	configuraComunicacao();

	// Configura thread.
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	finalizada = false;
	return pthread_create(&tid, &attr, bridge, this);
}

void Comunicacao::abrePortaSerialComunicacao() {
	/* File descriptor for the port */
	int fd;
	fd = open(PORTA_SERIAL, O_WRONLY | O_NOCTTY | O_NDELAY); //O_RDWR

	if (fd == -1)
		perror("open_port: Unable to open /dev/ttyS0 - ");
	else
		fcntl(fd, F_SETFL, 0);

	this->fd = fd;
}

void Comunicacao::configuraComunicacao() {
	struct termios options;

	tcgetattr(this->fd, &options);
	cfsetispeed(&options, B4800); //in
	cfsetospeed(&options, B4800); //out
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB; // Mask the character size to 8 bits, no parity
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8; // Select 8 data bits
	options.c_cflag &= ~CRTSCTS; // Disable hardware flow control
	options.c_lflag &= ~(ICANON | ECHO | ISIG);
	options.c_cc[VMIN] = 0; //it will wait for one byte at a time.
	options.c_cc[VTIME] = 10; // it will wait for 0.1s at a time.
	tcsetattr(this->fd, TCSANOW, &options);
}

void Comunicacao::enviaDadosRobo(Robo** time) {
	// Copia atributos dos robôs para o buffer de comunicação.
	byte[0] = time[0]->getMotorRobo() & 0x7F;
	byte[1] = time[1]->getMotorRobo() | 0x80;
	byte[2] = time[2]->getMotorRobo() | 0x80;
	byte[3] = (byte[0] ^ byte[1] ^ byte[2]) | 0x80;
}

void Comunicacao::finaliza() {
	finalizada = true;
	pthread_join(tid, NULL);
}
