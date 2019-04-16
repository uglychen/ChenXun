package server

import (
	"github.com/BurntSushi/toml"
	_ "github.com/go-sql-driver/mysql"
	"github.com/golang/glog"
)

var(
	Title   string

	RedisMaster  string
	//RedisSlave1  string
	//RedisSlave2  string
	RedisPorts 	 string

	MysqlServer   string
	MysqlPorts      string
	MysqlDbUser   string
	MysqlDbPassword   string
	MysqlDbName   string
)

var MysqlAddress string
var RedisMasterAddress string

type tomlConfig struct {
	Title       string

	RedisMaster  string
	RedisSlave1  string
	RedisSlave2  string
	RedisPorts 	 string

	MysqlServer     string
	MysqlPorts      string
	MysqlDbUser     string
	MysqlDbPassword string
	MysqlDbName     string
}

func init()  {
	glog.Info("start initConf")
	var config tomlConfig
	if _, err := toml.DecodeFile("./config.toml", &config); err != nil {
		glog.Info("toml 文件解析失败...", err)
	}

	Title = config.Title

	//redis的配置信息
	RedisMaster = config.RedisMaster
	//RedisSlave1 = config.RedisSlave1
	//RedisSlave2 = config.RedisSlave2
	RedisPorts = config.RedisPorts

	//mysql的配置信息
	MysqlServer = config.MysqlServer
	MysqlPorts = config.MysqlPorts
	MysqlDbUser = config.MysqlDbUser
	MysqlDbPassword = config.MysqlDbPassword
	MysqlDbName = config.MysqlDbName

	//mysql的地址
	MysqlAddress = MysqlDbUser + ":" + MysqlDbPassword + "@tcp(" + MysqlServer + ":" + MysqlPorts + ")/" + MysqlDbName + "?charset=utf8"

	//redis的地址
	RedisMasterAddress = RedisMaster + ":" + RedisPorts
}

func getDBAddress() string{
	return MysqlAddress
}

func getRedisMasterAddress() string{
	return RedisMasterAddress
}
