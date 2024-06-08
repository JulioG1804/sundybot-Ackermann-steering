#include "ros/ros.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/Int32.h"
#include <sensor_msgs/Imu.h>
#include <tf/tf.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <cmath>

using namespace std;
float VelocidadLineal, sp, cv, pv, RPMmotor, RPMrueda, VangularR, VlinealR, sentidoGiro, AnguloGiro, PWM;
float Roll, Pitch, Yaw, RollA, PitchA, YawA, RollOFF, PitchOFF, YawOFF, EstadoBNO055, PBvoltage, LIPOvoltage;
float temperaspy, cpuraspy;
int flag;
float L = 0.27;
float thetat, theta, yt, xt, y, x, z, thetaF;
float tms = 0.1;

// Función para leer la temperatura del CPU desde el sistema de archivos
float readCPUTemperature()
{
  ifstream file("/sys/class/thermal/thermal_zone0/temp");
  if (!file.is_open())
  {
    cerr << "Error al abrir el archivo de temperatura." << endl;
    return -1; // Retorna -1 en caso de error
  }
  float temp;
  file >> temp;
  file.close();
  return temp / 1000; // Convierte de miligrados a grados Celsius
}

//Función para leer la carga de la CPU desde el sistema de archivos************************************
//*****************************************************************************************************
float readCPUUsage()
{
  ifstream file("/proc/stat");
  if (!file.is_open())
  {
    cerr << "Error al abrir el archivo stat." << endl;
    return -1; // Retorna -1 en caso de error
  }

  string line;
  getline(file, line);
  file.close();

  // Extraer los tiempos de CPU del archivo
  vector<long> times;
  string time;
  istringstream iss(line);
  iss >> time; // Descarta la primera palabra "cpu"

  while (iss >> time)
  {
    times.push_back(stol(time));
  }

  // Calcular el porcentaje de uso de la CPU
  long idle_time = times[3]; // Tiempo ocioso
  long total_time = accumulate(times.begin(), times.end(), 0L);

  static long prev_idle_time = 0, prev_total_time = 0;

  long delta_total = total_time - prev_total_time;
  long delta_idle = idle_time - prev_idle_time;

  float cpu_usage = 100.0f * (1.0f - ((float)delta_idle / delta_total));

  // Actualizar valores previos
  prev_idle_time = idle_time;
  prev_total_time = total_time;

  return cpu_usage;
}
//*****************************************************************************************************
//*****************************************************************************************************


//Función millis***************************************************************************************
//*****************************************************************************************************
long long milis()
{
  auto ahora = std::chrono::system_clock::now();
  auto milisegundos = std::chrono::time_point_cast<std::chrono::milliseconds>(ahora);
  auto valor = milisegundos.time_since_epoch();
  return valor.count();
}

//*****************************************************************************************************
//*****************************************************************************************************


//ROS programa*****************************************************************************************
//*****************************************************************************************************
void Callback1(const std_msgs::Float32MultiArray::ConstPtr &msg)
{
  VelocidadLineal = msg->data[0];
  sp = msg->data[1];
  cv = msg->data[2];
  pv = msg->data[3];
  RPMrueda = msg->data[4];
  RPMmotor = RPMrueda * 9;
  VangularR = msg->data[5];
  VlinealR = msg->data[6];
  sentidoGiro = msg->data[7];
  PWM = msg->data[8];
  AnguloGiro = msg->data[9];
}

void Callback2(const std_msgs::Float32MultiArray::ConstPtr &msg2)
{
  RollA = msg2->data[0];
  PitchA = msg2->data[1];
  YawA = msg2->data[2];
  EstadoBNO055 = msg2->data[3];
  PBvoltage = msg2->data[4];
  LIPOvoltage = msg2->data[5];
}

void Callback3(const std_msgs::Int32::ConstPtr &msg3)
{
  flag = msg3->data;
}

//Función que muestra offset
void offset()
{
  //Reseteamos variables
  RollOFF = 0.0;  // Roll
  PitchOFF = 0.0; // Pitch
  YawOFF = 0.0;   // Yaw
  for (int8_t i = 0; i < 20; i++)
  {

    //Guardar valores
    RollOFF = RollOFF + RollA;
    PitchOFF = PitchOFF + PitchA;
    YawOFF = YawOFF + YawA;
  }
  RollOFF = RollOFF / 20.0;   // Roll
  PitchOFF = PitchOFF / 20.0; // Pitch
  YawOFF = YawOFF / 20.0;     // Yaw
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "Sundy_collector");
  ros::NodeHandle n;
  ros::Subscriber sub1 = n.subscribe("ArduinoNano_data", 1000, Callback1);
  ros::Subscriber sub2 = n.subscribe("ArduinoUno_data", 1000, Callback2);
  ros::Subscriber sub3 = n.subscribe("flag_estado", 1000, Callback3);
  ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 50);
  tf::TransformBroadcaster odom_broadcaster;
  tf::TransformBroadcaster br;
  geometry_msgs::TransformStamped transformStamped;
  static tf2_ros::StaticTransformBroadcaster static_broadcaster;
  geometry_msgs::TransformStamped static_transform;

  ros::Publisher pub = n.advertise<sensor_msgs::Imu>("imu", 10);
  ros::Publisher pub2 = n.advertise<std_msgs::Float32MultiArray>("Data_total", 10);
  ros::Rate loop_rate(10); // Define la tasa de mensaje
  std::vector<float_t> vec;
  long long tiempoAnterior = milis();

  while (ros::ok())
  {
    //****************** MANIPULACION DE ANGULOS DE EULER********
    //***********************************************************
    if (flag == 1)
    {
      offset();
    }
    if (flag == 2)
    {
      RollOFF = 0.0;  // Roll
      PitchOFF = 0.0; // Pitch
      YawOFF = 0.0;   // Yaw
    }
    Roll = RollA - RollOFF;
    Pitch = PitchA - PitchOFF;
    Yaw = YawA - YawOFF;
    if (Yaw < 0)
    {
      Yaw = Yaw + 6.28319;
      if (Yaw == 6.28319)
      {
        Yaw = 0.0;
      }
    }


    //****************** PUBLICACION DATOS IMU ******************
    //***********************************************************
    tf::Quaternion q = tf::createQuaternionFromRPY(Roll, Pitch, Yaw); // convertimos los angulos
    sensor_msgs::Imu imu_msg;                                         // Crea un mensaje de IMU


    //****************** PUBLICACION DATOS IMU ******************
    //***********************************************************
    imu_msg.header.stamp = ros::Time::now();
    imu_msg.header.frame_id = "imu_sundy";
    quaternionTFToMsg(q, imu_msg.orientation);


    //************************ ODOMETRIA ************************
    //***********************************************************
    if (flag == 1)
    {
      x = 0;
      y = 0;
      z = 0;
    }
    long long tiempoActual = milis();
    if (tiempoActual - tiempoAnterior >= 100)
    {
      thetat = (sentidoGiro * tan(AnguloGiro)) / L;
      theta = theta + (thetat * tms);
      thetaF = Yaw;
      yt = sentidoGiro * sin(thetaF);
      xt = sentidoGiro * cos(thetaF);
      y = y + (yt * tms); // tms
      x = x + (xt * tms); // tms

      geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(theta);
      geometry_msgs::TransformStamped odom_trans;
      odom_trans.header.stamp = ros::Time::now();
      odom_trans.header.frame_id = "odom";
      odom_trans.child_frame_id = "base_link";

      nav_msgs::Odometry odom;
      odom.header.stamp = ros::Time::now();
      odom.header.frame_id = "odom";

      odom.pose.pose.position.x = x;
      odom.pose.pose.position.y = y;
      odom.pose.pose.position.z = 0.0;
      odom.pose.pose.orientation.x = 0.0;
      odom.pose.pose.orientation.y = 0.0;
      odom.pose.pose.orientation.z = sin(thetaF / 2.0);
      odom.pose.pose.orientation.w = cos(thetaF / 2.0);

      odom.child_frame_id = "base_link";
      odom.twist.twist.linear.x = sentidoGiro;
      odom.twist.twist.linear.y = 0.0;
      odom.twist.twist.angular.z = theta;

      // Publicar TF***********************************************************
      //**********************************************************************
      transformStamped.header.stamp = ros::Time::now();
      transformStamped.header.frame_id = "odom";
      transformStamped.child_frame_id = "base_link";

      transformStamped.transform.translation.x = x;
      transformStamped.transform.translation.y = y;
      transformStamped.transform.translation.z = 0.0;

      transformStamped.transform.rotation.x = 0.0;
      transformStamped.transform.rotation.y = 0.0;
      transformStamped.transform.rotation.z = sin(thetaF / 2.0);
      transformStamped.transform.rotation.w = cos(thetaF / 2.0);

      br.sendTransform(transformStamped);

      odom_pub.publish(odom);
      tiempoAnterior = tiempoActual;
    }

    // Publicar TF statico lidar y body**************************************
    //**********************************************************************
    static_transform.header.stamp = ros::Time::now();
    static_transform.header.frame_id = "base_link";
    static_transform.child_frame_id = "lidar_link";
    static_transform.transform.translation.x = 0.27;
    static_transform.transform.translation.y = 0.0;
    static_transform.transform.translation.z = 0.125;
    static_transform.transform.rotation.x = 0.0;
    static_transform.transform.rotation.y = 0.0;
    static_transform.transform.rotation.z = 0.0;
    static_transform.transform.rotation.w = 1.0;
    static_broadcaster.sendTransform(static_transform);

    //******************** PUBLICACION DATOS ********************
    //***********************************************************
    std_msgs::Float32MultiArray msg;
    vec.resize(20);
    msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
    msg.layout.dim[0].size = vec.size();
    msg.layout.dim[0].stride = 1;
    msg.layout.dim[0].label = std::string("vectorSundy");
    msg.data = vec;

    temperaspy = readCPUTemperature();
    cpuraspy = readCPUUsage();

    vec[0] = x;                // x odom
    vec[1] = y;                // y odom
    vec[2] = 0.0;              // z odom
    vec[3] = Roll;             // Roll
    vec[4] = Pitch;            // Pitch
    vec[5] = Yaw;              // Yaw
    vec[6] = EstadoBNO055;     // Estado IMU
    vec[7] = sp;               // SP
    vec[8] = cv;               // CV
    vec[9] = pv;               // PV
    vec[10] = RPMmotor;        // RPMmotor
    vec[11] = PWM;             // SpeedMotor o PWM
    vec[12] = RPMrueda;        // RPMvehicle
    vec[13] = sentidoGiro;     // Speedvehicle
    vec[14] = PBvoltage;       // BC
    vec[15] = LIPOvoltage;     // BE
    vec[16] = temperaspy;      // temperatura raspy
    vec[17] = cpuraspy;        // CPU
    vec[18] = 19.0;            // temp externa
    vec[19] = VelocidadLineal; // temp externa

    // Publica el mensaje
    pub2.publish(msg);
    pub.publish(imu_msg);
    ros::spinOnce();   // siempre se agrega
    loop_rate.sleep(); // espera lo necesario para completar la frecuencia de espera
  }
  return 0;
}
