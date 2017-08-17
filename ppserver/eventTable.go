package main

import (
    "database/sql"
    _ "github.com/go-sql-driver/mysql"
    "log"
    "time"
)

var eventChan = make(chan eventData, 10)

type eventData struct {
    userId     string
    taskId     int
    date       string
    taskStatus int
    //addGoldCoin int
    //addCash     float32
}

func update_eventTable() {

    sql_str := `update eventTable set taskStatus=? where userId = ? and taskId=? and date=?`
    for {
        data := <-eventChan
        log.Println("update_eventTable: recv data from eventChan", data)
        tx, _ := Db.Begin()
        _, err := tx.Exec(sql_str, data.taskStatus, data.userId, data.taskId, data.date)
        if err != nil {
            log.Println("err:", err)
        }
        tx.Commit()
    }
}

func getTaskInfoTable(taskType string) [](map[string]interface{}) {

    var taskId int
    var taskStatus int
    var addGoldCoin int
    var addCash float32

    slice := make([](map[string]interface{}), 0)
    if taskType == "one" {
        str_sql := `select taskId, taskStatus, addGoldCoin, addCash 
        from taskInfoTable where taskType = ?`
        rows, err := Db.Query(str_sql, taskType)
        if err != nil {
            log.Println("exe sql error:", err)
        }
        defer rows.Close()

        for rows.Next() {
            dataMap := make(map[string]interface{}, 0)
            err := rows.Scan(&taskId, &taskStatus, &addGoldCoin, &addCash)
            if err != nil {
                log.Println(err)
            }

            dataMap["taskId"] = taskId
            dataMap["taskType"] = taskType
            dataMap["taskStatus"] = taskStatus
            dataMap["addGoldCoin"] = addGoldCoin
            dataMap["addCash"] = addCash
            slice = append(slice, dataMap)
            log.Println("task info:", dataMap)
        }
    } else if taskType == "two" {
        str_sql := `select taskId, taskStatus, addGoldCoin, addCash 
        from taskInfoTable where taskType = ?`
        rows, err := Db.Query(str_sql, taskType)
        if err != nil {
            log.Println("exe sql error:", err)
        }
        defer rows.Close()

        for rows.Next() {
            dataMap := make(map[string]interface{}, 0)
            err := rows.Scan(&taskId, &taskStatus, &addGoldCoin, &addCash)
            if err != nil {
                log.Println(err)
            }
            dataMap["taskId"] = taskId
            dataMap["taskType"] = taskType
            dataMap["taskStatus"] = taskStatus
            dataMap["addGoldCoin"] = addGoldCoin
            dataMap["addCash"] = addCash
            slice = append(slice, dataMap)
            log.Println("task info:", dataMap)
        }
    }
    log.Println("*************getTaskInfoTable:**************", slice)
    return slice
}

func createEvent(userId int, slice [](map[string]interface{}), db *sql.DB) {
    log.Println("*************createEvent:**************", slice)
    date := time.Now().Format("2006-01-02 00:00:00")
    //注意dataMap中taskStatus和eventTable中的taskStatus意义不同
    tx, _ := db.Begin()
    str_sql := `insert eventTable(userId, taskId, date, taskStatus, addGoldCoin, addCash)
     values(?, ?, ?, ?, ?, ?)`

    for _, dataMap := range slice {
        if dataMap["taskStatus"].(int) == 1 {
            //先查询每日任务是否创建
            rows, err := tx.Query("select eventId from eventTable where userId=? and date=? and taskId=?",
                userId, date, dataMap["taskId"])
            log.Println("error info:", err)
            defer rows.Close()

            flag := rows.Next()
            if flag {
                for rows.Next() {
                }
                log.Println("taskId:", dataMap["taskId"], "have in eventTable")

            } else {

                for rows.Next() {
                }
                tx.Exec(str_sql, userId, dataMap["taskId"], date,
                    0, dataMap["addGoldCoin"], dataMap["addCash"])
            }
        }
    }
    tx.Commit()
}

func getEventMap(userId string, taskId int, date string, db *sql.DB) map[string]interface{} {

    log.Println("call getEventMap")
    eventMap := make(map[string]interface{})
    var taskStatus int
    var eventId int
    var addGoldCoin int
    var addCash float32

    rows, err := db.Query(`SELECT taskStatus, eventId, addGoldCoin, addCash FROM eventTable where 
        userId=? and taskId=? and date=?`, userId, taskId, date)
    if err != nil {
        log.Println("getEventMap---query error info:", err)
    }
    defer rows.Close()

    if rows.Next() {
        err := rows.Scan(&taskStatus, &eventId, &addGoldCoin, &addCash)
        if err != nil {
            log.Println("error info:", err)
        }
        //log.Println("get userId, taskId, date, taskStatus:", userId, taskId, date, taskStatus)
    }
    eventMap["taskStatus"] = taskStatus
    eventMap["eventId"] = eventId
    eventMap["addGoldCoin"] = addGoldCoin
    eventMap["addCash"] = addCash
    log.Println("getEventMap:", eventMap)
    return eventMap
}
