#include <stdio.h>
#include <windows.h>
#include "mensagens.h"

int main() {
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

    iniciaCommSerial(&hSerial, &dcbSerialParams, &timeouts);

    Mensgs Mensgs = {0};

    char buffer[256];

    while (1) {
        readSerial(hSerial, buffer, sizeof(buffer));
        if (strlen(buffer) > 0) {
            if (strstr(buffer, "RECEBER_DO_PC") != NULL) {
                receberMgsVscodepArduino(hSerial, &Mensgs);
            } else if (strstr(buffer, "RECEBER_DO_INO") != NULL) {
                receberMgsArduinopVscode(hSerial, &Mensgs);
            } else if (strstr(buffer, "IMPRIMIR_DO_PRIMEIRO") != NULL) {
                mostraMensagemAntigoparaRecente(&Mensgs);
            } else if (strstr(buffer, "IMPRIMIR_DO_ULTIMO") != NULL) {
                mostraMensagemRecenteparaAntigo(&Mensgs);
            }
        }
        Sleep(100);
    }

    closeSerial(hSerial);
    return 0;
}
