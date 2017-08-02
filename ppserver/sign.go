package main

import (
    //"database/sql"
    "encoding/json"
    "fmt"
    _ "github.com/go-sql-driver/mysql"
    "log"
    "net/http"
    "time"
)

type signJson struct {
    Addgold   int `json:"addGold"`
    SignCount int `json:"signCount"`
}

func userSignIn(w http.ResponseWriter, req *http.Request) {

    var retValue = NewBaseJsonData()

    req.ParseForm()
    param_id, _ := req.Form["userId"]
    userId := param_id[0]
    taskId := 3 //taskId = 3

    flag := checkUserId(userId, Db)
    if !flag {
        str := `{"code":400,"message":"userId not in db"}`
        fmt.Fprint(w, str, "\n")
        log.Println("用户未注册userId", userId)
        return
    }

    tx, _ := Db.Begin()
    defer tx.Commit()

    today_date := time.Now().Format("2006-01-02 00:00:00")
    tmp := getEventMap(userId, taskId, today_date, Db)
    tody_sign_status := tmp["taskStatus"]
    log.Println("tody_sign_status:", tody_sign_status)

    if tody_sign_status.(int) > 0 {

        var retjson signJson
        retjson.Addgold = 0
        retjson.SignCount = 0
        retValue.Code = 300
        retValue.Data = retjson
        retValue.Message = "failed: had signed"
        bytes, _ := json.Marshal(retValue)
        fmt.Fprint(w, string(bytes), "\n")
        log.Println("今天已经签到过了")

    } else {
        sign_date := time.Now().AddDate(0, 0, -1).Format("2006-01-02 00:00:00")
        log.Println("last sign date:", sign_date)

        eventMap := getEventMap(userId, taskId, sign_date, Db)
        userMap := getUserInfo(userId, Db)
        log.Println(eventMap)
        log.Println(userMap)

        old_goldcion, _ := userMap["gold"].(int)
        var sign_status int
        if len(eventMap) > 0 {
            sign_status = eventMap["taskStatus"].(int)
        } else {
            sign_status = 0
        }
        log.Println("old_gold:", old_goldcion, "last sign_status:", sign_status)

        s := []int{10, 15, 20, 25, 30, 35, 40}

        var addGoldCoin int
        if sign_status < 7 {
            addGoldCoin = s[sign_status]
        } else {
            addGoldCoin = 40
        }
        log.Println("last sign count:", sign_status, "this time sign:", sign_status+1, "addGoldCoin:", addGoldCoin)

        var retjson signJson
        retjson.Addgold = addGoldCoin
        retjson.SignCount = sign_status + 1
        retValue.Code = 200
        retValue.Data = retjson
        retValue.Message = "success"
        bytes, _ := json.Marshal(retValue)
        fmt.Fprint(w, string(bytes), "\n")

        //1、修改userInfo表 2、修改evecntTable 3、插入gold_cash金币零钱明细
        tx.Exec("update userInfo set `gold` =? where `userId`=?", old_goldcion+addGoldCoin, userId)

        var data1 eventData
        data1.userId = userId
        data1.taskId = 3 // taskId = 3
        data1.date = today_date
        data1.taskStatus = sign_status + 1
        eventChan <- data1

        var data2 goldData
        data2.userId = userId
        data2.taskId = taskId
        data2.addGoldCoin = addGoldCoin
        data2.addCash = 0
        data2.eventId = tmp["eventId"].(int)
        goldDataChan <- data2
    }
}
