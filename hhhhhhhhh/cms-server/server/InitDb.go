package server

import (
	"database/sql"
	"log"

	_ "github.com/go-sql-driver/mysql"
)

var (
	//dbIP       = "127.0.0.1:3306"
	//dbUser     = "root"
	//dbPassword = "123456"
	//DBaddress    = dbUser + ":" + dbPassword + "@tcp(" + dbIP + ")/proxypro?charset=utf8"
	Db         *sql.DB
)

func init() {
	var err error
	DBAddress := getDBAddress()
	Db, err = sql.Open("mysql", DBAddress)
	Db.SetMaxIdleConns(5)
	Db.SetMaxOpenConns(5)

	if err != nil {
		log.Println("failed to open database:", err.Error())
		return
	}
	//defer Db.Close()
}
