import sqlite3
import csv

db_path = r'D:/CS class/database/experiment_new/DiDa_Hitch_Ride/DiDa_Hitch_Ride/DiDa_Hitch_Ride/data.db'

def import_csv_to_table(csv_file, table, columns):
    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    with open(csv_file, 'r', encoding='utf-8-sig') as f:
        reader = csv.reader(f)
        next(reader)  # 跳过表头
        for row in reader:
            placeholders = ','.join(['?'] * len(columns))
            sql = f"INSERT INTO {table} ({','.join(columns)}) VALUES ({placeholders})"
            cur.execute(sql, row)
    conn.commit()
    conn.close()
    print(f"{csv_file} 导入 {table} 完成！")

if __name__ == '__main__':
    import_csv_to_table('Drivers.csv', 'Drivers', ['dno', 'dname'])
    import_csv_to_table('Parking.csv', 'Parking', ['pno', 'parea', 'pstatus'])
    import_csv_to_table('Cars.csv', 'Cars', ['cno', 'clicense', 'pno', 'username', 'cin', 'cinHour', 'cinMin', 'cout', 'coutHour', 'coutMin', 'ctype'])
    import_csv_to_table('DriverCar.csv', 'DriverCar', ['dno', 'cno'])
    import_csv_to_table('username.csv', 'username', ['username', 'password', 'phone'])
    print('所有数据导入完成！') 