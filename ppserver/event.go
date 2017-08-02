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

type userData struct {
    UserId   int
    NickName string
    HeadUrl  string
    Gender   byte
    Age      string
}
type shareJson struct {
    Code int    `json:"code,omitempty"`
    Gold int    `json:"gold,omitempty"`
    Msg  string `json:"message,omitempty"`
}
type retJson struct {
    Code    int    `json:"code,omitempty"`
    Gold    int    `json:"gold,omitempty"`
    Message string `json:"message,omitempty"`
}

func updateUserData(w http.ResponseWriter, req *http.Request) {

    //主要是检查用户年龄age是否更新
    var data userData
    result, _ := ioutil.ReadAll(req.Body)
    req.Body.Close()
    log.Println("updateUserData:", string(result))
    json.Unmarshal([]byte(result), &data)
    userId := strconv.Itoa(data.UserId)
    age := data.Age

    flag := checkUserId(userId, Db)
    log.Println(flag)

    if !flag {
        str := `{"code":400,"message":"userId not in db"}`
        fmt.Fprint(w, str, "\n")
        log.Println("用户未注册userId", userId)
        return
    }

    taskId := 2
    //today_date := time.Now().Format("2006-01-02 00:00:00")
    rows, err := Db.Query(`SELECT taskStatus, eventId, addGoldCoin, addCash FROM eventTable where 
        userId=? and taskId=?`, userId, taskId)
    defer rows.Close()
    if err != nil {
        log.Println("updateUserData---query error info:", err)
        return
    }

    var taskStatus int
    var eventId int
    var addGoldCoin int
    var addCash float32
    if rows.Next() {
        err := rows.Scan(&taskStatus, &eventId, &addGoldCoin, &addCash)
        if err != nil {
            log.Println(err)
        }
        log.Println("get userId, taskId, taskStatus, eventId:", userId, taskId, taskStatus, eventId)
    } else {
        retValue := `{"code":400,"message":"userId no register"}`
        fmt.Fprint(w, retValue, "\n")
        log.Println("db no userId:", userId)
        return
    }

    if taskStatus > 0 {
        retValue := `{"code":300,"message":"user had updated the information"}`
        fmt.Fprint(w, retValue, "\n")
        log.Println("用户更新过资料")
    } else {
        retValue := `{"code":200,"gold":400,"message":"success"}`
        fmt.Fprint(w, retValue, "\n")

        //1、修改userInfo表 2、修改evecntTable 3、插入gold_cash金币零钱明细
        Db.Exec("update userInfo set age = ?, gold = gold + ? where userId = ? ", age, addGoldCoin, userId)
        //修改eventTable表
        Db.Exec("update eventTable set taskStatus=? where userId=? and taskId=? and eventId=?",
            1, userId, taskId, eventId)

        var data2 goldData
        data2.userId = userId
        data2.taskId = taskId
        data2.addGoldCoin = addGoldCoin
        data2.addCash = 0
        data2.eventId = eventId
        goldDataChan <- data2
    }
}

func shareApp(w http.ResponseWriter, req *http.Request) {
    //分享app至微信
    taskId := 4
    req.ParseForm()
    param_userId, _ := req.Form["userId"]
    userId := param_userId[0]

    flag := checkUserId(userId, Db)
    if !flag {
        str := `{"code":400,"message":"userId not in db"}`
        fmt.Fprint(w, str, "\n")
        log.Println("用户未注册userId", userId)
        return
    }

    if flag {
        today_date := time.Now().Format("2006-01-02 00:00:00")
        eventMap := getEventMap(userId, taskId, today_date, Db)
        taskStatus := eventMap["taskStatus"].(int)
        log.Println("tody_shareApp_status:", taskStatus)

        if taskStatus > 0 {
            retValue := `{"code":300,"message":"user  had shared the APP"}`
            fmt.Fprint(w, retValue, "\n")
            log.Println("今天用户已经分享过app了")
        } else {
            var retValue shareJson
            retValue.Code = 200
            retValue.Gold = eventMap["addGoldCoin"].(int)
            retValue.Msg = "success"
            bytes, _ := json.Marshal(retValue)
            fmt.Fprint(w, string(bytes), "\n")

            //1、修改userInfo表 2、修改evecntTable 3、插入gold_cash金币零钱明细
            Db.Exec("update userInfo set gold=gold+? where userId=?", eventMap["addGoldCoin"], userId)

            var data1 eventData
            data1.userId = userId
            data1.taskId = taskId // taskId = 4
            data1.date = today_date
            data1.taskStatus = 1
            eventChan <- data1

            var data2 goldData
            data2.userId = userId
            data2.taskId = taskId
            data2.addGoldCoin = eventMap["addGoldCoin"].(int)
            data2.addCash = 0
            data2.eventId = eventMap["eventId"].(int)
            goldDataChan <- data2
        }
    } else {
        retValue := `{"code":400,"message":"userId not in db"}`
        fmt.Fprint(w, retValue, "\n")
        log.Println("用户未注册userId", userId)
    }
}

func readAward(w http.ResponseWriter, req *http.Request) {

    req.ParseForm()

    param_userId, _ := req.Form["userId"]
    param_type, _ := req.Form["type"]
    userId := param_userId[0]
    tab_type := param_type[0]
    taskId, _ := strconv.Atoi(tab_type)

    if tab_type == "5" || tab_type == "6" || tab_type == "7" || tab_type == "8" {

        flag := checkUserId(userId, Db)
        if !flag {
            str := `{"Code":400,"message":"userId not in db"}`
            fmt.Fprint(w, str, "\n")
            log.Println("用户未注册userId", userId)
            return
        }

        today_date := time.Now().Format("2006-01-02 00:00:00")
        eventMap := getEventMap(userId, taskId, today_date, Db)
        taskStatus := eventMap["taskStatus"].(int)
        log.Println("taskId:", taskId, "tody_read_status:", taskStatus)

        if taskStatus > 0 {
            retValue := `{"Code":300,"message":"user had finish read"}`
            fmt.Fprint(w, retValue, "\n")
            log.Println("今天用户已经完成taskId=", taskId, "的阅读任务")
        } else {
            var retValue retJson
            retValue.Code = 200
            retValue.Gold = eventMap["addGoldCoin"].(int)
            retValue.Message = "success"
            bytes, _ := json.Marshal(retValue)
            fmt.Fprint(w, string(bytes), "\n")

            //1、修改userInfo表 2、修改evecntTable 3、插入gold_cash金币零钱明细
            Db.Exec("update userInfo set gold=gold+? where userId=?", eventMap["addGoldCoin"], userId)

            var data1 eventData
            data1.userId = userId
            data1.taskId = taskId // taskId = 5
            data1.date = today_date
            data1.taskStatus = 1
            eventChan <- data1

            var data2 goldData
            data2.userId = userId
            data2.taskId = taskId
            data2.addGoldCoin = eventMap["addGoldCoin"].(int)
            data2.addCash = 0
            data2.eventId = eventMap["eventId"].(int)
            goldDataChan <- data2
        }
    } else {
        var retValue string = `{"message":"error: event type is not read task" }`
        fmt.Fprint(w, retValue, "\n")
        //return
    }
}
