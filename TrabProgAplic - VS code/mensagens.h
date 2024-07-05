#ifndef MENSAGENS_H
#define MENSAGENS_H

#include <windows.h>


#define MGS 16


#define COM_PORT "COM9"
#define BAUD_RATE CBR_9600


typedef enum {
    RECEBER_DO_VS_CODE,
    RECEBER_DO_ARDUINO,
    IMPRIMIR_DO_PRIMEIRO,
    IMPRIMIR_DO_ULTIMO,
    TOTAL_OPCOES
} Opcoes;


typedef struct No {
    char mgs[MGS + 1];      
    struct No* proximo;     
    struct No* anterior;    
} No;


typedef struct {
    No* primeiro;          
    No* ultimo;            
    int cont_mgs;           
} Mensgs;


void adicionaNoMensagem(Mensgs* Mensgs, const char* msg);
void mostraMensagemAntigoparaRecente(Mensgs* Mensgs);
void mostraMensagemRecenteparaAntigo(Mensgs* Mensgs);
void enviaParduino(HANDLE hSerial, const char* mgs);
void receberMgsVscodepArduino(HANDLE hSerial, Mensgs* Mensgs);
void receberMgsArduinopVscode(HANDLE hSerial, Mensgs* Mensgs);
void iniciaCommSerial(HANDLE* hSerial, DCB* dcbSerialParams, COMMTIMEOUTS* timeouts);
void closeSerial(HANDLE hSerial);
void readSerial(HANDLE hSerial, char* buffer, int bufferSize);

#endif 
