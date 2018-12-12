from flask import Flask, request, render_template, jsonify, send_from_directory, abort
import subprocess as sb
from uuid import uuid1
from os import path


_base_path = path.dirname(path.realpath(__file__))
app = Flask(__name__, static_url_path="/file")


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/synth', methods=["POST"])
def synth():
    try:
        clock = request.form["clock"]
        coefficients = request.form["coefficient"]
    except:
        return jsonify({
            "code": 500,
            "messgae": "error to get parameters"
        })
    uuid_str = str(uuid1().hex)
    coefficients_file_path = '/tmp/ee252/coefficients/' + uuid_str + ".txt"
    with open(coefficients_file_path, "w") as file:
        file.write(coefficients)
    uuid_str2 = str(uuid1().hex)
    sb.run(["bash", path.join(_base_path, "flask_synth.sh"), "coefficients_file_path", str(clock),
            uuid_str2], shell=True, timeout=120)
    return jsonify({
        "code": 200,
        "messgae": uuid_str2
    })


@app.route('/qor/<string:uuid_str>')
def qor(uuid_str):
    qor_logs = ""
    try:
        with open(path.join(_base_path, "qor", uuid_str + ".txt"), 'r') as file:
            qor_logs = file.read()
    except:
        return jsonify({
            "code": 404,
            "messgae": "results not found"
        })
    else:
        return jsonify({
            "code": 200,
            "message": qor_logs
        })


@app.route('/zip/<string:uuid_str>')
def get_file(uuid_str):
    zip_file_path = path.join(_base_path, "zip", "synth"+uuid_str+".zip")
    print(zip_file_path)
    if path.isfile(zip_file_path):
        print("heihei")
        return send_from_directory('zip', "synth"+uuid_str+".zip")
    else:
        return abort(404)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port="11998")
