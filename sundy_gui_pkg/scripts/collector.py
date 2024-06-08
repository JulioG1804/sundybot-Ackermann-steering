#!/usr/bin/env python3
import rospy
from tkinter import *
import tkinter as tk
from tkinter import filedialog
from std_msgs.msg import Float32MultiArray
import csv
import os
import threading

print("NOTE: To close this NODE and free the terminal close the window directly!!!")
global root
class DataCollector:
    def __init__(self):
        self.data = []
        self.is_recording = False
        self.subscriber = None  # Placeholder for the subscriber

    def setup_subscriber(self, topic_name="Data_total"):
        self.subscriber = rospy.Subscriber(topic_name, Float32MultiArray, self.callback)

    def callback(self, msg):
        if self.is_recording:
            rounded_data = [round(num, 3) for num in msg.data]
            self.data.append(rounded_data)
            rospy.loginfo("Data collected.")

    def start_recording(self):
        self.data = []  # Clear previous data
        self.is_recording = True
        rospy.loginfo("Recording started.")

    def stop_recording(self):
        self.is_recording = False
        rospy.loginfo("Recording stopped and ready to save data.")
        

    def save_to_csv(self, filepath):
        with open(filepath, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            for row in self.data:
                writer.writerow(row)
        rospy.loginfo(f"Data saved to {filepath}")
        print("NOTE: To close this NODE and free the terminal close the window directly!!!")

class Application(tk.Frame):
    def __init__(self, master=None):
        super().__init__(master)
        self.pack()
        self.create_widgets()
        self.collector = DataCollector()

    def create_widgets(self):
        self.name_label = tk.Label(self, text="Nombre del archivo:", font=('Ubuntu', 13))
        self.name_label.pack()

        self.name_entry = tk.Entry(self)
        self.name_entry.pack()

        self.path_label = tk.Label(self, text="Ruta de guardado:", font=('Ubuntu', 13))
        self.path_label.pack()

        self.path_entry = tk.Entry(self)
        self.path_entry.pack()

        self.browse_button = tk.Button(self, text="Buscar", command=self.browse_folder, font=('Ubuntu', 10))
        self.browse_button.pack()

        self.start_button = tk.Button(self, text="Empezar a guardar", command=self.start_recording, font=('Ubuntu', 10))
        self.start_button.pack()

        self.stop_button = tk.Button(self, text="Detener y guardar", command=self.stop_recording, font=('Ubuntu', 10))
        self.stop_button.pack()

    def browse_folder(self):
        directory = filedialog.askdirectory()
        self.path_entry.delete(0, tk.END)
        self.path_entry.insert(0, directory)

    def start_recording(self):
        if not rospy.is_shutdown():
            self.collector.start_recording()

    def stop_recording(self):
        if not rospy.is_shutdown():
            self.collector.stop_recording()
            filename = self.name_entry.get()
            directory = self.path_entry.get()
            filepath = os.path.join(directory, filename)
            threading.Thread(target=self.collector.save_to_csv, args=(filepath,)).start()

def on_closing():
    global root
    # Detener ROS
    rospy.signal_shutdown('Window Closed')
    print("NOTE: Interface and ROS nodes closed correctly :)")
    root.destroy()
    
def main():
    global root
    rospy.init_node('Collector_gui', anonymous=False)
    root = tk.Tk()
    root.title("Data Collector")  # Set the window title here
    icono = PhotoImage(file='/home/julio1804/sundybot/src/sundy_gui_pkg/imgs/logo UE.png')  # Usa un archivo .png
    root.iconphoto(True, icono)
    root.geometry("300x200")  # Set the size of the window
    root.resizable(False, False)
    app = Application(master=root)
    app.collector.setup_subscriber()  # Setup ROS subscriber
    root.protocol("WM_DELETE_WINDOW", on_closing)  # Handle window close even
    app.mainloop()

if __name__ == '__main__':
    main()

