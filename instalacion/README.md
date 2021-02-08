# Funginet
Sistema gratis de control y monitoreo para el cultivo de hongos.<br>
# Requisitos
# Software<br>
<br>
<a href="https://www.arduino.cc/en/software">Arduino IDE</a><br>
<a href="https://docs.m5stack.com/#/en/quick_start/m5core/m5stack_core_get_started_Arduino_Windows?id=_4-install-m5stack-library">M5Stack Arduino Guia de instalacion</a><br>

# Instalacion de placa M5Stack<br>
<br>
Una vez instalado el IDE de Arduino abrir el programa.<br>
1- Abrir la pestaña "File->Peferences->Settings".<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/Arduino_1.webp" alt="Instalacion de placa M5Stack" width="800" height="500"><br>

2- Copiar el siguiente enlace en "Aditional Board Manager URL"<br>
https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/package_m5stack_index.json<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/arduino_board_config.webp" alt="Instalacion de placa M5Stack" width="800" height="500"><br>

3-Ir a seleccionar la placa desde el Board Manager "Tools->Board:->Boards Manager..."<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/board_manage.webp" alt="Instalacion de placa M5Stack" width="800" height="500"><br>

4- Escribir en el buscador "m5stack" debe aparecer "Istalled" sino revisar desde el paso 1<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/search_M5STACK.webp" alt="Instalacion de placa M5Stack" width="800" height="500"><br>

5- Seleccionar la placa "Tools->Board:->M5Stack-Core2"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/board_select.webp" alt="Instalacion de placa M5Stack" width="800" height="500"><br>

# Instalacion de Librerias M5Stack<br>
<br>
1- Seleccionar "Sketch->Include Library:->Manage Libraries"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/manage_libraries.webp" alt="Instalacion de Libreria M5Stack" width="800" height="500"><br>

2- Buscar "M5Core2" by M5Stack en la ventana de librerias, una vez encontrada presionar "Install"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/install_library.webp" alt="Instalacion de Libreria M5Stack" width="800" height="500"><br>

Listo con esto podemos utilizar el M5Stack para cualquier proyecto. Ahora vamos con las librerias particulares de este proyecto.<br><br>
# Instalacion de Librerias desde este repositorio
1- Descargar las librerias del repositorio y copiarlas en el directorio libraries de la carpeta Arduino o en el directorio libraries de documentos/Arduino<br>

# Instalacion de Librerias Adafruit sgp30 Sensor<br>

1- Seleccionar "Sketch->Include Library:->Manage Libraries"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/manage_libraries.webp" alt="Instalacion de Libreria Adafruit sgp30 Sensor" width="800" height="500"><br>

2- Buscar "Adafruit sgp30 Sensor" en la ventana de librerias, una vez encontrada presionar "Install" Aceptal ALL si lo pide<br>

# Instalacion de Librerias Adafruit BMP280<br>

1- Seleccionar "Sketch->Include Library:->Manage Libraries"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/manage_libraries.webp" alt="Instalacion de Libreria Adafruit BMP280" width="800" height="500"><br>

2- Buscar "Adafruit BMP280" en la ventana de librerias, una vez encontrada presionar "Install" Aceptal ALL si lo pide<br>



