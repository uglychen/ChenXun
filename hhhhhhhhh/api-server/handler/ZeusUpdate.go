package handler

import (
	. "api-server/utils"
	"encoding/json"
	"github.com/golang/glog"
	"github.com/gomodule/redigo/redis"
	"net/http"
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
	glog.Info("ZeusUpdate p_platformCode:", p_platformCode)


	w.Header().Set("Content-Type", "application/json;charset=UTF-8")

	//redis缓存
	conn:= NewRedisPool().Get()
	defer conn.Close()
	reply, e := conn.Do("HGET", "ZeusUpdate",p_platformCode)
	if e!=nil {
		w.Write([]byte("redis error"))
		glog.Error(e," conn do ")
		FailRet("网络环境不好，请再次刷新")
		return
	}
	areaData,err := redis.String(reply, e )
	if err!=nil {
		glog.Error("redis转换失败")
	}
	if reply != nil {
		glog.Info("缓存中存在,CacheZeusUpdate",reply)
	} else {
		glog.Error("缓存中不存在 CacheZeusUpdate，需要从数据库中读取")
		//CacheZeusUpdate()
		//reply, e = conn.Do("HGET", "ZeusUpdate",p_platformCode)
		//if e!=nil {
		//	fmt.Println(e," conn do ")
		//}
		//areaData,err = redis.String(reply, e )
	}

	zud:=ZeusUpdateData{}

	e = json.Unmarshal([]byte(string(areaData)), &zud)
	if e!=nil {
		ret := FailRet("json　Marshal出错")
		glog.Error("json　Marshal出错", err)
		w.Write(ret)
		return
	}

	retValue := &ResponseData{}
	retValue.Success = true
	retValue.Code = 0
	retValue.Data = zud
	retValue.Message = ""
	bytes, err := json.Marshal(retValue)
	if err != nil {
		ret := FailRet("json　Marshal出错")
		w.Write(ret)
		glog.Error("json　Marshal出错", err)
		return
	}
	w.Write(bytes)

}


