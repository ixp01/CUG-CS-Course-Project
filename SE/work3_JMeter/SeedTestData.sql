-- 方案1：为每个JMeter线程创建专属测试数据
-- 10个线程，每个线程10个学生ID

USE StudentDB;
GO

-- 保留原始种子数据（ID 1-3）
-- 为10个线程创建数据：
-- 线程1: ID 101-110
-- 线程2: ID 201-210
-- 线程3: ID 301-310
-- ...
-- 线程10: ID 1001-1010

SET IDENTITY_INSERT Students ON;

DECLARE @thread INT = 1;
DECLARE @studentNum INT;

WHILE @thread <= 10
BEGIN
    SET @studentNum = 1;
    WHILE @studentNum <= 10
    BEGIN
        DECLARE @id INT = @thread * 100 + @studentNum;
        
        INSERT INTO Students (Id, Name, Age, Email, EnrollmentDate)
        VALUES (
            @id,
            CONCAT('线程', @thread, '学生', @studentNum),
            18 + (@id % 13),  -- 年龄 18-30
            CONCAT('thread', @thread, '_student', @studentNum, '@test.com'),
            DATEADD(DAY, -(@id % 365), GETDATE())
        );
        
        SET @studentNum = @studentNum + 1;
    END
    SET @thread = @thread + 1;
END

SET IDENTITY_INSERT Students OFF;
GO

-- 验证创建的数据
SELECT 
    CASE 
        WHEN Id BETWEEN 101 AND 110 THEN '线程1'
        WHEN Id BETWEEN 201 AND 210 THEN '线程2'
        WHEN Id BETWEEN 301 AND 310 THEN '线程3'
        WHEN Id BETWEEN 401 AND 410 THEN '线程4'
        WHEN Id BETWEEN 501 AND 510 THEN '线程5'
        WHEN Id BETWEEN 601 AND 610 THEN '线程6'
        WHEN Id BETWEEN 701 AND 710 THEN '线程7'
        WHEN Id BETWEEN 801 AND 810 THEN '线程8'
        WHEN Id BETWEEN 901 AND 910 THEN '线程9'
        WHEN Id BETWEEN 1001 AND 1010 THEN '线程10'
        ELSE '其他'
    END AS 线程分组,
    COUNT(*) AS 学生数量
FROM Students
WHERE Id >= 101
GROUP BY 
    CASE 
        WHEN Id BETWEEN 101 AND 110 THEN '线程1'
        WHEN Id BETWEEN 201 AND 210 THEN '线程2'
        WHEN Id BETWEEN 301 AND 310 THEN '线程3'
        WHEN Id BETWEEN 401 AND 410 THEN '线程4'
        WHEN Id BETWEEN 501 AND 510 THEN '线程5'
        WHEN Id BETWEEN 601 AND 610 THEN '线程6'
        WHEN Id BETWEEN 701 AND 710 THEN '线程7'
        WHEN Id BETWEEN 801 AND 810 THEN '线程8'
        WHEN Id BETWEEN 901 AND 910 THEN '线程9'
        WHEN Id BETWEEN 1001 AND 1010 THEN '线程10'
        ELSE '其他'
    END
ORDER BY 线程分组;

-- 查看示例数据
SELECT TOP 5 * FROM Students WHERE Id BETWEEN 101 AND 110;  -- 线程1的数据
SELECT TOP 5 * FROM Students WHERE Id BETWEEN 201 AND 210;  -- 线程2的数据
