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
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/Arduino_1.webp" alt="Instalacion de placa M5Stack" width="400" height="250"><br>

2- Copiar el siguiente enlace en "Aditional Board Manager URL"<br>
https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/package_m5stack_index.json<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/arduino_board_config.webp" alt="Instalacion de placa M5Stack" width="400" height="250"><br>

3-Ir a seleccionar la placa desde el Board Manager "Tools->Board:->Boards Manager..."<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/board_manage.webp" alt="Instalacion de placa M5Stack" width="400" height="250"><br>

4- Escribir en el buscador "m5stack" debe aparecer "Istalled" sino revisar desde el paso 1<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/search_M5STACK.webp" alt="Instalacion de placa M5Stack" width="400" height="250"><br>

5- Seleccionar la placa "Tools->Board:->M5Stack-Core2"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/board_select.webp" alt="Instalacion de placa M5Stack" width="400" height="250"><br>

# Instalacion de Librerias M5Stack<br>
<br>
1- Seleccionar "Sketch->Include Library:->Manage Libraries"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/manage_libraries.webp" alt="Instalacion de Libreria M5Stack" width="400" height="250"><br>

2- Buscar "M5Core2" by M5Stack en la ventana de librerias, una vez encontrada presionar "Install"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/install_library.webp" alt="Instalacion de Libreria M5Stack" width="400" height="250"><br>

Listo con esto podemos utilizar el M5Stack para cualquier proyecto. Ahora vamos con las librerias particulares de este proyecto.<br><br>
# Instalacion de Librerias desde este repositorio
1- Descargar las librerias del repositorio y copiarlas en el directorio libraries de la carpeta Arduino o en el directorio libraries de documentos/Arduino<br><br>
Estas son las librerias necesarias:<br>

1-Adafruit_BMP280_Library<br>
2-ENV_II_LIB<br>
3-PubSubClient<br>
4-TVOC_SGP30_LIB<br>
5-separador<br>
6-DHT_sensor_library<br>

Ahora conecte el M5Stack en algun puerto USB.<br>
Seleccione el puerto USB desde "Sketch->Compile".<br>
Si todo va bien deberia aparecer un mensaje como este.<br>

<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/ok.png" alt="Instalacion de Libreria M5Stack" width="1366" height="126"><br>
Presione Subir para cargar el programa al M5Stack y listo.<br>

Si lo prefiere puede instalar las librerias desde el administrador de librerias como se detalla a continuacion.<br>
Todas las librerias estan disponibles salvo "sapara" que es una libreria propiedad de Ioticos<br>
Esta debe copiarse de forma manual.<br>


# Instalacion de Librerias Adafruit sgp30 Sensor<br>

1- Seleccionar "Sketch->Include Library:->Manage Libraries"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/manage_libraries.webp" alt="Instalacion de Libreria Adafruit sgp30 Sensor" width="400" height="250"> <br>

2- Buscar "Adafruit sgp30 Sensor" en la ventana de librerias, una vez encontrada presionar "Install" Aceptal ALL si lo pide<br>

# Instalacion de Librerias Adafruit BMP280<br>

1- Seleccionar "Sketch->Include Library:->Manage Libraries"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/manage_libraries.webp" alt="Instalacion de Libreria Adafruit BMP280" width="400" height="250"><br>

2- Buscar "Adafruit BMP280" en la ventana de librerias, una vez encontrada presionar "Install" Aceptal ALL si lo pide<br>

# Instalacion de Librerias SHT3X<br>

1- Seleccionar "Sketch->Include Library:->Manage Libraries"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/manage_libraries.webp" alt="Instalacion de Libreria Adafruit BMP280" width="400" height="250"><br>

2- Buscar "Adafruit SHT3" en la ventana de librerias, una vez encontrada presionar "Install" Aceptal ALL si lo pide<br>

# Instalacion de Librerias PubSubclient<br>

1- Seleccionar "Sketch->Include Library:->Manage Libraries"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/manage_libraries.webp" alt="Instalacion de Libreria Adafruit BMP280" width="400" height="250"><br>

2- Buscar "PubSubclient" by Nick O'Leary en la ventana de librerias, una vez encontrada presionar "Install"<br>

# Instalacion de Librerias DHT<br>

1- Seleccionar "Sketch->Include Library:->Manage Libraries"<br>
<img src="https://github.com/christianfunginet/Funginet/blob/main/instalacion/manage_libraries.webp" alt="Instalacion de Libreria Adafruit BMP280" width="400" height="250"><br>

2- Buscar "DHT" by Adafruit en la ventana de librerias, una vez encontrada presionar "Install"<br>

# Listo trabajo terminado<br>

Ahora conecte el M5Stack en algun puerto USB.<br>
Seleccione el puerto USB desde "Tools->Port:->".<br>
Presione Subir para cargar el programa al M5Stack y listo.



