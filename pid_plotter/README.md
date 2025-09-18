# Plotter App

## Cómo usar

1. Tener Python instalado con el paquete virtualenv que se puede instalar desde la consola con:

```bash
py -m pip install virtualenv # En Windows
```

```
python3 -m pip install virtualenv # En Linux/macOS
```

2. Con el repositorio clonado, abrir la terminal en este directorio y crear un entorno virtual

```bash
py -m venv .venv # En Windows
```

```bash
python3 -m venv .venv # En Linux/macOS
```

3. Activar el entorno virtual

```bash
.venv\Scripts\activate # En Windows
```

```bash
source .venv/bin/activate
```

4. Instalar los paquetes necesarios

```bash
py -m pip install -r requirements.txt # En Windows
```

```bash
python -m pip install -r requirements.txt # En Linux/macOS
```

5. Correr la aplicación

```bash
py app.py # En Windows
```

```bash
python  app.py # En Linux/macOS
```

6. Para desactivar el entorno virtual:

```bash
deactivate
```
