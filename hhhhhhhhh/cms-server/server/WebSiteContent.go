package server

import (
	"encoding/json"
	"github.com/garyburd/redigo/redis"
	"github.com/golang/glog"
	"net/http"
	"runtime"
	"sync"
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

	//SELECT * FROM proxypro.cb_officialwebsite;
	retValue := &ResponseData{}
	retValue.Success = true
	retValue.Code = 0
	retValue.Data = cache_WebSiteContent_slice
	retValue.Message = ""
	bytes, _ := json.Marshal(retValue)
	w.Write([]byte(bytes))
}

//  cache_WebSiteContent_slice
var cache_WebSiteContent_slice []interface{}
var WebSiteContent_mutex sync.Mutex

func Cache_WebSiteContent(){
	WebSiteContent_mutex.Lock()
	defer  WebSiteContent_mutex.Unlock()

	row, err := Db.Query(`SELECT orderIndex,content,title2,title1,img,pageTitle FROM proxypro.cb_officialwebsite  order by orderIndex asc;`)
	if err != nil {
		glog.Info(err)
	}
	defer row.Close()

	cache_WebSiteContent_slice = make([]interface{},0)
	for row.Next() {
		var orderIndex int
		var content string
		var title2 string
		var title1 string
		var img string
		var pageTitle string

		err := row.Scan(&orderIndex, &content, &title2, &title1, &img,&pageTitle)
		if err != nil {
			glog.Info(err)
		}
		tmp_WebSiteContentData := &WebSiteContentData{}
		tmp_WebSiteContentData.OrderIndex = orderIndex
		tmp_WebSiteContentData.Content = content
		tmp_WebSiteContentData.Title2 = title2
		tmp_WebSiteContentData.Title1 = title1
		tmp_WebSiteContentData.Img = img
		tmp_WebSiteContentData.PageTitle = pageTitle
		cache_WebSiteContent_slice = append(cache_WebSiteContent_slice, tmp_WebSiteContentData)
	}



	bytes, err := json.Marshal(cache_WebSiteContent_slice)
	if err!=nil {
		glog.Info(err," json 序列化失败！")
	}
	glog.Info("json 序列化 cache_WebSiteContent_slice=> ", bytes)
	runtime.Gosched()
	conn,_ := redis.Dial("tcp", getRedisMasterAddress())
	conn.Do("set", "WebSiteContent", string(bytes))
	conn.Close()
}