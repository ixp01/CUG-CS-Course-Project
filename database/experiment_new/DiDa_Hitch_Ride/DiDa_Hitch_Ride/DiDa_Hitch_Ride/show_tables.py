import sqlite3

db_path = r'D:/CS class/database/experiment_new/DiDa_Hitch_Ride/DiDa_Hitch_Ride/DiDa_Hitch_Ride/data.db'

conn = sqlite3.connect(db_path)
cur = conn.cursor()
cur.execute("SELECT name FROM sqlite_master WHERE type='table';")
tables = cur.fetchall()
print("当前数据库中的表：", [t[0] for t in tables])
conn.close() 