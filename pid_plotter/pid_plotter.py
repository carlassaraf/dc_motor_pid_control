import dearpygui.dearpygui as dpg
import serial.tools.list_ports
import serial
import json


class PIDPlotter:
    def __init__(self, width, height):
        self._width = width
        self._height = height

        # Tiempo de muestreo
        self._ts = 20e-3

        # Parametros para graficar
        self._reference_data = [0.0]
        self._position_data = [0.0]
        self._pwm_data = [0.0]
        self._error_data = [0.0]
        self._time = [0.0]

        # Puerto serial
        self._port = None

        # Maxima cantidad de puntos para graficar
        self._max_points = 100

        # Configurar DearPyGUI
        dpg.create_context()
        dpg.create_viewport(title="PID Plotter", width=width, height=height)

        # Configuro una ventana para el ploteo del PID
        with dpg.window(label="App", tag="app_window"):

            # Crear la ventana de selección de puerto serial
            with dpg.child_window(tag="serial_window"):
                dpg.add_text("Seleccione un puerto serial:")
                
                # Listar los puertos seriales disponibles y crear el combo box
                ports = [port.device for port in serial.tools.list_ports.comports()]

                if ports:
                    dpg.add_combo(ports, label="Puertos", callback=self._port_selected_callback, tag="serial_combo")
                else:
                    dpg.add_text("No se encontraron puertos seriales.")

                dpg.add_text("", tag="serial_status")

            # Vista para el grafico del PID
            with dpg.child_window(tag="position_window"):
                # Ventana de plot
                with dpg.plot(label="Posicion y Referencia del Motor", height=-1, width=-1):
                    dpg.add_plot_legend()
                    dpg.add_plot_axis(dpg.mvXAxis, label="Tiempo [s]", tag="position_time_axis")

                    with dpg.plot_axis(dpg.mvYAxis, label="Referencia [deg]", tag="reference_axis"):
                        dpg.add_line_series([], [], label="Referencia", tag="reference_plot")
                        dpg.set_axis_limits("reference_axis", -180.0, 180.0)

                    with dpg.plot_axis(dpg.mvYAxis, label="Posicion [deg]", tag="position_axis"):
                        dpg.add_line_series([], [], label="Posición", tag="position_plot")
                        dpg.set_axis_limits("position_axis", -180.0, 180.0)

            # Vista para el PWM y el Error
            with dpg.child_window(tag="pwm_window"):
                with dpg.plot(label="PWM y Error", height=-1, width=-1):
                    dpg.add_plot_legend()
                    dpg.add_plot_axis(dpg.mvXAxis, label="Tiempo [s]", tag="pwm_time_axis")
                    
                    with dpg.plot_axis(dpg.mvYAxis, label="PWM [%]", tag="pwm_axis"):
                        dpg.add_line_series([], [], label="PWM", tag="pwm_plot")
                    
                    with dpg.plot_axis(dpg.mvYAxis, label="Error [deg]", tag="error_axis"):
                        dpg.add_line_series([], [], label="Error", tag="error_plot")
                        dpg.set_axis_limits("error_axis", -180.0, 180.0)

        dpg.setup_dearpygui()
        dpg.show_viewport()
        # Configurar el tamaño inicial de la ventana
        self._resize_window_callback(None, None)

    def run(self):
        """
        Corre la interfaz en loop.
        """
        while dpg.is_dearpygui_running():
            # Verifico los datos del puerto serie
            if self._port:
                try:
                    if self._port.in_waiting > 0:
                        # Leo y decodifico el JSON
                        data = self._port.readline().decode().strip()
                        data = json.loads(data)
                        # Veo si hay datos para actualizar
                        self._position_data.append(data["position"])
                        self._reference_data.append(data["reference"])
                        self._error_data.append(data["error"])
                        self._time.append(self._time[-1] + self._ts)
                        # Veo si me excedi de las muestras
                        if len(self._time) > self._max_points:
                            # Elimino el primer punto
                            self._position_data = self._position_data[1:]
                            self._reference_data = self._reference_data[1:]
                            self._error_data = self._error_data[1:]
                            self._time = self._time[1:]
                
                except:
                    pass
            
            # Actualizo con los puntos que tengo
            self._update_plot()
            self._refresh_ports()
            dpg.render_dearpygui_frame()

        dpg.destroy_context()

    def _update_plot(self):
        """
        Actualiza los graficos.
        """
        # Actualiza cada grafico
        dpg.set_value("reference_plot", [self._time, self._reference_data])
        dpg.set_value("position_plot", [self._time, self._position_data])
        dpg.set_value("pwm_plot", [self._time, self._pwm_data])
        dpg.set_value("error_plot", [self._time, self._error_data])
        # Ajusto los limites horizontales
        dpg.set_axis_limits("position_time_axis", min(self._time), max(self._time))
        dpg.set_axis_limits("pwm_time_axis", min(self._time), max(self._time))
        dpg.set_axis_limits("error_axis", min(self._error_data), max(self._error_data))

    def _resize_window_callback(self, sender, app_data):
        """
        Callback para ajustar el tamaño de la ventana 
        """
        # Obtengo las dimensiones del viewport y las configuro para la ventana
        width, height = dpg.get_viewport_client_width() - 22.5, dpg.get_viewport_client_height() - 50
        dpg.set_item_width("serial_window", width)
        dpg.set_item_height("serial_window", height // 7)
        dpg.set_item_width("position_window", width)
        dpg.set_item_height("position_window", 3 * height // 7)
        dpg.set_item_width("pwm_window", width)
        dpg.set_item_height("pwm_window", 3 * height // 7)

    def _refresh_ports(self):
        """
        Callback que refresca la lista de puertos.
        """
        # Listar los puertos seriales disponibles y crear el combo box
        ports = [port.device for port in serial.tools.list_ports.comports()]
        if ports:
            dpg.configure_item("serial_combo", items=ports)
        else:
            dpg.configure_item("serial_combo", items=["No se encontraron puertos seriales."])

    def _port_selected_callback(self, sender, app_data):
        """
        Obtiene el valor seleccionado del menu desplegable
        """
        if self._port:
            self._port.close()
            self._port = None
        else:
            try:
                self._port = serial.Serial(app_data, 115200)
                dpg.set_value(item="serial_status", value=f"Puerto {app_data} conectado con exito!")
            except:
                dpg.set_value(item="serial_status", value="Error conectando al puerto!")
