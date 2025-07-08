import dearpygui.dearpygui as dpg
import serial.tools.list_ports
import serial
import json


class PIDPlotter:
    def __init__(self, width, height):
        self._width = width
        self._height = height

        # Tiempo de muestreo
        self._ts = 0.0

        # Lapso de tiempo de ploteo
        self._tp = 0.0

        # Parametros para graficar
        self._reference_data = [0.0]
        self._position_data = [0.0]
        self._pwm_data = [0.0]
        self._error_data = [0.0]
        self._time = [0.0]

        # Constantes PID
        self._kp = 0.0
        self._ki = 0.0
        self._kd = 0.0

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

            # Vista para configurar las constantes de PID
            with dpg.child_window(tag="pid_tuning_window"):
                with dpg.group(horizontal=True):
                    with dpg.group():
                        dpg.add_text("Ajuste manual de PID:")
                        dpg.add_slider_float(label="Kp", tag="slider_kp", default_value=0.0, min_value=0.0, max_value=10.0, width=600)
                        dpg.add_slider_float(label="Ki", tag="slider_ki", default_value=0.0, min_value=0.0, max_value=2.0, width=600)
                        dpg.add_slider_float(label="Kd", tag="slider_kd", default_value=0.0, min_value=0.0, max_value=1.0, width=600)

                    with dpg.group():
                        dpg.add_spacer(height=20)
                        dpg.add_button(label="Enviar PID", callback=self._send_pid_callback, width=200, height=50)

            # Vista para el grafico del PID
            with dpg.child_window(tag="position_window"):
                # Constantes de PID
                dpg.add_text("PID constants:", tag="pid_constants")
                with dpg.group(horizontal=True):
                    dpg.add_text("Kp: 0.00", tag="Kp")
                    dpg.add_text("Ki: 0.00", tag="Ki")
                    dpg.add_text("Kd: 0.00", tag="Kd")
                    dpg.add_text("Ts [ms]: ", tag="Ts")
                # Ventana de plot
                with dpg.plot(label="Posicion y Referencia del Motor", height=-1, width=-1):
                    dpg.add_plot_legend()
                    dpg.add_plot_axis(dpg.mvXAxis, label="Tiempo [s]", tag="position_time_axis")

                    with dpg.plot_axis(dpg.mvYAxis, label="[deg]", tag="degree_axis"):
                        dpg.add_line_series([], [], label="Referencia", tag="reference_plot")
                        dpg.add_line_series([], [], label="Posición", tag="position_plot")
                        dpg.add_line_series([], [], label="Error", tag="error_plot")
                        dpg.set_axis_limits("degree_axis", -90.0, 90.0)


            # Vista para el PWM y el Error
            with dpg.child_window(tag="pwm_window"):
                with dpg.plot(label="PWM y Error", height=-1, width=-1):
                    dpg.add_plot_legend()
                    dpg.add_plot_axis(dpg.mvXAxis, label="Tiempo [s]", tag="pwm_time_axis")
                    
                    with dpg.plot_axis(dpg.mvYAxis, label="PWM [%]", tag="pwm_axis"):
                        dpg.add_line_series([], [], label="PWM", tag="pwm_plot")
                        dpg.set_axis_limits("pwm_axis", -5, 105)

        dpg.setup_dearpygui()
        dpg.show_viewport()
        # Configurar el tamaño inicial de la ventana
        self._resize_window_callback(None, None)

    def run(self):
        """
        Corre la interfaz en loop.
        """
        while dpg.is_dearpygui_running():

            # Actualizo con los puntos que tengo
            self._update_plot()
            self._refresh_ports()
            dpg.render_dearpygui_frame()

            # Verifico los datos del puerto serie
            if not self._port:
                continue

            try:
                # Si no hay nada en el puerto, continuo
                if not self._port.in_waiting:
                    continue

                # Leo y decodifico el JSON
                data = self._port.readline().decode().strip()
                data = json.loads(data)
                # Veo si hay datos para actualizar
                self._position_data.append(data["position"])
                self._reference_data.append(data["reference"])
                self._error_data.append(data["error"])
                self._pwm_data.append(data["pwm"])
                self._time.append(self._time[-1] + self._tp)
                self._kp = data["kp"]
                self._ki = data["ki"]
                self._kd = data["kd"]
                self._tp = data["tp"]
                self._ts = data["ts"]
                # Veo si me excedi de las muestras
                if len(self._time) > self._max_points:
                    # Elimino el primer punto
                    self._position_data = self._position_data[1:]
                    self._reference_data = self._reference_data[1:]
                    self._error_data = self._error_data[1:]
                    self._pwm_data = self._pwm_data[1:]
                    self._time = self._time[1:]
            
            except:
                pass

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
        # Actualizo valores de constantes de PID
        dpg.set_value("Kp", f"Kp: {self._kp:.2f}")
        dpg.set_value("Ki", f"Ki: {self._ki:.2f}")
        dpg.set_value("Kd", f"Kd: {self._kd:.2f}")
        dpg.set_value("Ts", f"Ts [ms]: {(self._ts * 1000):.2f}")
        # Ajusto los limites horizontales
        dpg.set_axis_limits("position_time_axis", min(self._time), max(self._time))
        dpg.set_axis_limits("pwm_time_axis", min(self._time), max(self._time))

    def _resize_window_callback(self, sender, app_data):
        """
        Callback para ajustar el tamaño de la ventana 
        """
        # Obtengo las dimensiones del viewport y las configuro para la ventana
        width, height = dpg.get_viewport_client_width() - 22.5, dpg.get_viewport_client_height() - 50
        dpg.set_item_width("serial_window", width)
        dpg.set_item_height("serial_window", height // 10)
        dpg.set_item_width("position_window", width)
        dpg.set_item_height("position_window", 3 * height // 5)
        dpg.set_item_width("pwm_window", width)
        dpg.set_item_height("pwm_window", 3 * height // 5)
        dpg.set_item_width("pid_tuning_window", width)
        dpg.set_item_height("pid_tuning_window", height // 8)

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

    def _send_pid_callback(self, sender, app_data):
        """
        Callback cuando cambia algún slider de Kp, Ki o Kd.
        """
        kp = dpg.get_value("slider_kp")
        ki = dpg.get_value("slider_ki")
        kd = dpg.get_value("slider_kd")

        # Enviá por serial los nuevos valores si el puerto está activo
        if self._port and self._port.is_open:
            try:
                payload = {
                    "kp": kp,
                    "ki": ki,
                    "kd": kd
                }
                self._port.write((json.dumps(payload) + "\n").encode())
            except Exception as e:
                dpg.set_value(item="serial_status", value=f"Error al enviar valores PID: {e}")
