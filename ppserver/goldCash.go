package main

import (
    //"database/sql"
    "encoding/json"
    "fmt"
    _ "github.com/go-sql-driver/mysql"
    "log"
    "net/http"
    //"strconv"
    "time"
)

var goldDataChan = make(chan goldData, 20)

type goldData struct {
    userId      string
    taskId      int
    addGoldCoin int
    addCash     float32
    eventId     int
}

func saveGoldAndCash() {

    str_sql := `insert goldAndCash (userId, taskId, addGoldCoin, addCash, eventId)
     values (?, ?, ?, ?, ?)`

    /*db, err := sql.Open("mysql", address)
      if err != nil {
          log.Println(err)
      }
      defer db.Close()*/

    for {
        data := <-goldDataChan
        log.Println("recv data from goldDataChan", data)
        tx, _ := Db.Begin()
        // today_date := time.Now().Format("2006-01-02 00:00:00")
        _, err := tx.Exec(str_sql, data.userId, data.taskId, data.addGoldCoin, data.addCash, data.eventId)
        if err != nil {
            log.Println("error info:", err)
        }
        tx.Commit()
    }
}

func incomeList(w http.ResponseWriter, req *http.Request) {
    req.ParseForm()
    param_userId, _ := req.Form["userId"]
    userId := param_userId[0]

    flag := checkUserId(userId, Db)
    if flag {
        tx, _ := Db.Begin()

        str_sql := `select goldAndCash.taskId, taskInfoTable.taskName, eventTime, 
                    goldAndCash.addGoldCoin, goldAndCash.addCash  from goldAndCash 
                    left JOIN taskInfoTable ON goldAndCash.taskId = taskInfoTable.taskId 
                    where userId= ? and eventTime >= ?`

        date := time.Now().AddDate(0, 0, -4).Format("2006-01-02 00:00:00")
        rows1, err1 := tx.Query(str_sql, userId, date)
        log.Println("getEvent query err", err1)
        if err1 != nil {
            log.Println(err1)
        }
        defer rows1.Close()
        defer tx.Commit()

        var taskId int
        var taskName string
        var eventTime string
        var addGoldCoin int
        var addCash1 float32

        slice := make([](map[string]interface{}), 0)
        for rows1.Next() {
            dataMap := make(map[string]interface{})
            err := rows1.Scan(&taskId, &taskName, &eventTime, &addGoldCoin, &addCash1)
            if err != nil {
                log.Println(err)
            }

            t1, _ := time.ParseInLocation("2006-01-02 15:04:05", eventTime, time.Local)
            //dataMap["userId"], _ = strconv.Atoi(userId)
            //dataMap["taskId"] = taskId
            dataMap["taskName"] = taskName
            dataMap["eventTime"] = t1.Unix()
            dataMap["addGoldCoin"] = addGoldCoin
            dataMap["addCash"] = addCash1
            log.Println("getEvent-->dataMap:", dataMap)
            slice = append(slice, dataMap)
        }

        //转换记录
        var transTime string
        var reduceGold int
        var addCash2 float32
        var eventId int

        str_sql2 := `select  transTime, reduceGold, addCash, eventId from transTable 
        where userId= ? and transTime >= ?`
        rows2, err2 := tx.Query(str_sql2, userId, date)
        if err2 != nil {
            log.Println("getEvent query error info:", err2)
        }
        defer rows2.Close()

        for rows2.Next() {
            dataMap := make(map[string]interface{})
            err := rows2.Scan(&transTime, &reduceGold, &addCash2, &eventId)
            if err != nil {
                log.Println("getEvent scan error info:", err)
            }

            t2, _ := time.ParseInLocation("2006-01-02 15:04:05", transTime, time.Local)
            //dataMap["userId"], _ = strconv.Atoi(userId)
            dataMap["eventTime"] = t2.Unix()
            dataMap["addGoldCoin"] = reduceGold * (-1)
            dataMap["addCash"] = addCash2
            dataMap["taskName"] = "兑换零钱"
            log.Println("transRecord-->dataMap:", dataMap)
            slice = append(slice, dataMap)
        }

        //提现详细记录
        str_sql3 := `select commitTime, amount from withdrawal where userId= ? and commitTime >= ?`
        rows3, err3 := tx.Query(str_sql3, userId, date)
        if err3 != nil {
            log.Println("getEvent query error info:", err3)
        }
        defer rows2.Close()

        var commitTime string
        var amount float32
        for rows3.Next() {
            dataMap := make(map[string]interface{})
            err := rows3.Scan(&commitTime, &amount)
            if err != nil {
                log.Println("getEvent scan error info:", err)
            }

            t3, _ := time.ParseInLocation("2006-01-02 15:04:05", commitTime, time.Local)
            //dataMap["userId"], _ = strconv.Atoi(userId)
            dataMap["eventTime"] = t3.Unix()
            dataMap["addGoldCoin"] = 0
            dataMap["addCash"] = amount * (-1)
            dataMap["taskName"] = "提现"
            log.Println("withdrawal-->dataMap:", dataMap)
            slice = append(slice, dataMap)
        }

        //返回
        log.Println("slice len:", len(slice))
        bytes, _ := json.Marshal(slice)
        fmt.Fprint(w, string(bytes), "\n")
    } else {
        retValue := `{"code":400,"message:":"userId not in db"}`
        fmt.Fprint(w, retValue, "\n")
        log.Println("用户未注册userId", userId)
    }
}

func transRecord(w http.ResponseWriter, req *http.Request) {
    req.ParseForm()
    param_userId, _ := req.Form["userId"]
    userId := param_userId[0]

    flag := checkUserId(userId, Db)
    if !flag {
        str := `{"code":400,"message":"userId not in Db"}`
        fmt.Fprint(w, str, "\n")
        log.Println("用户未注册userId", userId)
        return
    }

    tx, _ := Db.Begin()
    var transTime string
    var reduceGold int
    var addCash float32
    var eventId int

    str_sql := `select  transTime, reduceGold, addCash, eventId from transTable 
        where userId= ? and transTime >= ?`

    date := time.Now().AddDate(0, 0, -4).Format("2006-01-02 00:00:00")
    rows, err := tx.Query(str_sql, userId, date)
    if err != nil {
        log.Println("getEvent query error info:", err)
    }
    defer rows.Close()
    defer tx.Commit()

    slice := make([](map[string]interface{}), 0)
    for rows.Next() {
        dataMap := make(map[string]interface{})
        err := rows.Scan(&transTime, &reduceGold, &addCash, &eventId)
        if err != nil {
            log.Println("getEvent scan error info:", err)
        }

        t, _ := time.ParseInLocation("2006-01-02 15:04:05", transTime, time.Local)
        //dataMap["userId"], _ = strconv.Atoi(userId)
        dataMap["transTime"] = t.Unix()
        dataMap["reduceGold"] = reduceGold
        dataMap["addCash"] = addCash
        dataMap["eventId"] = eventId
        log.Println("transRecord-->dataMap:", dataMap)
        slice = append(slice, dataMap)
    }

    log.Println("slice len:", len(slice))
    bytes, _ := json.Marshal(slice)
    fmt.Fprint(w, string(bytes), "\n")
}
