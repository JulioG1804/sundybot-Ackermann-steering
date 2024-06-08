//Librerias ROS
#include <ros.h>
#include <std_msgs/Float32MultiArray.h>

//Librerias Adicionales
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//Variables de BNO055
int calibration;         //Parametro de calibracion
float Roll, Pitch, Yaw;  //Normales

//Variables lectura de voltage
float PBvoltage, LIPOvoltage, PBlecture, LIPOlecture;

//Variable Array
float data[6] = {};

//flags e indicadores
int offsetST;
float flag = 0.0;

//Declaracion de objetos
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

//ROS NODES********************************************************************************************
ros::NodeHandle nh;  //configuracion del nodo
std_msgs::Float32MultiArray Auno_msg;
ros::Publisher pub("ArduinoUno_data", &Auno_msg);

void setup(void) {
  //Parametros iniciales de ROS
  nh.initNode();  //inicializacion del nodo
  nh.advertise(pub);
  flag = 0.0;

  //*******************Comprobacion de estado de IMU*******************
  //*******************************************************************

  //Inicailizacion de la IMU
  if (!bno.begin()) {
    flag = 1.0;  //No hay IMU
  }

  if (flag == 0.0) {
    while (calibration != 3) {
      uint8_t sys, gyro, accel, mag;
      flag = 2.0;  //IMU calibrando

      //Verificar el estado de calibración del sensor
      bno.getCalibration(&sys, &gyro, &accel, &mag);
      calibration = mag;
      Voltagelecture();

      Auno_msg.data_length = 6;  //tamaño del vecto
      data[0] = 0.0;
      data[1] = 0.0;
      data[2] = 0.0;
      data[3] = flag;
      data[4] = PBvoltage;    //PBvoltage
      data[5] = LIPOvoltage;  //LIPOvoltage
      Auno_msg.data = data;
      pub.publish(&Auno_msg);
      nh.spinOnce();
    }
  }
  //*******************************************************************
  //*******************************************************************
}

void loop() {
  //buscarBNO055();  //Comprobamos que hay IMU

  if (flag == 1.0) {
    Auno_msg.data_length = 6;  //tamaño del vecto
    data[0] = 0.0;
    data[1] = 0.0;
    data[2] = 0.0;
    data[3] = flag;
    data[4] = 0.0;  //PBvoltage
    data[5] = 0.0;  //LIPOvoltage
    Auno_msg.data = data;
  } else {
    flag = 3.0;  //IMU en funcionamiento

    //Leer el cuaternión y conversion a angulos de Euler (Roll, pitch, Yaw)
    imu::Quaternion quat = bno.getQuat();
    imu::Vector<3> euler = quat.toEuler();
    //**************************Calculo de Valores**************************
    //**********************************************************************
    Roll = (euler.z() - 0.044);   //Roll
    Pitch = (euler.y() - 0.047);  //Pitch
    Yaw = euler.x();              //Yaw
    if (Yaw < 0) {
      Yaw = Yaw + 6.28319;
      if (Yaw == 6.28319) {
        Yaw = 0.0;
      }
    }
    //**********************************************************************
    //**********************************************************************
    Voltagelecture();
    Auno_msg.data_length = 6;  //tamaño del vecto
    data[0] = Roll;
    data[1] = Pitch;
    data[2] = Yaw;
    data[3] = flag;
    data[4] = PBvoltage;    //PBvoltage
    data[5] = LIPOvoltage;  //LIPOvoltage
    Auno_msg.data = data;
  }

  pub.publish(&Auno_msg);  //publicamos informacion
  nh.spinOnce();           //siempre debe ir
}

void Voltagelecture() {
  // Lee el valor de los pines analógicos A3 y A2
  PBlecture = analogRead(A2);    //voltage en Powerbank
  LIPOlecture = analogRead(A3);  //voltage en LIPO

  // Convierte el valor analógico a voltage
  PBvoltage = PBlecture * (5.0 / 1023.0) * 5.0;
  LIPOvoltage = LIPOlecture * (5.0 / 1023.0) * 5.0;
}
