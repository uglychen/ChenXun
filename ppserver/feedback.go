package main

import (
    //"database/sql"
    "encoding/json"
    "fmt"
    _ "github.com/go-sql-driver/mysql"
    "io/ioutil"
    "log"
    "net/http"
    "strconv"
    "time"
)

type feedbackData struct {
    UserId      int
    Android_id  string
    Pkg_name    string
    Contact_way string
    Content     string
    Channel     string
    App_version string
    Os_version  string
}

type retFeedback struct {
    Code    int    `json:"code"`
    Message string `json:"message"`
}

type effectiveJson struct {
    UserId     int
    CommitTime string
}

func feedback(w http.ResponseWriter, req *http.Request) {
    log.Println("feedback http listen")

    var data feedbackData
    result, _ := ioutil.ReadAll(req.Body)
    req.Body.Close()
    str := json.Unmarshal([]byte(result), &data)
    log.Println("feedbackData:", data, str)

    flag := checkUserId(strconv.Itoa(data.UserId), Db)
    if !flag {
        retValue := `{"Code":400,"Msg:":"userId not in Db"}`
        fmt.Fprint(w, retValue, "\n")
        log.Println("用户未注册userId", data.UserId)
        return
    }

    tx, _ := Db.Begin()
    defer tx.Commit()
    var retValue retFeedback
    if data.Content != "" {
        str_sql := `insert feedback(userId, android_id, pkg_name, contact_way, content, channel,
        app_version, os_version, valid) values (?, ?, ?, ?, ?, ?, ?, ?, ?)`

        status, err := tx.Exec(str_sql, data.UserId, data.Android_id, data.Pkg_name, data.Contact_way,
            data.Content, data.Channel, data.App_version, data.Os_version, 0)
        log.Println(status, err)

        retValue.Code = 200
        retValue.Message = "success"
        bytes, _ := json.Marshal(retValue)
        fmt.Fprint(w, string(bytes), "\n")
    } else {
        retValue.Code = 300
        retValue.Message = "failed"
        bytes, _ := json.Marshal(retValue)
        fmt.Fprint(w, string(bytes), "\n")
    }
}

func listFeedback(w http.ResponseWriter, req *http.Request) {
    log.Println("listFeedback http listen")

    retValue := NewBaseJsonData()

    tx, _ := Db.Begin()
    defer tx.Commit()

    var userId int
    var android_id string
    var pkg_name string
    var contact_way string
    var content string
    var channel string
    var commitTime string
    var app_version string
    var os_version string

    tody_date := time.Now()
    date := tody_date.AddDate(0, 0, -6).Format("2006-01-02 15:04:05")
    str_sql := `select userId, android_id, pkg_name, contact_way, content,
                        channel, commitTime, app_version, os_version 
                        from feedback where commitTime >= ? and valid = '0'`

    rows, err := tx.Query(str_sql, date)
    if err != nil {
        log.Println("error info:", err)
    }
    defer rows.Close()

    slice := make([](map[string]interface{}), 0)
    for rows.Next() {
        dataMap := make(map[string]interface{})
        err := rows.Scan(&userId, &android_id, &pkg_name, &contact_way, &content,
            &channel, &commitTime, &app_version, &os_version)
        if err != nil {
            log.Println(err)
        }
        dataMap["userId"] = userId
        dataMap["android_id"] = android_id
        dataMap["pkg_name"] = pkg_name
        dataMap["contact_way"] = contact_way
        dataMap["content"] = content
        dataMap["channel"] = channel
        dataMap["commitTime"] = commitTime
        dataMap["app_version"] = app_version
        dataMap["os_version"] = os_version

        log.Println("listFeedback-->dataMap:", dataMap)
        slice = append(slice, dataMap)
    }

    retValue.Code = 200
    retValue.Data = slice
    retValue.Message = "success"
    bytes, _ := json.Marshal(retValue)
    fmt.Fprint(w, string(bytes), "\n")
}

func effectiveFeedback(w http.ResponseWriter, req *http.Request) {

    log.Println("listen effectiveFeedback ")
    var data effectiveJson
    result, _ := ioutil.ReadAll(req.Body)
    req.Body.Close()
    str := json.Unmarshal([]byte(result), &data)
    log.Println("effectiveFeedback:", data, str)

    userId := strconv.Itoa(data.UserId)
    commitTime := data.CommitTime
    taskId := 9

    flag := checkUserId(userId, Db)
    if !flag {
        str := `{"Code":400,"Msg:":"userId not in db"}`
        fmt.Fprint(w, str, "\n")
        log.Println("用户未注册userId", data.UserId)
        return
    }

    var retValue retFeedback
    date := time.Now().Format("2006-01-02 00:00:00")
    eventMap := getEventMap(userId, taskId, date, Db)

    //检查有效的返回是否已近有效了
    var valid int
    str_sql := `select valid from feedback where userId=? and commitTime=?`
    rows, err1 := Db.Query(str_sql, userId, commitTime)
    if err1 != nil {
        log.Println("error info:", err1)
    }
    defer rows.Close()

    if rows.Next() {
        rows.Scan(&valid)
        log.Println("effectiveFeedback valid:", valid)
    } else {
        retValue.Code = 500
        retValue.Message = "db no record"
        bytes, _ := json.Marshal(retValue)
        fmt.Fprint(w, string(bytes), "\n")
        log.Println("db 中不存在此条用户反馈")
        return
    }

    if valid > 0 {
        retValue.Code = 600
        retValue.Message = "have already done"
        bytes, _ := json.Marshal(retValue)
        fmt.Fprint(w, string(bytes), "\n")
        log.Println("用户反馈已经处理过了")
        return
    }

    tx, _ := Db.Begin()
    _, err2 := tx.Exec("update feedback set valid = 1 where userId=? and commitTime=?", userId, commitTime)
    if err2 != nil {
        log.Println("error info:", err2)
    } else {
        tx.Exec("update userInfo set gold=gold+? where userId=?", eventMap["addGoldCoin"], userId)
        var data2 goldData
        data2.userId = userId
        data2.taskId = taskId
        data2.addGoldCoin = eventMap["addGoldCoin"].(int)
        data2.addCash = 0
        data2.eventId = eventMap["eventId"].(int)
        goldDataChan <- data2

        retValue := NewBaseJsonData()
        retValue.Code = 200
        retValue.Message = "success"
        bytes, _ := json.Marshal(retValue)
        fmt.Fprint(w, string(bytes), "\n")
    }

    tx.Commit()
}
