# Imágen base ligera de Python
FROM python:3.9-slim

# Instalar las librerías necesarias
RUN pip install paho-mqtt

# Copiar el código del consumidor
WORKDIR /app
COPY consumer.py ./

# Ejecutar el consumidor
CMD ["python", "-u", "consumer.py"]