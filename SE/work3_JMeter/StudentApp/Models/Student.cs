using System.ComponentModel.DataAnnotations;

namespace StudentApp.Models
{
    public class Student
    {
        [Key]
        public int Id { get; set; }

        [Required(ErrorMessage = "姓名是必填项")]
        [StringLength(50, ErrorMessage = "姓名长度不能超过50个字符")]
        [Display(Name = "姓名")]
        public string Name { get; set; } = string.Empty;

        [Range(1, 150, ErrorMessage = "年龄必须在1到150之间")]
        [Display(Name = "年龄")]
        public int Age { get; set; }

        [EmailAddress(ErrorMessage = "请输入有效的电子邮件地址")]
        [StringLength(100)]
        [Display(Name = "电子邮件")]
        public string? Email { get; set; }

        [Display(Name = "注册日期")]
        [DataType(DataType.Date)]
        public DateTime EnrollmentDate { get; set; } = DateTime.Now;
    }
}
