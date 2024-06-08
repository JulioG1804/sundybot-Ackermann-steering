//Librerias de ROS
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Float32MultiArray.h>
#include <math.h>   // Libreria necesaria para operaciones matematicas
#include <Servo.h>  // Librería necesaria para las ESC de control

//Declaracion de objetos
Servo myESC;   // Crea un objeto Servo para el ESC
Servo servo1;  //definimos el objeto Servo en nuestro servo

//Variables de Entrada
float VelocidadLineal, AnguloGiro, AnguloGiroAck;  //Variable de entrada para trabajo de proceso


//Variables sensor Hall
#define HALL_B 2           //Pin digital 2
#define HALL_C 3           //Pin digital 3
int stateB, stateC;        //Estado de los sensores
int stateBant, stateCant;  //Estado de los anterior de los sensores
float sentidoGiro;         //Guarda el sentido de giro

//Variable de potenciometro y salida PWM
int vel;  //Conversion de resolucion de entrada a PWM de salida

//Variables de funcion millis
unsigned long previusMillis = 0;   //Tiempo previo
unsigned long previusMillis2 = 0;  //Tiempo previo
long interval = 10;                //Intervalo
long interval2 = 100;              //Intervalo
volatile int contador = 0;         //Contador de pulsos
long int contadorROS = 0;

//Variables controlador PID
float sp, pv;                 //Set point y variable de proceso
float cv, cv1;                //variable de control
float error, error1, error2;  //errores
float kp;                     //ganancia proporcional
float ki;                     //ganancia integral (0.4)
float kd;                     //ganancia derivativa
float Tm;                     //tiempo de muestreo

//Variable de filtro media exponecial
float alpha = 0.5;  //0.3

//Variables descriptivas del modelo para RPM y velocidades lineales y angulares
int pulXseg;                                                    //pulsos por segundo
float RPMfilter1, RPMfilter2, RPM1, RPM2, VangularR, VlinealR;  //variables de angulares y lineales
float rRueda = 0.086 / 2;                                       //radio de la rueda de entrada

//Variable Array
float data[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

//otras cosas
float flag2;

//ROS NODES********************************************************************************************
ros::NodeHandle nh;  //configuracion del nodo
std_msgs::Float32MultiArray encoder_msg;
ros::Publisher pubPM("ArduinoNano_data", &encoder_msg);


//Callback para suscriptor de cmd_vel
void cmdVelCallback(const geometry_msgs::Twist &vel) {
  VelocidadLineal = vel.linear.x;
  AnguloGiro = vel.angular.z;
}

//Declaracion del Suscriptor y variable
ros::Subscriber<geometry_msgs::Twist> sub_cmd_vel("cmd_vel", cmdVelCallback);
//*****************************************************************************************************

void setup() {
  //Parametros iniciales de ROS
  nh.initNode();  //inicializacion del nodo
  nh.advertise(pubPM);
  nh.subscribe(sub_cmd_vel);  //suscripcion ROS

  //PinMode
  pinMode(HALL_B, INPUT);  // Configurar pin 2 como entrada
  pinMode(HALL_C, INPUT);  // Configurar pin 3 como entrada

  //configuracion de pines e inicializaciones
  myESC.attach(9);   // Pin de control de ESC, pin 9
  servo1.attach(8);  //inicializamos el servo 1

  //configuracion de enlace con el variador y servo
  myESC.writeMicroseconds(1450);  //valor de inicializacion del variador
  servo1.write(90);               //valor de inicializacion del servo
  delay(2000);                    //espera de 5 segundos

  //Interrupciones de Encoder
  attachInterrupt(0, interrupcion2, CHANGE);  //flancos de subida pin 2 (HALL_B)
  attachInterrupt(1, interrupcion2, CHANGE);  //flancos de subida pin 3 (HALL_C)

  //parametros iniciales para el filtrado
  RPM1 = 0.0;
  RPMfilter1 = 0.0;
  RPM2 = 0.0;
  RPMfilter2 = 0.0;
}

void loop() {
  //VelocidadLineal = 1.5;

  //Angulo de Giro Ackerman****************
  AckermanAngle();
  servo1.write(AnguloGiroAck);  //escribimos los grados a girar
  //***************************************

  //Calculo de RPM y PID*******************
  CalRPM();                      //Calculos de RPMs
  PIDcontroller();               //Calculo de velocidad
  myESC.writeMicroseconds(vel);  //mandamos PWM a variador
  //***************************************

  //Envio de Datos*************************
  encoder_msg.data_length = 10;  //tamaño del vecto
  data[0] = VelocidadLineal;     //Velocidad lineal introducida
  data[1] = sp;                  //Setpoit
  data[2] = cv;                  //Variable de control
  data[3] = pv;                  //Variable de proceso
  data[4] = RPMfilter2;          //RPM de la Rueda
  data[5] = VangularR;           //Velocidad Angular de la Rueda
  data[6] = VlinealR;            //Velocidad lineal de la Rueda
  data[7] = sentidoGiro;         //Velocidad lineal con sentido de Giro
  data[8] = vel;                 //PWM enviadas al motor
  data[9] = AnguloGiro;          //Angulo de Giro
  encoder_msg.data = data;
  //***************************************

  //condicion motor detenido
  if (pv == 0.0) sentidoGiro = 0.0;

  //publicacion de valores PID y RPM
  pubPM.publish(&encoder_msg);  //publicamos informacion
  nh.spinOnce();                //siempre debe ir
}

//interrupcion 1
void interrupcion2() {
  contador++;
  stateB = digitalRead(HALL_B);  // Leer estado del pin 2
  stateC = digitalRead(HALL_C);  // Leer estado del pin 3
  if (stateB == LOW && stateC == HIGH && stateBant == LOW && stateCant == LOW) sentidoGiro = VlinealR;
  if (stateB == HIGH && stateC == HIGH && stateBant == LOW && stateCant == HIGH) sentidoGiro = VlinealR;
  if (stateB == HIGH && stateC == LOW && stateBant == HIGH && stateCant == HIGH) sentidoGiro = VlinealR;
  if (stateB == LOW && stateC == LOW && stateBant == HIGH && stateCant == HIGH) sentidoGiro = VlinealR;

  if (stateB == HIGH && stateBant == HIGH && stateC == HIGH && stateCant == LOW) sentidoGiro = VlinealR * -1.0;
  if (stateB == LOW && stateBant == HIGH && stateC == HIGH && stateCant == HIGH) sentidoGiro = VlinealR * -1.0;
  if (stateB == LOW && stateBant == LOW && stateC == LOW && stateCant == HIGH) sentidoGiro = VlinealR * -1.0;
  if (stateB == HIGH && stateBant == LOW && stateC == LOW && stateCant == HIGH) sentidoGiro = VlinealR * -1.0;
  stateBant = stateB;
  stateCant = stateC;
}

void PIDcontroller() {
  //************************************************************************************************
  //************************************* CONTROL PID **********************************************
  if (VelocidadLineal >= 0) {
    sp = VelocidadLineal;
  }
  if (VelocidadLineal < 0) {
    sp = VelocidadLineal * -1;
  }
  //sp = VelocidadLineal;
  kp = 1.0;    //ganancia proporcional
  ki = 0.5;    //ganancia integral (0.4) -2.0
  kd = 0.001;  //ganancia derivativa (0.001)
  Tm = 0.1;    //tiempo de muestreo
  //Error
  error = sp - pv;
  //Ecuacion de diferencias
  cv = cv1 + (kp + kd / Tm) * error + (-kp + ki * Tm - 2 * kd / Tm) * error1 + (kd / Tm) * error2;
  //recursividad error
  cv1 = cv;
  error2 = error1;
  error1 = error;

  //saturacion de salida
  if (cv > 5.0) {
    cv = 5.0;
  }
  if (sp <= 0.0) {  //0.1 (El potenciometro tiene mucho ruido)
    cv = 0.0;
    cv1 = 0.0;
    pv = 0.0;
    sp = 0.0;
  }
  if (VelocidadLineal >= 0) {
    vel = mapear(cv, 0.0, 20.0, 1450.0, 1946.0);  //mapear(cv, 0.0, 20.0, 1450.0, 1946.0);
  } else if (VelocidadLineal < 0) {
    vel = mapear(cv, 0.0, 20.0, 1450.0, 953.0);
  }
}

void CalRPM() {
  //************************************* CALCULO RPM **********************************************
  //************************************************************************************************
  if ((millis() - previusMillis) >= interval) {
    //RPM1 = 100.0 * contador * (60.0 / 4.0);                  //RPM del eje del motor
    //RPMfilter1 = alpha * RPM1 + (1.0 - alpha) * RPMfilter1;  //filtro exponencial EMA
    RPM2 = 100.0 * contador * (60.0 / 36.0);                 //RPM del eje de la rueda (4x9=36)
    RPMfilter2 = alpha * RPM2 + (1.0 - alpha) * RPMfilter2;  //filtro exponencial EMA
    VangularR = 2.0 * PI * RPMfilter2 / 60;                  //conversion de RPM a rad/s
    VlinealR = VangularR * rRueda;                           //sabiendo que el diametro de la rueda es 0.086m
    pv = VlinealR;
    pulXseg = contador * 10;   //pulsos por segundo
    contador = 0;              //reiniciamos contador
    previusMillis = millis();  //igualamos al tiempo actual
  }
  //***********************************************************************************************
  //***********************************************************************************************
}

//funcion de mapeo para float
float mapear(float valor, float inMin, float inMax, float outMin, float outMax) {
  return (valor - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void AckermanAngle() {
  AnguloGiroAck = mapear(AnguloGiro, 0.78, -0.78, 0.0, 180.0);
}