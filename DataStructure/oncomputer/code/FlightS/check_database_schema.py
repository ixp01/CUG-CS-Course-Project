#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
检查数据库表结构脚本
"""

import sqlite3

def check_database_schema(db_path="airticket.db"):
    try:
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        
        # 获取所有表名
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
        tables = cursor.fetchall()
        
        print("数据库表结构:")
        print("=" * 60)
        
        for table_name in tables:
            table = table_name[0]
            print(f"\n表名: {table}")
            print("-" * 40)
            
            # 获取表结构
            cursor.execute(f"PRAGMA table_info({table});")
            columns = cursor.fetchall()
            
            for col in columns:
                cid, name, type_name, notnull, default, pk = col
                pk_str = " (PRIMARY KEY)" if pk else ""
                null_str = " NOT NULL" if notnull else ""
                default_str = f" DEFAULT {default}" if default else ""
                print(f"  {name:<20} {type_name:<15}{null_str}{default_str}{pk_str}")
        
        conn.close()
        print("\n" + "=" * 60)
        
    except Exception as e:
        print(f"检查数据库失败: {e}")

if __name__ == "__main__":
    check_database_schema() 