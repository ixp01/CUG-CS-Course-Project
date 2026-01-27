using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

#pragma warning disable CA1814 // Prefer jagged arrays over multidimensional

namespace StudentApp.Migrations
{
    /// <inheritdoc />
    public partial class InitialCreate : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "Students",
                columns: table => new
                {
                    Id = table.Column<int>(type: "int", nullable: false)
                        .Annotation("SqlServer:Identity", "1, 1"),
                    Name = table.Column<string>(type: "nvarchar(50)", maxLength: 50, nullable: false),
                    Age = table.Column<int>(type: "int", nullable: false),
                    Email = table.Column<string>(type: "nvarchar(100)", maxLength: 100, nullable: true),
                    EnrollmentDate = table.Column<DateTime>(type: "datetime2", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Students", x => x.Id);
                });

            migrationBuilder.InsertData(
                table: "Students",
                columns: new[] { "Id", "Age", "Email", "EnrollmentDate", "Name" },
                values: new object[,]
                {
                    { 1, 20, "zhangsan@example.com", new DateTime(2025, 11, 16, 8, 58, 15, 579, DateTimeKind.Local).AddTicks(8221), "张三" },
                    { 2, 22, "lisi@example.com", new DateTime(2025, 11, 26, 8, 58, 15, 579, DateTimeKind.Local).AddTicks(8234), "李四" },
                    { 3, 21, "wangwu@example.com", new DateTime(2025, 12, 6, 8, 58, 15, 579, DateTimeKind.Local).AddTicks(8236), "王五" }
                });
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "Students");
        }
    }
}
