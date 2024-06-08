#include "ros/ros.h"
#include "sensor_msgs/Joy.h"
#include "geometry_msgs/Twist.h"
float giro, avance;

// Limites de velocidad
float inputMin = -3.0;
float inputMax = 5.0;

// funcion de mapeo trabaja igual que la funcion map de arduino
float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void joyCallback(const sensor_msgs::Joy::ConstPtr &JS)
{
  // ROS_INFO("Axes: [%f, %f]", JS->axes[0], JS->axes[3]);
  giro = JS->axes[0];
  avance = JS->axes[3];
}

// funcion principal
int main(int argc, char **argv)
{
  ros::init(argc, argv, "teleop_gamepad");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("joy", 1000, joyCallback);

  ros::Publisher cmd_vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
  ros::Rate loop_rate(10);

  while (ros::ok())
  {
    geometry_msgs::Twist msg;

    if (avance == -0.0)
    {
      msg.linear.x = 0.0;
      msg.angular.z = map(giro, -1.0, 1.0, -0.78, 0.78);
    }
    else
    {
      msg.angular.z = map(giro, -1.0, 1.0, -0.78, 0.78);
      if (avance >= 0)
      {
        msg.linear.x = map(avance, 0.0, 1.0, 0.0, inputMax);
      }
      if (avance < 0)
      {
        msg.linear.x = map(avance, -1.0, 0.0, inputMin, 0.0);
      }
    }

    cmd_vel_pub.publish(msg);
    ros::spinOnce(); 
    loop_rate.sleep();
  }
  return 0;
}
