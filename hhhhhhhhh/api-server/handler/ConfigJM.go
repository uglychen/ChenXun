package handler

import (
	"api-server/utils"
	"encoding/json"
	"github.com/golang/glog"
	"github.com/gomodule/redigo/redis"
	"net/http"
)

/*
	http://192.168.2.153:5678/ConfigFile/ConfigJM?version=66
*/

type ResponseConfigJM struct {
	Success string      `json:"success"`
	Code    int         `json:"code"`
	Data    interface{} `json:"data"`
	Message string      `json:"message"`
}

type DataSlice struct {
	LoadTime int      `json:"load-time"`
	Redirect []string `json:"redirect"`
	Dnsproxy []string `json:"dns-proxy"`
	Version  int      `json:"version"`
}

//修改后的ConfigJM
func ConfigJM(w http.ResponseWriter, req *http.Request) {

	glog.Info("ConfigJM")

	w.Header().Set("Content-Type", "application/json;charset=UTF-8")

	if req.Method == "GET" {
		req.ParseForm()
		var req_version string
		tmp_version, param_err := req.Form["version"]
		glog.Info(tmp_version)
		if param_err == true {
			req_version = tmp_version[0]
		}

		var req_platformCode string
		tmp_platformCode, param_err := req.Form["platformCode"]
		glog.Info(req_platformCode)
		if param_err == true {
			req_platformCode = tmp_platformCode[0]
		} else {
			req_platformCode = "1"
			//req_version = "0"
		}


		glog.Info("req_version:",req_version)


		//连接池方式
		conn := utils.NewRedisPool().Get()


		// redis 缓存　
		reply, e := conn.Do("HGET", "configJM", req_platformCode)
		if e != nil {
			glog.Info(e, " conn do ")
			FailRet("网络环境不好，请再次刷新")
			return
		}
		areaData, err := redis.String(reply, e)
		if reply != nil {
			glog.Info("缓存中存在,ConfigJM", string(reply.([]uint8)))
			//resp["data"] = areaDat
		} else {
			glog.Info("缓存中不存在 ConfigJM，数据读取错误")
			ret := FailRet("缓存中不存在 ConfigJM，数据读取错误")
			w.Write(ret)
			return

		}
		marshData := make(map[string]interface{}, 0)
		err = json.Unmarshal([]byte(areaData), &marshData)
		if err != nil {
			glog.Info(err, " json.Unmarshal 3333 出错",err)
		}
		configJMReqstatus := marshData["Status"]
		if err != nil {
			glog.Info(err)
		}


		//对字段进行判断
		if configJMReqstatus.(float64) == 0 || marshData["Version"] == req_version {
			retValue := &ResponseData{}
			retValue.Success = true
			retValue.Code = 0
			retValue.Data = ""
			retValue.Message = ""

			//TODO 返回数据
			bytes2, _ := json.Marshal(retValue)
			w.Write([]byte(bytes2))
			return
		}
		encodeString := marshData["Content"]
		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = encodeString
		retValue.Message = ""
		bytes2, _ := json.Marshal(retValue)
		w.Write(bytes2)
	} else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}