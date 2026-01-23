import pandas as pd

# 读取xlsx文件
df = pd.read_excel('省会城市邻接矩阵.xlsx')

# 将数据保存为csv文件
df.to_csv('省会城市邻接矩阵.csv', index=False, encoding='utf-8-sig')

print("转换完成！文件已保存为'省会城市邻接矩阵.csv'") 