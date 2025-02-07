from flask import Flask, request
import psycopg2
import json

app = Flask(__name__)

# Connect to PostgreSQL
conn = psycopg2.connect(database="ai_results", user="admin", password="password", host="db", port="5432")
cur = conn.cursor()

@app.route('/store_result', methods=['POST'])
def store_result():
    data = request.json
    cur.execute("INSERT INTO results (id, score, time_taken, success) VALUES (%s, %s, %s, %s)",
                (data['id'], data['score'], data['time_taken'], data['success']))
    conn.commit()
    return {"status": "success"}, 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
