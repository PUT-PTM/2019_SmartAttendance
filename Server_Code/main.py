# Imports
from datetime import datetime
from flask import Flask
from flask import request
from flask import Response
from flask_cors import CORS, cross_origin
from flask import Flask, render_template
import pymssql
import json
from pathlib import Path

app = Flask(__name__, template_folder='templates')

# Add CORS (required for use in browser)
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'

conn = None
cursor = None


# Connecting to database
def db_init():
    global conn
    global cursor
    content = Path('config.json').read_text()
    config_json = json.loads(content)
    # Connect to Microsoft SQL server
    conn = pymssql.connect(
        server=config_json['server'],
        user=config_json['user'],
        password=config_json['password'],
        database=config_json['database']
    )
    cursor = conn.cursor()


@app.route('/')
def empty():
    return 'OK'


@app.route('/SmartAttendance')
def site():
    return render_template('Index.html', name='Main Page')


@app.route('/test')
def test():
    return 'This is a long message response. Deal with it! Muahahahahahaha!!! xd wtf da fuq!'


def student_exists(sid):
    # Bad request
    if sid is None:
        return Response(status=400)

    # Correct request
    cursor.execute('select COUNT(1) from StudentInfo where SID=' + str(sid) + ';')
    result = cursor.fetchone()
    return str(result[0]) == '1'


@app.route('/tables/StudentInfo/<int:sid>', methods=['GET', 'DELETE'])
def tables_student_exists_delete(sid):
    if request.method == 'GET':
        return student_exists(sid)
    # end if GET
    if request.method == 'DELETE':
        cursor.execute('delete from StudentInfo where SID=' + str(sid) + ';')
        conn.commit()
        return Response(status=200)
    # end if DELETE


@app.route('/tables/StudentInfo/', methods=['GET', 'POST'])
def tables_student():
    if request.method == 'GET':
        # Get all students from database
        cursor.execute('select * from StudentInfo order by SID asc;')

        # Convert table into json
        payload = json.loads('{"elements":[]}')
        row = cursor.fetchone()
        while row:
            # Creating json table with data
            payload['elements'].append(
                # Creating json table with data
                json.loads(
                    '{"SID":' + str(row[0]) + ',' +
                    '"fName": "' + row[1] + '",' +
                    '"lName": "' + row[2] + '"}'
                )
            )
            row = cursor.fetchone()
        # While end

        resp = Response(status=200)
        resp.data = json.dumps(payload)
        return resp
    # end if GET
    elif request.method == 'POST':
        # Parse json payload
        payload = json.loads(request.data)

        # Can't add students with existing index
        if student_exists(str(payload['SID'])):
            return Response(status=409)

        # Get values from JSON
        sid = str(payload['SID'])
        f_name = '\'' + payload['fName'] + '\''
        l_name = '\'' + payload['lName'] + '\''

        # Add entry to database
        sql_req = 'insert into StudentInfo values (' + sid + ',' + f_name + ',' + l_name + ');'
        cursor.execute(sql_req)
        conn.commit()
        print('Finished sql req!')
        return Response(status=200)
    # end elif POST


@app.route('/tables/Presence/', methods=['GET', 'POST'])
def tables_presence():
    if request.method == 'GET':
        cursor.execute('select * from Presence order by [Date] asc;')

        # Convert table into json
        payload = json.loads('{"elements":[]}')
        row = cursor.fetchone()
        while row:
            # Creating json table with data
            payload['elements'].append(
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

        resp = Response(status=200)
        resp.data = json.dumps(payload)
        return resp
    # end if GET
    elif request.method == 'POST':
        # Parse json payload
        payload = json.loads(request.data)

        # Index must exist
        if not student_exists(payload['SID']):
            return Response(status=404)

        # Get values from JSONs
        sid = str(payload.get('SID'))
        date = '\'' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\''
        cid = payload.get('CID')
        room = '\'' + payload.get('Room') + '\''

        # Request building
        sql_req = 'insert into Presence values (' + sid + ',' + date
        if cid is not None:
            sql_req += ',' + str(cid)
        if len(room) > 2:
            sql_req += ',' + room
        sql_req += ');'

        cursor.execute(sql_req)
        conn.commit()
        print('Finished sql req!')
        return Response(status=200)
    # end elif POST


if __name__ == '__main__':
    db_init()
    app.run(
        host='0.0.0.0',
        port=80,
        threaded=True,
        debug=False
    )
# end main
