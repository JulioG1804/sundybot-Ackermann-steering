#!/usr/bin/env python3
# import tkinter as tk
import rospy
from std_msgs.msg import Float32MultiArray
from std_msgs.msg import Int32
from tkinter import *
import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import datetime
import random

global valx_var, valy_var, valz_var, valRoll_var, valPitch_var, valYaw_var, valEst_var, lbvalEst, valRPMm_var, valSpeedm_var, valRPMw_var, valSpeedw_var
global valBt1_var, valBt2_var, valtemp_var, valcpu_var, valtempex_var, valhora_var, hora_actual, fig
global sp, cv, pv, sp_data, cv_data, pv_data, line_sp, line_cv, line_pv, x_data, canvas
sp = 0
cv = 0
pv = 0

print("NOTE: To close this NODE and free the terminal close the window directly!!!")


def callback(msg):
    global valx_var, valy_var, valz_var, valRoll_var, valPitch_var, valYaw_var, valEst_var, lbvalEst, valRPMm_var, valSpeedm_var, valRPMw_var, valSpeedw_var
    global valBt1_var, valBt2_var, valtemp_var, valcpu_var, valtempex_var, valhora_var, hora_actual
    global sp, cv, pv
    valx_var.set(f"{round(msg.data[0], 3)}")
    valy_var.set(f"{round(msg.data[1], 3)}")
    valz_var.set(f"{round(msg.data[2], 3)}")
    valRoll_var.set(f"{round(msg.data[3], 3)}")
    valPitch_var.set(f"{round(msg.data[4], 3)}")
    valYaw_var.set(f"{round(msg.data[5], 3)}")
    valRPMm_var.set(f"{int(msg.data[10])}")
    valSpeedm_var.set(f"{int(msg.data[11])}")
    valRPMw_var.set(f"{int(msg.data[12])}")
    valSpeedw_var.set(f"{round(msg.data[13], 3)}")
    valBt1_var.set(f"{round(msg.data[14], 2)}V")
    valBt2_var.set(f"{round(msg.data[15], 2)}V")
    valtemp_var.set(f"{int(msg.data[16])}º")
    valcpu_var.set(f"{int(msg.data[17])}%")
    valtempex_var.set(f"{round(msg.data[18], 3)}°")
    actualizar_hora()

    sp = msg.data[7]
    cv = msg.data[8]
    pv = msg.data[9]

    if (msg.data[6] == 1):
        valEst_var.set("Dis ...")
        lbvalEst.config(bg="#FF4242")

    if (msg.data[6] == 2):
        valEst_var.set("Cal ...")
        lbvalEst.config(bg="#FFEB42")

    if (msg.data[6] == 3):
        valEst_var.set("Set")
        lbvalEst.config(bg="#76FF42")


def update_plot(frame):
    global sp, cv, pv, sp_data, cv_data, pv_data, line_sp, line_cv, line_pv, x_data, canvas
    # Actualizar los datos
    sp_data.append(sp)
    cv_data.append(cv)
    pv_data.append(pv)
    sp_data.pop(0)
    cv_data.pop(0)
    pv_data.pop(0)

    # Actualizar los gráficos
    line_sp.set_data(x_data, sp_data)
    line_cv.set_data(x_data, cv_data)
    line_pv.set_data(x_data, pv_data)

    # Necesario para redibujar el gráfico
    canvas.draw()


# Iniciar nodo ROS
rospy.init_node('Dashboard_gui', anonymous=False)
publisher = rospy.Publisher('flag_estado', Int32, queue_size=10)
subscriber = rospy.Subscriber("Data_total", Float32MultiArray, callback)

def mainprogram(root):
	global valx_var, valy_var, valz_var, valRoll_var, valPitch_var, valYaw_var, valEst_var, lbvalEst, valRPMm_var, valSpeedm_var, valRPMw_var, valSpeedw_var
	global valBt1_var, valBt2_var, valtemp_var, valcpu_var, valtempex_var, valhora_var, hora_actual, fig
	global sp, cv, pv, sp_data, cv_data, pv_data, line_sp, line_cv, line_pv, x_data, canvas
	root.title("Dashboard")
	icono = PhotoImage(file='/home/julio1804/sundybot/src/sundy_gui_pkg/imgs/logo UE.png')  # Usa un archivo .png
	root.iconphoto(True, icono)
	root.resizable(False, False)

	# Crear una figura y los ejes
	fig, ax = plt.subplots()
	line_sp, = ax.plot([], [], label='SP', color='blue')
	line_cv, = ax.plot([], [], label='CV', color='green')
	line_pv, = ax.plot([], [], label='PV', color='red')

	# Configurar los límites y leyendas del gráfico
	ax.set_xlim(0, 100)
	ax.set_ylim(0, 8)
	ax.set_xticklabels([])
	ax.xaxis.set_visible(False)
	# ax.legend()

	# ************************************* FRAME1 **************************************
	# ***********************************************************************************

	# Frame 1 principal
	frame1 = Frame(root, width=400, height=160)
	frame1.grid(row=0, column=0)

	# LabelFrame Odom configuration
	labelframe1 = LabelFrame(frame1, text=" Odom configuration ", font=('Ubuntu', 14))
	labelframe1.place(x=10, y=10, width=380, height=140)

	# Label para mostrar x y z
	lbx = Label(labelframe1, text="x", font=('Ubuntu', 14))
	lbx.place(x=200, y=10, width=20, height=20)
	lby = Label(labelframe1, text="y", font=('Ubuntu', 14))
	lby.place(x=200, y=40, width=20, height=20)
	lbz = Label(labelframe1, text="z", font=('Ubuntu', 14))
	lbz.place(x=200, y=70, width=20, height=20)

	# Entry para mostrar valores
	valx_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valx_var.set(" - ")
	lbvalx = Label(labelframe1, textvariable=valx_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5, relief="solid")
	lbvalx.place(x=240, y=10, width=80, height=20)
	valy_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valy_var.set(" - ")
	lbvaly = Label(labelframe1, textvariable=valy_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5, relief="solid")
	lbvaly.place(x=240, y=40, width=80, height=20)
	valz_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valz_var.set(" - ")
	lbvalz = Label(labelframe1, textvariable=valz_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5, relief="solid")
	lbvalz.place(x=240, y=70, width=80, height=20)
	# Button OdomSet
	btn1 = Button(labelframe1, text="Odom set", font=('Ubuntu', 12))
	btn1.place(x=30, y=30, width=100, height=40)
	btn1.bind('<ButtonPress>', lambda event: publisher.publish(1))
	btn1.bind('<ButtonRelease>', lambda event: publisher.publish(0))
	# ***********************************************************************************
	# ***********************************************************************************


	# ************************************* FRAME2 **************************************
	# ***********************************************************************************

	# Frame 2 principal
	frame2 = Frame(root, width=400, height=190)
	frame2.grid(row=1, column=0)

	# LabelFrame Odom configuration
	labelframe2 = LabelFrame(frame2, text=" BNO055 status ", font=('Ubuntu', 14))
	labelframe2.place(x=10, y=10, width=380, height=170)

	# Label para mostrar Roll Pitch Yaw
	lbRoll = Label(labelframe2, text="Roll", font=('Ubuntu', 14))
	lbRoll.place(x=180, y=10, width=40, height=20)
	lbPitch = Label(labelframe2, text="Pitch", font=('Ubuntu', 14))
	lbPitch.place(x=180, y=40, width=45, height=20)
	lbYaw = Label(labelframe2, text="Yaw", font=('Ubuntu', 14))
	lbYaw.place(x=180, y=70, width=40, height=20)
	lbEst = Label(labelframe2, text="State", font=('Ubuntu', 14))
	lbEst.place(x=170, y=100, width=50, height=20)

	# Entry para mostrar valores
	valRoll_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valRoll_var.set(" - ")
	lbvalRoll = Label(labelframe2, textvariable=valRoll_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		          relief="solid")
	lbvalRoll.place(x=240, y=10, width=80, height=20)
	valPitch_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valPitch_var.set(" - ")
	lbvalPitch = Label(labelframe2, textvariable=valPitch_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		           relief="solid")
	lbvalPitch.place(x=240, y=40, width=80, height=20)
	valYaw_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valYaw_var.set(" - ")
	lbvalYaw = Label(labelframe2, textvariable=valYaw_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5, relief="solid")
	lbvalYaw.place(x=240, y=70, width=80, height=20)
	valEst_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valEst_var.set(" - ")
	lbvalEst = Label(labelframe2, textvariable=valEst_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5, relief="solid")
	lbvalEst.place(x=240, y=100, width=80, height=20)

	# Button IMUReset
	btn2 = Button(labelframe2, text="IMU reset", font=('Ubuntu', 12))
	btn2.place(x=30, y=40, width=100, height=40)
	btn2.bind('<ButtonPress>', lambda event: publisher.publish(2))
	btn2.bind('<ButtonRelease>', lambda event: publisher.publish(0))
	# ***********************************************************************************
	# ***********************************************************************************


	# ************************************* FRAME3 **************************************
	# ***********************************************************************************

	# Frame 3 principal
	frame3 = Frame(root, width=400, height=220)
	frame3.grid(row=2, column=0)

	# LabelFrame Odom configuration
	labelframe3 = LabelFrame(frame3, text=" PID monitor ", font=('Ubuntu', 14))
	labelframe3.place(x=10, y=10, width=380, height=200)

	# Label para mostrar kp ki kd
	lbkp = Label(labelframe3, text="kp", font=('Ubuntu', 14))
	lbkp.place(x=20, y=20, width=20, height=20)
	lbki = Label(labelframe3, text="ki", font=('Ubuntu', 14))
	lbki.place(x=20, y=50, width=20, height=20)
	lbkd = Label(labelframe3, text="kd", font=('Ubuntu', 14))
	lbkd.place(x=20, y=80, width=20, height=20)
	lbSP = Label(labelframe3, text="SP", font=('Ubuntu', 14), fg="blue")
	lbSP.place(x=10, y=115, width=30, height=20)
	lbCV = Label(labelframe3, text="CV", font=('Ubuntu', 14), fg="green")
	lbCV.place(x=45, y=115, width=30, height=20)
	lbPV = Label(labelframe3, text="PV", font=('Ubuntu', 14), fg="red")
	lbPV.place(x=80, y=115, width=30, height=20)

	# Entry para mostrar valores
	lbvalkp = Label(labelframe3, text=" 1.0 ", font=('Ubuntu', 13), bg="white", borderwidth=0.5, relief="solid")
	lbvalkp.place(x=60, y=20, width=50, height=20)
	lbvalki = Label(labelframe3, text=" 0.5 ", font=('Ubuntu', 13), bg="white", borderwidth=0.5, relief="solid")
	lbvalki.place(x=60, y=50, width=50, height=20)
	lbvalkd = Label(labelframe3, text=" 0.001 ", font=('Ubuntu', 13), bg="white", borderwidth=0.5, relief="solid")
	lbvalkd.place(x=60, y=80, width=50, height=20)

	canvas = FigureCanvasTkAgg(fig, master=labelframe3)
	canvas_widget = canvas.get_tk_widget()
	canvas_widget.config(highlightthickness=2, highlightbackground='black')
	canvas_widget.place(x=120, y=0, width=250, height=165)

	# Datos iniciales
	x_data = list(range(100))
	sp_data = [0] * 100
	cv_data = [0] * 100
	pv_data = [0] * 100

	# Animación
	#ani = animation.FuncAnimation(fig, update_plot, interval=10)
	# ***********************************************************************************
	# ***********************************************************************************

	# ************************************* FRAME4 **************************************

	# Frame 4 principal
	frame4 = Frame(root, width=400, height=345)
	frame4.grid(row=3, column=0)

	# LabelFrame Motor and wheel data configuration
	labelframe4 = LabelFrame(frame4, text=" Other information ", font=('Ubuntu', 14))
	labelframe4.place(x=10, y=10, width=380, height=325)

	# LabelFrame Motor data -> is a subframe of LabelFreme4----------------------------
	motorFrame = Frame(labelframe4, width=185, height=115)
	motorFrame.grid(row=0, column=0)
	labelmotorFrame = LabelFrame(motorFrame, text=" Motor data ", font=('Ubuntu', 14))
	labelmotorFrame.place(x=10, y=10, width=170, height=100)

	lbRPMm = Label(labelmotorFrame, text="RPM", font=('Ubuntu', 14))
	lbRPMm.place(x=10, y=10, width=40, height=20)
	lbSpeedm = Label(labelmotorFrame, text="PWM", font=('Ubuntu', 14))
	lbSpeedm.place(x=10, y=40, width=47, height=20)

	valRPMm_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valRPMm_var.set(" - ")
	lbvalRPMm = Label(labelmotorFrame, textvariable=valRPMm_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		          relief="solid")
	lbvalRPMm.place(x=80, y=10, width=80, height=20)
	valSpeedm_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valSpeedm_var.set(" - ")
	lbvalSpeedm = Label(labelmotorFrame, textvariable=valSpeedm_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		            relief="solid")
	lbvalSpeedm.place(x=80, y=40, width=80, height=20)

	# LabelFrame wheel data -> is a subframe of LabelFreme4-----------------------------
	wheelFrame = Frame(labelframe4, width=185, height=115)
	wheelFrame.grid(row=0, column=1)
	labelwheelFrame = LabelFrame(wheelFrame, text=" Wheel data ", font=('Ubuntu', 14))
	labelwheelFrame.place(x=10, y=10, width=170, height=100)

	lbRPMw = Label(labelwheelFrame, text="RPM", font=('Ubuntu', 14))
	lbRPMw.place(x=10, y=10, width=40, height=20)
	lbSpeedw = Label(labelwheelFrame, text="Speed", font=('Ubuntu', 14))
	lbSpeedw.place(x=10, y=40, width=55, height=20)

	valRPMw_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valRPMw_var.set(" - ")
	lbvalRPMw = Label(labelwheelFrame, textvariable=valRPMw_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		          relief="solid")
	lbvalRPMw.place(x=80, y=10, width=80, height=20)
	valSpeedw_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valSpeedw_var.set(" - ")
	lbvalSpeedw = Label(labelwheelFrame, textvariable=valSpeedw_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		            relief="solid")
	lbvalSpeedw.place(x=80, y=40, width=80, height=20)

	# LabelFrame Battery data -> is a subframe of LabelFreme4-----------------------------
	bateryFrame = Frame(labelframe4, width=185, height=115)
	bateryFrame.grid(row=1, column=0)
	labelbateryFrame = LabelFrame(bateryFrame, text=" Battery data ", font=('Ubuntu', 14))
	labelbateryFrame.place(x=10, y=10, width=170, height=100)

	lbBt1c = Label(labelbateryFrame, text="BC", font=('Ubuntu', 14))
	lbBt1c.place(x=10, y=10, width=40, height=20)
	lbBt2esc = Label(labelbateryFrame, text="BE", font=('Ubuntu', 14))
	lbBt2esc.place(x=10, y=40, width=40, height=20)

	valBt1_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valBt1_var.set(" - ")
	lbvalBt1c = Label(labelbateryFrame, textvariable=valBt1_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		          relief="solid")
	lbvalBt1c.place(x=80, y=10, width=80, height=20)
	valBt2_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valBt2_var.set(" - ")
	lbvalBt2c = Label(labelbateryFrame, textvariable=valBt2_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		          relief="solid")
	lbvalBt2c.place(x=80, y=40, width=80, height=20)

	# LabelFrame Raspy data -> is a subframe of LabelFreme4-----------------------------
	RaspyFrame = Frame(labelframe4, width=185, height=115)
	RaspyFrame.grid(row=1, column=1)
	labelRaspyFrame = LabelFrame(RaspyFrame, text=" Raspy data ", font=('Ubuntu', 14))
	labelRaspyFrame.place(x=10, y=10, width=170, height=100)

	lbtemp = Label(labelRaspyFrame, text="temp", font=('Ubuntu', 14))
	lbtemp.place(x=10, y=10, width=45, height=20)
	lbcpu = Label(labelRaspyFrame, text="CPU", font=('Ubuntu', 14))
	lbcpu.place(x=10, y=40, width=40, height=20)

	valtemp_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valtemp_var.set(" - ")
	lbvaltemp = Label(labelRaspyFrame, textvariable=valtemp_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		          relief="solid")
	lbvaltemp.place(x=80, y=10, width=80, height=20)
	valcpu_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valcpu_var.set(" - ")
	lbvalcpu = Label(labelRaspyFrame, textvariable=valcpu_var, font=('Ubuntu', 13), bg="white", borderwidth=0.5,
		         relief="solid")
	lbvalcpu.place(x=80, y=40, width=80, height=20)

	# LabelFrame Raspy data -> is a subframe of LabelFreme5-----------------------------
	clockTempFrame = Frame(labelframe4, width=375, height=67)
	clockTempFrame.grid(row=2, columnspan=2)

	valhora_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valhora_var.set(" - ")
	Time = Label(clockTempFrame, textvariable=valhora_var, font=('DS-Digital', 40))
	#Time.place(x=10, y=10, width=200, height=45)
	Time.place(x=10, y=10, width=350, height=45)

	valtempex_var = StringVar()  # Variable de Tkinter para almacenar el estado de IMU
	valtempex_var.set(" - ")
	Temp = Label(clockTempFrame, textvariable=valtempex_var, font=('DS-Digital', 40))
	#Temp.place(x=250, y=10, width=95, height=45)


	# ***********************************************************************************

def actualizar_hora():
    # Obtiene la hora actual del sistema
    global hora_actual, valhora_var
    hora_actual = datetime.datetime.now().strftime("%H:%M:%S")
    valhora_var.set(hora_actual)

def on_close():
    global root
    # Detener ROS
    rospy.signal_shutdown('Window Closed')

    # Cerrar la ventana de Tkinter
    root.destroy()
    plt.close('all')  # Asegurarse de cerrar todas las figuras de Matplotlib
    plt.close(fig)
    print("NOTE: Interface and ROS nodes closed correctly :)")


def main():
    global valx_var, valy_var, valz_var, valRoll_var, valPitch_var, valYaw_var, valEst_var, lbvalEst, valRPMm_var, valSpeedm_var, valRPMw_var, valSpeedw_var
    global fig
    global sp, cv, pv, sp_data, cv_data, pv_data, line_sp, line_cv, line_pv, x_data, canvas
    global root
    try:
        root = Tk()
        mainprogram(root)
        root.protocol("WM_DELETE_WINDOW", on_close)  # Vincula el manejo de cierre con la función on_close

        # Animación
        ani = animation.FuncAnimation(fig, update_plot, interval=10)

        # Ejecutar el bucle principal de Tkinter
        root.mainloop()
    except rospy.ROSInterruptException:
        pass


if __name__ == '__main__':
    main()

