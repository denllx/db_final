CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, PRIMARY KEY(stu_id), stu_name CHAR,grade INT);
INSERT INTO oop_info(stu_id, stu_name,grade) VALUES (2018011243, "a",100);
INSERT INTO oop_info(stu_id, stu_name,grade) VALUES (2018011043, "a",90);
INSERT INTO oop_info(stu_id, stu_name,grade) VALUES (2018010243, "a",92);
INSERT INTO oop_info(stu_id, stu_name,grade) VALUES (2018001344, "b",93);
INSERT INTO oop_info(stu_id, stu_name,grade) VALUES (2018001394, "b",99);
INSERT INTO oop_info(stu_id, stu_name,grade) VALUES (2018011445, "c",98);
SELECT MAX(grade) from oop_info;
SELECT MAX(grade) from oop_info GROUP BY stu_name;
SELECT MIN(grade) from oop_info;
SELECT MIN(grade) from oop_info GROUP BY stu_name;
