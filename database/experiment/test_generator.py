import pandas as pd
import random
from datetime import datetime, timedelta
import string
from generate_test_data import *

def test_generate_license_plate():
    print("测试车牌号生成：")
    for _ in range(5):
        print(generate_license_plate())

def test_generate_phone():
    print("\n测试手机号生成：")
    for _ in range(5):
        print(generate_phone())

def test_generate_name():
    print("\n测试姓名生成：")
    for _ in range(5):
        print(generate_name())

def test_generate_date():
    print("\n测试日期生成：")
    start_date = datetime(2023, 1, 1)
    end_date = datetime(2023, 12, 31)
    for _ in range(5):
        print(generate_date(start_date, end_date))

def test_generate_drivers():
    print("\n测试车主表数据生成：")
    df = generate_drivers(n=5)
    print("数据形状:", df.shape)
    print("列名:", df.columns.tolist())
    print(df)
    return df

def test_generate_baoans():
    print("\n测试保安表数据生成：")
    df = generate_baoans(n=5)
    print("数据形状:", df.shape)
    print("列名:", df.columns.tolist())
    print(df)
    return df

def test_generate_parking_spaces():
    print("\n测试停车位表数据生成：")
    df = generate_parking_spaces(n=5)
    print("数据形状:", df.shape)
    print("列名:", df.columns.tolist())
    print(df)
    return df

def test_generate_shifts(baoans_df):
    print("\n测试值班表数据生成：")
    print("输入的保安表形状:", baoans_df.shape)
    print("输入的保安表列名:", baoans_df.columns.tolist())
    df = generate_shifts(n=5, baoans_df=baoans_df)
    print("生成的值班表形状:", df.shape)
    print("生成的值班表列名:", df.columns.tolist())
    print(df)
    return df

def test_generate_temporary_cars(baoans_df, parking_spaces_df):
    print("\n测试临时车表数据生成：")
    print("输入的保安表形状:", baoans_df.shape)
    print("输入的停车位表形状:", parking_spaces_df.shape)
    df = generate_temporary_cars(n=5, parking_spaces_df=parking_spaces_df, baoans_df=baoans_df)
    print("生成的临时车表形状:", df.shape)
    print("生成的临时车表列名:", df.columns.tolist())
    print(df)
    return df

def test_generate_monthly_cars(baoans_df):
    print("\n测试月卡车表数据生成：")
    print("输入的保安表形状:", baoans_df.shape)
    df = generate_monthly_cars(n=5, baoans_df=baoans_df)
    print("生成的月卡车表形状:", df.shape)
    print("生成的月卡车表列名:", df.columns.tolist())
    print(df)
    return df

def test_generate_payments(temporary_cars_df, monthly_cars_df):
    print("\n测试费用记录表数据生成：")
    print("输入的临时车表形状:", temporary_cars_df.shape)
    print("输入的临时车表列名:", temporary_cars_df.columns.tolist())
    print("临时车表的车牌号：", temporary_cars_df['Clicense'].tolist())
    
    print("\n输入的月卡车表形状:", monthly_cars_df.shape)
    print("输入的月卡车表列名:", monthly_cars_df.columns.tolist())
    print("月卡车表的车牌号：", monthly_cars_df['Clicense'].tolist())
    
    # 合并车牌号并检查
    all_licenses = pd.concat([temporary_cars_df['Clicense'], monthly_cars_df['Clicense']])
    print("\n合并后的所有车牌号：", all_licenses.tolist())
    print("合并后的唯一车牌号数量：", len(all_licenses.unique()))
    
    df = generate_payments(n=5, temporary_cars_df=temporary_cars_df, monthly_cars_df=monthly_cars_df)
    print("\n生成的费用记录表形状:", df.shape)
    print("生成的费用记录表列名:", df.columns.tolist())
    print(df)
    return df

if __name__ == "__main__":
    print("开始测试数据生成函数...")
    print("="*50)
    test_generate_license_plate()
    print("="*50)
    test_generate_phone()
    print("="*50)
    test_generate_name()
    print("="*50)
    test_generate_date()
    print("="*50)
    test_generate_drivers()
    print("="*50)
    
    # 生成基础数据
    print("\n生成基础数据...")
    print("="*50)
    baoans_df = test_generate_baoans()
    parking_spaces_df = test_generate_parking_spaces()
    
    # 生成依赖数据
    print("\n生成依赖数据...")
    print("="*50)
    shifts_df = test_generate_shifts(baoans_df)
    temporary_cars_df = test_generate_temporary_cars(baoans_df, parking_spaces_df)
    monthly_cars_df = test_generate_monthly_cars(baoans_df)
    payments_df = test_generate_payments(temporary_cars_df, monthly_cars_df)
    
    print("\n所有测试完成！") 