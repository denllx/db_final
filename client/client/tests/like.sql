CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, PRIMARY KEY(stu_id), stu_name CHAR);
INSERT INTO oop_info(stu_id, stu_name) VALUES (2016012351, "jack love rose");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018001344, "jack");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011243, "akira");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011445, "sam");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011043, "jake");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018010243, "kitty");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018001394, "borjas");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2017014232, "ham");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2015123613, "dont leave him");
SELECT * from oop_info WHERE stu_name LIKE 'ja%';
SELECT * from oop_info WHERE stu_name LIKE '%ki%';
SELECT * from oop_info WHERE stu_name LIKE '%am';
SELECT * from oop_info WHERE stu_name LIKE 'jack%rose';
SELECT * from oop_info WHERE stu_name LIKE '%leave%';
DROP DATABASE OOP;

