#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
航班票务系统测试数据生成脚本
生成符合参照完整性的测试数据并导入SQLite数据库
"""

import sqlite3
import random
import hashlib
from datetime import datetime, timedelta
import json

class FlightDataGenerator:
    def __init__(self, db_path="airticket.db"):
        self.db_path = db_path
        self.conn = None
        
        # 中国主要城市数据
        self.cities = [
            "北京", "上海", "广州", "深圳", "杭州", "南京", "苏州", "成都", 
            "重庆", "武汉", "西安", "天津", "青岛", "大连", "厦门", "福州",
            "昆明", "贵阳", "南宁", "海口", "三亚", "长沙", "郑州", "济南",
            "太原", "石家庄", "呼和浩特", "沈阳", "长春", "哈尔滨", "兰州", "西宁"
        ]
        
        # 航空公司数据
        self.airlines = [
            "中国国际航空", "中国东方航空", "中国南方航空", "海南航空",
            "深圳航空", "厦门航空", "四川航空", "山东航空", "上海航空",
            "春秋航空", "吉祥航空", "华夏航空", "天津航空", "奥凯航空"
        ]
        
        # 用户类型
        self.user_types = ["Normal", "VIP"]
        
        # 航班状态
        self.flight_statuses = ["Scheduled", "Delayed", "Cancelled", "Completed"]
        
        # 票务状态
        self.ticket_statuses = ["booked", "cancelled"]
        
    def connect_db(self):
        """连接数据库"""
        try:
            self.conn = sqlite3.connect(self.db_path)
            self.conn.execute("PRAGMA foreign_keys = ON")  # 启用外键约束
            print(f"成功连接到数据库: {self.db_path}")
            return True
        except Exception as e:
            print(f"数据库连接失败: {e}")
            return False
    
    def close_db(self):
        """关闭数据库连接"""
        if self.conn:
            self.conn.close()
            print("数据库连接已关闭")
    
    def hash_password(self, password):
        """密码哈希"""
        return hashlib.sha256(password.encode()).hexdigest()
    
    def clear_existing_data(self):
        """清空现有数据"""
        try:
            cursor = self.conn.cursor()
            
            # 按照外键依赖顺序删除数据
            tables = ["reservation_queue", "ticket", "flight", "manager", "user"]
            
            for table in tables:
                cursor.execute(f"DELETE FROM {table}")
                print(f"清空表 {table}")
            
            self.conn.commit()
            print("所有数据已清空")
            
        except Exception as e:
            print(f"清空数据失败: {e}")
            self.conn.rollback()
    
    def generate_users(self, count=50):
        """生成用户数据"""
        try:
            cursor = self.conn.cursor()
            
            users = []
            for i in range(1, count + 1):
                username = f"user{i:03d}"
                password = self.hash_password("123456")  # 默认密码
                email = f"user{i:03d}@example.com"
                phone = f"1{random.randint(3,9)}{random.randint(100000000, 999999999)}"
                
                users.append((username, password, email, phone))
            
            cursor.executemany(
                "INSERT INTO user (username, password, email, phone) VALUES (?, ?, ?, ?)",
                users
            )
            
            self.conn.commit()
            print(f"成功生成 {count} 个用户")
            
        except Exception as e:
            print(f"生成用户数据失败: {e}")
            self.conn.rollback()
    
    def generate_managers(self, count=3):
        """生成管理员数据"""
        try:
            cursor = self.conn.cursor()
            
            managers = [
                ("admin", self.hash_password("admin123")),
                ("manager1", self.hash_password("manager123")),
                ("manager2", self.hash_password("manager123"))
            ]
            
            cursor.executemany(
                "INSERT INTO manager (username, password) VALUES (?, ?)",
                managers[:count]
            )
            
            self.conn.commit()
            print(f"成功生成 {count} 个管理员")
            
        except Exception as e:
            print(f"生成管理员数据失败: {e}")
            self.conn.rollback()
    
    def generate_flights(self, count=300):
        """生成航班数据"""
        try:
            cursor = self.conn.cursor()
            
            flights = []
            flight_numbers = set()
            
            for i in range(count):
                # 生成唯一航班号
                while True:
                    airline_code = random.choice(["CA", "MU", "CZ", "HU", "ZH", "MF", "3U", "SC"])
                    flight_num = f"{airline_code}{random.randint(1000, 9999)}"
                    if flight_num not in flight_numbers:
                        flight_numbers.add(flight_num)
                        break
                
                # 随机选择出发地和目的地
                departure_city = random.choice(self.cities)
                arrival_city = random.choice([city for city in self.cities if city != departure_city])
                
                # 生成时间
                base_date = datetime.now() + timedelta(days=random.randint(1, 90))
                departure_time = base_date.replace(
                    hour=random.randint(6, 23),
                    minute=random.choice([0, 15, 30, 45]),
                    second=0,
                    microsecond=0
                )
                
                # 飞行时间（1-8小时）
                flight_duration = timedelta(hours=random.randint(1, 8), minutes=random.choice([0, 30]))
                arrival_time = departure_time + flight_duration
                
                # 航空公司
                airline = random.choice(self.airlines)
                
                # 座位数和价格
                total_seats = random.choice([120, 150, 180, 200, 250, 300])
                available_seats = random.randint(0, total_seats)
                
                # 根据距离和时间计算价格
                base_price = 300 + random.randint(100, 1500)
                if flight_duration.total_seconds() > 4 * 3600:  # 超过4小时
                    base_price += 500
                
                price = base_price + random.randint(-100, 300)
                
                # 状态
                status = random.choices(
                    self.flight_statuses,
                    weights=[85, 10, 3, 2],  # 大部分是正常状态
                    k=1
                )[0]
                
                # 经停（20%概率有经停）
                stopover = ""
                if random.random() < 0.2:
                    stopover_city = random.choice([city for city in self.cities 
                                                 if city not in [departure_city, arrival_city]])
                    stopover = stopover_city
                
                flights.append((
                    flight_num, airline, departure_city, arrival_city,
                    departure_time.isoformat(), arrival_time.isoformat(),
                    stopover, total_seats, available_seats, status
                ))
            
            cursor.executemany("""
                INSERT INTO flight 
                (flight_number, airline, departure_city, arrival_city, 
                 departure_time, arrival_time, stopover, total_seats, 
                 available_seats, status) 
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            """, flights)
            
            self.conn.commit()
            print(f"成功生成 {count} 个航班")
            
        except Exception as e:
            print(f"生成航班数据失败: {e}")
            self.conn.rollback()
    
    def generate_tickets(self, count=200):
        """生成票务数据"""
        try:
            cursor = self.conn.cursor()
            
            # 获取用户ID和航班号
            cursor.execute("SELECT user_id FROM user")
            user_ids = [row[0] for row in cursor.fetchall()]
            
            cursor.execute("SELECT flight_number FROM flight WHERE available_seats < total_seats")
            flight_numbers = [row[0] for row in cursor.fetchall()]
            
            if not user_ids or not flight_numbers:
                print("没有足够的用户或航班数据生成票务")
                return
            
            tickets = []
            used_seats = {}  # 记录每个航班已使用的座位
            
            for i in range(min(count, len(flight_numbers) * 2)):  # 限制票务数量
                user_id = random.choice(user_ids)
                flight_number = random.choice(flight_numbers)
                
                # 生成价格（根据航线距离等因素）
                price = 300 + random.randint(100, 1500)
                
                # 生成座位号
                if flight_number not in used_seats:
                    used_seats[flight_number] = set()
                
                # 生成座位号 (A-F列, 1-50排)
                while True:
                    row = random.randint(1, 50)
                    col = random.choice(['A', 'B', 'C', 'D', 'E', 'F'])
                    seat = f"{row:02d}{col}"
                    if seat not in used_seats[flight_number]:
                        used_seats[flight_number].add(seat)
                        break
                
                status = random.choices(
                    self.ticket_statuses,
                    weights=[90, 10],  # 90%正常，10%取消
                    k=1
                )[0]
                
                tickets.append((
                    flight_number, user_id, status, price, seat
                ))
            
            cursor.executemany("""
                INSERT INTO ticket 
                (flight_number, user_id, status, price, seat_number) 
                VALUES (?, ?, ?, ?, ?)
            """, tickets)
            
            self.conn.commit()
            print(f"成功生成 {len(tickets)} 张票务")
            
        except Exception as e:
            print(f"生成票务数据失败: {e}")
            self.conn.rollback()
    
    def generate_reservations(self, count=30):
        """生成预约队列数据"""
        try:
            cursor = self.conn.cursor()
            
            # 获取用户ID和已满座的航班
            cursor.execute("SELECT user_id FROM user")
            user_ids = [row[0] for row in cursor.fetchall()]
            
            cursor.execute("SELECT flight_number FROM flight WHERE available_seats = 0")
            full_flights = [row[0] for row in cursor.fetchall()]
            
            if not user_ids or not full_flights:
                print("没有足够的数据生成预约队列")
                return
            
            reservations = []
            for i in range(min(count, len(full_flights) * 3)):
                user_id = random.choice(user_ids)
                flight_number = random.choice(full_flights)
                
                # 检查是否已经预约过
                cursor.execute(
                    "SELECT 1 FROM reservation_queue WHERE user_id = ? AND flight_number = ?",
                    (user_id, flight_number)
                )
                if cursor.fetchone():
                    continue
                
                priority = random.choices([0, 1, 2], weights=[70, 25, 5], k=1)[0]  # 普通、VIP、紧急
                request_time = datetime.now() - timedelta(
                    days=random.randint(0, 10),
                    hours=random.randint(0, 23)
                )
                
                reservations.append((
                    flight_number, user_id, priority, request_time.isoformat()
                ))
            
            cursor.executemany("""
                INSERT INTO reservation_queue 
                (flight_number, user_id, priority, request_time) 
                VALUES (?, ?, ?, ?)
            """, reservations)
            
            self.conn.commit()
            print(f"成功生成 {len(reservations)} 个预约")
            
        except Exception as e:
            print(f"生成预约数据失败: {e}")
            self.conn.rollback()
    
    def update_flight_seats(self):
        """更新航班可用座位数（基于实际售票情况）"""
        try:
            cursor = self.conn.cursor()
            
            # 获取每个航班的售票数量
            cursor.execute("""
                SELECT flight_number, COUNT(*) as sold_tickets
                FROM ticket 
                WHERE status = 'booked'
                GROUP BY flight_number
            """)
            
            sold_data = cursor.fetchall()
            
            for flight_number, sold_tickets in sold_data:
                # 更新可用座位数
                cursor.execute("""
                    UPDATE flight 
                    SET available_seats = total_seats - ?
                    WHERE flight_number = ?
                """, (sold_tickets, flight_number))
            
            self.conn.commit()
            print("航班座位数已更新")
            
        except Exception as e:
            print(f"更新航班座位失败: {e}")
            self.conn.rollback()
    
    def generate_statistics(self):
        """生成并显示数据统计"""
        try:
            cursor = self.conn.cursor()
            
            # 统计各表数据量
            tables = ["user", "manager", "flight", "ticket", "reservation_queue"]
            stats = {}
            
            for table in tables:
                cursor.execute(f"SELECT COUNT(*) FROM {table}")
                count = cursor.fetchone()[0]
                stats[table] = count
            
            # 城市统计
            cursor.execute("SELECT COUNT(DISTINCT departure_city) FROM flight")
            unique_cities = cursor.fetchone()[0]
            
            # 收入统计
            cursor.execute("SELECT SUM(price) FROM ticket WHERE status = 'booked'")
            total_revenue = cursor.fetchone()[0] or 0
            
            print("\n" + "="*50)
            print("数据生成完成! 统计信息:")
            print("="*50)
            for table, count in stats.items():
                print(f"{table:20}: {count:6d} 条记录")
            print(f"{'城市数量':20}: {unique_cities:6d}")
            print(f"{'总收入':20}: ¥{total_revenue:8.2f}")
            print("="*50)
            
        except Exception as e:
            print(f"生成统计失败: {e}")
    
    def run(self):
        """运行数据生成流程"""
        print("开始生成航班票务系统测试数据...")
        
        if not self.connect_db():
            return False
        
        try:
            # 清空现有数据
            self.clear_existing_data()
            
            # 按顺序生成数据
            self.generate_managers(3)
            self.generate_users(50)
            self.generate_flights(300)
            self.generate_tickets(200)
            self.update_flight_seats()
            self.generate_reservations(30)
            
            # 显示统计
            self.generate_statistics()
            
            print("\n数据生成成功！")
            print("默认登录信息:")
            print("管理员 - 用户名: admin, 密码: admin123")
            print("用户 - 用户名: user001, 密码: 123456")
            
            return True
            
        except Exception as e:
            print(f"数据生成过程中出错: {e}")
            return False
        
        finally:
            self.close_db()

if __name__ == "__main__":
    generator = FlightDataGenerator()
    generator.run() 