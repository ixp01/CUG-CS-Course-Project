from datetime import datetime, timedelta

airlines = ["中国国航", "东方航空", "南方航空", "海南航空", "厦门航空"]
cities = ["北京", "上海", "广州", "深圳", "成都", "重庆", "南京", "杭州", "西安", "青岛"]

with open("D:/myCollege/CS class/DataStructure/oncomputer/codes/FightS/flights.txt", "w", encoding="utf-8") as f:
    base_time = datetime(2024, 3, 20, 8, 0)
    for i in range(120):
        flight_id = f"CA{1001+i}"
        airline = airlines[i % len(airlines)]
        from_city = cities[i % len(cities)]
        to_city = cities[(i+1) % len(cities)]
        dep_time = base_time + timedelta(hours=i)
        arr_time = dep_time + timedelta(hours=2)
        total_seats = 150 + (i % 51)
        price = 500 + (i % 20) * 50
        status = 0  # 全部正常，如需延误/取消可自行调整
        f.write(f"{flight_id}\n{airline}\n{dep_time.strftime('%Y-%m-%d %H:%M')}\n{arr_time.strftime('%Y-%m-%d %H:%M')}\n{from_city}\n{to_city}\n{total_seats}\n{total_seats}\n{price:.2f}\n{status}\n0\n") 