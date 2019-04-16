package utils

import (
	"fmt"
	"github.com/golang/glog"
	"github.com/gomodule/redigo/redis"
	"time"
)



var (
	RedisAddress    string
	MasterRedisAddress string
)




const (
	//RedisURL            = "redis://*****:6379"
	redisIdleTimeoutSec = 240 //最大空闲连接时间
)




//NewRedisPool 返回redis连接池
func NewRedisPool() *redis.Pool {
	return &redis.Pool{
		MaxIdle:     RedisMaxIdle,
		IdleTimeout: redisIdleTimeoutSec * time.Second,
		MaxActive:RedisMaxActive,
		Dial: func() (redis.Conn, error) {
			glog.Info("redis://"+RedisAddress)
			c, err := redis.DialURL("redis://"+RedisAddress)
			if err != nil {
				return nil, fmt.Errorf("redis connection error: %s", err)
			}
			return c, err
		},
		TestOnBorrow: func(c redis.Conn, t time.Time) error {
			_, err := c.Do("PING")
			if err != nil {
				return fmt.Errorf("ping redis error: %s", err)
			}
			return nil
		},
	}
}



// NewRedisPool 返回redis连接池
func NewMasterRedisPool() *redis.Pool {
	return &redis.Pool{
		MaxIdle:     RedisMaxIdle,
		IdleTimeout: redisIdleTimeoutSec * time.Second,
		MaxActive:RedisMaxActive,
		Dial: func() (redis.Conn, error) {
			glog.Info("redis://"+RedisAddress)
			c, err := redis.DialURL("redis://"+MasterRedisAddress)
			if err != nil {
				return nil, fmt.Errorf("redis connection error: %s", err)
			}
			return c, err
		},
		TestOnBorrow: func(c redis.Conn, t time.Time) error {
			_, err := c.Do("PING")
			if err != nil {
				return fmt.Errorf("ping redis error: %s", err)
			}
			return nil
		},
	}
}


//////直连　redis
var RedisPool redis.Pool
func NewDirectRedisPool() (redis.Conn,error) {
	conn, err := redis.Dial("tcp", RedisAddress)
	if err != nil {
		glog.Info("connect redis error :", err)
	}
	return conn,err
}


