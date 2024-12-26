import sqlite3
import pandas as pd
import streamlit as st
import plotly.express as px


database_path = '/db/data.db'


def query_data(table):
    try:
        with sqlite3.connect(database_path) as conn:
            df = pd.read_sql_query(
                f'SELECT * FROM {table}',
                conn
            )
            df['timestamp'] = pd.to_datetime(df['timestamp'])
            return df
    except Exception as e:
        return pd.DataFrame()


st.title("Huerto inteligente")

df_humidity = query_data('humidity')
df_humidity['sensor'] = 'Humedad ambiente'
df_soil_moisture = query_data('soil_moisture')
df_soil_moisture['sensor'] = 'Humedad del suelo'
df_temperature = query_data('temperature')
df_temperature['sensor'] = 'Temperatura'
df_altitude = query_data('altitude')
df_altitude['sensor'] = 'Altitud'
df_pressure = query_data('pressure')
df_pressure['sensor'] = 'Presión'

df = pd.concat([df_humidity, df_soil_moisture, df_temperature,
               df_altitude, df_pressure], axis=0)

controller_id = st.selectbox(
    'Selecciona Microcontrolador:', options=df['controller_id'].unique())
sensor = st.selectbox('Selecciona Tipo de Sensor:',
                      options=df['sensor'].unique())

filtered_df = df[(df['controller_id'] == controller_id)
                 & (df['sensor'] == sensor)]

min_fecha = filtered_df['timestamp'].min()
max_fecha = filtered_df['timestamp'].max()
inicio_rango = max_fecha - pd.Timedelta(days=1)

fig = px.line(
    filtered_df,
    x='timestamp',
    y='value',
    title=f"Serie Temporal - Micro {controller_id}, Sensor {sensor}",
    labels={'value': 'Valor', 'timestamp': 'Fecha'}
)

fig.update_xaxes(
    range=[inicio_rango, max_fecha],
    rangeslider_visible=True
)

st.plotly_chart(fig)
