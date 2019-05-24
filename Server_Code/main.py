# Imports
from datetime import datetime
from flask import Flask, render_template, request, Response
from flask_cors import CORS
import pymssql
import json
from pathlib import Path

app = Flask(__name__, template_folder='templates')

# Add CORS (required for use in browser)
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'

config_json = json.loads(Path('config.json').read_text())


# Connecting to database
def db_connect():
    global config_json
    # Connect to Microsoft SQL server
    conn = pymssql.connect(
        server=config_json['server'],
        user=config_json['user'],
        password=config_json['password'],
        database=config_json['database']
    )
    return conn


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
    conn = db_connect()
    cursor = conn.cursor()
    # Bad request
    if sid is None:
        return Response(status=400)

    # Correct request
    cursor.execute('select COUNT(1) from StudentInfo where SID=' + str(sid) + ';')
    result = cursor.fetchone()
    conn.close()
    return str(result[0]) == '1'


@app.route('/tables/StudentInfo/', methods=['GET', 'HEAD', 'POST', 'DELETE'])
def tables_student():
    if request.method == 'GET':
        conn = db_connect()
        cursor = conn.cursor()
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

        conn.close()
        resp = Response(status=200)
        resp.data = json.dumps(payload)
        return resp
    # end if GET
    elif request.method == 'HEAD':
        sid = request.args.get('sid')
        if sid is None:
            return Response(status=400)
        else:
            if student_exists(sid):
                return Response(status=200)
            else:
                return Response(status=404)
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

        conn = db_connect()
        cursor = conn.cursor()
        cursor.execute(sql_req)
        conn.commit()
        conn.close()
        print('Finished sql req!')
        return Response(status=201)
    elif request.method == 'DELETE':
        sid = request.args.get('sid')
        if sid is not None:
            conn = db_connect()
            cursor = conn.cursor()
            cursor.execute('delete from StudentInfo where SID=' + str(sid) + ';')
            conn.commit()
            conn.close()
            return Response(status=200)
        else:
            return Response(status=400)


@app.route('/tables/', methods=['GET'])
def tables():
    if request.method == 'GET':
        conn = db_connect()
        cursor = conn.cursor()
        # Get all students from database
        cursor.execute(
            'select S.SID, P.Date, C.sName, P.Room from Presence as P join Courses as C on P.CID=C.CID join StudentInfo as S on S.SID=P.SID;'
        )

        # Convert table into json
        payload = json.loads('{"elements":[]}')
        row = cursor.fetchone()
        while row:
            # Creating json table with data
            payload['elements'].append(
                # Creating json table with data
                json.loads(
                    '{"SID":' + str(row[0]) + ',' +
                    '"Date": "' + str(row[1]) + '",' +
                    '"sName": "' + row[2] + '",' +
                    '"Room": "' + row[3] + '"}'
                )
            )
            row = cursor.fetchone()

        conn.close()
        resp = Response(status=200)
        resp.data = json.dumps(payload)
        return resp
    # end if GET


@app.route('/tables/Presence/', methods=['GET', 'POST'])
def tables_presence():
    if request.method == 'GET':
        conn = db_connect()
        cursor = conn.cursor()

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
        conn.close()

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

        conn = db_connect()
        cursor = conn.cursor()
        cursor.execute(sql_req)
        conn.commit()
        conn.close()
        print('Finished sql req!')
        return Response(status=201)
    # end elif POST


if __name__ == '__main__':
    app.run(
        host='0.0.0.0',
        port=80,
        threaded=True,
        debug=False
    )
# end main
