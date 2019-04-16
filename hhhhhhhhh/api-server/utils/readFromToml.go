package utils

import (
	. "database/sql"
	. "github.com/BurntSushi/toml"
	_ "github.com/go-sql-driver/mysql"
	"github.com/golang/glog"
)

var(
	Title   string
	RedisServer      string
	RedisPorts   string

	RedisMaxIdle   int  //最大空闲连接数
	RedisMaxActive int

	MasterRedisServer string



	MysqlServer   string
	MysqlPorts      string
	MysqlDbUser   string
	MysqlDbPassword   string
	MysqlDbName   string


	ThisServer string
	ThisPort string
)


type tomlConfig struct {
	Title   string
	RedisServer      string
	RedisPorts   string

	RedisMaxIdle   int  //最大空闲连接数
	RedisMaxActive int

	MasterRedisServer string

	MysqlServer   string
	MysqlPorts      string
	MysqlDbUser   string
	MysqlDbPassword   string
	MysqlDbName   string

	ThisServer string
	ThisPort string
}

func init()  {
	var config tomlConfig
	if _, err := DecodeFile("config.toml", &config); err != nil {
		glog.Error("toml 文件解析失败...",err)

	}

	Title=config.Title

	//redis的配置信息
	RedisServer=config.RedisServer
	RedisPorts=config.RedisPorts

	RedisMaxIdle=config.RedisMaxIdle
	RedisMaxActive=config.RedisMaxActive

	glog.Info("RedisMaxIdle:",RedisMaxIdle," RedisMaxActive:",RedisMaxActive)

	MasterRedisServer=config.MasterRedisServer

	//mysql的配置信息
	MysqlServer=config.MysqlServer
	MysqlPorts=config.MysqlPorts
	MysqlDbUser=config.MysqlDbUser
	MysqlDbPassword=config.MysqlDbPassword
	MysqlDbName=config.MysqlDbName


	ThisServer=config.ThisServer
	ThisPort=config.ThisPort



	//mysql的地址
	MysqlAddress=MysqlDbUser + ":" + MysqlDbPassword + "@tcp(" + MysqlServer+":"+MysqlPorts + ")/"+MysqlDbName+"?charset=utf8"

	//redis的地址
	RedisAddress=RedisServer+":"+RedisPorts

	//master redis的地址
	MasterRedisAddress=MasterRedisServer+":"+RedisPorts


	//当前服务的地址
	ThisAddress=ThisServer+":"+ThisPort
	glog.Info("ThisAddress:",ThisAddress)

	glog.Info("MysqlAddress:",MysqlAddress)
	glog.Info("RedisAddress:",RedisAddress)



	var err error
	Db, err = Open("mysql", MysqlAddress)
	if err!=nil {
		glog.Info("打开Mysql数据库失败...")
	}else {
		glog.Info("打开Mysql数据库success...")
	}

}
