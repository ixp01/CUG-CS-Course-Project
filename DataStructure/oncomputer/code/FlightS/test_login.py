#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
登录测试脚本 - 验证用户密码
"""

import sqlite3
import hashlib

def hash_password(password):
    """密码哈希"""
    return hashlib.sha256(password.encode()).hexdigest()

def test_login(db_path="airticket.db"):
    try:
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        
        print("航班管理系统 - 登录测试")
        print("=" * 40)
        
        # 测试管理员登录
        print("\n测试管理员登录:")
        username = "admin"
        password = "admin123"
        hashed_password = hash_password(password)
        
        cursor.execute("SELECT manager_id FROM manager WHERE username = ? AND password = ?",
                      (username, hashed_password))
        result = cursor.fetchone()
        
        if result:
            print(f"✅ 管理员登录成功: {username} / {password}")
            print(f"   管理员ID: {result[0]}")
        else:
            print(f"❌ 管理员登录失败: {username} / {password}")
        
        # 测试用户登录
        print("\n测试用户登录:")
        username = "user001"
        password = "123456"
        hashed_password = hash_password(password)
        
        cursor.execute("SELECT user_id FROM user WHERE username = ? AND password = ?",
                      (username, hashed_password))
        result = cursor.fetchone()
        
        if result:
            print(f"✅ 用户登录成功: {username} / {password}")
            print(f"   用户ID: {result[0]}")
        else:
            print(f"❌ 用户登录失败: {username} / {password}")
        
        # 显示前10个用户账号
        print("\n前10个用户账号:")
        print("-" * 30)
        cursor.execute("SELECT user_id, username FROM user LIMIT 10")
        users = cursor.fetchall()
        
        for user_id, username in users:
            print(f"  ID: {user_id:2d} | 用户名: {username:<10} | 密码: 123456")
        
        print("\n所有账号的密码都是明文输入，系统会自动加密验证")
        print("=" * 40)
        
        conn.close()
        
    except Exception as e:
        print(f"测试过程出错: {e}")

if __name__ == "__main__":
    test_login() 