import pandas as pd
import random
from datetime import datetime, timedelta
import string

# 设置随机种子以确保可重复性
random.seed(42)

# 生成随机车牌号
def generate_license_plate():
    provinces = ['京', '沪', '粤', '津', '渝', '冀', '晋', '蒙', '辽', '吉', '黑', '苏', '浙', '皖', '闽', '赣', '鲁', '豫', '鄂', '湘']
    letters = string.ascii_uppercase
    numbers = string.digits
    
    province = random.choice(provinces)
    letter = random.choice(letters)
    number = ''.join(random.choices(numbers, k=5))
    return f"{province}{letter}{number}"

# 生成随机手机号
def generate_phone():
    return '1' + ''.join(random.choices(string.digits, k=10))

# 生成随机姓名
def generate_name():
    first_names = ['张', '王', '李', '赵', '刘', '陈', '杨', '黄', '周', '吴']
    last_names = ['伟', '芳', '娜', '秀英', '敏', '静', '丽', '强', '磊', '军', '洋', '勇', '艳', '杰', '涛', '明', '超', '秀兰', '霞', '平']
    return random.choice(first_names) + random.choice(last_names)

# 生成随机日期
def generate_date(start_date, end_date):
    time_between_dates = end_date - start_date
    days_between_dates = time_between_dates.days
    random_number_of_days = random.randrange(days_between_dates)
    return start_date + timedelta(days=random_number_of_days)

# 生成车主表数据
def generate_drivers(n=150):
    data = []
    for _ in range(n):
        data.append({
            'Dphone': generate_phone(),
            'Dname': generate_name(),
            'Clicense': generate_license_plate()
        })
    return pd.DataFrame(data)

# 生成保安表数据
def generate_baoans(n=120):
    data = []
    for _ in range(n):
        data.append({
            'Bphone': generate_phone(),
            'Bname': generate_name()
        })
    return pd.DataFrame(data)

# 生成停车位表数据
def generate_parking_spaces(n=200):
    data = []
    for i in range(n):
        area = random.choice(['A', 'B', 'C', 'D'])
        number = str(i+1).zfill(3)
        status = random.choice(['空闲', '占用'])
        # 只有当状态为占用时才生成车牌号
        clicense = generate_license_plate() if status == '占用' else None
        
        data.append({
            'SpaceID': i+1,
            'Location': f"{area}区-{number}",
            'Type': random.choice(['普通', '残疾人', '充电桩']),
            'Status': status,
            'Clicense': clicense
        })
    return pd.DataFrame(data)

# 生成值班表数据
def generate_shifts(n=180, baoans_df=None):
    data = []
    start_date = datetime(2023, 1, 1)
    end_date = datetime(2023, 12, 31)
    
    for _ in range(n):
        data.append({
            'ShiftID': _ + 1,
            'Bphone': random.choice(baoans_df['Bphone']),
            'ShiftDate': generate_date(start_date, end_date).strftime('%Y-%m-%d'),
            'ShiftTime': random.choice(['早班', '晚班'])
        })
    return pd.DataFrame(data)

# 生成临时车表数据
def generate_temporary_cars(n=200, parking_spaces_df=None, baoans_df=None):
    data = []
    start_date = datetime(2023, 1, 1)
    end_date = datetime(2023, 12, 31)
    
    for _ in range(n):
        cin_time = generate_date(start_date, end_date)
        cout_time = cin_time + timedelta(hours=random.randint(1, 24))
        fee = round(random.uniform(5, 100), 2)
        
        data.append({
            'Clicense': generate_license_plate(),
            'CinTime': cin_time.strftime('%Y-%m-%d %H:%M:%S'),
            'CoutTime': cout_time.strftime('%Y-%m-%d %H:%M:%S'),
            'Fee': fee,
            'Bphone': random.choice(baoans_df['Bphone'])
        })
    return pd.DataFrame(data)

# 生成月卡车表数据
def generate_monthly_cars(n=150, baoans_df=None):
    data = []
    start_date = datetime(2023, 1, 1)
    end_date = datetime(2023, 12, 31)
    
    for _ in range(n):
        start = generate_date(start_date, end_date)
        end = start + timedelta(days=30)
        fee = round(random.uniform(300, 500), 2)
        
        data.append({
            'Clicense': generate_license_plate(),
            'StartDate': start.strftime('%Y-%m-%d'),
            'EndDate': end.strftime('%Y-%m-%d'),
            'Fee': fee,
            'Bphone': random.choice(baoans_df['Bphone'])
        })
    return pd.DataFrame(data)

# 生成费用记录表数据
def generate_payments(n=250, temporary_cars_df=None, monthly_cars_df=None):
    data = []
    start_date = datetime(2023, 1, 1)
    end_date = datetime(2023, 12, 31)
    
    # 获取所有车牌号列表
    all_licenses = list(pd.concat([temporary_cars_df['Clicense'], monthly_cars_df['Clicense']]).unique())
    
    for _ in range(n):
        data.append({
            'PaymentID': _ + 1,
            'Clicense': random.choice(all_licenses),
            'Amount': round(random.uniform(5, 500), 2),
            'PaymentTime': generate_date(start_date, end_date).strftime('%Y-%m-%d %H:%M:%S'),
            'Method': random.choice(['现金', '支付宝', '微信'])
        })
    return pd.DataFrame(data)

# 生成所有数据并保存为CSV文件
def generate_all_data():
    # 生成基础数据
    drivers_df = generate_drivers()
    baoans_df = generate_baoans()
    parking_spaces_df = generate_parking_spaces()
    
    # 生成依赖数据
    shifts_df = generate_shifts(baoans_df=baoans_df)
    temporary_cars_df = generate_temporary_cars(parking_spaces_df=parking_spaces_df, baoans_df=baoans_df)
    monthly_cars_df = generate_monthly_cars(baoans_df=baoans_df)
    payments_df = generate_payments(temporary_cars_df=temporary_cars_df, monthly_cars_df=monthly_cars_df)
    
    # 保存为CSV文件，使用UTF-8编码并添加BOM
    drivers_df.to_csv('drivers.csv', index=False, encoding='utf-8-sig')
    baoans_df.to_csv('baoans.csv', index=False, encoding='utf-8-sig')
    parking_spaces_df.to_csv('parking_spaces.csv', index=False, encoding='utf-8-sig')
    shifts_df.to_csv('shifts.csv', index=False, encoding='utf-8-sig')
    temporary_cars_df.to_csv('temporary_cars.csv', index=False, encoding='utf-8-sig')
    monthly_cars_df.to_csv('monthly_cars.csv', index=False, encoding='utf-8-sig')
    payments_df.to_csv('payments.csv', index=False, encoding='utf-8-sig')

if __name__ == "__main__":
    generate_all_data()
    print("测试数据生成完成！") 