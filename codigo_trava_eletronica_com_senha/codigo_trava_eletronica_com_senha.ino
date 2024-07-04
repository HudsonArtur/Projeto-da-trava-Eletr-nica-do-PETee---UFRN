/* ******************** Controle de acesso por senha Manual do Mundo ********************
   
   Criado por: Fernando A Souza e Iberê Thenório
   Rev.: 01
   Data: 17.06.2019

   Guia de conexão:
    
   Arduino:
   LINHA 1 TECLADO: Pino Digital 11
   LINHA 2 TECLADO: Pino Digital 10
   LINHA 3 TECLADO: Pino Digital 9
   LINHA 4 TECLADO: Pino Digital 8

   COLUNA 1 TECLADO: Pino Digital 7
   COLUNA 2 TECLADO: Pino Digital 6
   COLUNA 3 TECLADO: Pino Digital 5
   COLUNA 4 TECLADO: Pino Digital 4
   
   LED VERMELHO: Pino Digital 12
   LED VERDE: Pino Digital 13
   MÓDULO RELÊ: Pino Digital 3 (na simulação do Tinkercad temos um led azul no lugar do módulo relê, quando ligamos o módulo não precisamos usar o resistor)
  
   Este código utiliza a biblioteca Keypad
      
 ***************************************************************************** */

// Inclusão da biblioteca do módulo teclado
#include <Keypad.h>

// Inclusão da biblioteca do display
#include <LiquidCrystal_I2C.h>

/************************ DEFINIÇÃO DE SENHA ******************************* */

String senha = "ABC123";        // defina a senha aqui, ela deve conter seis dígitos
String senhaMestra = "321ABC";  // define a senha mestra, use essa senha quando a porta estiver fechada
int estadoSenha = 0;            // 0: senha padrão; 1: senha mestra

/************************ CONFIGURAÇÕES DISPLAY ******************************* */

LiquidCrystal_I2C lcd(32, 16, 2);

int flag = 6;
int tamanho_da_senha = senha.length();  // Variável usada para saber o tamanho da senha
String palavra = "";                    // Variável usada para comparar com a senha

/************************ CONFIGURAÇÕES TECLADO ******************************* */

const byte LINHAS = 4;   // quantidade de linhas do teclado
const byte COLUNAS = 4;  // quantidade de colunas do teclado

char digitos[LINHAS][COLUNAS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};  // todos os caracteres do teclado

byte pinosLinha[LINHAS] = { 11, 10, 9, 8 };  // pinos de conexão das linhas no arduino
byte pinosColuna[COLUNAS] = { 7, 6, 5, 4 };  // pinos de conexão das colunas no arduino

Keypad keypad = Keypad(makeKeymap(digitos), pinosLinha, pinosColuna, LINHAS, COLUNAS);  // objeto de leitura das teclas com os parâmetros lidos

/************************ VARIÁVEIS AUXILIARES ******************************* */

int position = 0;
int ledVermelho = 12;
int ledVerde = 13;
int releFechadura = 3;  //led azul substituir por módulo relé
int tempoAberto = 2000;

/***************************************************************************** */

void setup() {


  Serial.begin(9600);  // Inicia porta serial
  lcd.init();          // Inicia o display da porta
  lcd.setBacklight(HIGH);

  estadoPorta(true);  // diz se a porta está ou não trancada

  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(releFechadura, OUTPUT);
}

void loop() {

  lcd.setCursor(0, 0);
  lcd.print("PETee - UFRN");
  lcd.setCursor(0, 1);
  lcd.print("SENHA:");

  char digito = keypad.getKey();  // faz a leitura das teclas

  if (digito != 0) {
    Serial.println(digito);  // Imprime a tecla pressionada na porta serial
    lcd.setCursor(flag, 1);
    lcd.print("*");
    flag++;
    palavra += digito;
    position++;
    delay(10);


    if (estadoSenha == 0) { //
      if (((flag - 6) == position) && (palavra == senhaMestra))  // altera estado da porta
      {
        estadoSenha = 1; // acesso restrito ativado
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ACESSO RESTRITO");
        lcd.setCursor(0, 1);
        lcd.print("ATIVADO!");
        delay(1000);
        lcd.clear();
        flag = 6;
        palavra = "";
        position = 0;
      }
      if (((flag - 6) == tamanho_da_senha) && (palavra != senha)) {
        position = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ACESSO NEGADO");
        lcd.setCursor(0, 1);
        lcd.print("Senha incorreta!");
        delay(1000);
        lcd.clear();
        flag = 6;
        palavra = "";
      }
      if (((flag - 6) == position) && (palavra == senha))  // altera estado da porta
      {
        estadoPorta(false);
      }
      delay(100);
    }
    else
    {
      if (((flag - 6) == position) && (palavra == senha))  // altera estado da porta
      {
        position = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ACESSO NEGADO");
        lcd.setCursor(0, 1);
        lcd.print("SENHA PADRAO");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ACESSO RESTRITO");
        lcd.setCursor(0, 1);
        lcd.print("USE SENHA MESTRA!");
        delay(1500);
        flag = 6;
        palavra = "";
        lcd.clear();
      }
      if (((flag - 6) == tamanho_da_senha) && (palavra != senhaMestra)) {
        position = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ACESSO NEGADO");
        lcd.setCursor(0, 1);
        lcd.print("Senha Mestra incorreta!");
        delay(1000);
        lcd.clear();
        flag = 6;
        palavra = "";
      }
      if (((flag - 6) == position) && (palavra == senhaMestra))  // altera estado da porta
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ACESSO RESTRITO");
        lcd.setCursor(0, 1);
        lcd.print("DESATIVADO!");
        delay(1000);
        lcd.clear();
        estadoSenha = 0; // acesso restrito desativado
        estadoPorta(false);
      }
      delay(100);
    }
  }
}

void estadoPorta(int trancado) {

  if (trancado)  // trancamento da porta
  {
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(ledVerde, LOW);
    digitalWrite(releFechadura, LOW);  // importante: para uso com rele de acionamento da fechadura LOW deve ser subsstituído por HIGH
  }

  else  // abertura da porta
  {
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledVerde, HIGH);
    digitalWrite(releFechadura, HIGH);  // importante: para uso com rele de acionamento da fechadura HIGH deve ser subsstituído por LOW
    delay(tempoAberto);
    position = 0;
    digitalWrite(releFechadura, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" ACESSO LIBERADO");
    lcd.setCursor(0, 1);
    lcd.print(" Bem-Vindo(a)!");
    delay(1000);
    lcd.clear();
    flag = 6;
    palavra = "";

    estadoPorta(true);  // tranca a porta novamente
  }
}