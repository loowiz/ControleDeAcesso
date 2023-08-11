/* ===================================================
CURSO TÉCNICO EM ELETRÔNICA - SENAI EaD

PROJETO FINAL - UC4 Sistemas Digitais

Nome: 
---------
Controle de acesso microcontrolado

Descrição:
---------
O sistema de controle de acesso possui um teclado numérico,
um display digital, dois LEDs indicadores da trava (ativada
ou desativada) e um rele que será utilizado para acionar 
a trava mecânica da porta. 

Requisitos do sistema:
---------
a) Alimentação por bateria de 5V;
b) LED verde indica trava aberta (desativada);
c) LED vermelho indica trava fechada (ativada);
d) Senha de 4 dígitos (exibir * quando digitada);
e) Aguardar 5 segundos para digitar senha.
f) Rele para travar ou destravar a fechadura;
g) Botão para reprogramar a senha;

Desenvolvimento:
---------
Luiz Ricardo Bitencourt

Histórico de versões:
---------
10/08/2023: Primeira versão funcional
11/08/2023: Inclusão de TimeOut
==================================================== */


/* ==================================================*/
/* INCLUSÃO DE BIBLIOTECAS */
/* ==================================================*/
#include <LiquidCrystal.h>
#include <Keypad.h>
/* ==================================================*/


/* ==================================================*/
/* DEFINIÇÕES DE HARDWARE*/
/* ==================================================*/
#define KB_LINHA1  A5
#define KB_LINHA2 A4
#define KB_LINHA3 A3
#define KB_LINHA4 A2
#define KB_COLUNA1  A1
#define KB_COLUNA2  A0
#define KB_COLUNA3  8
#define KB_COLUNA4  9

#define LCD_RS    12
#define LCD_E   11
#define LCD_DB4   5
#define LCD_DB5   4
#define LCD_DB6   3
#define LCD_DB7   2

#define LED_VERM  1
#define LED_VERDE 0
#define RELE    7

#define BT_REDEF  6
/* ==================================================*/


/* ==================================================*/
/* DECLARAÇÃO DE CONSTANTES E VARIÁVEIS GLOBAIS */
/* ==================================================*/

// Criação da matriz para mapeamento do teclado:
const char TECLAS[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Definição dos pinos usados no teclado:
byte pinLINHAS[4] = {KB_LINHA1,KB_LINHA2,KB_LINHA3,KB_LINHA4};
byte pinCOLUNAS[4] = {KB_COLUNA1,KB_COLUNA2,KB_COLUNA3,KB_COLUNA4};

// Variáveis para armazenar a senha:
String senhaAtual = "1234";
String senhaInserida = "";

// Controle da quantidade de dígitos da senha:
int contaDigitos = 0;

// Definição da função do sistema:
//  0 - Leitura de senha para destravar a porta;
//  1 - Checa senha atual para alteração de senha;
//  2 - Executa alteração de senha.
int funcao = 0;

// Flag para indicar estado da seção:
//  0 - Inativa;
//  1 - Ativa (usuário iniciou digitação de senha).
boolean secaoAtiva = 0;

// Controle do tempo de seção ativa:
int tempoSecao;       // Marcação do tempo
const int TIMEOUT = 10000;  // Tempo máximo de seção (em ms)
/* ==================================================*/


/* ==================================================*/
/* INSTÂNCIAS DOS OBJETOS */
/* ==================================================*/
Keypad teclado = Keypad(makeKeymap(TECLAS), pinLINHAS, pinCOLUNAS, 4, 4);
LiquidCrystal display(LCD_RS, LCD_E, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);
/* ==================================================*/


/* ==================================================*/
/* CONFIGURAÇÕES INICIAIS */
/* ==================================================*/
void setup() {
  
  // Definições de direção dos pinos usados:
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERM, OUTPUT);
  pinMode(RELE, OUTPUT);
  pinMode(BT_REDEF, INPUT);
  
  // Estados iniciais dos pinos de saída:
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERM, HIGH);
  digitalWrite(RELE, LOW);
  
  // Estado inicial das variáveis de controle:
  contaDigitos = 0;
  funcao = 0;
  senhaInserida = "";
  secaoAtiva = 0;
  
  // Inicialização do display (16 colunas, 2 linhas):
  display.begin(16, 2);
  
  // Tela de boas vindas:
  display.setCursor(0,0);
  display.print("  Controle  de  ");
  display.setCursor(0,1);
  display.print("     Acesso     ");
  delay(1000);
  display.setCursor(0,0);
  display.print("    S E N A I   ");
  display.setCursor(0,1);
  display.print("  Porto Alegre  ");
  delay(1000);
  
  // Tela inicial aguardando digitação de senha:
  display.setCursor(0,0);
  display.print("Digite a senha: ");
  display.setCursor(0,1);
  display.print("                ");
  display.setCursor(0,1);
}
/* ==================================================*/


/* ==================================================*/
/* LOOP PRINCIPAL */
/* ==================================================*/
void loop() {
  
  // Realiza varredura do teclado:
  char teclaLida = teclado.getKey();
  
  // Verificação do acionamento do botão de alteração de senha:
  if (digitalRead(6) == HIGH) {
    display.setCursor(0,0);
    display.print("Alterar senha?  ");
    display.setCursor(0,1);
    display.print("Senha atual:    ");
    display.setCursor(12,1);
    funcao = 1;
    contaDigitos = 0;
    senhaInserida = "";
  }
  
  // Controle do tempo da seção (caso ativa):
  if ((secaoAtiva == 1) && (millis() - tempoSecao >= TIMEOUT)) {
    display.setCursor(0,0);
    display.print("Tempo  esgotado!");
    display.setCursor(0,1);
    display.print("Reiniciando...  ");
    delay(3000);
    setup();
  }
  
  // Sempre que for lida alguma tecla no teclado...
  if (teclaLida) {
    
    // Verifica se já está em uma seção ativa...
    if (secaoAtiva == 0) {
      // Se não estiver, a partir de agora está!
      secaoAtiva = 1;
      // Marca o tempo do início de seção
      tempoSecao = millis();
    }
    
    // Função de leitura de senha para destravar porta:
    if (funcao == 0) {
      
      // Limita número de dígitos da senha:
      if (contaDigitos < 4) {
          contaDigitos++;
          senhaInserida = senhaInserida + teclaLida;
          display.print("*");
      }
      
      // Se já digitou os 4 números, aguarda # para validar ou não
      else {
        if (teclaLida == '#') {
          if (senhaInserida == senhaAtual) {
            display.setCursor(0,0);
            display.print("Acesso liberado!");
            display.setCursor(0,1);
            display.print("                ");
            digitalWrite(LED_VERDE, HIGH);
            digitalWrite(LED_VERM, LOW);
            digitalWrite(RELE, HIGH);
            delay(3000);
            setup();
          }
          else {
            display.setCursor(0,0);
            display.print("Acesso bloqueado");
            display.setCursor(0,1);
            display.print("Senha incorreta!");
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_VERM, HIGH);
            digitalWrite(RELE, LOW);
            delay(3000);
            setup();
          }
        }
      }
    }
    
    // Cadastro de nova senha (validação da senha atual):
    else if (funcao == 1) {
      if (contaDigitos < 4) {
          contaDigitos++;
          senhaInserida = senhaInserida + teclaLida;
          display.print("*");
      }
      else {
        if (teclaLida == '#') {
          if (senhaInserida == senhaAtual) {
            display.setCursor(0,0);
            display.print("Cadastrar senha ");
            display.setCursor(0,1);
            display.print("Nova senha:     ");
            display.setCursor(12,1);
            funcao = 2;
            contaDigitos = 0;
            senhaInserida = "";
          }
          else {
            display.setCursor(0,0);
            display.print("Nao autorizado! ");
            display.setCursor(0,1);
            display.print("                ");
            delay(3000);
            setup();
          }
        }
      }
    }
    
    // Alteração da senha:
    else if (funcao == 2) {
      if (contaDigitos < 4) {
          contaDigitos++;
          senhaInserida = senhaInserida + teclaLida;
          display.print("*");
      }
      else {
        if (teclaLida == '#') {
          senhaAtual = senhaInserida;
          display.setCursor(0,0);
          display.print(" Senha alterada ");
          display.setCursor(0,1);
          display.print("  com sucesso!  ");
          delay(3000);
          setup();
        }
        else if (teclaLida == '*') {
          display.setCursor(0,0);
          display.print("   Senha  nao   ");
          display.setCursor(0,1);
          display.print("    alterada    ");
          delay(3000);
          setup();
        }
      }
    }
  }
}
/* ==================================================*/
