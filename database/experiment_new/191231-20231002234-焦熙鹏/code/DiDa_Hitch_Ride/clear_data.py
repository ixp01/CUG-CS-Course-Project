import sqlite3

db_path = r'D:/CS class/database/experiment_new/DiDa_Hitch_Ride/DiDa_Hitch_Ride/DiDa_Hitch_Ride/data.db'

def clear_table(table):
    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    try:
        cur.execute(f"DELETE FROM {table}")
        conn.commit()
        print(f"{table} 已清空！")
    except Exception as e:
        print(f"{table} 清空失败：{e}")
    finally:
        conn.close()

if __name__ == '__main__':
    for table in ['DriverCar', 'Cars', 'Drivers', 'Parking', 'username']:
        clear_table(table)
    print('所有表已清空！') 