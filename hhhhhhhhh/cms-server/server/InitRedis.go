package server

import (
	"github.com/garyburd/redigo/redis"
)

var RedisPool redis.Pool
func init()  {      //init 用于初始化一些参数，先于main执行\

	redis_address :=getRedisMasterAddress()

	RedisPool = redis.Pool{
		MaxIdle:     5,
		MaxActive:   5,
		IdleTimeout: 30,
		Dial: func() (redis.Conn, error) {
			return redis.Dial("tcp", redis_address)
		},
	}
}
