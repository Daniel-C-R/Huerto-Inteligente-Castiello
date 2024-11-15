# Imágen base ligera de Python
FROM python:3.9-slim

# Instalar las librerías necesarias
RUN pip install --no-cache-dir paho-mqtt

# Copiar el código del consumidor
WORKDIR /app
COPY consumer.py ./
COPY database.py ./
COPY init_db.sql ./

# Ejecutar el consumidor
CMD ["python", "-u", "consumer.py"]
