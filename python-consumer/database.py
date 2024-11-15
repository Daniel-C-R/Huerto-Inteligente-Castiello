import sqlite3


database_path = '/db/data.db'
schema_path = 'init_db.sql'


def create_tables_if_not_exists():
    '''
    Crear las tablas en caso de que no existan
    '''

    try:
        with open(schema_path, 'r') as f:
            schema_script = f.read()

        with sqlite3.connect(database_path) as conn:
            cursor = conn.cursor()

            cursor.executescript(schema_script)

            conn.commit()
            cursor.close()

            print("Las tablas están listas en la base de datos.")
    except sqlite3.Error as e:
        print(f'Error al crear las tablas en caso de que no existan: {e}')


create_tables_if_not_exists()


def insert_row(table, *data):
    '''
    Insertar una fila en una tabla
    '''

    try:
        with sqlite3.connect(database_path) as conn:
            cursor = conn.cursor()

            query = f'''
            INSERT INTO {table} (controller_id, timestamp, value) VALUES (?, ?, ?)
            '''
            cursor.execute(query, data)

            conn.commit()
            cursor.close()

            print(f'Introducida en la tabla {table}: {data}')
    except sqlite3.Error as e:
        print(f'Error al insertar datos: {e}')
