# Imports
from flask import Flask
from flask import request
from flask import Response
from flask_cors import CORS, cross_origin
from flask import Flask, render_template
import pymssql
import json
from pathlib import Path

app = Flask(__name__, template_folder='templates')
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'

conn = None
cursor = None


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
    cursor.execute('select COUNT(1) from StudentInfo where StudentID=' + str(sid) + ';')
    result = cursor.fetchone()

    if str(result[0]) == '1':
        return Response(status=409)
    else:
        return Response(status=200)


@app.route('/tables/StudentInfo/<int:sid>', methods=['GET', 'DELETE'])
def tables_student_exists_delete(sid):
    if request.method == 'GET':
        return student_exists(sid)
    # end if GET
    if request.method == 'DELETE':
        cursor.execute('delete from StudentInfo where StudentID=' + str(sid) + ';')
        conn.commit()
        return Response(status=200)
    # end if DELETE


@app.route('/tables/StudentInfo/', methods=['GET', 'POST'])
def tables_student():
    if request.method == 'GET':
        cursor.execute('select * from StudentInfo order by StudentID asc;')

        resp = Response(status=200)

        # Convert table into json
        payload = json.loads('{"elements":[]}')
        row = cursor.fetchone()
        while row:
            payload['elements'].append(
                json.loads(
                    '{' +
                    '"SID":' + str(row[0]) + ',' +
                    '"fName": "' + row[1] + '",' +
                    '"lName": "' + row[2] + '"' +
                    '}'
                )
            )
            row = cursor.fetchone()
        # While end
        resp.data = json.dumps(payload)
        return resp
    # end if GET
    elif request.method == 'POST':
        payload = json.loads(request.data)
        if student_exists(str(payload['SID'])).status_code != 200:
            return Response(status=409)
        sqlReq = 'insert into StudentInfo values (' + str(payload['SID']) + ',\'' + payload['fName'] + '\',\'' + \
                 payload['lName'] + '\');'
        print(sqlReq)
        cursor.execute(sqlReq)
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
