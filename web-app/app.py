import sqlite3
import pandas as pd
import streamlit as st
import plotly.graph_objects as go

# Ruta de la base de datos
database_path = '/db/data.db'

# Función para consultar los datos
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

# Título de la aplicación
st.title("Huerto inteligente")

# Consultar datos de cada sensor
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

# Combinar todos los datos en un solo dataframe
df = pd.concat([df_humidity, df_soil_moisture, df_temperature,
               df_altitude, df_pressure], axis=0)

# Selección del microcontrolador y el tipo de sensor
controller_id = st.selectbox(
    'Selecciona Microcontrolador:', options=df['controller_id'].unique())
sensor = st.selectbox('Selecciona Tipo de Sensor:',
                      options=df['sensor'].unique())

# Filtrar datos según selección
filtered_df = df[(df['controller_id'] == controller_id)
                 & (df['sensor'] == sensor)]

# Configurar rango de fechas para mostrar
min_fecha = filtered_df['timestamp'].min()
max_fecha = filtered_df['timestamp'].max()
inicio_rango = max_fecha - pd.Timedelta(days=1)

# Crear gráfico
fig = go.Figure()

if sensor == 'Humedad del suelo':
    # Obtener temperatura para las comparaciones
    temp_df = df[(df['controller_id'] == controller_id) & (df['sensor'] == 'Temperatura')]
    temp_df = temp_df.set_index('timestamp').reindex(filtered_df['timestamp'], method='nearest').reset_index()
    filtered_df['temp'] = temp_df['value']

    # Determinar colores
    filtered_df['color'] = 'green'
    filtered_df.loc[filtered_df['value'] < 70, 'color'] = 'red'
    filtered_df.loc[
        (filtered_df['value'] < 85) & (filtered_df['temp'] > 25),
        'color'
    ] = 'red'

    # Crear segmentos para mantener línea continua con cambio de color
    prev_color = filtered_df['color'].iloc[0]
    segment_start = 0

    for i in range(1, len(filtered_df)):
        current_color = filtered_df['color'].iloc[i]
        if current_color != prev_color:
            # Dibujar segmento de línea con color actual
            fig.add_trace(go.Scatter(
                x=filtered_df['timestamp'].iloc[segment_start:i],
                y=filtered_df['value'].iloc[segment_start:i],
                mode='lines',
                line=dict(color=prev_color),
                showlegend=(segment_start == 0),  # Añadir leyenda solo para el primer segmento
                name='Húmedo' if prev_color == 'green' else 'Seco'
            ))
            segment_start = i
            prev_color = current_color

    # Agregar el último segmento
    fig.add_trace(go.Scatter(
        x=filtered_df['timestamp'].iloc[segment_start:],
        y=filtered_df['value'].iloc[segment_start:],
        mode='lines',
        line=dict(color=prev_color),
        showlegend=(segment_start == 0),
        name='Húmedo' if prev_color == 'green' else 'Seco'
    ))

    # Rango del eje Y
    fig.update_yaxes(range=[0, 100])

else:
    # Gráfico estándar para otros sensores
    fig.add_trace(go.Scatter(
        x=filtered_df['timestamp'],
        y=filtered_df['value'],
        mode='lines',
        line=dict(color='blue'),
        name=sensor
    ))

# Configurar el eje X
fig.update_xaxes(
    range=[inicio_rango, max_fecha],
    rangeslider_visible=True
)

# Título del gráfico
fig.update_layout(
    title=f"Serie Temporal - Micro {controller_id}, Sensor {sensor}",
    xaxis_title='Fecha',
    yaxis_title='Valor'
)

# Mostrar gráfico
st.plotly_chart(fig)