package handler

import (
	. "api-server/utils"
	"encoding/json"
	"fmt"
	"github.com/golang/glog"
	"github.com/gomodule/redigo/redis"
	"net/http"
)

type siteItemsBoyd struct {
	SiteItems []*siteItems `json:"siteItems"`
	TypeName string `json:"typeName"`
}

type  siteItems struct{
	SiteImg string   `json:"siteImg"`
	SiteUrl string     `json:"siteUrl"`
	Introduction string  `json:"introduction"`
	SiteName string	 `json:"siteName"`
}

func PullSites(w http.ResponseWriter, req *http.Request) {

	glog.Info("--------------PullSites----------------")
	//glog.Info(" http://192.168.2.153:5678/ConfigFile/PullSites")
	//redis缓存
	conn:= NewRedisPool().Get()
	//defer conn.Close()

	reply, e := conn.Do("GET", "PullSites")
	if e!=nil {
		glog.Error(e," conn do ")
		FailRet("网络环境不好，请再次刷新")
		return
	}

	w.Header().Set("Content-Type", "application/json;charset=UTF-8")

	areaData,err := redis.String(reply, e )
	if err!=nil {
		glog.Error("redis转换失败")
		return
	}
	if reply != nil {
		glog.Info("缓存中存在,PullSites",reply)
	} else {
		glog.Error("缓存中不存在 PullSites，redis数据错误",err)
		bytes := FailRet("缓存中不存在 PullSites，redis数据错误")
		w.Write(bytes)
		glog.Error("json　Marshal出错", err)
		return
	}

	glog.Info("--------PullGrids,areaData---111---------------", areaData)
	fmt.Println("--------PullGrids,areaData---111---------------", areaData)

	sib:= []*siteItemsBoyd{}

	e = json.Unmarshal([]byte(areaData), &sib)
	if e!=nil {
		glog.Error("json解析出错：",e)
		ret := FailRet("json解析出错")
		w.Write(ret)
		return
	}


	//sibTmps:= []*siteItemsBoyd{}
	//sibTmp:= siteItemsBoyd{}

	for _,value := range sib{
		glog.Info(value.TypeName)
		for _,v:=range value.SiteItems{
			glog.Info(v)
		}
	}



	retValue := &ResponseData{}
	retValue.Success = true
	retValue.Code = 0
	retValue.Data = sib
	retValue.Message = ""
	bytes, err := json.Marshal(retValue)
	if err != nil {
		w.Write([]byte("json　Marshal出错"))
		glog.Error("json　Marshal出错", err)
		return
	}
	w.Write(bytes)
}

func FailRet(message string) ([]byte) {
	retValue := &ResponseData{}
	retValue.Success = false
	retValue.Code = 1
	retValue.Data = ""
	retValue.Message = message
	bytes, _ := json.Marshal(retValue)
	return bytes
}