🇬🇧 This README is also available in [English](#english_readme) below.

# 🇪🇸 Huerto Inteligente

Este proyecto consiste en un huerto inteligente desarrollado para el CPEE Castiello Bernueces en la asignatura Redes de Sensores: Aplicaciones Médicas e Internet de las Cosas del Grado en Ciencia e Ingeniería de datos.

El funcionamiento es el siguiente. Unos sensores BME280 (en algunos casos BMP280) y unos higrómetros Capacitive Soil Moisture Sensore V1.2, conectados a microcontroladores ESP32S3, toman datos de unas plantas (temperatura, altitud, presión, humedad ambiente y del suelo). Los microcontroladores iluminan su LED dependiendo de si es necesario regar o no a partir de los datos del higrómetro capacitivo y, además, envían los datos a un servidor por Wi-Fi con el protocolo MQTT.

El servidor recibe los mensajes y los guarda en una base de datos para su posterior consulta. Además, sirve una página web que permite visualizar el histórico de mediciones.

A continuación se muestran los integrantes y colaboradores del proyecto:

|Papel|Nombre y apellidos|GitHub|LinkedIn| 
|-----|------------------|------|--------|
Estudiante|Alberto Micó Torres|https://github.com/UO288244 https://github.com/Tails5000|https://www.linkedin.com/in/alberto-mic%C3%B3-torres-a67312240/|
Estudiante|Nicolás Menéndez Llaca|https://github.com/nicomnndzz|https://www.linkedin.com/in/nicol%C3%A1s-men%C3%A9ndez-llaca-376909349/|
Estudiante|Lucas Alonso Miravalles|https://github.com/lucasalonsomiravalles|https://www.linkedin.com/in/lucas-alonso-miravalles-590492251/|
Estudiante|Nera Solís Castrillón|https://github.com/Nereasolis|https://www.linkedin.com/in/nereasoliscastrillon/|
Estudiante|Daniel Castaño Rodríguez|https://github.com/Daniel-C-R|https://www.linkedin.com/in/danielcr1/|
Profesor|Germán León Fernández|https://github.com/gleonfer|https://www.linkedin.com/in/german-leon-fernandez-595a5b156/|

También hay que agradecer a [Luis Ángel Oliveira](https://www.linkedin.com/in/laoliveirar/?lipi=urn%3Ali%3Apage%3Ad_flagship3_people_connections%3BYm0JTO9BTm26bpNd13vG6w%3D%3D) por habernos echado un cable a la hora de hacer funcionar las redes para conectar los sensores y el servidor.

## Despliegue

A continuación se incluyen instrucciones para el despliegue de la aplicación. Por un lado se darán las instrucciones para configurar los microcontroladores y por otro para desplegar el servidor.

### Configuración de los microcontroladores

La plantilla del código de los microcontroladores se guarda en `esp32/program.ino`. Se recomienda abrirla con Arduino IDE configurado para el desarrollo de placas ESP32. También, es necesario tener instaladas las extensiones de `ArduinoJSON`, `Adafruit_Sensor` y `Adafruit_BME280`.

Al comienzo del programa hay una serie de parámetros a configurar:

* **ID del microcontrolador:** Como vamos a tener varios ESP32 funcionando a la vez, es necesario distinguir cada uno de ellos asignándoles un ID. Es importante que **cada microcontrolador tenga un ID distinto**.
* **Parámetros del higrómetro capacitivo:** Hay que especificar de qué pin se van a leer los datos de este sensor y la información de calibración. Para calibrar el sensor, hay que indicar qué valor se consideran 0% de humedad (suelo seco), y qué valor se considera 100% de humedad (suelo recién regado).
* **Nombre y contraseña de la red wifi:** Es importante que los microcontroladores estén **conectados a la misma red que el servidor**.
* **Dirección y puerto de borker de MQTT:** Aquí se especifica la dirección IP del servidor y el puerto por el que escucha el broker de MQTT.
* **Parámetros del BME280** Se pueden configurar los pines por los que se van a leer los datos de los sensores BME280.

**Importante:** Algunos de los sensores eran BMP280 en vez de BME280. Para ellos se ha creado otro programa específico (`esp32/program_bmp280`), donde se utiliza la librería `Adafruit_BMP280` en su lugar y donde se ha eliminado la funcionalidad para leer la humedad ambiente ya que estos sensores no tienen esta capacidad.

### Despliegue del servidor

El servidor ha sido desarrollado como un proyecto de Docker Compose. Por tanto, para lanzar los distintos servicios que componen la aplicación simplemente habría que ejecutar `docker compose up` en la carpeta del repositorio. La primera vez que se lance se creará un archivo de SQLite en `~/huertodb/data.db` con las tablas donde se guardarán los datos.

<a name="english_readme"></a>
# 🇬🇧 Smart Garden

This project consists of a smart garden developed for CPEE Castiello Bernueces in the subject Sensor Networks: Medical Applications and Internet of Things of the Degree in Data Science and Engineering.

The operation is as follows. Some BME280 sensors (in some cases BMP280) and some Capacitive Soil Moisture Sensor V1.2 hygrometers, connected to ESP32S3 microcontrollers, collect data from some plants (temperature, altitude, pressure, ambient and soil humidity). The microcontrollers light up their LED depending on whether it is necessary to water or not based on the data from the capacitive hygrometer and, in addition, send the data to a server via Wi-Fi using the MQTT protocol.

The server receives the messages and stores them in a database for later consultation. It also serves a web page that allows you to view the history of measurements.

## Deployment

Below are instructions for deploying the application. On the one hand, instructions will be given to configure the microcontrollers and on the other to deploy the server.

### Microcontroller Configuration

The microcontroller code template is stored in `esp32/program.ino`. It is recommended to open it with Arduino IDE configured for ESP32 boards development. Also, it is necessary to have the `ArduinoJSON`, `Adafruit_Sensor` and `Adafruit_BME280` extensions installed.

At the beginning of the program, there are a series of parameters to configure:

* **Microcontroller ID:** Since we will have several ESP32s running at the same time, it is necessary to distinguish each of them by assigning them an ID. It is important that **each microcontroller has a different ID**.
* **Capacitive hygrometer parameters:** You need to specify which pin the data from this sensor will be read from and the calibration information. To calibrate the sensor, you need to indicate what value is considered 0% humidity (dry soil), and what value is considered 100% humidity (freshly watered soil).
* **Wi-Fi network name and password:** It is important that the microcontrollers are **connected to the same network as the server**.
* **MQTT broker address and port:** Here you specify the server's IP address and the port on which the MQTT broker listens.
* **BME280 parameters:** You can configure the pins from which the BME280 sensor data will be read.

**Important:** Some of the sensors were BMP280 instead of BME280. For them, another specific program (`esp32/program_bmp280`) has been created, where the `Adafruit_BMP280` library is used instead and where the functionality to read ambient humidity has been removed since these sensors do not have this capability.

### Server Deployment

The server has been developed as a Docker Compose project. Therefore, to launch the different services that make up the application, you simply need to run `docker compose up` in the repository folder. The first time it is launched, an SQLite file will be created in `~/huertodb/data.db` with the tables where the data will be stored.