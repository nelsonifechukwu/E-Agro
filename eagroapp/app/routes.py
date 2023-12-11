from app import app 
from flask import session, make_response, redirect, render_template, request, flash, url_for, Response
from app.models import checklogin, delete_green_data, insert_green_data, get_green_data, get_green_state, update_green_state, insert_control_input, get_control_input
import json, random

API = 'xdol'

#login
@app.route('/', methods = ('GET', 'POST'))
@app.route('/login' , methods = ('GET', 'POST'))
def login():
    if (request.method=='POST'):
        username = request.form['username']
        password = request.form['password']

        succeed = checklogin(username, password)
        if (succeed):
            session['username'] = username
            #global m_user
            #m_user = username
            flash('Welcome! Login Successful')
            return redirect(url_for('plot'))
        else:
            flash('Error: Invalid Username or Password')
            return redirect(url_for('login'))
    else:
        return render_template('login.html')

#plot data
@app.route('/plot')
def plot():
    #create a session to secure this endpoint
    if "username" not in session:
        return redirect(url_for('login'))
    #data = get_transformer_data()
    return render_template('plot.html', name=session["username"])


#design the web_api
@app.route('/update/key=<api_key>/c1=<int:c1>/c2=<int:c2>/c3=<int:c3>/c4=<int:c4>/c5=<int:c5>', methods=['GET'])
def update(api_key, c1, c2, c3, c4, c5):
    if (api_key == API):
        insert_green_data(c1, c2, c3, c4, c5)
        return Response(status=200)
    else:
        return Response(status=500)

#fetch data to be plotted
@app.route('/data', methods=['GET', 'POST'])
def get_data():
    data = get_green_data()
    data = str(data)
    data = data.strip('[]()')
    return str(data)

#fetch data to be plotted
@app.route('/data/arr', methods=['GET', 'POST'])
def get_data_arr():
    data = get_green_data()
    data = str(data)
    data = data.strip('[]()')
    return str(data)

#get button state (for the hardware)
@app.route('/get_state', methods = ['GET'])
def get_state():
    green_state = get_green_state('all')
    green_state = str(green_state)
    green_state = green_state.strip('[]()')
    return str(green_state)

@app.route('/get_control_input', methods = ['GET'])
def get_control():
    data = get_control_input('all')
    data = str(data)
    data = data.strip('[]()')
    return str(data)

@app.route('/get_control_input/<id>', methods = ['GET'])
def get_input(id):
    control_input = get_control_input(id)
    control_input = str(control_input)
    control_input = control_input.strip('[]()')
    return str(control_input)


@app.route('/insert/<int:b1>/<int:b2>/<int:b3>', methods=['GET','POST'])
def insert_input(b1, b2, b3):
    insert_control_input(b1, b2, b3)
    return Response(status=200)    

#update button state
@app.route('/updatestate/button=<id>', methods = ['GET', 'POST'])
def update_state(id):
    #get the previous state
    state = get_green_state(id)
    state =  str(state)
    #Strip the string from the database and update the previous state
    state = state.strip('()[]').strip(',')
    newstate =  1-int(state)
    update_green_state(newstate, id)
    return str(newstate)

#delete all data
@app.route('/delete', methods = ('GET', 'POST'))
def delete():
    if request.method == 'POST':
        delete_green_data()
        insert_green_data(0,0,0,0,0)
        return redirect(url_for('plot'))

#get individual state
@app.route('/get_state/<id>', methods = ['GET'])
def get_id_state(id):
    green_state = get_green_state(id)
    green_state = str(green_state)
    green_state = green_state.strip('[]()')
    return str(green_state)

#logout of session
@app.route('/logout', methods=['POST'])
def log_out():
    session.pop("username")
    return redirect(url_for('login'))