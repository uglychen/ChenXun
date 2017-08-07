package main

import (
    "database/sql"
    "encoding/json"
    "fmt"
    _ "github.com/go-sql-driver/mysql"
    "io/ioutil"
    "log"
    "net/http"
    "strconv"
    "time"
)

type withdrawalData struct {
    UserId int
    Alipay string
    Name   string
    Amount int
}

type handlCash struct {
    UserId     int
    Alipay     string
    Name       string
    Amount     int
    CommitTime string
    Status     int
}

type withdrawJson struct {
    Code int
}

type recordData struct {
    UserId     int
    CommitTime string
    Status     int
    Amount     int
}

func getWithddraw(userId string, db *sql.DB) ([](map[string]interface{}), bool) {
    var flag bool = false
    str_sql := "select status, amount,commitTime from withdrawal where userId=?"
    rows, err := db.Query(str_sql, userId)
    if err != nil {
        log.Println("error info:", err)
    }
    defer rows.Close()

    var status int
    var amount int
    var commitTime string

    slice := make([](map[string]interface{}), 0)
    for rows.Next() {

        dataMap := make(map[string]interface{})
        err := rows.Scan(&status, &amount, &commitTime)
        if err != nil {
            log.Println(err)
        }

        t, _ := time.ParseInLocation("2006-01-02 15:04:05", commitTime, time.Local)
        dataMap["userId"], _ = strconv.Atoi(userId)
        dataMap["status"] = status
        dataMap["amount"] = amount
        dataMap["commitTime"] = t.Unix()
        if status == 0 {
            flag = true
        }

        log.Println("getEvent-->dataMap:", dataMap)
        slice = append(slice, dataMap)
    }

    return slice, flag
}

func withdrawal(w http.ResponseWriter, req *http.Request) {

    //提现请求
    retValue := NewBaseJsonData()
    var data withdrawalData
    result, _ := ioutil.ReadAll(req.Body)
    req.Body.Close()
    json.Unmarshal([]byte(result), &data)
    log.Println("withdrawalData:", data)

    userId := strconv.Itoa(data.UserId)
    alipay := data.Alipay
    name := data.Name
    amount := data.Amount

    flag := checkUserId(userId, Db)
    if flag {
        _, b := getWithddraw(userId, Db)
        if b {
            retValue.Code = 300
            retValue.Message = "have one record about withdrawal"
            bytes, _ := json.Marshal(retValue)
            fmt.Fprint(w, string(bytes), "\n")
            log.Println("还有未审核通过的提现申请")
        } else {
            userMap := getUserInfo(userId, Db)
            log.Println("userMap:", userMap)
            log.Println("用户提交提现申请事件")
            total_cash := (int)(userMap["cash"].(float32))
            log.Println("用户拥有的总额cash：", total_cash)
            if amount <= total_cash && amount >= 30 {
                retValue.Code = 200
                retValue.Message = "success"
                bytes, _ := json.Marshal(retValue)
                fmt.Fprint(w, string(bytes), "\n")

                log.Println("在db中插入提现记录")
                tx, _ := Db.Begin()
                _, err1 := tx.Exec(`insert withdrawal(userId, alipay, name, status, amount)
                values(?, ?, ?, ?, ? )`, userId, alipay, name, 0, amount)
                if err1 != nil {
                    log.Println("error info:", err1)
                }

                _, err2 := tx.Exec(`update userInfo set cash = cash- ? where userId = ?`, amount, userId)
                if err2 != nil {
                    log.Println("error info:", err2)
                }

                tx.Commit()
            } else {
                retValue.Code = 400
                retValue.Message = "failed"
                bytes, _ := json.Marshal(retValue)
                fmt.Fprint(w, string(bytes), "\n")
            }
        }
    } else {
        retValue := `{"code":500,"message":"userId not in db"}`
        fmt.Fprint(w, retValue, "\n")
        log.Println("用户未注册userId", userId)
    }
}

func withdrawalRcord(w http.ResponseWriter, req *http.Request) {
    //提现记录接口
    req.ParseForm()
    param_id, _ := req.Form["userId"]
    userId := param_id[0]

    slice, _ := getWithddraw(userId, Db)
    log.Println("提现记录:", slice)
    bytes, _ := json.Marshal(slice)
    fmt.Fprint(w, string(bytes), "\n")
}

func listWithdraw(w http.ResponseWriter, req *http.Request) {

    tody_date := time.Now()
    date := tody_date.AddDate(0, 0, -6).Format("2006-01-02 15:04:05")
    str_sql := `select * from withdrawal where commitTime >= ?`
    var userId int
    var alipay string
    var name string
    var commitTime string
    var amount int
    var status int

    tx, _ := Db.Begin()
    defer tx.Commit()

    rows, err := tx.Query(str_sql, date)
    if err != nil {
        log.Println("error info:", err)
    }
    defer rows.Close()

    slice := make([](map[string]interface{}), 0)
    for rows.Next() {
        dataMap := make(map[string]interface{})
        err := rows.Scan(&userId, &alipay, &name, &commitTime, &status, &amount)
        if err != nil {
            log.Println("error info:", err)
        }
        dataMap["userId"] = userId
        dataMap["alipay"] = alipay
        dataMap["name"] = name
        dataMap["commitTime"] = commitTime
        dataMap["status"] = status
        dataMap["amount"] = amount
        log.Println("listFeedback-->dataMap:", dataMap)
        slice = append(slice, dataMap)
    }

    bytes, _ := json.Marshal(slice)
    fmt.Fprint(w, string(bytes), "\n")
}

func HandleWithdrawal(w http.ResponseWriter, req *http.Request) {

    //处理客户支付宝提现
    log.Println("listen ListWithdrawal ")
    var data handlCash
    result, _ := ioutil.ReadAll(req.Body)
    req.Body.Close()
    json.Unmarshal([]byte(result), &data)
    log.Println("HandleWithdrawal withdrawalData:", data)
    userId := strconv.Itoa(data.UserId)

    flag := checkUserId(userId, Db)
    if !flag {
        str := `{"Code":400,"message:":"userId not in db"}`
        fmt.Fprint(w, str, "\n")
        log.Println("用户未注册userId", data.UserId)
        return
    }

    tx, _ := Db.Begin()
    defer tx.Commit()
    if data.Status != 1 {
        sql_str1 := "update withdrawal set status = ? where userID = ? and commitTime = ?"
        st1, err1 := tx.Exec(sql_str1, 1, userId, data.CommitTime)
        log.Println("db exec result:", st1, " err info:", err1)

        //tx.Exec("update userInfo set cash = cash - ? where userId=?", amount, userId)
        //sql_str2 := "update userInfo set cash= cash- ? where userID = ?"
        //st2, err2 := tx.Exec(sql_str2, data.Amount, userId)
        //log.Println("db exec result:", st2, " err info:", err2)
        if err1 != nil {
            retValue := NewBaseJsonData()
            retValue.Code = 300
            retValue.Data = 0
            retValue.Message = "failed"
            bytes, _ := json.Marshal(retValue)
            fmt.Fprint(w, string(bytes), "\n")
        } else {
            retValue := NewBaseJsonData()
            retValue.Code = 200
            retValue.Data = 0
            retValue.Message = "success"
            bytes, _ := json.Marshal(retValue)
            fmt.Fprint(w, string(bytes), "\n")
        }
    } else {
        retValue := NewBaseJsonData()
        retValue.Code = 600
        retValue.Data = 0
        retValue.Message = "failed"
        bytes, _ := json.Marshal(retValue)
        fmt.Fprint(w, string(bytes), "\n")
    }
}
