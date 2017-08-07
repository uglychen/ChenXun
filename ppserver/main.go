package main

import (
    "database/sql"
    "fmt"
    _ "github.com/go-sql-driver/mysql"
    "io"
    "log"
    "net/http"
    "os"
    "time"
)

var dataChain = make(chan map[string]interface{}, 5)
var HandlerMap = make(map[string]HandlersFunc)
var (
    db_ip     = "rds8q6sd63m3voxhtk09.mysql.rds.aliyuncs.com:3306"
    db_user   = "paopao"
    db_passwd = "paoMhxzKhl123"
    //address   = db_user + ":" + db_passwd + "@tcp(" + db_ip + ")/ppserver?charset=utf8"
    //address = "reportadmin:123456@tcp(202.120.1.109:3306)/ppserver?charset=utf8"
    address = "root:Paopao123`@tcp(127.0.0.1:3306)/ppserver?charset=utf8"
    Db      *sql.DB
)

func init() {
    var err error
    Db, err = sql.Open("mysql", address)
    Db.SetMaxIdleConns(10)
    Db.SetMaxOpenConns(10)

    if err != nil {
        log.Println("failed to open database:", err.Error())
        return
    }
    //defer Db.Close()
}

func main() {

    logFile, err := os.OpenFile("./log/"+time.Now().Format("20060102")+".ppserver.log",
        os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0666)
    if err != nil {
        fmt.Println("open file error=", err.Error())
        os.Exit(-1)
    }
    defer logFile.Close()

    writers := []io.Writer{
        logFile,
        os.Stdout,
    }

    fileAndStdoutWriter := io.MultiWriter(writers...)
    log.SetOutput(fileAndStdoutWriter)
    log.SetFlags(log.Lshortfile | log.LstdFlags)

    log.Println("===start the ppserver===.")

    //修改eventTable
    go update_eventTable()

    //gold cash明细
    go saveGoldAndCash()

    //开启handle listen
    InitHandlerMap(HandlerMap)
    SetListenHandle()

    if err := http.ListenAndServe(":9091", nil); err != nil {
        panic(err)
    }

    Db.Close()
}
