from datetime import datetime
import json
from pathlib import Path

import pymssql

config_json: dict = json.loads(Path('config.json').read_text())


# Connecting to database
def connect():
    global config_json
    # Connect to Microsoft SQL server
    conn = pymssql.connect(
        server=config_json['server'],
        user=config_json['user'],
        password=config_json['password'],
        database=config_json['database']
    )
    return conn


def student_exists(sid: int) -> bool:
    conn = connect()
    cursor = conn.cursor()

    # Correct request
    cursor.execute('select COUNT(1) from StudentInfo where SID=' + str(sid) + ';')
    result = cursor.fetchone()
    conn.close()
    return str(result[0]) == '1'


def student_info_get() -> dict:
    conn = connect()
    cursor = conn.cursor()
    # Get all students from database
    cursor.execute('select * from StudentInfo order by SID;')

    # Convert table into json
    data: dict = json.loads('{"elements":[]}')
    row = cursor.fetchone()
    while row:
        # Creating json table with data
        data['elements'].append(
            # Creating json table with data
            json.loads(
                '{"SID":' + str(row[0]) + ',' +
                '"fName": "' + row[1] + '",' +
                '"lName": "' + row[2] + '"}'
            )
        )
        row = cursor.fetchone()
    # While end

    conn.close()
    return data


def student_info_insert(info: dict) -> None:
    # Get values from JSON
    sid = str(info['SID'])
    f_name = '\'' + info['fName'] + '\''
    l_name = '\'' + info['lName'] + '\''

    # Add entry to database
    sql_req = 'insert into StudentInfo values (' + sid + ',' + f_name + ',' + l_name + ');'

    conn = connect()
    cursor = conn.cursor()
    cursor.execute(sql_req)
    conn.commit()
    conn.close()
    print('Finished sql req!')


def student_info_delete(sid: int) -> None:
    conn = connect()
    cursor = conn.cursor()
    cursor.execute('delete from StudentInfo where SID=' + str(sid) + ';')
    conn.commit()
    conn.close()


def presence_get() -> dict:
    conn = connect()
    cursor = conn.cursor()

    cursor.execute('select * from Presence order by Date;')

    # Convert table into json
    data = json.loads('{"elements":[]}')
    row = cursor.fetchone()
    while row:
        # Creating json table with data
        data['elements'].append(
            # Creating json table with data
            json.loads(
                '{"SID":' + str(row[0]) + ',' +
                '"Date": "' + row[1] + '",' +
                '"CID": "' + row[2] + '"' +
                '"Room": "' + row[3] + '"}'
            )
        )
        row = cursor.fetchone()
    # While end
    conn.close()
    return data


def presence_insert(info: dict):
    # Get values from JSONs
    sid = str(info.get('SID'))
    date = '\'' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\''
    cid = info.get('CID')
    room = '\'' + info.get('Room') + '\''

    # Request building
    sql_req = 'insert into Presence values (' + sid + ',' + date
    if cid is not None:
        sql_req += ',' + str(cid)
    if len(room) > 2:
        sql_req += ',' + room
    sql_req += ');'

    conn = connect()
    cursor = conn.cursor()
    cursor.execute(sql_req)
    conn.commit()
    conn.close()
    print('Finished sql req!')
