package handler

import (
	. "api-server/utils"
	"encoding/json"
	"github.com/golang/glog"
	"github.com/gomodule/redigo/redis"
	"net/http"
)


type WebSiteContentData struct{
	OrderIndex int `json:"orderIndex"`
	Content string`json:"content"`
	Title2 string`json:"title2"`
	Title1 string`json:"title1"`
	Img string`json:"img"`
	PageTitle string`json:"pageTitle"`
}


func WebSiteContent(w http.ResponseWriter, req *http.Request) {

	//redis缓存
	conn:= NewRedisPool().Get()
	defer conn.Close()
	reply, e := conn.Do("GET", "WebSiteContent")
	if e!=nil {
		glog.Info(e," conn do ")
		FailRet("网络环境不好，请再次刷新")
		return
	}

	w.Header().Set("Content-Type", "application/json;charset=UTF-8")

	areaData,err := redis.String(reply, e )
	if err!=nil {
		glog.Error("redis转换失败")
		FailRet("网络环境不好，轻轻新")
		return
	}
	if reply != nil {
		glog.Info("缓存中存在,Cache_WebSiteContent",reply)
	} else {
		glog.Error("缓存中不存在 Cache_WebSiteContent，数据读取错误")
	}

	glog.Info("--------Cache_WebSiteContent,areaData------------------",string(areaData))

	wcd:=[]WebSiteContentData{}

	e=json.Unmarshal([]byte(string(areaData)),&wcd)
	if e!=nil {
		glog.Error("json.Unmarshal错误")
		ret := FailRet("json.Unmarshal错误")
		w.Write(ret)
	}
	glog.Info(wcd)

	retValue := &ResponseData{}
	retValue.Success = true
	retValue.Code = 0
	retValue.Data = wcd
	retValue.Message = ""
	bytes, err := json.Marshal(retValue)
	if err != nil {
		w.Write([]byte("json　Marshal出错"))
		glog.Error("json　Marshal出错", err)
		return
	}
	w.Write(bytes)

}

