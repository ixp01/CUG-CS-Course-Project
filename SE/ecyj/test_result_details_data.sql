-- 为13条检测记录添加详细测量数据（ao/bo/co数据）

-- 记录1 (test_record_id=1): PT1测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (1, 'PT1', 50.02, 0.12, 0.15, 100.05, 99.95,
        50.01, 0.11, 0.14, 100.03, 99.97,
        49.99, 0.10, 0.13, 100.02, 99.98,
        '不带自校', 0.08, '合格');

-- 记录2 (test_record_id=2): CT1测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (2, 'CT1', 49.98, 0.15, 0.18, 100.08, 99.92,
        50.03, 0.13, 0.16, 100.06, 99.94,
        50.00, 0.12, 0.15, 100.04, 99.96,
        '按1测量', 0.09, '合格');

-- 记录3 (test_record_id=3): PT2测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (3, 'PT2', 50.05, 0.14, 0.17, 100.07, 99.93,
        49.97, 0.16, 0.19, 100.09, 99.91,
        50.01, 0.15, 0.18, 100.08, 99.92,
        '按2储存', 0.10, '合格');

-- 记录4 (test_record_id=4): CT2测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (4, 'CT2', 50.00, 0.11, 0.14, 100.05, 99.95,
        50.02, 0.10, 0.13, 100.03, 99.97,
        49.99, 0.12, 0.15, 100.04, 99.96,
        '不带自校', 0.07, '合格');

-- 记录5 (test_record_id=5): PT1测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (5, 'PT1', 49.96, 0.13, 0.16, 100.06, 99.94,
        50.04, 0.14, 0.17, 100.07, 99.93,
        50.01, 0.11, 0.14, 100.05, 99.95,
        '按1测量', 0.09, '合格');

-- 记录6 (test_record_id=6): PT2测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (6, 'PT2', 50.03, 0.12, 0.15, 100.04, 99.96,
        49.98, 0.15, 0.18, 100.08, 99.92,
        50.00, 0.13, 0.16, 100.06, 99.94,
        '按2储存', 0.08, '合格');

-- 记录7 (test_record_id=7): CT1测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (7, 'CT1', 50.01, 0.16, 0.19, 100.09, 99.91,
        50.02, 0.11, 0.14, 100.05, 99.95,
        49.99, 0.14, 0.17, 100.07, 99.93,
        '不带自校', 0.11, '合格');

-- 记录8 (test_record_id=8): PT1测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (8, 'PT1', 49.97, 0.10, 0.13, 100.03, 99.97,
        50.05, 0.12, 0.15, 100.05, 99.95,
        50.00, 0.15, 0.18, 100.08, 99.92,
        '按1测量', 0.08, '合格');

-- 记录9 (test_record_id=9): CT2测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (9, 'CT2', 50.04, 0.13, 0.16, 100.06, 99.94,
        49.96, 0.14, 0.17, 100.07, 99.93,
        50.02, 0.11, 0.14, 100.04, 99.96,
        '按2储存', 0.09, '合格');

-- 记录10 (test_record_id=10): PT2测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (10, 'PT2', 50.00, 0.15, 0.18, 100.08, 99.92,
        50.01, 0.10, 0.13, 100.03, 99.97,
        49.99, 0.12, 0.15, 100.05, 99.95,
        '不带自校', 0.10, '合格');

-- 记录11 (test_record_id=11): CT1测量数据（最后一条有轻微超标）
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (11, 'CT1', 50.08, 0.18, 0.22, 100.12, 99.88,
        49.92, 0.19, 0.23, 100.13, 99.87,
        50.01, 0.16, 0.20, 100.10, 99.90,
        '按1测量', 0.13, '需复检');

-- 记录12 (test_record_id=12): PT1测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (12, 'PT1', 50.02, 0.11, 0.14, 100.04, 99.96,
        49.99, 0.13, 0.16, 100.06, 99.94,
        50.01, 0.12, 0.15, 100.05, 99.95,
        '按2储存', 0.08, '合格');

-- 记录13 (test_record_id=13): CT2测量数据
INSERT INTO test_result_details (test_record_id, item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, 
                                   bo_f, bo_d, bo_du, bo_upt, bo_uyb,
                                   co_f, co_d, co_du, co_upt, co_uyb,
                                   pt_check_note, r_percentage, measurement_result)
VALUES (13, 'CT2', 49.98, 0.14, 0.17, 100.07, 99.93,
        50.03, 0.12, 0.15, 100.05, 99.95,
        50.00, 0.10, 0.13, 100.03, 99.97,
        '不带自校', 0.07, '合格');
