import sqlite3
import csv
import os

def export_table_to_csv(db_path, table_name, csv_filename):
    """将数据库表导出为CSV文件"""
    try:
        # 连接数据库
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        
        # 获取表的所有数据
        cursor.execute(f"SELECT * FROM {table_name}")
        rows = cursor.fetchall()
        
        # 获取列名
        cursor.execute(f"PRAGMA table_info({table_name})")
        columns = [column[1] for column in cursor.fetchall()]
        
        # 写入CSV文件
        with open(csv_filename, 'w', newline='', encoding='utf-8-sig') as f:
            writer = csv.writer(f)
            writer.writerow(columns)  # 写入表头
            writer.writerows(rows)    # 写入数据
            
        print(f"成功导出 {table_name} 到 {csv_filename}")
        
    except Exception as e:
        print(f"导出 {table_name} 时出错: {str(e)}")
    finally:
        if conn:
            conn.close()

def main():
    # 数据库文件路径
    db_path = "data.db"
    
    # 要导出的表名和对应的CSV文件名
    tables = {
        "Drivers": "Drivers_export.csv",
        "Parking": "Parking_export.csv",
        "Cars": "Cars_export.csv",
        "DriverCar": "DriverCar_export.csv",
        "username": "username_export.csv"
    }
    
    # 导出每个表
    for table_name, csv_filename in tables.items():
        export_table_to_csv(db_path, table_name, csv_filename)

if __name__ == "__main__":
    main() 