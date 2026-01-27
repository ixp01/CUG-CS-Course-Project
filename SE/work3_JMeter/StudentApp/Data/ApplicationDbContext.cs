using Microsoft.EntityFrameworkCore;
using StudentApp.Models;

namespace StudentApp.Data
{
    public class ApplicationDbContext : DbContext
    {
        public ApplicationDbContext(DbContextOptions<ApplicationDbContext> options)
            : base(options)
        {
        }

        public DbSet<Student> Students { get; set; } = null!;

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            // 配置表名
            modelBuilder.Entity<Student>().ToTable("Students");

            // 可以在这里添加种子数据
            modelBuilder.Entity<Student>().HasData(
                new Student { Id = 1, Name = "张三", Age = 20, Email = "zhangsan@example.com", EnrollmentDate = DateTime.Now.AddDays(-30) },
                new Student { Id = 2, Name = "李四", Age = 22, Email = "lisi@example.com", EnrollmentDate = DateTime.Now.AddDays(-20) },
                new Student { Id = 3, Name = "王五", Age = 21, Email = "wangwu@example.com", EnrollmentDate = DateTime.Now.AddDays(-10) }
            );
        }
    }
}
