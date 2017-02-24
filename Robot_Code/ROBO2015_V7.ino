//Dados Fixos
#define ID 'C' //ID do Robo 
#define I1A  5 //Motor da Direita
#define I1B  6
#define I2A  11 //Motor da Esquerda
#define I2B  10

//Variáveis
String inputString = "";
boolean stringComplete = false;
bool msgComplete = false;
bool myMessage = false;

unsigned long previousMillis = 0;
unsigned long  timer = 0;
double Dt = 0;

double encoderRIGHT = 0;
double encoderLEFT = 0;

int PWMs[2];
String vel1 = "";
String vel2 = "";

String KP = "";
String KI = "";
String KD = "";

double actualVelocity[] = {0.0, 0.0};
double desiredVelocity[] = {0.0, 0.0};

bool calibrar = false;

//Variáveis - Controle PID
double kp = 3;    //...Constante PROPORCIONAL
double ki = 0;  //...Constante INTEGRATIVA
double kd = 0; //...Constante DERIVATIVA
double accumulator[] = {0, 0};
double lastError[] = {0, 0};
double actualError[] = {0, 0};
double P_Gain[] = {0, 0};
double I_Gain[] = {0, 0};
double D_Gain[] = {0, 0};
double PID[] = {0, 0};

//Setup Principal - Sala da Justiça
void setup() {

  Serial.begin(57600);
  Serial.print("Robo ");
  Serial.print(ID);
  Serial.println("- Iniciado");

  //Reserva 200 bytes para a variavel inputString
  inputString.reserve(200);

  pinMode(I1B, OUTPUT);
  pinMode(I1A, OUTPUT);
  pinMode(I2B, OUTPUT);
  pinMode(I2A, OUTPUT);

  timer = millis();

  //Ativa interrupcoes dos encoders
  attachInterrupt(0, ENC_RIGHT, RISING); //Pin2
  attachInterrupt(1, ENC_LEFT, RISING);  //Pin3
}

//Loop principal - Casa do Batman
void loop() {
  
  //Delay da zueira - So funciona com ele aqui
  delay(1);
  
  //  Serial.print(inputString);
  //  Serial.print(" ER - ");
  //  Serial.print(encoderRIGHT);
  //  Serial.print(" EL - ");
  //  Serial.print(encoderLEFT);
  //  Serial.print(" 0: ");
  //  Serial.print(actualVelocity[0]);
  //  Serial.print(" 1: ");
  //  Serial.print(actualVelocity[1]);

  if (stringComplete) {
      //Serial.println(inputString);
    if (calibrar) {
      calibrarPID(inputString);
      Serial.print("Robo ");
      Serial.print(ID);
      Serial.print(" - Kp = ");
      Serial.print(kp);
      Serial.print(" Ki = ");
      Serial.print(ki);
      Serial.print(" Kd = ");
      Serial.print(kd);
      Serial.println(" - Calibrado");

      inputString = "";
      stringComplete = false;
    }
    else {
      interpretaMSG(inputString);
      CriaVELCITY();
      previousMillis = millis();
      inputString = "";
      stringComplete = false;
    }
  }

  if ((millis() - previousMillis) < 100) {
    
    PIDControler();
    if(desiredVelocity[0]==0)
    PWMs[0] = 0;
    if(desiredVelocity[1]==0)
    PWMs[1] = 0;
    Run(PWMs);
  }
  else {
    Stop();
  }
}

//Rotina para calibrar as constantes do PID
void calibrarPID(String msg) {
  char floatbuf[32];
  int cont = 0;

  for (int j = 2; j < msg.length(); j++)
  {
    if (msg[j] != ';')
    {
      if (cont == 0)
        KP += msg[j];
      if (cont == 1)
        KI += msg[j];
      if (cont == 2)
        KD += msg[j];
    }
    else //Se encontrou um ';' significa que o primeiro pwm ja foi lido
      cont++;
  }

  //Zueira... Trasforma para vetor de char para poder usar função atof
  KP.toCharArray(floatbuf, sizeof(floatbuf));
  kp = atof(floatbuf);
  KI.toCharArray(floatbuf, sizeof(floatbuf));
  ki = atof(floatbuf);
  KD.toCharArray(floatbuf, sizeof(floatbuf));
  kd = atof(floatbuf);
  calibrar = false;

  KP = "";
  KI = "";
  KD = "";
}

//Controlador PID - O Zueiro
void PIDControler() {

  //  Serial.print(" P_GAIN: ");
  //  Serial.print(actualError[0]);
  //  Serial.print(" desiredVelocity0: ");
  //  Serial.print(desiredVelocity[0]);
  //  Serial.print(" desiredVelocity1: ");
  //  Serial.println(desiredVelocity[1]);
  //  Serial.print(" actualVelocity0: ");
  //  Serial.print(actualVelocity[0]);
  //  Serial.print(" actualVelocity1: ");
  //  Serial.println(actualVelocity[1]);
  //  Serial.print(" actualError: ");
  //  Serial.print(actualError[0]);
  //  Serial.print(" lastError: ");
  //  Serial.println(lastError[0]);

  //Conrolador PROPORCIONAL - Contribui para a estabilidade, tem resposabilidade mediana
  Amostra();
  actualError[0] = desiredVelocity[0] - (PWMs[0] / abs(PWMs[0])) * actualVelocity[0];
  actualError[1] = desiredVelocity[1] - (PWMs[1] / abs(PWMs[1])) * actualVelocity[1];
  P_Gain [0] =  actualError[0] * kp;
  P_Gain [1] =  actualError[1] * kp;

  //Controlador INTEGRAL - Rapido tempo de subida, lenta resposta
  accumulator[0] += actualError[0];
  accumulator[1] += actualError[1];
  I_Gain [0] =  accumulator[0] * ki;
  I_Gain [1] =  accumulator[1] * ki;

  //  Serial.print(" accumulator: ");
  //  Serial.print(accumulator[0]);

  //Controlador DERIVATIVO - Antecipa decisões, é rapido, sensivel a ruidos
  D_Gain [0] =  (actualError[0] - lastError[0]) * kd;
  D_Gain [1] =  (actualError[1] - lastError[1]) * kd;
  lastError[0] = actualError[0];
  lastError[1] = actualError[1];

  //Contolador PID Final.... Ganho em Velocidade -- (((((((CORRIGIDO PARA PWM))))))
  PID[0] = P_Gain[0] + I_Gain[0] + D_Gain[0];
  PID[1] = P_Gain[1] + I_Gain[1] + D_Gain[1];

  //  Serial.print(" P_GAIN: ");
  //  Serial.print(P_Gain[0]);
  //  Serial.print(" I_GAIN: ");
  //  Serial.print(I_Gain[0]);
  //  Serial.print(" D_GAIN: ");
  //  Serial.print(D_Gain[0]);
  //  Serial.print(" PID: ");
  //  Serial.print(PID[0]);

  //Corrigindo PWM
  PWMs[0] += (int)PID[0];
  PWMs[1] += (int)PID[1];

  //Evitar saturações
  if (PWMs[0] > 255) PWMs[0] = 255;
  if (PWMs[1] > 255) PWMs[1] = 255;
  if (PWMs[0] < -255) PWMs[0] = -255;
  if (PWMs[1] < -255) PWMs[1] = -255;
}

//Verifica Serial toda vez que um loop é finalizado -> Loop menor = Leitura mais rápida
void serialEvent() {

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    // Serial.print(inChar);
    if (inChar == ID || myMessage) {

      if (inChar == 'K') {
        calibrar = true;
        // Serial.println("Calibrando");
      }
     

      myMessage = true;

       if (inChar == '#') {
        myMessage = false;
        stringComplete = true;
        return;
      }
      inputString += inChar;
    }
  }
  
}

//Separa as velocidade pada cada motor
void interpretaMSG(String msg) {

  bool f = false;

  for (int j = 1; j < msg.length(); j++)
  {
    if (msg[j] != ';')
    {
      if (f == false) //Ainda esta lendo o primeiro pwm
        vel1 += msg[j];
      else
        vel2 += msg[j];
    }
    else //Se encontrou um ';' significa que o primeiro pwm ja foi lido
      f = true;
  }
}

//Converte PWM em inteiro e armazena no vetor PWMs[]
void CriaVELCITY() {

  char floatbuf[32];
  vel1.toCharArray(floatbuf, sizeof(floatbuf));
  desiredVelocity[0] = atof(floatbuf);
  vel2.toCharArray(floatbuf, sizeof(floatbuf));
  desiredVelocity[1] = atof(floatbuf);

  vel1 = "";
  vel2 = "";
}

//Atualiza PWMs dos Motores
void Run(int PWMs[]) {

  int PWMA, PWMB;
  PWMA = PWMs[0];
  PWMB = PWMs[1];

  //  Serial.print(" PWM_1: ");
  //  Serial.print(PWMs[0]);
  //  Serial.print(" PWM_2: ");
  //  Serial.println(PWMs[1]);

  if (PWMA < 0) {
    analogWrite(I1A, -PWMA); //Seta Rotação Normal
    digitalWrite(I1B, LOW);
  }
  else {
    digitalWrite(I1A, LOW); //Seta Rotação Contrária
    analogWrite(I1B, PWMA);
  }

  if (PWMB < 0) {
    analogWrite(I2A, -PWMB); //Seta Rotação Contrária
    digitalWrite(I2B, LOW);
  }
  else {
    digitalWrite(I2A, LOW); //Seta Rotação Contrária
    analogWrite(I2B, PWMB);
  }
}

//Para os Motores
void Stop()
{
  analogWrite(I2A, 0);
  digitalWrite(I2B, LOW);
  analogWrite(I1A, 0);
  digitalWrite(I1B, LOW);
}

//Coleta a velocidade atual dos Motores
void Amostra() {
  Dt = (millis() - timer);

  //  Serial.print(" Dt: ");
  //  Serial.print(Dt);
  //  Serial.print(" eRIGHT: ");
  //  Serial.print(encoderRIGHT);
  //  Serial.print(" - eLEFT: ");
  //  Serial.println(encoderLEFT);

  actualVelocity[0] = 0;
  actualVelocity[1] = 0;

  if (Dt != 0) {
    actualVelocity[0] = 1000 * encoderRIGHT / (5 * 75 * Dt); //(5 - Numero de pulsos por volta) (75/1 - 1 volta da roda por 75 do motor) (1000 transformar em milisegundos para segundos)
    actualVelocity[1] = 1000 * encoderLEFT / (5 * 75 * Dt); //(5 - Numero de pulsos por volta) (75/1 - 1 volta da roda por 75 do motor) (1000 transformar em milisegundos para segundos)
  }

  //  Serial.print(" DesiredVEL_1: ");
  //  Serial.print(desiredVelocity[0]);
  //  Serial.print(" DesiredVEL_2: ");
  //  Serial.print(desiredVelocity[1]);
  //  Serial.print(" ActualVEL_1: ");
  //  Serial.print(actualVelocity[0]);
  //  Serial.print(" ActualVEL_2: ");
  //  Serial.println(actualVelocity[1]);

  encoderRIGHT = 0;
  encoderLEFT = 0;

  timer = millis();
}

//Conta pulsos do encoder da Direita ->INTERRUPÇÃO INT0
void ENC_RIGHT() {
  encoderRIGHT++;
}

//Conta pulsos do encoder da Esquerda ->INTERRUPÇÃO INT1
void ENC_LEFT() {
  encoderLEFT++;
}

/* Very Small @2015 - Núcleo de Robotica Pequi Mecânico - Universidade Federal de Goias
 *
 * Algoritimo utilizado nos robos: OJUARA, LENHADOR e GOLEIRO
 *
 * Utilizando:  - Controlador PID
 *              - Comunicação sem fio através de Xbee S1
 *              - Encoders nos motores
 *              - Padrao de Mensagem: ID+VelocityOne+;+VelocityTwo+# -> Exemplo: "A1.8;2.5#"
 *
 * Algoritimo desenvolvido pela equipe VS-2015:
 *
 *    - Lucas da Silva Assis          - Apolo Silva Marton
 *    - Vinicius Araújo Santos        - Vinicius Paulo Lopes de Oliveira
 *    - Walisson Gobbo de Aguas       - Kléber Macedo Cabral
 *    - Rafael Ribeiro Carvalho Vaz   - Pedro Santos de Rezende Alves
 *    - Gabriel Pereira Lima Reis     - Átila Gracco
 *    - Alexandre Monteiro Ribeiro    - Victor Philipe Moreira dos Santos Camargo
 *
 *Todos os direitos reservados;
 *
*/
