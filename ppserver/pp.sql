#用户表userInfo 用户id 微信openID 昵称 头像地址 性别 年龄 金币 现金
CREATE TABLE IF NOT EXISTS `ppserver`.`userInfo`(
   `userId`     int UNSIGNED AUTO_INCREMENT NOT null,
   `wechatId`   VARCHAR(100)    DEFAULT null unique,
   `nickName`   VARCHAR(100)    DEFAULT null,
   `headUrl`    VARCHAR(1000)   DEFAULT null,
   `gender`     VARCHAR(15)     DEFAULT null,
   `age`        VARCHAR(10)     DEFAULT '0000-00-00',
   `gold`       int             DEFAULT null,
   `cash`       FLOAT           DEFAULT null,
   PRIMARY KEY (`userId`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

#任务内容表
CREATE TABLE IF NOT EXISTS `ppserver`.`taskInfoTable`(
   `taskId`      INT UNSIGNED    NOT null,
   `taskType`    VARCHAR(100)    DEFAULT null,
   `taskName`    VARCHAR(100)    DEFAULT null,
   `taskContent` VARCHAR(256)    DEFAULT null,
   `taskStatus`  INT             DEFAULT 1,
   `validTime`   TIMESTAMP       DEFAULT CURRENT_TIMESTAMP,
   `addGoldCoin` int             DEFAULT null,
   `addCash`     FLOAT           DEFAULT null,
   PRIMARY KEY ( `taskId` )
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

#任务事件表
CREATE TABLE IF NOT EXISTS `ppserver`.`eventTable`(
   `userId`         INT UNSIGNED      NOT null,
   `taskId`         INT UNSIGNED      NOT null,
   `date`           TIMESTAMP         DEFAULT CURRENT_TIMESTAMP,
   `createTime`     TIMESTAMP         DEFAULT CURRENT_TIMESTAMP,
   `endTime`        TIMESTAMP         DEFAULT CURRENT_TIMESTAMP,
   `taskStatus`     INT               DEFAULT NULL,
   `addGoldCoin`    int               DEFAULT NULL,
   `addCash`        FLOAT             DEFAULT NULL,
   `eventId`        INT UNSIGNED AUTO_INCREMENT not null,
   unique index(`userId` ,`taskId`,`date`),
   PRIMARY KEY ( `eventId`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

#金币现金表
CREATE TABLE IF NOT EXISTS `ppserver`.`goldAndCash`(
   `userId`         INT UNSIGNED      not null,
   `taskId`         INT UNSIGNED      not null,
   `eventTime`      TIMESTAMP         DEFAULT CURRENT_TIMESTAMP,
   `taskType`       VARCHAR(100)      DEFAULT null,
   `currentGold`    int               DEFAULT null,
   `currentBalance` int               DEFAULT null,
   `addGoldCoin`    int               DEFAULT null,
   `addCash`        FLOAT             DEFAULT null,
   `glodBalance`    int               DEFAULT null,
   `cashBalance`    int               DEFAULT null,
   `eventId`        int               not null,
   PRIMARY KEY ( `userId`, `taskId`, `eventTime` )
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

#申请提现withdrawal表
CREATE TABLE IF NOT EXISTS `ppserver`.`withdrawal`(
   `userId`         INT UNSIGNED      NOT null,
   `alipay`         VARCHAR(100)      DEFAULT null,
   `name`           VARCHAR(50)       NOT null,
   `commitTime`     TIMESTAMP         DEFAULT CURRENT_TIMESTAMP,
   `status`         int               DEFAULT null,
   `amount`         INT               DEFAULT null,
   PRIMARY KEY ( `userId` , `commitTime`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

#用户反馈表
CREATE TABLE IF NOT EXISTS `ppserver`.`feedback`(
   `userId`                   INT UNSIGNED      NOT null,
   `android_id`               VARCHAR(100)      DEFAULT null,
   `pkg_name`                 VARCHAR(50)       DEFAULT null,
   `contact_way`              VARCHAR(20)       DEFAULT null,
   `content`                  VARCHAR(500)      DEFAULT null,
   `channel`                  VARCHAR(50)       DEFAULT null,
   `commitTime`               TIMESTAMP         DEFAULT CURRENT_TIMESTAMP,
   `app_version`              VARCHAR(30)       DEFAULT null,
   `os_version`               VARCHAR(30)       DEFAULT null,
   `valid`                    VARCHAR(1)        DEFAULT 0,
   `op`                       VARCHAR(1)        DEFAULT 0,
   unique index(`userId`, `commitTime`),
   PRIMARY KEY( `userId`, `commitTime`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

#图标
CREATE TABLE IF NOT EXISTS `ppserver`.`icon`(
   `id`                      int UNSIGNED AUTO_INCREMENT   NOT null,
   `version`                 VARCHAR(100)                  DEFAULT null,
   `image_url`               VARCHAR(512)                  DEFAULT null,
   `icon_name`               VARCHAR(45)                   DEFAULT null,
   `url`                     VARCHAR(128)                  DEFAULT null,
   PRIMARY KEY( `id` )
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `ppserver`.`transTable`(
   `userId`                  int UNSIGNED                  NOT null,
   `transTime`               TIMESTAMP                     DEFAULT CURRENT_TIMESTAMP,
   `reduceGold`              int                           DEFAULT null,
   `addCash`                 FLOAT                         DEFAULT null,
   `eventId`                 int UNSIGNED AUTO_INCREMENT   NOT null,
   PRIMARY KEY( `eventId`,`userId`,`transTime`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
