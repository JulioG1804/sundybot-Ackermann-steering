# sundybot-Ackermann-steering
Este repositorio contiene los paquetes necesarios para la puesta en marcha de un robot con sistema de direccionamiento Ackermann, llamado sundybot.

## Arquitectura de Hardware
La arquitectura de hardware del robot involucra dos Arduinos que desempeñan funciones críticas:
- **Arduino Nano**: Actúa como controlador de velocidad para un motor brushless YSIDO sensored, calculando las RPM y determinando la velocidad sobre el eje de giro de la rueda mediante una relación de reducción.
- **Arduino Uno**: Captura valores del sensor BNO055 y está conectado a un divisor de tensión que mide el voltaje de una batería LiPo de dos celdas.

Programacion de Arduino UNO y Arduino Nano [Programacion Arduino](https://github.com/JulioG1804/sundybot-Ackermann-steering/tree/main/Recursos%20complementarios).

Los diagramas de conexión y los esquemas de la arquitectura de software y hardware se encuentran en la carpeta de recursos complementarios [Recursos complementarios](https://github.com/JulioG1804/sundybot-Ackermann-steering/tree/main/Recursos%20complementarios).

## Paquetes Proporcionados
Para visualizar al robot desde una computadora remota se incluyen dos paquetes:
- [sundy_gui_PKG](https://github.com/JulioG1804/sundybot-Ackermann-steering/tree/main/sundy_gui_pkg): Este paquete contiene dos interfaces gráficas (Dashboard y Collector).
- [sundy_model_PKG](https://github.com/JulioG1804/sundybot-Ackermann-steering/tree/main/sundy_model_pkg): Este paquete incluye los modelos URDF del robot así como programas para la interfaz gráfica de RViz.

Para la ejecución del robot y la activación de los diferentes tópicos y nodos, se incluye el siguiente paquete:
- [sundy_operation](https://github.com/JulioG1804/sundybot-Ackermann-steering/tree/main/sundy_operation): Este paquete contiene un nodo de teleoperación y el nodo maestro del robot.

## Paquetes Necesarios
Este robot utiliza varios paquetes complementarios que deben ser instalados tanto en la computadora remota como en el robot:

### Para la computadora remota
- [hector_slam](https://github.com/JulioG1804/sundybot-Ackermann-steering/tree/main/hector_slam): Este paquete ha sido modificado, por lo que la versión mencionada aquí es exclusivamente compatible con la arquitectura ROS descrita para este robot.

### Para el robot
- [joy](https://github.com/ros-drivers/joystick_drivers/tree/main/joy): Permite conectar el mando a distancia F710.
- [rosserial](https://github.com/ros-drivers/rosserial): Facilita la comunicación serial con los Arduinos.
- [ldlidar_stl_ros](https://github.com/JulioG1804/sundybot-Ackermann-steering/tree/main/ldlidar_stl_ros): Levanta la información obtenida por el LIDAR LD19. Este paquete ha sido modificado para adaptarlo a la arquitectura desarrollada para este robot.

## Programas de Arduino
- [ArduinoUno](https://github.com/ros-drivers/joystick_drivers/tree/main/joy): Programacion de arduino UNO.
- [ArduinoNano](https://github.com/ros-drivers/rosserial): Programacion de Arduino Nano.

