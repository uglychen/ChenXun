package main

import (
    // "database/sql"
    "encoding/json"
    "fmt"
    _ "github.com/go-sql-driver/mysql"
    "log"
    "net/http"
    "strconv"
    "time"
)

func getTaskStatus(w http.ResponseWriter, req *http.Request) {

    //获取每日任务状态
    req.ParseForm()
    param_userId, _ := req.Form["userId"]
    userId := param_userId[0]

    flag := checkUserId(userId, Db)
    if flag {
        today_date := time.Now().Format("2006-01-02 00:00:00")
        str_sql := `SELECT eventTable.taskId, taskInfoTable.taskName, eventTable.taskStatus, 
            date, eventId, eventTable.addGoldCoin, eventTable.addCash 
            FROM eventTable LEFT JOIN taskInfoTable ON  taskInfoTable.taskId = eventTable.taskId 
            WHERE userId=? and date=?`

        rows, err := Db.Query(str_sql, userId, today_date)
        if err != nil {
            log.Println("getTaskStatus error info:", err)
        }
        defer rows.Close()

        var taskId int
        var taskName string
        var taskStatus int
        var eventId int
        var addGoldCoin int
        var addCash float32
        var tmp_date string

        slice := make([](map[string]interface{}), 0)
        for rows.Next() {
            dataMap := make(map[string]interface{})

            err := rows.Scan(&taskId, &taskName, &taskStatus, &tmp_date, &eventId, &addGoldCoin, &addCash)
            if err != nil {
                log.Println("error info:", err)
            }

            t, _ := time.ParseInLocation("2006-01-02 00:00:00", tmp_date, time.Local)
            dataMap["userId"], _ = strconv.Atoi(userId)
            dataMap["taskId"] = taskId
            dataMap["taskNmae"] = taskName
            dataMap["date"] = t.Unix()
            if taskStatus > 0 {
                dataMap["taskStatus"] = 1
            } else {
                dataMap["taskStatus"] = 0
            }
            log.Println("getEvent-->dataMap:", dataMap)
            slice = append(slice, dataMap)
        }

        //查找任务taskId 1 2的任务
        str_sql1 := `SELECT eventTable.taskId, taskInfoTable.taskName, eventTable.taskStatus, 
                    date, eventId, eventTable.addGoldCoin, eventTable.addCash 
                        FROM eventTable LEFT JOIN taskInfoTable ON 
                        taskInfoTable.taskId = eventTable.taskId where userId=? and eventTable.taskId =?`

        rows1, err1 := Db.Query(str_sql1, userId, 1)
        if err1 != nil {
            log.Println("error info:", err1)
        }
        defer rows1.Close()

        if rows1.Next() {
            dataMap := make(map[string]interface{})
            err := rows1.Scan(&taskId, &taskName, &taskStatus, &tmp_date, &eventId, &addGoldCoin, &addCash)
            if err != nil {
                log.Println("error info:", err)
            }
            t, _ := time.ParseInLocation("2006-01-02 00:00:00", tmp_date, time.Local)
            dataMap["userId"], _ = strconv.Atoi(userId)
            dataMap["taskId"] = 1
            dataMap["taskNmae"] = taskName
            dataMap["date"] = t.Unix()
            if taskStatus > 0 {
                dataMap["taskStatus"] = 1
            } else {
                dataMap["taskStatus"] = 0
            }
            log.Println("getEvent-->dataMap:", dataMap)
            slice = append(slice, dataMap)
        }

        str_sql2 := `SELECT eventTable.taskId, taskInfoTable.taskName, eventTable.taskStatus, 
                    date, eventId, eventTable.addGoldCoin, eventTable.addCash 
                        FROM eventTable LEFT JOIN taskInfoTable ON 
                        taskInfoTable.taskId = eventTable.taskId where userId=? and eventTable.taskId =?`

        rows2, err2 := Db.Query(str_sql2, userId, 2)
        if err2 != nil {
            log.Println("error info:", err2)
        }
        defer rows2.Close()

        if rows2.Next() {
            dataMap := make(map[string]interface{})
            err := rows2.Scan(&taskId, &taskName, &taskStatus, &tmp_date, &eventId, &addGoldCoin, &addCash)
            if err != nil {
                log.Println("error info:", err)
            }

            t, _ := time.ParseInLocation("2006-01-02 00:00:00", tmp_date, time.Local)
            dataMap["userId"], _ = strconv.Atoi(userId)
            dataMap["taskId"] = 2
            dataMap["taskNmae"] = taskName
            dataMap["date"] = t.Unix()
            if taskStatus > 0 {
                dataMap["taskStatus"] = 1
            } else {
                dataMap["taskStatus"] = 0
            }

            log.Println("getEvent-->dataMap:", dataMap)
            slice = append(slice, dataMap)
        }

        log.Println("slice len:", len(slice))
        bytes, _ := json.Marshal(slice)
        fmt.Fprint(w, string(bytes), "\n")
    } else {
        retValue := `{"Code":400,"Msg:":"userId not in db"}`
        fmt.Fprint(w, retValue, "\n")
        log.Println("用户未注册userId", userId)
    }
}
