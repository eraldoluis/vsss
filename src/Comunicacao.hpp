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

class Comunicacao: public robo {
private:
	int fd;
	unsigned char* byte;
	unsigned char* copia;

public:
	static bool enviarPacote;

	/* Construtor */
	Comunicacao() {
		byte = new unsigned char[4];
		copia = new unsigned char[4];
		enviarPacote = false;
	}

	void abrePortaSerialComunicacao() {
		/* File descriptor for the port */
		int fd;
		fd = open(PORTA_SERIAL, O_WRONLY | O_NOCTTY | O_NDELAY); //O_RDWR

		if (fd == -1)
			perror("open_port: Unable to open /dev/ttyS0 - ");
		else
			fcntl(fd, F_SETFL, 0);

		this->fd = fd;
	}

	void configuraComunicacao() {
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

	void enviaDadosRobo(robo** time) {
		if (enviarPacote) {
			unsigned char teste = 1;
			byte[0] = time[0]->getMotorRobo() & 0x7F;
			byte[1] = time[1]->getMotorRobo() | 0x80;
			byte[2] = time[2]->getMotorRobo() | 0x80;
			byte[3] = (byte[0] ^ byte[1] ^ byte[2]) | 0x80;
			write(this->fd, byte, 4);
			enviarPacote = false;
		} else {
			/* envia sempre o ultimo pacote para não pegar nenhum ruido */
			write(this->fd, byte, 4);
		}
	}

	int getFileDescriptor() {
		return this->fd;
	}

};

bool Comunicacao::enviarPacote;

#endif //__Comunicacao_cpp
