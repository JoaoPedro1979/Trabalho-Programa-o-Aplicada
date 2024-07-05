#include <Wire.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int botaoverde = 2;
const int botaovermelho = 3;

bool modo_envio = false;
bool modo_receber = false;
char mgs[17];
int posicao_cursor = 0;
char vogais[] = "aeiou ";
int vogal_atual = 0;
String mgs_recebida = "";

const char* menus[] = {
    "Receber do PC",
    "Receber do Ino",
    "Imprimir mgs ASC",
    "Imprimir mgs DES"
};
int menu_atual = 0;
const int total_menu = 4;

void mostraMenu() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Escolha a opcao:");
    lcd.setCursor(0, 1);
    lcd.print(menus[menu_atual]);
}

void setup() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("Trab Prog Aplic");
    lcd.setCursor(0, 1);
    lcd.print("JPV DR CR GA");
    pinMode(botaoverde, INPUT);
    pinMode(botaovermelho, INPUT);
    Serial.begin(9600);
    delay(2000);
    mostraMenu();
}

void loop() {
    if (Serial.available() > 0) {
        char c = (char)Serial.read();
        mgs_recebida += c;
        if (mgs_recebida.endsWith("CMD_ENVIO")) {
            modo_envio = true;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Digite sua mgs");
            lcd.setCursor(0, 1);
            vogal_atual = 0;
            posicao_cursor = 0;
            lcd.print(vogais[vogal_atual]);
            mgs_recebida = "";
        } else if (modo_receber) {
            if (c == '\n') {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Esperando...");
                lcd.setCursor(0, 1);
                lcd.print(mgs_recebida);
                mgs_recebida = "";
                modo_receber = false;
            }
        } else {
            if (c == '\n') {
                lcd.clear();
                lcd.print(mgs_recebida);
                mgs_recebida = "";
            }
        }
    }


    if (digitalRead(botaoverde) == HIGH) {
        delay(200); 
        if (modo_envio) {
            vogal_atual = (vogal_atual + 1) % (sizeof(vogais) - 1);
            lcd.setCursor(posicao_cursor, 1);
            lcd.print(vogais[vogal_atual]);
        } else {    
            menu_atual = (menu_atual + 1) % total_menu;
            mostraMenu();
        }
    }


    if (digitalRead(botaovermelho) == HIGH) {
        delay(200); 
        if (modo_envio) {
            if (vogais[vogal_atual] == ' ') {
                mgs[posicao_cursor] = '\0';
                Serial.println(mgs);
                modo_envio = false;
                lcd.clear();
                mostraMenu();
            } else {
                mgs[posicao_cursor] = vogais[vogal_atual];
                posicao_cursor++;
                if (posicao_cursor < 16) {
                    lcd.setCursor(posicao_cursor, 1);
                    vogal_atual = 0;
                    lcd.print(vogais[vogal_atual]);
                } else {
                    mgs[posicao_cursor] = '\0';
                    Serial.println(mgs);
                    modo_envio = false;
                    lcd.clear();
                    mostraMenu();
                }
            }
        } else {
            switch (menu_atual) {
                case 0:
                    Serial.println("RECEBER_DO_PC");
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Esperando...");
                    modo_receber = true;
                    break;
                case 1:
                    Serial.println("RECEBER_DO_INO");
                    break;
                case 2:
                    Serial.println("IMPRIMIR_DO_PRIMEIRO");
                    break;
                case 3:
                    Serial.println("IMPRIMIR_DO_ULTIMO");
                    break;
            }
        }
    }
}
