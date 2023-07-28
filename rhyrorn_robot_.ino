#include <Wire.h>
// Endereço em hexadecimal do sensor MPU 6050
const int ENDERECO_SENSOR = 0x68;
int girX, girY, girZ, acelX, acelY, acelZ, temperatura;
const int numAmostras = 5;  // Número de amostras para a média móvel
int amostrasX[numAmostras];
int indice = 0;
int ladeira = 0;

#include <Ultrasonic.h>
Ultrasonic ultrassom(A0, A1);  // define o nome do sensor(ultrassom)
//e onde esta ligado o trig(A0) e o echo(A1) respectivamente

long distancia;
//===========================================================
//Motores:
// Definição de portas dos motores
#define EnA 2  // Pino PWM para controle de velocidade do motor A
#define IN1 3  // Pino de controle direcional do motor D1
#define IN2 4  // Pino de controle direcional do motor D2
#define IN3 5  // Pino de controle direcional do motor E1
#define IN4 6  // Pino de controle direcional do motor E2
#define EnB 7  // Pino PWM para controle de velocidade do motor B

//Sensores de Refletância (Segue-linha)
#define sensorR1 A2
#define sensorR2 A3
#define sensorR3 A4
#define sensorR4 A5

int velocidadeA = 40;
int velocidadeB = 40;
//==========================================================
//Pinos de conexao do modulo 1
#define pinS0 24
#define pinS1 23
#define pinS2 27
#define pinS3 28
#define pinOut 26
#define pinLED 25
unsigned int valorVermelho = 0;
unsigned int valorVerde = 0;
unsigned int valorAzul = 0;
unsigned int valorBranco = 0;
//Pinos de conexao do modulo 1
#define DpinS0 30
#define DpinS1 31
#define DpinS2 33
#define DpinS3 34
#define DpinOut 32
#define DpinLED 29
unsigned int valorVermelhoB = 0;
unsigned int valorVerdeB = 0;
unsigned int valorAzulB = 0;
unsigned int valorBrancoB = 0;

void detectaCor();
void detectaCorB();

void setup() {
  //Configuração dos pinos como entrada/saída
  //MOTORES:
  pinMode(EnA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EnB, OUTPUT);
  //==============================================
  pinMode(pinS0, OUTPUT);
  pinMode(pinS1, OUTPUT);
  pinMode(pinS2, OUTPUT);
  pinMode(pinS3, OUTPUT);
  pinMode(pinLED, OUTPUT);
  pinMode(pinOut, INPUT);

  pinMode(DpinS0, OUTPUT);
  pinMode(DpinS1, OUTPUT);
  pinMode(DpinS2, OUTPUT);
  pinMode(DpinS3, OUTPUT);
  pinMode(DpinLED, OUTPUT);
  pinMode(DpinOut, INPUT);

  Serial.begin(9600);
  digitalWrite(DpinS0, HIGH);
  digitalWrite(DpinS1, LOW);
  digitalWrite(pinS0, HIGH);
  digitalWrite(pinS1, LOW);
  delay(2000);
  //Liga LED
  digitalWrite(pinLED, HIGH);
  digitalWrite(DpinLED, HIGH);

  // Inicializa a biblioteca Wire
  Wire.begin();
  Wire.beginTransmission(ENDERECO_SENSOR);
  Wire.write(0x6B);

  // Inicializa o sensor
  Wire.write(0);
  Wire.endTransmission(true);
}
void loop() {
  inclinacaoX();

  detectaCor();
  detectaCorB();

  distancia = ultrassom.Ranging(CM);  // ultrassom.Ranging(CM) retorna a distancia em
                                      // centímetros(CM) ou polegadas(INC)
  Serial.print(distancia);            //imprime o valor da variável distancia
  Serial.println("cm");
  bool estadoR1 = digitalRead(sensorR1);
  bool estadoR2 = digitalRead(sensorR2);
  bool estadoR3 = digitalRead(sensorR3);
  bool estadoR4 = digitalRead(sensorR4);
  Serial.print(estadoR1);
  Serial.print(" ");
  Serial.print(estadoR2);
  Serial.print(" ");
  Serial.print(" ");
  Serial.print(estadoR3);
  Serial.print(" ");
  Serial.print(estadoR4);
  Serial.println();
  //==============================================================================================================
  if ((estadoR2 == 0) && (estadoR3 == 0)) {
    frente(42, 43);
    Serial.println("FRENTE");
  } else if (estadoR2 == 1 && estadoR3 == 0) {
    MovEsquerda();
    Serial.println("ESQUERDA");
  } else if (estadoR2 == 0 && estadoR3 == 1) {
    MovDireita();
    Serial.println("DIREITA");
  }

  if (estadoR1 == 1) {

    // Verifica a cor verde após avançar
    if ((valorVerde < valorVermelho) && (valorVerde < valorAzul) && (valorBranco < 100)) {
      Serial.println("Verde");
      Mov90GrausEsquerda(300);  // Ajuste o tempo de rotação conforme necessário
    } else if ((valorVerdeB < valorVermelhoB) && (valorVerdeB < valorAzulB) && (valorBrancoB < 100)) {
      Serial.println("Verde");
      Mov90GrausDireita(300);  // Ajuste o tempo de rotação conforme necessário
    }
    parar();
    delay(500);
    frente(50, 50);
    delay(300);
    parar();
    delay(500);
    while (estadoR3 == 0) {
      girarEsquerda();
      estadoR3 = digitalRead(sensorR3);
      Serial.print(estadoR3);
      Serial.println("==================================");
    }
    girarDireita();
    delay(150);
    return;  // Retorna ao início do loop()
  }

  if (estadoR4 == 1) {
    if ((valorVerde < valorVermelho) &&  //Verifica se a cor verde foi detectada
        (valorVerde < valorAzul) && (valorBranco < 100)) {
      Serial.println("Verde");
      Mov90GrausEsquerda(330);
    }

    else if ((valorVerdeB < valorVermelhoB) &&  //Verifica se a cor verde foi detectada
             (valorVerdeB < valorAzulB) && (valorBrancoB < 100)) {
      Serial.println("Verde");
      Mov90GrausDireita(300);
    }
    parar();
    delay(500);
    frente(50, 50);
    delay(300);
    parar();
    delay(500);
    while (estadoR2 == 0) {
      girarDireita();
      estadoR2 = digitalRead(sensorR2);
      Serial.print(estadoR2);
      Serial.println("giro esquerda");
    }
    girarEsquerda();
    delay(150);
    return;  // Retorna ao início do loop()
  }
}

void frente(int VelocidadeA, int VelocidadeB) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(EnA, VelocidadeA /*+ (ladeira * 20)*/);
  analogWrite(EnB, VelocidadeB /*+ (ladeira * 20)*/);
}
void MovDireita() {
  // Define o sentido de rotação dos motores
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Define as velocidades dos motores
  analogWrite(EnA, velocidadeA);
  analogWrite(EnB, velocidadeB);
}

void MovEsquerda() {
  // Define o sentido de rotação dos motores
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Define as velocidades dos motores
  analogWrite(EnA, velocidadeA);
  analogWrite(EnB, velocidadeB);
}

void Mov90GrausDireita(int tempo) {
  // Define o sentido de rotação dos motores
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Define as velocidades dos motores
  analogWrite(EnA, 70);
  analogWrite(EnB, 60);
  delay(tempo);
}

void Mov90GrausEsquerda(int tempo) {
  // Define o sentido de rotação dos motores
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Define as velocidades dos motores
  analogWrite(EnA, 60);
  analogWrite(EnB, 70);
  delay(tempo);
}

void girarEsquerda() {
  // Inserir aqui o código para girar o robô para a esquerda
  // Por exemplo, você pode definir os pinos IN1, IN2, IN3 e IN4 para girar o robô para a esquerda
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(EnA, 50);
  analogWrite(EnB, 50);
}

void girarDireita() {
  // Inserir aqui o código para girar o robô para a direita
  // Por exemplo, você pode definir os pinos IN1, IN2, IN3 e IN4 para girar o robô para a direita
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(EnA, 50);
  analogWrite(EnB, 50);
}

void parar() {
  // Define o sentido de rotação dos motores
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Define as velocidades dos motores
  analogWrite(EnA, velocidadeA);
  analogWrite(EnB, velocidadeB);
}

void tras() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(EnA, 35);
  analogWrite(EnB, 49);
}

// *********** Função de leitura so sensor de cor ********************
void detectaCor() {
  //Vermelho
  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, LOW);
  valorVermelho = pulseIn(pinOut, digitalRead(pinOut) == HIGH ? LOW : HIGH);

  //Sem filtro
  digitalWrite(pinS2, HIGH);
  valorBranco = pulseIn(pinOut, digitalRead(pinOut) == HIGH ? LOW : HIGH);

  //Azul
  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, HIGH);
  valorAzul = pulseIn(pinOut, digitalRead(pinOut) == HIGH ? LOW : HIGH);

  //Verde
  digitalWrite(pinS2, HIGH);
  valorVerde = pulseIn(pinOut, digitalRead(pinOut) == HIGH ? LOW : HIGH);
}
void detectaCorB() {
  //Vermelho
  digitalWrite(DpinS2, LOW);
  digitalWrite(DpinS3, LOW);
  valorVermelhoB = pulseIn(DpinOut, digitalRead(DpinOut) == HIGH ? LOW : HIGH);

  //Sem filtro
  digitalWrite(DpinS2, HIGH);
  valorBrancoB = pulseIn(DpinOut, digitalRead(DpinOut) == HIGH ? LOW : HIGH);

  //Azul
  digitalWrite(DpinS2, LOW);
  digitalWrite(DpinS3, HIGH);
  valorAzulB = pulseIn(DpinOut, digitalRead(DpinOut) == HIGH ? LOW : HIGH);

  //Verde
  digitalWrite(DpinS2, HIGH);
  valorVerdeB = pulseIn(DpinOut, digitalRead(DpinOut) == HIGH ? LOW : HIGH);
}