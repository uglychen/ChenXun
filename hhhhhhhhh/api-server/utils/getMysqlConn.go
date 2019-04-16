package utils

import (
	"database/sql"
	_ "github.com/go-sql-driver/mysql"
)


var (
	MysqlAddress    string
	Db         *sql.DB
)

var (
	ThisAddress string
)


