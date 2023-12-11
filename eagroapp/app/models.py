import sqlite3 as sql
from datetime import datetime

def checklogin(username, password):
    con = sql.connect("eagro.db")
    cur = con.cursor()
    cur.execute("SELECT username, password FROM users")
    users = cur.fetchall()

    if username == users[0][0] and password == users[0][1]:
        success = True
    else:
        success = False
    con.commit()
    con.close()
    return success

def insert_control_input(c1, c2, c3):
    c1 = f"{c1}"
    c2 = f"{c2}"
    c3 = f"{c3}"
    con = sql.connect("eagro.db")
    cur = con.cursor()
    query = f"UPDATE ranges SET c1 = {c1}, c2 = {c2}, c3 = {c3} WHERE id = 1"
    cur.execute(query)
    con.commit()
    con.close()

def get_control_input(id):
    id = f"{id}"
    con = sql.connect("eagro.db")
    cur = con.cursor()

    if id == 'all':
        query = f"SELECT c1, c2, c3 FROM ranges WHERE id = 1"
    else:
        query = f"SELECT {id} FROM ranges WHERE id = 1"
    cur.execute(query)
    control_input = cur.fetchall()
    con.commit()
    con.close()
    return control_input

def insert_green_data(c1, c2, c3, c4, c5):
    con = sql.connect("eagro.db")
    cur = con.cursor()
    #add datetime to the query
    cur.execute("INSERT INTO green_data(c1, c2, c3, c4, c5) VALUES (?,?,?,?,?)", (c1, c2, c3, c4, c5))# datetime.now()
    con.commit()
    con.close()

def get_green_data():
    con = sql.connect("eagro.db")
    cur = con.cursor()
    #get the latest data
    cur.execute("SELECT * FROM green_data ORDER BY id DESC LIMIT 40")
    green_data = cur.fetchall()
    con.commit()
    con.close()
    return green_data

def get_green_state(type):
    con = sql.connect("eagro.db")
    cur = con.cursor()

    if type == 'all':
        cur.execute("SELECT c1_state, c2_state, c3_state FROM green_state WHERE id = 1")
    else:
        cur.execute("SELECT {}_state FROM green_state WHERE id = 1".format(type))

    green_state = cur.fetchall()
    con.commit()
    con.close()
    return green_state

def update_green_state(state, type, all = None): #what is all?
    column_name = f"{type}_state"
    value = int(state)
    query = f"UPDATE green_state SET {column_name} = {value} WHERE id = 1"

    with sql.connect("eagro.db") as con:
        cur = con.cursor()
        cur.execute(query)
        con.commit()

def delete_green_data():
    con = sql.connect("eagro.db")
    cur = con.cursor()
    cur.execute("DELETE FROM green_data")
    con.commit()
    con.close()

