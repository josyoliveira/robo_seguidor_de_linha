int sensor[8] = {2, 4, 7, 8, 10, 11, 12, 18};
int valores[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int buzzer = A3;
int Vdiv = A7;
int motorAD = 9;//;5;
int motorAE = 6;
int motorBD = 5;
int motorBE = 3;
int PWMa = 3;
//int led_bat = 16; nao usar



float kp = 0;
float kd = 0;
float ki = 0;
float pid = 0;
int erro = 0;

int vel = 0;
int velA = 0;
int velB = 0;
int erroAnterior = 0;

int last_proportional = 0;
int media = 0;
int lastmedia = 0;
int derivative = 0;
int integral = 0;
int proportional = 0;
int k, tempo;
int extremidade = -1;
unsigned long int ut = 0;
boolean exibirStatus = false;

int maxVel = 70;

int pista = 0;
int linha = 0;

int preto = 0;
int branco = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < 8; i++) {
    pinMode(sensor[i], INPUT);
  }
  pinMode(buzzer, OUTPUT);
  pinMode(motorAE, OUTPUT);
  pinMode(motorAD, OUTPUT);
  pinMode(motorBE, OUTPUT);
  pinMode(motorBD, OUTPUT);
  //pinMode(led_bat, OUTPUT);
  pinMode(PWMa, OUTPUT);

  //delay(2000);
  //pisarFundoNaReta();

  pista = branco;
  linha = preto;
}

void lerSensores() {
  for (int i = 0; i < 8; i++) {
    valores[i] = digitalRead(sensor[i]);

  }
 // exibirValores();
}

void exibirValores() {
  for (int i = 0; i < 8; i++) {
    Serial.print(valores[i]);
  }
  Serial.println("");
}

void loop() {
  exibirStatus = false;
  lerSensores();
  selicionarControles();
  int setpoint = 3500;




  lerMedia();

  /* if(media == -1){
       media = lastmedia;
     }*/

  int velA = maxVel;
  int velB = maxVel;

  proportional = media - setpoint; // Replace set_point by your set point
  integral = integral + proportional;
  derivative = proportional - last_proportional;
  last_proportional = proportional;

  //bateria 100% velocidade 50 float  resultado =  (proportional * 1.3 + integral * 0.02 + derivative * 10);//vlocidade de curva 50 float  resultado =  (proportional * 1.3 + integral * 0.03 + derivative * 7);//velocidade de curva 45 float  resultado =  (proportional * 1.3 + integral * 0.02 + derivative * 7);
  ///velocidade 60   float  resultado =  (proportional * 2.5 + integral * 0.005 + derivative * 21);
  //voltagem acima de 7.90 e menor que 8.00 velocidade 50
  //velocidade com 6.4v  float  resultado =  (proportional * 2.2 + integral * 0.008 + derivative * 17);// 7.80 e 7.85
  float  resultado =  (proportional * kp + integral * ki + derivative * kd);//kp 3 ki 0.022 kd 30
  //float  resultado =  (proportional * 3 + integral * 0.005 + derivative * 32);
  exibirValores("Média", media);
  exibirValores("Resultado PID", resultado);
  exibirValores("prop", proportional * kp);
  exibirValores("int", integral * ki);
  exibirValores("derivativo", derivative * kd);

  if (resultado < 0) {

    velA = velA + (resultado / 100);
    velA = velA + (velA * 0.55);
    velB = maxVel;
  }
  else if (resultado > 0) {
    velB = velB - (resultado / 100);
    velB = velB - (velB * 0.5);

    velA = maxVel;
  }


  if (velA < 0) {
    velA = 0;
  }
  //if(velA >= 204){ velA = 100; }
  if (velB < 0) {
    velB = 0;
  }
  //if(velB >= 204){ velB = 100; }

  exibirValores("velocidade do motor A", velA);
  exibirValores("velocidade do motor B", velB);

  exibirValores("diferença", velA - velB);
  int riLf[2];

  checarSensoresDasExtremidades();
  if (media == 3500) {
    velA = 250;
    velB = 250;
  }

  if (media == -1) {
    if (lastmedia == 1) {
      inverterParaDireita(40, 40);//70 50
      
    }
    else if (lastmedia == 0) {
      inverterParaEsquerda(40, 40);
      
    }
  }
  else {
    frente(velA, velB);
  }

  //frente(velA, velB);
  lastmedia = extremidade;
  // lastmedia = media;
  extremidade = 2;
  //bateria();

}
void checarSensoresDasExtremidades() {
  verificarExtremidadeDireita();
  verificarExtremidadeEsquerda();
}

void verificarExtremidadeDireita() {
  for (int  i = 0; i < 8; i++) {
    if (valores[i] == 1) {
      if (i == 7 || i == 6) {
        extremidade = 1;
      }
    }
  }
}
void verificarExtremidadeEsquerda() {
  for (int  i = 0; i < 8; i++) {
    if (valores[i] == 1) {
      if (i == 0 || i == 1) {
        extremidade = 0;
      }
    }
  }
}



void lerMedia() {
  int sensores = 0;
  int somaDosValores = 0;

     

  for (int i = 0; i < 8; i++) {
   
  //  valores[i] =  mudarValorDosSensorParaPistaBranca(valores[i]);
    sensores += (valores[i] * i) * 1000;
    somaDosValores += valores[i];
  }
  media = sensores / somaDosValores;
}

int mudarValorDosSensorParaPistaBranca(int valorLido){
 
     if(valorLido == preto){
       return branco;
     }
   
  return preto;
}

void pisarFundoNaReta() {
  frente(200, 200);
  delay(800);
}

void selicionarControles() {

  //
  //  maxVel = 180;
  //  kp = 5;
  //  ki = 0.22;
  //  kd = 900;

  maxVel = 160;
  kp = 5;
  ki = 0.1;
  kd = 2000;


  /*maxVel = 150;
    kp = 5;
    ki = 0.2;
    kd = 2500;
  */
  //  maxVel = 90;
  //  kp = 4;
  //  ki = 0.015;
  //  kd = 200;




  //   maxVel = 70;
  //   kp = 2.5;
  //   ki = 0.032;
  //   kd = 51.2;
  ////
  //   maxVel = 60;
  //   kp = 2.92;
  //   ki = 0.0348 ;
  //   kd = 23.6;515

  //   maxVel = 50;
  //   kp = 2.1;
  //   ki = 0.029;
  //   kd = 13;
  //
}







void frente(int veloA, int veloB) {
  analogWrite(motorAD, 0);
  analogWrite(motorAE, veloA - (veloA * 0));
  analogWrite(motorBD, veloB - (veloB * 0.1));
  analogWrite(motorBE, 0);
}

void inverterParaDireita(int veloA, int veloB) {
  analogWrite(motorAD, 0);
  analogWrite(motorAE, veloA);
  analogWrite(motorBD, 0);
  analogWrite(motorBE, veloB);
}

void inverterParaEsquerda(int veloA, int veloB) {
  analogWrite(motorAD, veloA);
  analogWrite(motorAE, 0);
  analogWrite(motorBD, veloB);
  analogWrite(motorBE, 0);
}

void parar() {
  analogWrite(motorAD, 0);
  analogWrite(motorAE, 0);
  analogWrite(motorBD, 0);
  analogWrite(motorBE, 0);
}

void bateria() {
  if ((millis() - ut) >= tempo) {

    int  valor_bat = analogRead(Vdiv);
    Serial.println(valor_bat);

    //if (valor_bat <= 767) {//original
    if (valor_bat >= 500 && valor_bat <= 767) {
      k = !k;
    }
    else {
      k = 0;
    }

    //Serial.println(k);

    if (k == 1) {
      tone(buzzer, 4000);
      tempo = 100;
    }
    else {
      noTone(buzzer);
      tempo = 2000;
    }

    ut = millis();
  }
}

void exibirValores(String text, float valor)
{
  if (exibirStatus) {
    Serial.println(text);
    Serial.println(valor);
  }
}
