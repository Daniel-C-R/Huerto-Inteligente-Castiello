import sqlite3
import pandas as pd
import streamlit as st
from matplotlib import pyplot as plt

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
        st.write(f'Error al obtener los datos de la base de datos: {e}')


st.write('Web app')

df = query_data('humidity')

df = df[df['controller_id'] == 1]

fig, ax = plt.subplots(figsize=(10, 5))

ax.plot(df['timestamp'], df['value'])
ax.set_title('Serie Temporal')
ax.set_xlabel('Timestamp')
ax.set_ylabel('Value')
st.pyplot(fig)
