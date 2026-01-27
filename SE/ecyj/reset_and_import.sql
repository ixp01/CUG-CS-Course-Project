-- 清理所有测试数据
DELETE FROM test_result_details;
DELETE FROM test_result_items;
DELETE FROM test_records;
DELETE FROM equipment_info;
DELETE FROM manufacturers;

-- 重置自增ID
DELETE FROM sqlite_sequence WHERE name='test_result_details';
DELETE FROM sqlite_sequence WHERE name='test_result_items';
DELETE FROM sqlite_sequence WHERE name='test_records';
DELETE FROM sqlite_sequence WHERE name='equipment_info';
DELETE FROM sqlite_sequence WHERE name='manufacturers';
