# Usa la imagen base de Python
FROM python:3.9-slim

# Instala dependencias necesarias
RUN pip install --no-cache-dir streamlit pandas plotly

# Copia la aplicación al contenedor
WORKDIR /app
COPY app.py /app/app.py

# Exponer el puerto 8501 para Streamlit
EXPOSE 8501

# Comando para ejecutar la aplicación
CMD ["streamlit", "run", "app.py", "--server.port=8501", "--server.enableCORS=false"]