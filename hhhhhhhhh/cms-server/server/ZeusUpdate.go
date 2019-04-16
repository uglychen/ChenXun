package server

import (
	"encoding/json"
	"github.com/garyburd/redigo/redis"
	"github.com/golang/glog"
	"net/http"
	"runtime"
	"strconv"
	"sync"
)

type ZeusUpdateData struct{
	IsForce int`json:"isForce"`
	PlatformCode int`json:"platformCode"`
	Remark string`json:"remark"`
	ApkAddress string`json:"apkAddress"`
	VersionCode string`json:"versionCode"`
}

/*
		http://192.168.2.153:5678/ConfigFile/ZeusUpdate
*/

func ZeusUpdate(w http.ResponseWriter, req *http.Request){

	req.ParseForm()
	var p_platformCode string
	param_platformCode, flag := req.Form["platformCode"]
	if flag == true{
		p_platformCode  = param_platformCode[0]
	}else{
		p_platformCode = "1"
	}
	//req_version, _ := req.Form["versionCode"]
	//p_versionCode := req_version[0]
	//param_platformCode, _ := req.Form["cid"]
	//cid := param_platformCode[0]
	glog.Info("ZeusUpdate p_platformCode:", p_platformCode)

	ZeusUpdate_mutex.Lock()
	defer ZeusUpdate_mutex.Unlock()

	key,_ := strconv.Atoi(p_platformCode)
	retValue := &ResponseData{}
	retValue.Success = true
	retValue.Code = 0
	retValue.Data = CacheZeusUpdate_map[key]
	glog.Info(retValue.Data)
	retValue.Message = ""
	bytes, _ := json.Marshal(retValue)
	w.Write([]byte(bytes))
}



//添加ZeusUpdate缓存
var CacheZeusUpdate_map map[int]ZeusUpdateData
var ZeusUpdate_mutex sync.Mutex

func CacheZeusUpdate(){

	ZeusUpdate_mutex.Lock()
	defer ZeusUpdate_mutex.Unlock()
	CacheZeusUpdate_map=make(map[int]ZeusUpdateData,0)

	//var p_platformCode string
	row, err := Db.Query(`SELECT isForce, platformCode, remark, apkAddress, versionCode  FROM proxypro.cb_apkupdate`)
	if err != nil {
		glog.Info(err)
	}
	defer row.Close()


	for row.Next() {
		tmp := ZeusUpdateData{}
		var isForce int
		var platformCode int
		var remark string
		var apkAddress string
		var versionCode string

		err1 := row.Scan(&isForce, &platformCode, &remark, &apkAddress, &versionCode)
		if err1 != nil {
			glog.Info(err1)
		}

		tmp.IsForce = isForce
		tmp.PlatformCode = platformCode
		tmp.Remark = remark
		tmp.ApkAddress = apkAddress
		tmp.VersionCode = versionCode
		CacheZeusUpdate_map[platformCode]=tmp

		if platformCode == 1{
			CacheZeusUpdate_map[0]=tmp
		}
	}

	if len(CacheZeusUpdate_map)>0 {
		for key ,value := range CacheZeusUpdate_map{

			bytes, err := json.Marshal(value)
			if err!=nil {
				glog.Info(err," json 序列化失败！")
			}
			glog.Info("json 序列化 CacheZeusUpdate_map=> ", key ,"  ", bytes)
			//缓存到redis
			runtime.Gosched()
			conn,_ := redis.Dial("tcp", getRedisMasterAddress())
			conn.Do("hset", "ZeusUpdate", key, string(bytes))
			conn.Close()
		}
	}
}