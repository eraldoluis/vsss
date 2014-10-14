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

#ifndef __Configuracoes_hpp
#define __Configuracoes_hpp

////////////////////////////////////////////////////////////////////////////////
//Quantidades e Tamanhos
////////////////////////////////////////////////////////////////////////////////
#define NUM_CORES 10 //QUANTIDADE DE CORES
#define RAIO 8 //RAIO DO CIRCULO//9
#define RAIO_DISTANCIA 80//250 //RAIO DO CIRCULO

#define DISTANCIA_MINIMA 14 //DISTANCIA ENTRE AS CORES DO ROBO
#define DISTANCIA_MAXIMA 20 //DISTANCIA ENTRE AS CORES DO ROBO

////////////////////////////////////////////////////////////////////////////////
//Time
////////////////////////////////////////////////////////////////////////////////
#define COR_TIME_UM 0
#define COR_ROBO_UM 1
#define COR_ROBO_DOIS 2
#define COR_ROBO_TRES 3

////////////////////////////////////////////////////////////////////////////////
//Comunicação
////////////////////////////////////////////////////////////////////////////////
#define PORTA_SERIAL "/dev/ttyUSB0"
#define NUMERO_CAMERA 1

////////////////////////////////////////////////////////////////////////////////
//Orientacao dos robos
////////////////////////////////////////////////////////////////////////////////
#define FRENTE 0
#define TRAS 1


#endif // __Configuracoes_hpp
