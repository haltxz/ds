CREATE TABLE sqlite_sequence(name,seq);
CREATE TABLE "groups" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "name" TEXT,        
    "info" TEXT
);
CREATE TABLE sys_colnames (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL NOT NULL,
    "col" TEXT,
    "name" TEXT,
    "tab" TEXT,
    "width" INTEGER DEFAULT (0) NOT NULL,
    "info" TEXT
);
CREATE INDEX "sys_colnames_tab" on sys_colnames (tab ASC);
CREATE TABLE teachers (
    "id"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "fio" TEXT NOT NULL,    
    "ls" TEXT, 
    "group_id" INTEGER DEFAULT(1) NOT NULL, 
    "percent" INTEGER DEFAULT(0) NOT NULL,
    "info" TEXT
 );
CREATE TABLE classes (
    "id"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "name" TEXT, 
    "price" INTEGER DEFAULT (0) NOT NULL,
    "info" TEXT
 );
CREATE TABLE "pay" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "child_id" INTEGER DEFAULT (0) NOT NULL,
    "date" TEXT,
    "sum" REAL DEFAULT (0) NOT NULL,
    "info" TEXT
);
CREATE INDEX "pay_date" on pay (date ASC);
CREATE INDEX "pay_child_id" on pay (child_id ASC);
CREATE TABLE "accounts" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "child_id" INTEGER DEFAULT (0),
    "teacher_id" INTEGER DEFAULT (0),
    "classes_id" INTEGER DEFAULT (0),
    "group_id"  INTEGER DEFAULT(1) NOT NULL,
    "date" TEXT,
    "sum" REAL DEFAULT (0) NOT NULL,    
    hours  INTEGER DEFAULT (0) NOT NULL,
    "info" TEXT
 );
CREATE INDEX "accounts_child_id" on accounts (child_id ASC);
CREATE INDEX "accounts_teacher_id" on accounts (teacher_id ASC);
CREATE INDEX "accounts_classes_id" on accounts (classes_id ASC);
CREATE INDEX "accounts_group_id" on accounts (group_id ASC);
CREATE INDEX "accounts_date" on accounts (date ASC);
CREATE TABLE "payment" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "teacher_id" INTEGER DEFAULT (0) NOT NULL,
    "date" TEXT,
    "sum" REAL DEFAULT (0) NOT NULL,
    "info" TEXT
, "classes_id"  INTEGER DEFAULT (0) NOT NULL, "pay"  REAL DEFAULT (0) NOT NULL, "child_id" INTEGER DEFAULT (0) NOT NULL);
CREATE TABLE "sys_config" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "name" TEXT,
    "value" TEXT
);
CREATE INDEX "teachers_group_id" on teachers (group_id ASC);
CREATE INDEX "payment_teacher_id" on payment (teacher_id ASC);
CREATE INDEX "payment_date" on payment (date ASC);
CREATE TABLE childs (
    "id"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "fio" TEXT NOT NULL,    
    "ls" TEXT, 
    "email" TEXT,
    "phone" TEXT,
    "group_id" INTEGER DEFAULT (1) NOT NULL,
    "info" TEXT,
    "idx_fio" TEXT    
);
CREATE INDEX "childs_fio" ON childs (idx_fio ASC);
CREATE INDEX "childs_group_id" on childs (ls ASC);
CREATE INDEX "childs_ls" on childs (ls ASC);
CREATE VIEW "worked_money" AS
SELECT t2.fio, t0.teacher_id, t1.name AS classes, t0.classes_id, sum(t0.hours) AS hours , sum(t0.hours)*t1.price AS sum, t2.percent,  t0.child_id
FROM accounts t0 
LEFT OUTER  JOIN classes  t1 ON t0.classes_id = t1.id 
LEFT OUTER  JOIN teachers  t2 ON t0.teacher_id = t2.id 

GROUP BY  t0.teacher_id, t0.child_id, t0.classes_id;
CREATE VIEW "childs_pays" AS
SELECT child_id, sum(sum) AS sum FROM pay 
GROUP BY child_id;
CREATE INDEX "payment_classes_id" on payment (classes_id ASC);
CREATE INDEX "payment_child_id" on payment (child_id ASC);


INSERT INTO "sys_colnames" VALUES(1,'fio','ФИО','childs',0,'ФИО ребенка');
INSERT INTO "sys_colnames" VALUES(2,'info','Доп. инфо','childs',0,'Дополнительная информация');
INSERT INTO "sys_colnames" VALUES(3,'ls','Лицевой счет','childs',0,'Лицевой счет');
INSERT INTO "sys_colnames" VALUES(4,'fio','ФИО','teachers',0,'ФИО Воспитателя');
INSERT INTO "sys_colnames" VALUES(5,'info','Доп. инфо','teachers',0,'Дополнительная информация');
INSERT INTO "sys_colnames" VALUES(6,'ls','Табельный','teachers',0,'Табельный');
INSERT INTO "sys_colnames" VALUES(7,'name','Название','groups',0,'Название');
INSERT INTO "sys_colnames" VALUES(8,'info','Доп. инфо','groups',0,'Дополнительная информация');
INSERT INTO "sys_colnames" VALUES(9,'name','Название','classes',0,'Название');
INSERT INTO "sys_colnames" VALUES(10,'price','Цена','classes',0,'Цена за 1 час');
INSERT INTO "sys_colnames" VALUES(11,'child_id','ФИО Ребенка','accounts',0,'ФИО Ребенка');
INSERT INTO "sys_colnames" VALUES(12,'teacher_id','Педагог','accounts',0,'Педагог');
INSERT INTO "sys_colnames" VALUES(13,'classes_id','Занятие','accounts',0,'Занятие');
INSERT INTO "sys_colnames" VALUES(14,'group_id','Группа','accounts',0,'Группа');
INSERT INTO "sys_colnames" VALUES(15,'date','Дата','accounts',0,'Дата');
INSERT INTO "sys_colnames" VALUES(16,'sum','Сумма','accounts',0,'Сумма');
INSERT INTO "sys_colnames" VALUES(17,'hours','Часы','accounts',0,'Часы');
INSERT INTO "sys_colnames" VALUES(18,'info','Доп. инфо','accounts',0,'Доп. инфо');
INSERT INTO "sys_colnames" VALUES(19,'child_id','ФИО Ребенка','pay',0,'ФИО Ребенка');
INSERT INTO "sys_colnames" VALUES(20,'date','Дата','pay',0,'Дата');
INSERT INTO "sys_colnames" VALUES(21,'info','Доп. инфо','pay',0,'Доп. инфо');
INSERT INTO "sys_colnames" VALUES(22,'sum','Поступившие средства','pay',0,'Поступившие средства');
INSERT INTO "sys_colnames" VALUES(23,'group_id','Группа','teachers',0,'Группа');
INSERT INTO "sys_colnames" VALUES(24,'percent','Процент оплаты','teachers',0,'Процент оплаты');
INSERT INTO "sys_colnames" VALUES(25,'group_id','Группа','childs',0,'Группа');
INSERT INTO "sys_colnames" VALUES(26,'value','Значение','sys_config',0,'0');
INSERT INTO "sys_colnames" VALUES(27,'name','Название ключа','sys_config',0,'');
INSERT INTO "sys_colnames" VALUES(28,'tab','Таблица БД','sys_colnames',0,'0');
INSERT INTO "sys_colnames" VALUES(29,'info','Доп. информация','sys_colnames',0,'0');
INSERT INTO "sys_colnames" VALUES(30,'name','Псевдоним','sys_colnames',0,'0');
INSERT INTO "sys_colnames" VALUES(31,'col','Колонка БД','sys_colnames',0,'0');

INSERT INTO "groups" VALUES(1,'Без группы','Системная группа для новых записей');
INSERT INTO "groups" VALUES(2,'Архив','Системная группа для для ненужных');
