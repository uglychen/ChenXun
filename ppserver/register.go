package main

import (
    "database/sql"
    "encoding/json"
    "fmt"
    _ "github.com/go-sql-driver/mysql"
    "io/ioutil"
    "log"
    "net/http"
    "time"
)

type registerData struct {
    OpenId   string
    NickName string
    HeadUrl  string
    Gender   byte
    Age      string
}

type retLoginJson struct {
    OpenId     string
    NickName   string
    HeadUrl    string
    Gender     byte
    Age        string
    TaskStatus int
}

type retRgisterData struct {
    Code    int    `json:"code"`
    UserId  int    `json:"userId"`
    Message string `json:"message"`
}

func UserRegister(w http.ResponseWriter, req *http.Request) {

    if req.Method == "POST" {
        result, _ := ioutil.ReadAll(req.Body)
        req.Body.Close()
        log.Println("recv body:", string(result))

        var m registerData
        var userId int
        var nickName string
        json.Unmarshal([]byte(result), &m)
        log.Println(m)

        rows1, err := Db.Query("SELECT userId, nickName FROM userInfo where wechatId = ?", m.OpenId)
        if err != nil {
            log.Println(err)
        }
        defer rows1.Close()

        var flag bool = false
        if rows1.Next() {
            err := rows1.Scan(&userId, &nickName)
            if err != nil {
                log.Println("error info:", err)
            }
            log.Println("userId:", userId, "nickName:", nickName)
            flag = true
        }
        //flag := rows1.Next()
        log.Println("用户请求的openId是否已经注册过了：", flag)
        if flag {
            log.Println("userId:", userId, "nickName:", nickName)
            var retValue retRgisterData
            retValue.Code = 300
            retValue.UserId = userId
            retValue.Message = "have been registered"
            bytes, _ := json.Marshal(retValue)
            fmt.Fprintln(w, string(bytes)) //返回json
        } else {
            dataMap := make(map[string]interface{})
            dataMap["openId"] = m.OpenId
            dataMap["nickName"] = m.NickName
            dataMap["headUrl"] = m.HeadUrl
            dataMap["gender"] = m.Gender
            dataMap["age"] = m.Age
            dataMap["gold"] = 0
            dataMap["cash"] = 0
            dataMap["signCount"] = 0

            sql := `INSERT INTO userInfo(wechatId, nickName, headUrl, gender, age, gold, cash)
                   VALUES (?, ?, ?, ?, ?, ?, ?)`

            _, err1 := Db.Exec(sql, dataMap["openId"].(string), dataMap["nickName"].(string), dataMap["headUrl"],
                dataMap["gender"], dataMap["age"].(string), 0, 0)

            log.Println(sql)
            log.Println("error info:", err1)

            //查找userId
            rows2, err2 := Db.Query("SELECT userId, nickName FROM userInfo where wechatId = ?", m.OpenId)
            if err != nil {
                log.Println(err2)
            }
            defer rows2.Close()
            for rows2.Next() {
                err := rows2.Scan(&userId, &nickName)
                if err != nil {
                    log.Println("error info:", err)
                }
                log.Println("userId:", userId, "nickName:", nickName)
            }

            retValue := NewBaseJsonData()
            dataMap["userId"] = userId
            retValue.Code = 200
            retValue.Message = "success"
            retValue.Data = dataMap
            bytes, _ := json.Marshal(retValue)
            fmt.Fprintln(w, string(bytes)) //返回json

            //注册的时候创建一次性任务
            taskMap := getTaskInfoTable("one")
            createEvent(userId, taskMap, Db)
            //注册送2金币，此时要修改userInfo表和gold_cash表
            registerEvent(userId, Db)
        }
    } else {
        var retValue = `{"code":400,"message":"failed"}`
        fmt.Fprintln(w, retValue)
    }
}

func registerEvent(userId int, db *sql.DB) {

    taskId := 1
    //先在eventTable中查找eventId
    var eventId int
    var addGoldCoin int
    var addCash float32

    tx, _ := db.Begin()
    str_sql := `select eventId, addGoldCoin, addCash from eventTable where userId = ? and 
    taskId = ?`
    rows, err1 := tx.Query(str_sql, userId, taskId)
    if err1 != nil {
        log.Println("registerEvent error info:", err1)
    }
    defer rows.Close()
    for rows.Next() {
        err2 := rows.Scan(&eventId, &addGoldCoin, &addCash)
        if err2 != nil {
            log.Println("error info:", err2)
        }
        log.Println("taskId:", taskId, "addGoldCoin:", addGoldCoin, "addCash", addCash)
    }

    //eventTime := time.Now().Format("2006-01-02 00:00:00")
    str_sql2 := `insert goldAndCash (userId, taskId, addGoldCoin, addCash, eventId)
     values (?, ?, ?, ?, ?)`
    _, err3 := tx.Exec(str_sql2, userId, taskId, addGoldCoin, addCash, eventId)
    if err3 != nil {
        log.Println("error info:", err3)
    }

    //修改userinfo表和gold——cash表
    date := time.Now().Format("2006-01-02 00:00:00")
    _, err4 := tx.Exec("update userInfo set cash = cash + ? where userId =?", addCash, userId)
    if err4 != nil {
        log.Println("registerEvent error info:", err4)
    }

    //修改eventTable表的事件状态
    _, err5 := tx.Exec("update eventTable set taskStatus = ? where date = ? and userId=? and taskId=?",
        1, date, userId, taskId)
    if err5 != nil {
        log.Println("registerEvent error info:", err5)
    }
    tx.Commit()
}

func LoginAndOut(w http.ResponseWriter, req *http.Request) {

    req.ParseForm()
    param_type, _ := req.Form["type"]
    reqType := param_type[0]

    //用户注册
    if reqType == "register" {
        UserRegister(w, req)
    }

    //用户登录接口
    if reqType == "login" {
        UserLogin(w, req)
    }
}
