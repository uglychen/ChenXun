package handler

import (
	. "api-server/utils"
	"encoding/json"
	"github.com/golang/glog"
	"github.com/gomodule/redigo/redis"
	"net/http"
)

/*
http://192.168.2.153:5678/ConfigFile/PullGrids
*/

type reqPullGridsData struct{
	OrderIndex int   `json:"orderIndex"`
	Id	int 		`json:"id"`
	Img string 	`json:"img"`
	Url string `json:"url"`
	//ImgUrl string`json:"imgUrl"`
	Title string `json:"title"`
}

func  PullGrids(w http.ResponseWriter, req *http.Request){

	glog.Info("start PullGrids api")
	//SetHttpHeader(w,req)

	w.Header().Set("Content-Type", "application/json;charset=UTF-8")

	if req.Method == "GET"{

		req.ParseForm()
		var platformCode string
		tmp_platformCode, param_err := req.Form["platformCode"]
		glog.Info(platformCode)
		if param_err == true {
			platformCode = tmp_platformCode[0]
		}

		if platformCode == "2"{
			s := make([]interface{},0)
			retValue := &ResponseData{}
			retValue.Success =true
			retValue.Code = 0
			retValue.Data = s
			retValue.Message = ""
			bytes, err := json.Marshal(retValue)
			if err!=nil {
				glog.Error("json　Marshal出错",err)
			}
			w.Write(bytes)
			return
		}

		//存入缓存
		conn:= NewRedisPool().Get()
		//defer conn.Close()

		reply, e := conn.Do("GET", "PullGrids")
		if e!=nil {
			glog.Info("redis转换失败",e)
			FailRet("网络环境不好，请再次刷新")
			return
		}
		areaData,err := redis.String(reply, e )
		if reply != nil {
			glog.Info("缓存中存在,CachePullGrids",reply)
		} else {
			glog.Error("缓存中不存在 PullGrids，读取错误")
			return
		}

		glog.Info("--------PullGrids,areaData------------------",reply)

		tmp_body := []*reqPullGridsData{}
		err = json.Unmarshal([]byte(areaData), &tmp_body)
		if err!=nil {
			glog.Error(err)
		}
		glog.Info("tmp_body:", tmp_body)


		retValue := &ResponseData{}
		retValue.Success =true
		retValue.Code = 0
		retValue.Data = tmp_body
		retValue.Message = ""
		bytes, err := json.Marshal(retValue)
		if err!=nil {
			glog.Error("json　Marshal出错",err)
		}


		w.Write(bytes)
	}else{

		retValue := &ResponseData{}
		retValue.Success =false
		retValue.Code = 0
		retValue.Data = ""
		retValue.Message = ""
		bytes, err := json.Marshal(retValue)
		if err!=nil {
			glog.Error("json　Marshal出错",err)
		}

		w.Write(bytes)
	}
}


