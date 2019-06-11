# Imports
from flask import Flask, render_template, request, Response
from flask_cors import CORS
import json
from pathlib import Path
import database

app = Flask(__name__, template_folder='templates')

# Add CORS (required for use in some browsers)
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'

config_json = json.loads(Path('config.json').read_text())


@app.route('/')
def empty():
    return 'OK', 200


@app.route('/SmartAttendance')
def site():
    return render_template('Index.html', name='Main Page')


@app.route('/test')
def test():
    return 'Test ' + 'SA_OK'


@app.route('/tables/StudentInfo/', methods=['GET', 'HEAD', 'POST', 'DELETE'])
def tables_student():
    if request.method == 'GET':
        payload = database.student_info_get()
        resp = '', 200
        resp.data = json.dumps(payload)
        return resp
    elif request.method == 'HEAD':
        sid = request.args.get('sid')
        if sid is None:
            return '', 400
        elif database.student_exists(sid):
            return '', 200
        else:
            return '', 404
    elif request.method == 'POST':
        # Parse json payload
        payload = json.loads(request.data)
        print(request.data)
        # Can't add students with existing index
        if database.student_exists(payload['SID']):
            return '', 409

        database.student_info_insert(payload)

        return '', 201
    elif request.method == 'DELETE':
        sid = request.args.get('sid')
        if sid is not None:
            database.student_info_delete(sid)
            return '', 200
        else:
            return '', 400


@app.route('/tables/Presence/', methods=['GET', 'POST'])
def tables_presence():
    print(request.data)

    if request.method == 'GET':
        payload: dict = database.presence_get()

        resp = '', 200
        resp.data = json.dumps(payload)
        return resp
    elif request.method == 'POST':
        # Parse json payload
        payload = json.loads(request.data)

        # Index must exist
        if not database.student_exists(payload['SID']):
            return '', 404

        database.presence_insert(payload)
        return 'SA_OK', 201


def main():
    try:
        database.connect()
    except Exception:
        return

    app.run(
        host='0.0.0.0',
        port=80,
        threaded=True,
        debug=False
    )


if __name__ == '__main__':
    main()
