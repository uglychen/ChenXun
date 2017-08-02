package main

import (
    "database/sql"
    "encoding/json"
    "fmt"
    _ "github.com/go-sql-driver/mysql"
    "log"
    "net/http"
    "strconv"
    "time"
)

func UserLogin(w http.ResponseWriter, req *http.Request) {

    retValue := NewBaseJsonData()
    if req.Method == "GET" {
        param_openId, _ := req.Form["userId"]
        userId := param_openId[0]
        if userId == "" {
            fmt.Fprint(w, `{"code":"300","message":"user ID is null"}`, "\n")
        }

        retMap := getUserInfo(userId, Db)
        log.Println("userLogin:", retMap)

        if len(retMap) > 0 {
            //先查前一天的签到次数
            sign_date := time.Now().AddDate(0, 0, -1).Format(("2006-01-02 00:00:00"))
            log.Println("last sign date:", sign_date)
            eventMap := getEventMap(userId, 3, sign_date, Db)

            //查询今天的签到次数
            today_date := time.Now().Format(("2006-01-02 00:00:00"))
            log.Println("today_date:", today_date)
            today_eventMap := getEventMap(userId, 3, today_date, Db)

            id, _ := strconv.Atoi(userId)
            retMap["userId"] = id

            yestoday_sign := eventMap["taskStatus"].(int)
            today_sign := today_eventMap["taskStatus"].(int)
            if today_sign > 0 {
                retMap["signCount"] = today_sign
            } else {
                if yestoday_sign > 0 {
                    retMap["signCount"] = yestoday_sign + 1
                } else {
                    retMap["signCount"] = 1
                }
            }

            retValue.Code = 200
            retValue.Data = retMap
            retValue.Message = "success"
            bytes, _ := json.Marshal(retValue)
            fmt.Fprint(w, string(bytes), "\n")

            //创建重复性每日任务
            taskMap := getTaskInfoTable("two")
            createEvent(id, taskMap, Db)
        } else {
            retValue.Code = 300
            retValue.Message = "failed"
            bytes, _ := json.Marshal(retValue)
            fmt.Fprint(w, string(bytes), "\n")
        }

    } else {
        retValue.Code = 400
        retValue.Data = 0
        retValue.Message = "use get method"
        bytes, _ := json.Marshal(retValue)
        fmt.Fprint(w, string(bytes), "\n")
    }
}

func getUserInfo(userId string, db *sql.DB) map[string]interface{} {

    log.Println("call getUserInfo userID:", userId)

    retMap := make(map[string]interface{})

    var wechatId string
    var nickName string
    var headUrl string
    var gender byte
    var age string
    var gold int
    var cash float32

    sql := "select wechatId, nickName, headUrl, gender, age, gold, cash FROM userInfo where userId = ?"
    rows, err := db.Query(sql, userId)
    if err != nil {
        log.Println(err)
    }
    defer rows.Close()

    if rows.Next() {
        err := rows.Scan(&wechatId, &nickName, &headUrl, &gender, &age, &gold, &cash)
        if err != nil {
            log.Println("error info:", err)
        }
    } else {
        log.Println("no user info in db")
        return retMap
    }

    retMap["userId"] = userId
    retMap["openId"] = wechatId
    retMap["nickName"] = nickName
    retMap["headUrl"] = headUrl
    retMap["gender"] = gender
    retMap["age"] = age
    retMap["gold"] = gold
    retMap["cash"] = cash
    log.Println("getUserInfo:retMap", retMap)
    return retMap
}
