#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
数据验证脚本 - 检查生成数据的质量和参照完整性
"""

import sqlite3
from datetime import datetime

def verify_database(db_path="airticket.db"):
    try:
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        
        print("数据质量验证报告")
        print("=" * 60)
        
        # 1. 基础数据统计
        print("\n1. 基础数据统计:")
        print("-" * 30)
        
        tables = ['user', 'manager', 'flight', 'ticket', 'reservation_queue']
        for table in tables:
            cursor.execute(f"SELECT COUNT(*) FROM {table}")
            count = cursor.fetchone()[0]
            print(f"  {table:<20}: {count:6d} 条记录")
        
        # 2. 城市和航线统计
        print("\n2. 城市和航线统计:")
        print("-" * 30)
        
        cursor.execute("SELECT COUNT(DISTINCT departure_city) FROM flight")
        departure_cities = cursor.fetchone()[0]
        
        cursor.execute("SELECT COUNT(DISTINCT arrival_city) FROM flight")
        arrival_cities = cursor.fetchone()[0]
        
        cursor.execute("SELECT COUNT(DISTINCT departure_city || '-' || arrival_city) FROM flight")
        routes = cursor.fetchone()[0]
        
        print(f"  出发城市数量          : {departure_cities:6d}")
        print(f"  到达城市数量          : {arrival_cities:6d}")
        print(f"  航线数量              : {routes:6d}")
        
        # 3. 参照完整性检查
        print("\n3. 参照完整性检查:")
        print("-" * 30)
        
        # 检查ticket表中的user_id是否都存在于user表
        cursor.execute("""
            SELECT COUNT(*) FROM ticket t 
            LEFT JOIN user u ON t.user_id = u.user_id 
            WHERE u.user_id IS NULL
        """)
        orphan_tickets = cursor.fetchone()[0]
        print(f"  孤立票务记录          : {orphan_tickets:6d} (应为0)")
        
        # 检查ticket表中的flight_number是否都存在于flight表
        cursor.execute("""
            SELECT COUNT(*) FROM ticket t 
            LEFT JOIN flight f ON t.flight_number = f.flight_number 
            WHERE f.flight_number IS NULL
        """)
        orphan_flight_tickets = cursor.fetchone()[0]
        print(f"  无效航班票务          : {orphan_flight_tickets:6d} (应为0)")
        
        # 检查reservation_queue表的完整性
        cursor.execute("""
            SELECT COUNT(*) FROM reservation_queue r 
            LEFT JOIN user u ON r.user_id = u.user_id 
            WHERE u.user_id IS NULL
        """)
        orphan_reservations = cursor.fetchone()[0]
        print(f"  孤立预约记录          : {orphan_reservations:6d} (应为0)")
        
        # 4. 业务逻辑检查
        print("\n4. 业务逻辑检查:")
        print("-" * 30)
        
        # 检查航班座位数是否合理
        cursor.execute("""
            SELECT COUNT(*) FROM flight 
            WHERE available_seats > total_seats OR available_seats < 0
        """)
        invalid_seats = cursor.fetchone()[0]
        print(f"  座位数异常航班        : {invalid_seats:6d} (应为0)")
        
        # 检查航班时间是否合理（到达时间晚于出发时间）
        cursor.execute("""
            SELECT COUNT(*) FROM flight 
            WHERE datetime(arrival_time) <= datetime(departure_time)
        """)
        invalid_times = cursor.fetchone()[0]
        print(f"  时间异常航班          : {invalid_times:6d} (应为0)")
        
        # 检查票价是否合理
        cursor.execute("""
            SELECT COUNT(*) FROM ticket 
            WHERE price <= 0 OR price > 10000
        """)
        invalid_prices = cursor.fetchone()[0]
        print(f"  价格异常票务          : {invalid_prices:6d} (应为0)")
        
        # 5. 数据分布统计
        print("\n5. 数据分布统计:")
        print("-" * 30)
        
        # 航班状态分布
        cursor.execute("""
            SELECT status, COUNT(*) as count, 
                   ROUND(COUNT(*) * 100.0 / (SELECT COUNT(*) FROM flight), 2) as percentage
            FROM flight 
            GROUP BY status 
            ORDER BY count DESC
        """)
        flight_status = cursor.fetchall()
        print("  航班状态分布:")
        for status, count, percentage in flight_status:
            print(f"    {status:<15}: {count:3d} ({percentage:5.1f}%)")
        
        # 票务状态分布
        cursor.execute("""
            SELECT status, COUNT(*) as count,
                   ROUND(COUNT(*) * 100.0 / (SELECT COUNT(*) FROM ticket), 2) as percentage
            FROM ticket 
            GROUP BY status 
            ORDER BY count DESC
        """)
        ticket_status = cursor.fetchall()
        print("  票务状态分布:")
        for status, count, percentage in ticket_status:
            print(f"    {status:<15}: {count:3d} ({percentage:5.1f}%)")
        
        # 6. 收入统计
        print("\n6. 收入统计:")
        print("-" * 30)
        
        cursor.execute("SELECT SUM(price) FROM ticket WHERE status = 'booked'")
        total_revenue = cursor.fetchone()[0] or 0
        
        cursor.execute("SELECT AVG(price) FROM ticket WHERE status = 'booked'")
        avg_price = cursor.fetchone()[0] or 0
        
        cursor.execute("SELECT MAX(price) FROM ticket WHERE status = 'booked'")
        max_price = cursor.fetchone()[0] or 0
        
        cursor.execute("SELECT MIN(price) FROM ticket WHERE status = 'booked'")
        min_price = cursor.fetchone()[0] or 0
        
        print(f"  总收入                : ¥{total_revenue:10.2f}")
        print(f"  平均票价              : ¥{avg_price:10.2f}")
        print(f"  最高票价              : ¥{max_price:10.2f}")
        print(f"  最低票价              : ¥{min_price:10.2f}")
        
        # 7. 热门航线统计
        print("\n7. 热门航线统计 (Top 10):")
        print("-" * 30)
        
        cursor.execute("""
            SELECT 
                f.departure_city || ' → ' || f.arrival_city as route,
                COUNT(t.ticket_id) as ticket_count,
                SUM(t.price) as revenue
            FROM flight f
            LEFT JOIN ticket t ON f.flight_number = t.flight_number AND t.status = 'booked'
            GROUP BY f.departure_city, f.arrival_city
            ORDER BY ticket_count DESC, revenue DESC
            LIMIT 10
        """)
        popular_routes = cursor.fetchall()
        
        for i, (route, tickets, revenue) in enumerate(popular_routes, 1):
            revenue = revenue or 0
            print(f"  {i:2d}. {route:<20} {tickets:3d}票 ¥{revenue:8.0f}")
        
        # 8. 系统完整性结论
        print("\n" + "=" * 60)
        print("系统完整性评估:")
        
        issues = []
        if orphan_tickets > 0:
            issues.append("存在孤立票务记录")
        if orphan_flight_tickets > 0:
            issues.append("存在无效航班票务")
        if orphan_reservations > 0:
            issues.append("存在孤立预约记录")
        if invalid_seats > 0:
            issues.append("存在座位数异常航班")
        if invalid_times > 0:
            issues.append("存在时间异常航班")
        if invalid_prices > 0:
            issues.append("存在价格异常票务")
        
        if not issues:
            print("✅ 数据完整性检查通过，所有参照完整性约束都得到满足")
            print("✅ 业务逻辑检查通过，数据符合业务规则")
            print("✅ 系统可以正常使用")
        else:
            print("❌ 发现以下问题:")
            for issue in issues:
                print(f"   - {issue}")
        
        print("=" * 60)
        
        conn.close()
        
    except Exception as e:
        print(f"验证过程出错: {e}")

if __name__ == "__main__":
    verify_database() 