import dearpygui.dearpygui as dpg


class PIDPlotter:
    def __init__(self, width, height):
        self._width = width
        self._height = height

        # Parametros para graficar
        self._reference_data = [0.0]
        self._position_data = [0.0]
        self._pwm_data = [0.0]
        self._error_data = [0.0]

        # Configurar DearPyGUI
        dpg.create_context()
        dpg.create_viewport(title="PID Plotter", width=width, height=height)

        # Configuro una ventana para el ploteo del PID
        with dpg.window(label="App", tag="app_window"):
            # Vista para el grafico del PID
            with dpg.child_window(tag="position_window"):
                # Ventana de plot
                with dpg.plot(label="Posicion y Referencia del Motor", height=-1, width=-1):
                    dpg.add_plot_legend()
                    dpg.add_plot_axis(dpg.mvXAxis, label="Tiempo")
                    with dpg.plot_axis(dpg.mvYAxis, label="Grados"):
                        dpg.add_line_series([], [], label="Referencia", tag="reference_plot")
                        dpg.add_line_series([], [], label="Posición", tag="position_plot")

            # Vista para el PWM y el Error
            with dpg.child_window(tag="pwm_window"):
                with dpg.plot(label="PWM y Error", height=-1, width=-1):
                    dpg.add_plot_legend()
                    dpg.add_plot_axis(dpg.mvXAxis, label="Tiempo")
                    with dpg.plot_axis(dpg.mvYAxis, label="PWM/Error"):
                        dpg.add_line_series([], [], label="PWM", tag="pwm_plot")
                        dpg.add_line_series([], [], label="Error", tag="error_plot")

        dpg.setup_dearpygui()
        dpg.show_viewport()
        # Configurar el tamaño inicial de la ventana
        self._resize_window_callback(None, None)

    def run(self):
        while dpg.is_dearpygui_running():
            self._update_plot()
            dpg.render_dearpygui_frame()

        dpg.destroy_context()

    def _update_plot(self):
        dpg.set_value("reference_plot", [[0.0], [0.0]])
        dpg.set_value("position_plot", [[0.0], [0.0]])
        dpg.set_value("pwm_plot", [[0.0], [0.0]])
        dpg.set_value("error_plot", [[0.0], [0.0]])

    def _resize_window_callback(self, sender, app_data):
        """
        Callback para ajustar el tamaño de la ventana 
        """
        # Obtengo las dimensiones del viewport y las configuro para la ventana
        width, height = dpg.get_viewport_client_width() - 22.5, dpg.get_viewport_client_height() - 50
        # dpg.set_item_width("serial_window", width)
        # dpg.set_item_height("serial_window", height // 7)
        dpg.set_item_width("position_window", width)
        dpg.set_item_height("position_window", 3 * height // 7)
        dpg.set_item_width("pwm_window", width)
        dpg.set_item_height("pwm_window", 3 * height // 7)
