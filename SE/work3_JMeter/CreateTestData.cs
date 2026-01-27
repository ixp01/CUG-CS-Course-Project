using Microsoft.EntityFrameworkCore;
using StudentApp.Data;
using StudentApp.Models;

var builder = WebApplication.CreateBuilder(args);
builder.Services.AddDbContext<ApplicationDbContext>(options =>
    options.UseSqlServer(builder.Configuration.GetConnectionString("DefaultConnection")));

var app = builder.Build();

// 为JMeter测试创建线程专属数据
using (var scope = app.Services.CreateScope())
{
    var context = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();
    
    Console.WriteLine("开始创建测试数据...");
    
    // 为10个线程创建数据
    for (int thread = 1; thread <= 10; thread++)
    {
        for (int num = 1; num <= 10; num++)
        {
            int id = thread * 100 + num;
            
            // 检查是否已存在
            if (!context.Students.Any(s => s.Id == id))
            {
                var student = new Student
                {
                    Name = $"线程{thread}学生{num}",
                    Age = 18 + (id % 13),
                    Email = $"thread{thread}_student{num}@test.com",
                    EnrollmentDate = DateTime.Now.AddDays(-(id % 365))
                };
                
                context.Students.Add(student);
            }
        }
    }
    
    int count = await context.SaveChangesAsync();
    Console.WriteLine($"成功创建 {count} 条测试数据！");
    
    // 验证数据
    var totalCount = await context.Students.CountAsync();
    Console.WriteLine($"数据库中共有 {totalCount} 个学生");
    
    // 显示每个线程的数据范围
    for (int thread = 1; thread <= 10; thread++)
    {
        int start = thread * 100 + 1;
        int end = thread * 100 + 10;
        var threadCount = await context.Students
            .Where(s => s.Id >= start && s.Id <= end)
            .CountAsync();
        Console.WriteLine($"线程{thread}: ID {start}-{end}, 共 {threadCount} 个学生");
    }
}

Console.WriteLine("\n测试数据创建完成！按任意键退出...");
Console.ReadKey();
