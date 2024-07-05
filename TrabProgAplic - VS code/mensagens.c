#include "mensagens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void adicionaNoMensagem(Mensgs* Mensgs, const char* msg) {
    No* novo_no = (No*)malloc(sizeof(No));
    strncpy(novo_no->mgs, msg, MGS);
    novo_no->mgs[MGS] = '\0';
    novo_no->proximo = NULL;
    novo_no->anterior = Mensgs->ultimo;

    if (Mensgs->ultimo != NULL) {
        Mensgs->ultimo->proximo = novo_no;
    }
    Mensgs->ultimo = novo_no;

    if (Mensgs->primeiro == NULL) {
        Mensgs->primeiro = novo_no;
    }

    if (Mensgs->cont_mgs == 10) {
        No* temp = Mensgs->primeiro;
        Mensgs->primeiro = Mensgs->primeiro->proximo;
        if (Mensgs->primeiro != NULL) {
            Mensgs->primeiro->anterior = NULL;
        }
        free(temp);
    } else {
        Mensgs->cont_mgs++;
    }
}

void mostraMensagemAntigoparaRecente(Mensgs* Mensgs) {
    No* atual = Mensgs->ultimo;
    printf("Ultimas 10 mgs (antigas p/ recentes):\n");
    while (atual != NULL) {
        printf("%s\n", atual->mgs);
        atual = atual->anterior;
    }
}

void mostraMensagemRecenteparaAntigo(Mensgs* Mensgs) {
    No* atual = Mensgs->primeiro;
    printf("Ultimas 10 mgs (recentes p/ antigas):\n");
    while (atual != NULL) {
        printf("%s\n", atual->mgs);
        atual = atual->proximo;
    }
}

void enviaParduino(HANDLE hSerial, const char* mgs) {
    DWORD bytesWritten;
    if (!WriteFile(hSerial, mgs, strlen(mgs), &bytesWritten, NULL)) {
        fprintf(stderr, "Erro ao escrever na porta serial\n");
    }
}

void receberMgsVscodepArduino(HANDLE hSerial, Mensgs* Mensgs) {
    char buffer[MGS + 1];
    printf("Esperando...\n");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    strcat(buffer, "\n");
    enviaParduino(hSerial, buffer); 
    adicionaNoMensagem(Mensgs, buffer);
}

void receberMgsArduinopVscode(HANDLE hSerial, Mensgs* Mensgs) {
    const char* comando = "CMD_ENVIO";
    enviaParduino(hSerial, comando);
    char buffer[MGS + 1];
    DWORD bytesRead;
    while (1) {
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            if (bytesRead > 0 && buffer[bytesRead - 1] == '\n') {
                buffer[bytesRead - 1] = '\0';
                break;
            }
        }
    }

    printf("mgs do Arduino: %s\n", buffer);
    adicionaNoMensagem(Mensgs, buffer);
}

void iniciaCommSerial(HANDLE* hSerial, DCB* dcbSerialParams, COMMTIMEOUTS* timeouts) {
    *hSerial = CreateFile(COM_PORT, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (*hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Erro ao abrir a porta serial\n");
        exit(1);
    }

    dcbSerialParams->DCBlength = sizeof(*dcbSerialParams);
    if (!GetCommState(*hSerial, dcbSerialParams)) {
        fprintf(stderr, "Erro ao encontrar o estado\n");
        CloseHandle(*hSerial);
        exit(1);
    }

    dcbSerialParams->BaudRate = BAUD_RATE;
    dcbSerialParams->ByteSize = 8;
    dcbSerialParams->StopBits = ONESTOPBIT;
    dcbSerialParams->Parity = NOPARITY;

    if (!SetCommState(*hSerial, dcbSerialParams)) {
        fprintf(stderr, "Erro ao configurar o estado\n");
        CloseHandle(*hSerial);
        exit(1);
    }

    timeouts->ReadIntervalTimeout = 50;
    timeouts->ReadTotalTimeoutConstant = 50;
    timeouts->ReadTotalTimeoutMultiplier = 10;
    timeouts->WriteTotalTimeoutConstant = 50;
    timeouts->WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(*hSerial, timeouts)) {
        fprintf(stderr, "Erro na configuracao de timeouts\n");
        CloseHandle(*hSerial);
        exit(1);
    }
}

void closeSerial(HANDLE hSerial) {
    CloseHandle(hSerial);
}

void readSerial(HANDLE hSerial, char* buffer, int bufferSize) {
    DWORD bytesRead;
    if (!ReadFile(hSerial, buffer, bufferSize - 1, &bytesRead, NULL)) {
        fprintf(stderr, "Erro ao ler a porta serial\n");
    }
    buffer[bytesRead] = '\0';
}
