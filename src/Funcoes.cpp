#include "Funcoes.hpp"

/* Le os valores do arquivo hsv.aff e cria um objeto para cada cor */
Cor* guardaValoresHSV(FILE* arquivo, Cor* cores) {
	int number[8] = { 0 };
	int cont = 0;
	int numero;
	int HSVMin[3];
	int HSVMax[3];
	bool first = true;

	while (!feof(arquivo)) {
		fscanf(arquivo, "%d", &numero);

		// Skip one character.
		fgetc(arquivo);

		if (cont == 3)
			for (int i = 0; i < 3; i++)
				HSVMin[i] = number[i];
		else if (cont >= 7) {
			for (int i = 0; i < 3; i++)
				HSVMax[i] = number[i + 4];
			if (first) {
				cores = new Cor(HSVMin, HSVMax, numero);
				first = false;
			} else
				cores->adicionaCor(HSVMin, HSVMax, numero);
		} else
			number[cont] = numero;
		cont++;
		if (cont == 8)
			cont = 0;
	}

	return cores;
}

CvPoint gira(CvPoint entrada, CvPoint meio, double angulo) {
	double seno, coseno;
	CvPoint saida;

	seno = sin(angulo * M_PI / 180);
	coseno = cos(angulo * M_PI / 180);

	saida.x = entrada.x * coseno - entrada.y * seno + meio.x - meio.x * coseno
			+ meio.y * seno;
	saida.y = entrada.x * seno + entrada.y * coseno + meio.y - meio.y * coseno
			- meio.x * seno;

	return saida;
}

bool pixelReal(IplImage* color, int valueCor, int x, int y, Cor* cores) {
	int soma = 0;
	uchar* ptr;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) // 0 0, 0 -1, -1 0, -1 -1;
				{
			ptr = cvPtr2D(color, y - i, x - j, NULL);
			if (cores->verificaCor(ptr[0], ptr[1], ptr[2], valueCor))
				soma++;
		}
	}
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) // 0 0, 0 +1, +1 0, +1 +1;
				{
			ptr = cvPtr2D(color, y + i, x + j, NULL);
			if (cores->verificaCor(ptr[0], ptr[1], ptr[2], valueCor))
				soma++;
		}
	}

	ptr = cvPtr2D(color, y + 1, x - 1, NULL);
	if (cores->verificaCor(ptr[0], ptr[1], ptr[2], valueCor))
		soma++;

	ptr = cvPtr2D(color, y - 1, x + 1, NULL);
	if (cores->verificaCor(ptr[0], ptr[1], ptr[2], valueCor))
		soma++;

	if (soma == 10)
		return true;
	else
		return false;
}

float produtoEscalar(CvPoint direcao, CvPoint centro, CvPoint objetivo) {
	CvPoint2D32f vetorDirecao;
	CvPoint2D32f vetorObjetivo;
	float tam;

	vetorDirecao.x = centro.x - direcao.x;
	vetorDirecao.y = centro.y - direcao.y;

	tam = tamanhoVetor(vetorDirecao);

	vetorDirecao.x = vetorDirecao.x / tam;
	vetorDirecao.y = vetorDirecao.y / tam;

	vetorObjetivo.x = centro.x - objetivo.x;
	vetorObjetivo.y = centro.y - objetivo.y;

	tam = tamanhoVetor(vetorObjetivo);

	vetorObjetivo.x = vetorObjetivo.x / tam;
	vetorObjetivo.y = vetorObjetivo.y / tam;

	return vetorDirecao.x * vetorObjetivo.x + vetorDirecao.y * vetorObjetivo.y;
}
