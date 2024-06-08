# sundybot-Ackermann-steering
Este repositorio contiene los paquetes necesarios para la puesta en marcha de un robot con sistema de direccionamiento Ackermann, llamado sundybot.

## Arquitectura de Hardware
La arquitectura de hardware del robot involucra dos Arduinos que desempeñan funciones críticas:
- **Arduino Nano**: Actúa como controlador de velocidad para un motor brushless YSIDO sensored, calculando las RPM y determinando la velocidad sobre el eje de giro de la rueda mediante una relación de reducción.
- **Arduino Uno**: Captura valores del sensor BNO055 y está conectado a un divisor de tensión que mide el voltaje de una batería LiPo de dos celdas.

Los diagramas de conexión y los esquemas de la arquitectura de software y hardware se encuentran en la carpeta de recursos complementarios.

## Paquetes Proporcionados
Para visualizar al robot desde una computadora remota se incluyen dos paquetes:
- [sundy_gui_PKG](https://github.com/SteveMacenski/slam_toolbox): Este paquete contiene dos interfaces gráficas (Dashboard y Collector).
- [sundy_model_PKG](https://github.com/SteveMacenski/slam_toolbox): Este paquete incluye los modelos URDF del robot así como programas para la interfaz gráfica de RViz.

Para la ejecución del robot y la activación de los diferentes tópicos y nodos, se incluye el siguiente paquete:
- [sundy_operation](https://github.com/SteveMacenski/slam_toolbox): Este paquete contiene un nodo de teleoperación y el nodo maestro del robot.
