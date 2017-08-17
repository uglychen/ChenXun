package main

import (
    "database/sql"
    _ "github.com/go-sql-driver/mysql"
    "log"
    "net/http"
)

var HandlerMap = make(map[string]HandlersFunc)

type BaseJsonData struct {
    Code    int         `json:"code"`
    Data    interface{} `json:"data,omitempty"`
    Message string      `json:"message,omitempty"`
}

func NewBaseJsonData() *BaseJsonData {
    return &BaseJsonData{}
}

type HandlersFunc func(w http.ResponseWriter, req *http.Request)

func InitHandlerMap(m map[string]HandlersFunc) {

    HandlerMap["/loginAndOut"] = LoginAndOut             //用户注册和登录
    HandlerMap["/userSignIn"] = userSignIn               //签到
    HandlerMap["/updateUserData"] = updateUserData       //用户更行资料
    HandlerMap["/shareApp"] = shareApp                   //分享app
    HandlerMap["/readAward"] = readAward                 //阅读奖励
    HandlerMap["/getTaskStatus"] = getTaskStatus         //每日任务状态
    HandlerMap["/incomeList"] = incomeList               //收入明细
    HandlerMap["/withdrawal"] = withdrawal               //申请提现
    HandlerMap["/withdrawalRcord"] = withdrawalRcord     //提现记录包含通过审核的和未通过审核的
    HandlerMap["/feedback"] = feedback                   //用户反馈
    HandlerMap["/listFeedback"] = listFeedback           //反馈列表
    HandlerMap["/effectiveFeedback"] = effectiveFeedback //有效反馈
    HandlerMap["/HandleWithdrawal"] = HandleWithdrawal   //处理用户提现
    HandlerMap["/transRecord"] = transRecord             //用户金币兑换零钱记录
    HandlerMap["/icon"] = icon                           //icon图标
    HandlerMap["/listWithdraw"] = listWithdraw           //列出所有用户的提现记录

    log.Println("Listen HandlerMap's length:", len(HandlerMap))
}

func SetListenHandle() {

    for key, value := range HandlerMap {
        http.HandleFunc(key, value)
        log.Println("router:", key, "  handle:", value)
    }
}

func checkUserId(userId string, db *sql.DB) bool {

    str_sql := "select * from userInfo where userId = ?"
    log.Println("checkUserId", userId)
    rows, _ := db.Query(str_sql, userId)
    defer rows.Close()

    if rows.Next() {
        log.Println("checkUserId", userId)
        return true
    } else {
        return false
    }

}
