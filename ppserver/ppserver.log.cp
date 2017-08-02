2017/08/02 17:44:39 main.go:52: ===start the ppserver===.
2017/08/02 17:44:39 common.go:41: Listen HandlerMap's length: 15
2017/08/02 17:44:39 common.go:48: router: /effectiveFeedback   handle: 0x1262f80
2017/08/02 17:44:39 common.go:48: router: /updateUserData   handle: 0x125b530
2017/08/02 17:44:39 common.go:48: router: /shareApp   handle: 0x125c9a0
2017/08/02 17:44:39 common.go:48: router: /getTaskStatus   handle: 0x1264520
2017/08/02 17:44:39 common.go:48: router: /incomeList   handle: 0x1266fc0
2017/08/02 17:44:39 common.go:48: router: /withdrawal   handle: 0x1257cc0
2017/08/02 17:44:39 common.go:48: router: /withdrawalRcord   handle: 0x1259010
2017/08/02 17:44:39 common.go:48: router: /feedback   handle: 0x12610f0
2017/08/02 17:44:39 common.go:48: router: /listFeedback   handle: 0x1261d40
2017/08/02 17:44:39 common.go:48: router: /readAward   handle: 0x125d600
2017/08/02 17:44:39 common.go:48: router: /icon   handle: 0x126a500
2017/08/02 17:44:39 common.go:48: router: /loginAndOut   handle: 0x1255c40
2017/08/02 17:44:39 common.go:48: router: /userSignIn   handle: 0x1255db0
2017/08/02 17:44:39 common.go:48: router: /HandleWithdrawal   handle: 0x1259460
2017/08/02 17:44:39 common.go:48: router: /transRecord   handle: 0x1269450
2017/08/02 17:45:14 main.go:52: ===start the ppserver===.
2017/08/02 17:45:14 common.go:41: Listen HandlerMap's length: 15
2017/08/02 17:45:14 common.go:48: router: /getTaskStatus   handle: 0x1264520
2017/08/02 17:45:14 common.go:48: router: /incomeList   handle: 0x1266fc0
2017/08/02 17:45:14 common.go:48: router: /withdrawal   handle: 0x1257cc0
2017/08/02 17:45:14 common.go:48: router: /loginAndOut   handle: 0x1255c40
2017/08/02 17:45:14 common.go:48: router: /userSignIn   handle: 0x1255db0
2017/08/02 17:45:14 common.go:48: router: /transRecord   handle: 0x1269450
2017/08/02 17:45:14 common.go:48: router: /listFeedback   handle: 0x1261d40
2017/08/02 17:45:14 common.go:48: router: /HandleWithdrawal   handle: 0x1259460
2017/08/02 17:45:14 common.go:48: router: /feedback   handle: 0x12610f0
2017/08/02 17:45:14 common.go:48: router: /icon   handle: 0x126a500
2017/08/02 17:45:14 common.go:48: router: /readAward   handle: 0x125d600
2017/08/02 17:45:14 common.go:48: router: /withdrawalRcord   handle: 0x1259010
2017/08/02 17:45:14 common.go:48: router: /effectiveFeedback   handle: 0x1262f80
2017/08/02 17:45:14 common.go:48: router: /updateUserData   handle: 0x125b530
2017/08/02 17:45:14 common.go:48: router: /shareApp   handle: 0x125c9a0
2017/08/02 17:45:17 register.go:42: recv body: {"OpenId":"obDFh1IKeNgyXx2-7VPcTjl1MUGI","NickName":"华华华华子啊","HeadUrl":"http:\/\/wx.qlogo.cn\/mmopen\/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia\/0","Gender":2,"Age":""}
2017/08/02 17:45:17 register.go:48: {obDFh1IKeNgyXx2-7VPcTjl1MUGI 华华华华子啊 http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 2 }
2017/08/02 17:45:17 register.go:66: 用户请求的openId是否已经注册过了： false
2017/08/02 17:45:18 register.go:103: userId: 68 nickName: 华华华华子啊
2017/08/02 17:45:18 eventTable.go:78: task info: map[taskType:one taskStatus:1 addGoldCoin:0 addCash:2 taskId:1]
2017/08/02 17:45:18 eventTable.go:78: task info: map[taskType:one taskStatus:1 addGoldCoin:400 addCash:0 taskId:2]
2017/08/02 17:45:18 eventTable.go:122: error info: <nil>
2017/08/02 17:45:18 eventTable.go:122: error info: <nil>
2017/08/02 17:45:19 register.go:147: taskId: 1 addGoldCoin: 0 addCash 2
2017/08/02 17:45:19 common.go:55: checkUserId 68
2017/08/02 17:45:19 common.go:60: checkUserId 68
2017/08/02 17:45:19 getTaskStatus.go:63: getEvent-->dataMap: map[taskId:1 taskNmae:首次微信登录 date:1501632000 taskStatus:1 userId:68]
2017/08/02 17:45:19 getTaskStatus.go:63: getEvent-->dataMap: map[taskStatus:0 userId:68 taskId:2 taskNmae:完善个人资料 date:1501632000]
2017/08/02 17:45:19 getTaskStatus.go:90: getEvent-->dataMap: map[userId:68 taskId:1 taskNmae:首次微信登录 date:1501632000 taskStatus:1]
2017/08/02 17:45:20 getTaskStatus.go:118: getEvent-->dataMap: map[date:1501632000 taskStatus:0 userId:68 taskId:2 taskNmae:完善个人资料]
2017/08/02 17:45:20 getTaskStatus.go:122: slice len: 4
2017/08/02 17:45:20 common.go:55: checkUserId 68
2017/08/02 17:45:21 common.go:60: checkUserId 68
2017/08/02 17:45:21 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:45:21 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:45:22 goldCash.go:155: slice len: 1
2017/08/02 17:46:05 common.go:55: checkUserId 68
2017/08/02 17:46:05 common.go:60: checkUserId 68
2017/08/02 17:46:05 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:46:05 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:46:05 goldCash.go:155: slice len: 1
2017/08/02 17:46:16 common.go:55: checkUserId 68
2017/08/02 17:46:16 common.go:60: checkUserId 68
2017/08/02 17:46:16 eventTable.go:146: call getEventMap
2017/08/02 17:46:16 eventTable.go:171: getEventMap: map[taskStatus:0 eventId:0 addGoldCoin:0 addCash:0]
2017/08/02 17:46:16 sign.go:41: tody_sign_status: 0
2017/08/02 17:46:16 sign.go:57: last sign date: 2017-08-01 00:00:00
2017/08/02 17:46:16 eventTable.go:146: call getEventMap
2017/08/02 17:46:16 eventTable.go:171: getEventMap: map[taskStatus:0 eventId:0 addGoldCoin:0 addCash:0]
2017/08/02 17:46:16 login.go:80: call getUserInfo userID: 68
2017/08/02 17:46:16 login.go:117: getUserInfo:retMap map[cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:0]
2017/08/02 17:46:16 sign.go:61: map[taskStatus:0 eventId:0 addGoldCoin:0 addCash:0]
2017/08/02 17:46:16 sign.go:62: map[nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:0 cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI]
2017/08/02 17:46:16 sign.go:71: old_gold: 0 last sign_status: 0
2017/08/02 17:46:16 sign.go:81: last sign count: 0 this time sign: 1 addGoldCoin: 10
2017/08/02 17:46:16 goldCash.go:37: recv data from goldDataChan {68 3 10 0 0}
2017/08/02 17:46:16 eventTable.go:32: update_eventTable: recv data from eventChan {68 3 2017-08-02 00:00:00 1}
2017/08/02 17:46:44 common.go:55: checkUserId 68
2017/08/02 17:46:44 common.go:60: checkUserId 68
2017/08/02 17:46:45 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:46:45 goldCash.go:92: getEvent-->dataMap: map[eventTime:1501667119 addGoldCoin:0 addCash:2 taskName:首次微信登录]
2017/08/02 17:46:45 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:46:45 goldCash.go:155: slice len: 2
2017/08/02 17:47:32 common.go:55: checkUserId 68
2017/08/02 17:47:32 common.go:60: checkUserId 68
2017/08/02 17:47:33 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:47:33 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:47:33 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:47:33 goldCash.go:155: slice len: 2
2017/08/02 17:47:38 common.go:55: checkUserId 68
2017/08/02 17:47:38 common.go:60: checkUserId 68
2017/08/02 17:47:38 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:47:38 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:47:38 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:47:38 goldCash.go:155: slice len: 2
2017/08/02 17:47:45 common.go:55: checkUserId 68
2017/08/02 17:47:45 common.go:60: checkUserId 68
2017/08/02 17:47:45 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:47:45 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:47:45 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:47:45 goldCash.go:155: slice len: 2
2017/08/02 17:47:57 common.go:55: checkUserId 68
2017/08/02 17:47:57 common.go:55: checkUserId 68
2017/08/02 17:47:57 login.go:80: call getUserInfo userID: 68
2017/08/02 17:47:57 common.go:55: checkUserId 68
2017/08/02 17:47:57 login.go:80: call getUserInfo userID: 68
2017/08/02 17:47:57 common.go:60: checkUserId 68
2017/08/02 17:47:57 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:1 taskNmae:首次微信登录 date:1501632000 taskStatus:1]
2017/08/02 17:47:57 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:2 taskNmae:完善个人资料 date:1501632000 taskStatus:0]
2017/08/02 17:47:57 common.go:60: checkUserId 68
2017/08/02 17:47:57 login.go:117: getUserInfo:retMap map[openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:10 cash:2 userId:68]
2017/08/02 17:47:57 common.go:60: checkUserId 68
2017/08/02 17:47:57 getTaskStatus.go:90: getEvent-->dataMap: map[userId:68 taskId:1 taskNmae:首次微信登录 date:1501632000 taskStatus:1]
2017/08/02 17:47:57 login.go:25: userLogin: map[openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:10 cash:2 userId:68]
2017/08/02 17:47:57 login.go:30: last sign date: 2017-08-01 00:00:00
2017/08/02 17:47:57 eventTable.go:146: call getEventMap
2017/08/02 17:47:57 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:47:57 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:47:57 goldCash.go:92: getEvent-->dataMap: map[addCash:0 taskName:签到 eventTime:1501667176 addGoldCoin:10]
2017/08/02 17:47:58 login.go:117: getUserInfo:retMap map[userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:10 cash:2]
2017/08/02 17:47:58 login.go:25: userLogin: map[headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:10 cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊]
2017/08/02 17:47:58 login.go:30: last sign date: 2017-08-01 00:00:00
2017/08/02 17:47:58 eventTable.go:146: call getEventMap
2017/08/02 17:47:58 eventTable.go:171: getEventMap: map[taskStatus:0 eventId:0 addGoldCoin:0 addCash:0]
2017/08/02 17:47:58 login.go:35: today_date: 2017-08-02 00:00:00
2017/08/02 17:47:58 eventTable.go:146: call getEventMap
2017/08/02 17:47:58 eventTable.go:171: getEventMap: map[taskStatus:0 eventId:0 addGoldCoin:0 addCash:0]
2017/08/02 17:47:58 login.go:35: today_date: 2017-08-02 00:00:00
2017/08/02 17:47:58 eventTable.go:146: call getEventMap
2017/08/02 17:47:58 eventTable.go:171: getEventMap: map[taskStatus:0 eventId:0 addGoldCoin:0 addCash:0]
2017/08/02 17:47:58 eventTable.go:171: getEventMap: map[taskStatus:0 eventId:0 addGoldCoin:0 addCash:0]
2017/08/02 17:47:58 goldCash.go:155: slice len: 2
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskStatus:1 addGoldCoin:10 addCash:0 taskId:3 taskType:two]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskStatus:1 addGoldCoin:40 addCash:0 taskId:4 taskType:two]
2017/08/02 17:47:58 eventTable.go:102: task info: map[addGoldCoin:10 addCash:2 taskId:5 taskType:two taskStatus:1]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskStatus:1 addGoldCoin:10 addCash:2 taskId:6 taskType:two]
2017/08/02 17:47:58 eventTable.go:102: task info: map[addGoldCoin:10 addCash:2 taskId:7 taskType:two taskStatus:1]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskId:8 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskId:9 taskType:two taskStatus:1 addGoldCoin:50 addCash:2]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskType:two taskStatus:1 addGoldCoin:10 addCash:0 taskId:3]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskStatus:1 addGoldCoin:40 addCash:0 taskId:4 taskType:two]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskId:5 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskId:6 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskId:7 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskId:8 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:47:58 eventTable.go:102: task info: map[taskId:9 taskType:two taskStatus:1 addGoldCoin:50 addCash:2]
2017/08/02 17:47:58 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:47:58 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:47:58 getTaskStatus.go:118: getEvent-->dataMap: map[userId:68 taskId:2 taskNmae:完善个人资料 date:1501632000 taskStatus:0]
2017/08/02 17:47:58 getTaskStatus.go:122: slice len: 4
2017/08/02 17:47:58 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 goldCash.go:155: slice len: 2
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:58 eventTable.go:122: error info: <nil>
2017/08/02 17:47:59 eventTable.go:122: error info: <nil>
2017/08/02 17:47:59 eventTable.go:122: error info: <nil>
2017/08/02 17:48:28 common.go:55: checkUserId 68
2017/08/02 17:48:28 common.go:60: checkUserId 68
2017/08/02 17:48:28 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:48:28 goldCash.go:92: getEvent-->dataMap: map[addGoldCoin:0 addCash:2 taskName:首次微信登录 eventTime:1501667119]
2017/08/02 17:48:28 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:48:29 goldCash.go:155: slice len: 2
2017/08/02 17:48:39 common.go:55: checkUserId 68
2017/08/02 17:48:39 common.go:60: checkUserId 68
2017/08/02 17:48:40 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:48:40 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:48:40 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:48:40 goldCash.go:155: slice len: 2
2017/08/02 17:48:42 common.go:55: checkUserId 68
2017/08/02 17:48:42 common.go:60: checkUserId 68
2017/08/02 17:48:42 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:48:42 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:48:42 goldCash.go:92: getEvent-->dataMap: map[addGoldCoin:10 addCash:0 taskName:签到 eventTime:1501667176]
2017/08/02 17:48:42 goldCash.go:155: slice len: 2
2017/08/02 17:49:43 common.go:55: checkUserId 68
2017/08/02 17:49:43 common.go:60: checkUserId 68
2017/08/02 17:49:43 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:49:43 goldCash.go:92: getEvent-->dataMap: map[addCash:2 taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0]
2017/08/02 17:49:43 goldCash.go:92: getEvent-->dataMap: map[eventTime:1501667176 addGoldCoin:10 addCash:0 taskName:签到]
2017/08/02 17:49:44 goldCash.go:155: slice len: 2
2017/08/02 17:49:58 common.go:55: checkUserId 68
2017/08/02 17:49:58 common.go:60: checkUserId 68
2017/08/02 17:49:58 eventTable.go:146: call getEventMap
2017/08/02 17:49:58 eventTable.go:171: getEventMap: map[taskStatus:0 eventId:209 addGoldCoin:10 addCash:0]
2017/08/02 17:49:58 sign.go:41: tody_sign_status: 0
2017/08/02 17:49:58 sign.go:57: last sign date: 2017-08-01 00:00:00
2017/08/02 17:49:58 eventTable.go:146: call getEventMap
2017/08/02 17:49:58 eventTable.go:171: getEventMap: map[addCash:0 taskStatus:0 eventId:0 addGoldCoin:0]
2017/08/02 17:49:58 login.go:80: call getUserInfo userID: 68
2017/08/02 17:49:58 login.go:117: getUserInfo:retMap map[gold:10 cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00]
2017/08/02 17:49:58 sign.go:61: map[eventId:0 addGoldCoin:0 addCash:0 taskStatus:0]
2017/08/02 17:49:58 sign.go:62: map[userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:10 cash:2]
2017/08/02 17:49:58 sign.go:71: old_gold: 10 last sign_status: 0
2017/08/02 17:49:58 sign.go:81: last sign count: 0 this time sign: 1 addGoldCoin: 10
2017/08/02 17:49:58 goldCash.go:37: recv data from goldDataChan {68 3 10 0 209}
2017/08/02 17:49:58 eventTable.go:32: update_eventTable: recv data from eventChan {68 3 2017-08-02 00:00:00 1}
2017/08/02 17:53:01 common.go:55: checkUserId 68
2017/08/02 17:53:01 common.go:55: checkUserId 68
2017/08/02 17:53:01 common.go:55: checkUserId 68
2017/08/02 17:53:01 common.go:60: checkUserId 68
2017/08/02 17:53:01 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:53:01 goldCash.go:92: getEvent-->dataMap: map[addCash:2 taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0]
2017/08/02 17:53:01 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:53:01 goldCash.go:92: getEvent-->dataMap: map[addCash:0 taskName:签到 eventTime:1501667398 addGoldCoin:10]
2017/08/02 17:53:01 common.go:60: checkUserId 68
2017/08/02 17:53:02 common.go:60: checkUserId 68
2017/08/02 17:53:02 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:53:02 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:53:02 goldCash.go:155: slice len: 3
2017/08/02 17:53:02 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:53:02 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667398 addGoldCoin:10 addCash:0]
2017/08/02 17:53:02 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:53:02 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:53:02 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:53:02 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667398 addGoldCoin:10 addCash:0]
2017/08/02 17:53:02 goldCash.go:155: slice len: 3
2017/08/02 17:53:02 goldCash.go:155: slice len: 3
2017/08/02 17:53:17 login.go:80: call getUserInfo userID: 68
2017/08/02 17:53:17 login.go:80: call getUserInfo userID: 68
2017/08/02 17:53:17 common.go:55: checkUserId 68
2017/08/02 17:53:17 common.go:55: checkUserId 68
2017/08/02 17:53:17 login.go:117: getUserInfo:retMap map[age:0000-00-00 gold:20 cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2]
2017/08/02 17:53:17 login.go:117: getUserInfo:retMap map[cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:20]
2017/08/02 17:53:17 login.go:25: userLogin: map[gender:2 age:0000-00-00 gold:20 cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0]
2017/08/02 17:53:17 login.go:30: last sign date: 2017-08-01 00:00:00
2017/08/02 17:53:17 eventTable.go:146: call getEventMap
2017/08/02 17:53:17 login.go:25: userLogin: map[age:0000-00-00 gold:20 cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2]
2017/08/02 17:53:17 login.go:30: last sign date: 2017-08-01 00:00:00
2017/08/02 17:53:17 eventTable.go:146: call getEventMap
2017/08/02 17:53:17 eventTable.go:171: getEventMap: map[eventId:0 addGoldCoin:0 addCash:0 taskStatus:0]
2017/08/02 17:53:17 login.go:35: today_date: 2017-08-02 00:00:00
2017/08/02 17:53:17 eventTable.go:146: call getEventMap
2017/08/02 17:53:17 eventTable.go:171: getEventMap: map[addCash:0 taskStatus:0 eventId:0 addGoldCoin:0]
2017/08/02 17:53:17 login.go:35: today_date: 2017-08-02 00:00:00
2017/08/02 17:53:17 eventTable.go:146: call getEventMap
2017/08/02 17:53:18 eventTable.go:171: getEventMap: map[taskStatus:1 eventId:209 addGoldCoin:10 addCash:0]
2017/08/02 17:53:18 eventTable.go:171: getEventMap: map[taskStatus:1 eventId:209 addGoldCoin:10 addCash:0]
2017/08/02 17:53:18 common.go:60: checkUserId 68
2017/08/02 17:53:18 common.go:60: checkUserId 68
2017/08/02 17:53:18 eventTable.go:102: task info: map[taskId:3 taskType:two taskStatus:1 addGoldCoin:10 addCash:0]
2017/08/02 17:53:18 eventTable.go:102: task info: map[taskId:4 taskType:two taskStatus:1 addGoldCoin:40 addCash:0]
2017/08/02 17:53:18 eventTable.go:102: task info: map[addGoldCoin:10 addCash:2 taskId:5 taskType:two taskStatus:1]
2017/08/02 17:53:18 eventTable.go:102: task info: map[addCash:2 taskId:6 taskType:two taskStatus:1 addGoldCoin:10]
2017/08/02 17:53:18 eventTable.go:102: task info: map[addGoldCoin:10 addCash:2 taskId:7 taskType:two taskStatus:1]
2017/08/02 17:53:18 eventTable.go:102: task info: map[taskId:8 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:53:18 eventTable.go:102: task info: map[taskId:9 taskType:two taskStatus:1 addGoldCoin:50 addCash:2]
2017/08/02 17:53:18 eventTable.go:102: task info: map[taskId:3 taskType:two taskStatus:1 addGoldCoin:10 addCash:0]
2017/08/02 17:53:18 eventTable.go:102: task info: map[addCash:0 taskId:4 taskType:two taskStatus:1 addGoldCoin:40]
2017/08/02 17:53:18 eventTable.go:102: task info: map[taskType:two taskStatus:1 addGoldCoin:10 addCash:2 taskId:5]
2017/08/02 17:53:18 eventTable.go:102: task info: map[taskId:6 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:53:18 eventTable.go:102: task info: map[taskId:7 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:53:18 eventTable.go:102: task info: map[addCash:2 taskId:8 taskType:two taskStatus:1 addGoldCoin:10]
2017/08/02 17:53:18 eventTable.go:102: task info: map[taskId:9 taskType:two taskStatus:1 addGoldCoin:50 addCash:2]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:1 taskNmae:首次微信登录 date:1501632000 taskStatus:1]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:2 taskNmae:完善个人资料 date:1501632000 taskStatus:0]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[taskNmae:签到 date:1501632000 taskStatus:1 userId:68 taskId:3]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:3 taskNmae:签到 date:1501632000 taskStatus:1]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:4 taskNmae:分享app date:1501632000 taskStatus:0]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:4 taskNmae:分享app date:1501632000 taskStatus:0]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[taskStatus:0 userId:68 taskId:5 taskNmae:阅读一本小说超过5分钟 date:1501632000]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[date:1501632000 taskStatus:0 userId:68 taskId:5 taskNmae:阅读一本小说超过5分钟]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[date:1501632000 taskStatus:0 userId:68 taskId:6 taskNmae:在漫画tab超过5分钟]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:6 taskNmae:在漫画tab超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:7 taskNmae:在视频tab超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[taskId:7 taskNmae:在视频tab超过5分钟 date:1501632000 taskStatus:0 userId:68]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:8 taskNmae:在资讯tab超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:8 taskNmae:在资讯tab超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:9 taskNmae:有效反馈 date:1501632000 taskStatus:0]
2017/08/02 17:53:18 getTaskStatus.go:63: getEvent-->dataMap: map[date:1501632000 taskStatus:0 userId:68 taskId:9 taskNmae:有效反馈]
2017/08/02 17:53:18 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:53:18 goldCash.go:92: getEvent-->dataMap: map[addCash:2 taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0]
2017/08/02 17:53:18 goldCash.go:92: getEvent-->dataMap: map[eventTime:1501667176 addGoldCoin:10 addCash:0 taskName:签到]
2017/08/02 17:53:18 goldCash.go:92: getEvent-->dataMap: map[addGoldCoin:10 addCash:0 taskName:签到 eventTime:1501667398]
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 3 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 3 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 4 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 getTaskStatus.go:90: getEvent-->dataMap: map[userId:68 taskId:1 taskNmae:首次微信登录 date:1501632000 taskStatus:1]
2017/08/02 17:53:18 eventTable.go:130: taskId: 4 have in eventTable
2017/08/02 17:53:18 goldCash.go:155: slice len: 3
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 5 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 5 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 6 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 6 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 7 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 7 have in eventTable
2017/08/02 17:53:18 getTaskStatus.go:118: getEvent-->dataMap: map[userId:68 taskId:2 taskNmae:完善个人资料 date:1501632000 taskStatus:0]
2017/08/02 17:53:18 getTaskStatus.go:122: slice len: 18
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 8 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 8 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 9 have in eventTable
2017/08/02 17:53:18 eventTable.go:122: error info: <nil>
2017/08/02 17:53:18 eventTable.go:130: taskId: 9 have in eventTable
2017/08/02 17:53:18 common.go:55: checkUserId 68
2017/08/02 17:53:19 common.go:60: checkUserId 68
2017/08/02 17:53:19 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:53:19 goldCash.go:92: getEvent-->dataMap: map[addCash:2 taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0]
2017/08/02 17:53:19 goldCash.go:92: getEvent-->dataMap: map[eventTime:1501667176 addGoldCoin:10 addCash:0 taskName:签到]
2017/08/02 17:53:19 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667398 addGoldCoin:10 addCash:0]
2017/08/02 17:53:19 goldCash.go:155: slice len: 3
2017/08/02 17:53:36 common.go:55: checkUserId 68
2017/08/02 17:53:36 common.go:60: checkUserId 68
2017/08/02 17:53:36 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:53:36 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:53:36 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:53:36 goldCash.go:92: getEvent-->dataMap: map[eventTime:1501667398 addGoldCoin:10 addCash:0 taskName:签到]
2017/08/02 17:53:36 goldCash.go:155: slice len: 3
2017/08/02 17:53:56 common.go:55: checkUserId 68
2017/08/02 17:53:56 common.go:55: checkUserId 68
2017/08/02 17:53:56 login.go:80: call getUserInfo userID: 68
2017/08/02 17:53:56 login.go:80: call getUserInfo userID: 68
2017/08/02 17:53:56 common.go:55: checkUserId 68
2017/08/02 17:53:56 common.go:60: checkUserId 68
2017/08/02 17:53:56 common.go:60: checkUserId 68
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:1 taskNmae:首次微信登录 date:1501632000 taskStatus:1]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[taskNmae:完善个人资料 date:1501632000 taskStatus:0 userId:68 taskId:2]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[date:1501632000 taskStatus:1 userId:68 taskId:3 taskNmae:签到]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:3 taskNmae:签到 date:1501632000 taskStatus:1]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[date:1501632000 taskStatus:0 userId:68 taskId:4 taskNmae:分享app]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[taskStatus:0 userId:68 taskId:4 taskNmae:分享app date:1501632000]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:5 taskNmae:阅读一本小说超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:5 taskNmae:阅读一本小说超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:6 taskNmae:在漫画tab超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[date:1501632000 taskStatus:0 userId:68 taskId:6 taskNmae:在漫画tab超过5分钟]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:7 taskNmae:在视频tab超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:7 taskNmae:在视频tab超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:8 taskNmae:在资讯tab超过5分钟 date:1501632000 taskStatus:0]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[date:1501632000 taskStatus:0 userId:68 taskId:8 taskNmae:在资讯tab超过5分钟]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[userId:68 taskId:9 taskNmae:有效反馈 date:1501632000 taskStatus:0]
2017/08/02 17:53:56 getTaskStatus.go:63: getEvent-->dataMap: map[date:1501632000 taskStatus:0 userId:68 taskId:9 taskNmae:有效反馈]
2017/08/02 17:53:56 common.go:60: checkUserId 68
2017/08/02 17:53:56 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:53:56 goldCash.go:92: getEvent-->dataMap: map[addCash:2 taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0]
2017/08/02 17:53:56 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:53:56 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667398 addGoldCoin:10 addCash:0]
2017/08/02 17:53:56 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:53:56 goldCash.go:92: getEvent-->dataMap: map[eventTime:1501667119 addGoldCoin:0 addCash:2 taskName:首次微信登录]
2017/08/02 17:53:56 goldCash.go:92: getEvent-->dataMap: map[addCash:0 taskName:签到 eventTime:1501667176 addGoldCoin:10]
2017/08/02 17:53:56 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667398 addGoldCoin:10 addCash:0]
2017/08/02 17:53:57 login.go:117: getUserInfo:retMap map[openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:20 cash:2 userId:68]
2017/08/02 17:53:57 login.go:25: userLogin: map[gender:2 age:0000-00-00 gold:20 cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊 headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0]
2017/08/02 17:53:57 login.go:30: last sign date: 2017-08-01 00:00:00
2017/08/02 17:53:57 eventTable.go:146: call getEventMap
2017/08/02 17:53:57 login.go:117: getUserInfo:retMap map[headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:20 cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊]
2017/08/02 17:53:57 login.go:25: userLogin: map[headUrl:http://wx.qlogo.cn/mmopen/Fshnp0iaWxIdiabTskKI0dEIJQ2f18jJNJZp8CUxcW2JswOUicOTRLctKAsTp66fSNLMBYFJbpb4UJHPAYUEe98NecmTC5oQ8Uia/0 gender:2 age:0000-00-00 gold:20 cash:2 userId:68 openId:obDFh1IKeNgyXx2-7VPcTjl1MUGI nickName:华华华华子啊]
2017/08/02 17:53:57 login.go:30: last sign date: 2017-08-01 00:00:00
2017/08/02 17:53:57 eventTable.go:146: call getEventMap
2017/08/02 17:53:57 goldCash.go:155: slice len: 3
2017/08/02 17:53:57 eventTable.go:171: getEventMap: map[addGoldCoin:0 addCash:0 taskStatus:0 eventId:0]
2017/08/02 17:53:57 login.go:35: today_date: 2017-08-02 00:00:00
2017/08/02 17:53:57 eventTable.go:146: call getEventMap
2017/08/02 17:53:57 goldCash.go:155: slice len: 3
2017/08/02 17:53:57 eventTable.go:171: getEventMap: map[taskStatus:0 eventId:0 addGoldCoin:0 addCash:0]
2017/08/02 17:53:57 login.go:35: today_date: 2017-08-02 00:00:00
2017/08/02 17:53:57 eventTable.go:146: call getEventMap
2017/08/02 17:53:57 getTaskStatus.go:90: getEvent-->dataMap: map[taskId:1 taskNmae:首次微信登录 date:1501632000 taskStatus:1 userId:68]
2017/08/02 17:53:57 eventTable.go:171: getEventMap: map[taskStatus:1 eventId:209 addGoldCoin:10 addCash:0]
2017/08/02 17:53:57 eventTable.go:171: getEventMap: map[addCash:0 taskStatus:1 eventId:209 addGoldCoin:10]
2017/08/02 17:53:57 getTaskStatus.go:118: getEvent-->dataMap: map[date:1501632000 taskStatus:0 userId:68 taskId:2 taskNmae:完善个人资料]
2017/08/02 17:53:57 getTaskStatus.go:122: slice len: 18
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskId:3 taskType:two taskStatus:1 addGoldCoin:10 addCash:0]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskId:4 taskType:two taskStatus:1 addGoldCoin:40 addCash:0]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskId:5 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskId:6 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskType:two taskStatus:1 addGoldCoin:10 addCash:2 taskId:7]
2017/08/02 17:53:57 eventTable.go:102: task info: map[addGoldCoin:10 addCash:2 taskId:8 taskType:two taskStatus:1]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskId:9 taskType:two taskStatus:1 addGoldCoin:50 addCash:2]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskId:3 taskType:two taskStatus:1 addGoldCoin:10 addCash:0]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskId:4 taskType:two taskStatus:1 addGoldCoin:40 addCash:0]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskId:5 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskStatus:1 addGoldCoin:10 addCash:2 taskId:6 taskType:two]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskType:two taskStatus:1 addGoldCoin:10 addCash:2 taskId:7]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskId:8 taskType:two taskStatus:1 addGoldCoin:10 addCash:2]
2017/08/02 17:53:57 eventTable.go:102: task info: map[taskType:two taskStatus:1 addGoldCoin:50 addCash:2 taskId:9]
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:130: taskId: 3 have in eventTable
2017/08/02 17:53:57 eventTable.go:130: taskId: 3 have in eventTable
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:130: taskId: 4 have in eventTable
2017/08/02 17:53:57 eventTable.go:130: taskId: 4 have in eventTable
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:130: taskId: 5 have in eventTable
2017/08/02 17:53:57 eventTable.go:130: taskId: 5 have in eventTable
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:130: taskId: 6 have in eventTable
2017/08/02 17:53:57 eventTable.go:130: taskId: 6 have in eventTable
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:130: taskId: 7 have in eventTable
2017/08/02 17:53:57 eventTable.go:130: taskId: 7 have in eventTable
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:130: taskId: 8 have in eventTable
2017/08/02 17:53:57 eventTable.go:130: taskId: 8 have in eventTable
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:122: error info: <nil>
2017/08/02 17:53:57 eventTable.go:130: taskId: 9 have in eventTable
2017/08/02 17:53:57 eventTable.go:130: taskId: 9 have in eventTable
2017/08/02 17:55:05 common.go:55: checkUserId 68
2017/08/02 17:55:05 common.go:60: checkUserId 68
2017/08/02 17:55:05 goldCash.go:64: getEvent query err <nil>
2017/08/02 17:55:05 goldCash.go:92: getEvent-->dataMap: map[taskName:首次微信登录 eventTime:1501667119 addGoldCoin:0 addCash:2]
2017/08/02 17:55:05 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667176 addGoldCoin:10 addCash:0]
2017/08/02 17:55:05 goldCash.go:92: getEvent-->dataMap: map[taskName:签到 eventTime:1501667398 addGoldCoin:10 addCash:0]
2017/08/02 17:55:05 goldCash.go:155: slice len: 3
