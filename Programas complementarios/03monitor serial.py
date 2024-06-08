import serial
import tkinter as tk
from tkinter import ttk


def iniciar_recepcion():
    puerto = puerto_combo.get()
    velocidad = velocidad_combo.get()

    # Configurar el puerto serial
    puerto_serial = serial.Serial(puerto, int(velocidad))
    archivo_salida = 'datos_recibidos.txt'

    # Abre el archivo de salida en modo de escritura
    with open(archivo_salida, 'w') as archivo:
        try:
            while True:
                # Lee una línea de datos del puerto serial
                linea = puerto_serial.readline().decode('latin1').strip()
                print(linea)  # Muestra la línea recibida en la consola

                # Escribe la línea en el archivo de salida
                archivo.write(linea + '\n')

        except KeyboardInterrupt:
            print("Deteniendo la recepción de datos...")
            puerto_serial.close()


# Crear la ventana principal
ventana = tk.Tk()
ventana.title("Interfaz para Recepción de Datos")

# Crear y colocar etiquetas
puerto_label = ttk.Label(ventana, text="Puerto COM:")
puerto_label.grid(column=0, row=0, padx=10, pady=5, sticky="e")

velocidad_label = ttk.Label(ventana, text="Velocidad:")
velocidad_label.grid(column=0, row=1, padx=10, pady=5, sticky="e")

# Obtener la lista de puertos COM disponibles
puertos_disponibles = [f"COM{i + 1}" for i in range(256)]

# Crear y colocar combos
puerto_combo = ttk.Combobox(ventana, values=puertos_disponibles, width=10)
puerto_combo.grid(column=1, row=0, padx=10, pady=5)
puerto_combo.current(0)

velocidad_combo = ttk.Combobox(ventana, values=["9600", "19200", "38400", "57600", "115200"], width=10)
velocidad_combo.grid(column=1, row=1, padx=10, pady=5)
velocidad_combo.current(0)

# Crear y colocar botón
iniciar_button = ttk.Button(ventana, text="Iniciar Recepción", command=iniciar_recepcion)
iniciar_button.grid(column=0, row=2, columnspan=2, pady=10)

# Iniciar el bucle de la aplicación
ventana.mainloop()