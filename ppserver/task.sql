INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`) 
VALUES ('1', '一次性任务', '首次微信登录', '绑定微信', '1', CURRENT_TIMESTAMP, '0', '2');


INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`)
VALUES ('2', '一次性任务', '完善个人资料', '年龄、昵称、性别、头像', '1', CURRENT_TIMESTAMP, '400', '0');

INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`) 
VALUES ('3', '重复性任务', '签到', '连续签到n天', '1', CURRENT_TIMESTAMP, '10', '0');


INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`) 
VALUES ('4', '重复性任务', '分享app', '分享app至微信、qq', '1', CURRENT_TIMESTAMP, '40', '0');


INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`) 
VALUES ('5', '重复性任务', '阅读奖励', '阅读一本小说超过5分钟', '1', CURRENT_TIMESTAMP, '10', '2');

INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`) 
VALUES ('6', '一次性任务', '阅读奖励', '在漫画tab超过5分钟', '1', CURRENT_TIMESTAMP, '10', '2');

INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`) 
VALUES ('7', '一次性任务', '阅读奖励', '在视频tab超过5分钟', '1', CURRENT_TIMESTAMP, '10', '2');

INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`) 
VALUES ('8', '一次性任务', '阅读奖励', '在资讯tab超过5分钟', '1', CURRENT_TIMESTAMP, '10', '2');

INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`) 
VALUES ('9', '重复性任务', '有效反馈', '后台人员评定', '1', CURRENT_TIMESTAMP, '50', '2');

INSERT INTO `ppserver`.`taskinfotable` (`taskId`, `taskType`, `taskName`, 
    `taskContent`, `taskStatus`, `validTime`, `addGoldCoin`, `addCash`) 
VALUES ('10', '一次性任务', '首次微信登录', '绑定微信', '1', CURRENT_TIMESTAMP, '0', '2');




INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/hao_123_1500883433_cd57f7b0ab213e8495e8a3b0ee968c2e_eec492ee.png', 
'好123', 'http://m.hao123.com/');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/xiaoshuo_1500883484_9ef6f25103510cb0bb6c1da60305cf77_794a2ab0.png', 
'小说', '');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/baidu_1500883504_c1ad4fd9b2ccb9833cbfaa2ed67e5bec_3eb0be43.png', 
'百度', 'http://www.baidu.com/');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/tencent_video_1500883866_e3f45a9dd4883f407ca323e395c4268d_5e07e438.png', 
'腾讯视频', 'http://m.v.qq.com/');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/Bzhan_1500883896_70cd6d27e2f61a01f1b0610416d6d462_efa2a76c.png', 
'B站', 'http://m.bilibili.com/index.html');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/A_zhan_1500883934_7f08a96b10072441bb1359d50860532c_a0de31ad.png', 
'A站', 'http://m.acfun.cn/');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/qiushibaike_1500883961_1162e4f2b4232193890a3b8144a6fca2_af2cee31.png', 
'糗事百科', 'http://www.qiushibaike.com/');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/lengtu_1500883985_54653a3c8524d20c0b82b77fec78e15f_c00d1884.png', 
'冷兔', 'http://m.lengxiaohua.com/new');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/taobao_1500884014_8e6a9c21f63f45a9729db18110b775c1_c6ff829f.png', 
'淘宝', 'https://m.taobao.com/#index');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/jingdong_1500884038_df9668fc58ab1a81990883927f0a5e21_80616096.png', 
'京东', 'https://m.jd.com/');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/fenghuang_1500884069_7d12e57812c737033a854afca942c409_fcc3afe8.png', 
'凤凰', 'http://i.ifeng.com/?ch=ifengweb_2014');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/58_1500884096_f54fd4c10b9aaa879d383261b457a230_e78a80fa.png', 
'58', 'http://m.58.com');

INSERT INTO `ppserver`.`icon` (`id`, `version`, `image_url`, `icon_name`, `url`) VALUES 
(NULL, 
'1.0.1', 
'http://static.moogos.com/resource/ganji_1500884114_3a6f859080bcdd12bfc40fcdf035acac_6049d16d.png', 
'赶集', 'https://3g.ganji.com/');























