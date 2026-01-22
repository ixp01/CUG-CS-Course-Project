import csv
import random

def add_minutes(hour, minute, delta):
    total = hour * 60 + minute + delta
    new_hour = (total // 60) % 24
    new_minute = total % 60
    return new_hour, new_minute

# 生成100个车主
def generate_drivers():
    with open('Drivers.csv', 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(['dno', 'dname'])
        for i in range(1, 101):
            writer.writerow([i, f'车主{i}'])

# 生成200个停车位，其中100个占用，100个空闲
def generate_parking(occupied_pnos):
    with open('Parking.csv', 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(['pno', 'parea', 'pstatus'])
        for i in range(1, 201): # 生成200个停车位
            area = f'A区' if i <= 100 else 'B区'
            # 根据是否在 occupied_pnos 列表中设置状态
            status = '占用' if i in occupied_pnos else '空闲'
            writer.writerow([i, area, status])

# 生成100辆车
def generate_cars(occupied_pnos):
    with open('Cars.csv', 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(['cno', 'clicense', 'pno', 'username', 'cin', 'cinHour', 'cinMin', 'cout', 'coutHour', 'coutMin', 'ctype'])
        for i in range(1, 101):
            clicense = f'鄂A{random.randint(10000,99999)}'
            pno = occupied_pnos[i-1]  # 分配一个已经确定为占用的停车位
            username = f'user{random.randint(1, 20)}' # 保留保安号生成逻辑
            cinHour = random.randint(0, 23)
            cinMin = random.randint(0, 59)
            cin = f'{cinHour:02d}:{cinMin:02d}'
            
            # 出场时间留空
            cout = ''
            coutHour = 0 # 或者其他表示空的值，根据数据库设计调整
            coutMin = 0  # 或者其他表示空的值
            
            ctype = random.choice([1, 2])
            writer.writerow([i, clicense, pno, username, cin, cinHour, cinMin, cout, coutHour, coutMin, ctype])

# 生成100条车主-车辆关联 (假设车主和车辆仍是一一对应)
def generate_drivercar():
    with open('DriverCar.csv', 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(['dno', 'cno'])
        for i in range(1, 101):
            writer.writerow([i, i])  # 一一对应

# 生成100个用户 (保留原逻辑)
def generate_username():
    with open('username.csv', 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.writer(f)
        writer.writerow(['username', 'password', 'phone'])
        for i in range(1, 101):
            username = f'user{i}'
            password = f'pass{i:03d}'
            phone = f'13{random.randint(100000000, 999999999)}'
            writer.writerow([username, password, phone])

def main():
    generate_drivers()
    
    # 生成100个随机且不重复的停车位编号用于分配车辆
    occupied_pnos = random.sample(range(1, 201), 100)
    
    generate_parking(occupied_pnos)
    generate_cars(occupied_pnos) # 将占用的停车位列表传递给 generate_cars
    generate_drivercar()
    generate_username()
    print("所有CSV文件已生成！")

if __name__ == '__main__':
    main() 