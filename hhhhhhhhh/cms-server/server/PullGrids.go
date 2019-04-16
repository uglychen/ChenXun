package server

import (
	"encoding/json"
	"fmt"
	"github.com/garyburd/redigo/redis"
	"github.com/golang/glog"
	"io/ioutil"
	"net/http"
	"runtime"
	"strings"
	"sync"
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

	if req.Method == "GET"{

		/*QueryUser, err := Db.Query(`SELECT orderIndex, id, url, title, imgUrl FROM proxypro.cb_home where imgUrl != "" order by orderIndex asc`)
		if err != nil {
			glog.Info(err)
		}
		defer QueryUser.Close()

		tmp_slice := make([]*reqPullGridsData,0)
		for QueryUser.Next() {
			var orderIndex int
			var id	int
			var url string
			var title string
			var imgUrl string
			err := QueryUser.Scan(&orderIndex, &id, &url, &title, &imgUrl)
			if err != nil {
				glog.Info(err)
			}

			tmp:=&reqPullGridsData{}
			tmp.OrderIndex = orderIndex
			tmp.Id = id
			tmp.Url = url
			tmp.Title = title
			tmp.Img = imgUrl
			//tmp.ImgUrl = imgUrl
			tmp_slice = append(tmp_slice, tmp)
		}*/

		retValue := &BaseJsonData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = ""
		retValue.Data = CachePullGrids_slice
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &BaseJsonData{}
		retValue.Success = false
		retValue.Code = 1
		retValue.Message = ""
		retValue.Data = "please use get"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

var CachePullGrids_slice []*reqPullGridsData
var PullGrids_mutex sync.Mutex

func CachePullGrids(){
	PullGrids_mutex.Lock()
	defer  PullGrids_mutex.Unlock()
	QueryUser, err := Db.Query(`SELECT orderIndex, id, url, title, imgUrl FROM proxypro.cb_home where imgUrl != "" and state = 1 order by orderIndex asc`)
	if err != nil {
		glog.Info(err)
	}
	defer QueryUser.Close()

	CachePullGrids_slice = make([]*reqPullGridsData,0)
	for QueryUser.Next() {
		var orderIndex int
		var id	int
		var url string
		var title string
		var imgUrl string
		err := QueryUser.Scan(&orderIndex, &id, &url, &title, &imgUrl)
		if err != nil {
			glog.Info(err)
		}

		tmp:=&reqPullGridsData{}
		tmp.OrderIndex = orderIndex
		tmp.Id = id
		tmp.Url = url
		tmp.Title = title
		tmp.Img = imgUrl
		//tmp.ImgUrl = imgUrl
		CachePullGrids_slice = append(CachePullGrids_slice, tmp)
	}

	bytes, err := json.Marshal(CachePullGrids_slice)
	if err!=nil {
		glog.Info(err," json 序列化失败！")
	}
	glog.Info("json 序列化 CachePullGrids_slice=> ",bytes)
	runtime.Gosched()
	conn,_ := redis.Dial("tcp", getRedisMasterAddress())
	conn.Do("set", "PullGrids", string(bytes))
	conn.Close()
}


// TODO 新增修改目录
func  HomeOrderIndex(w http.ResponseWriter, req *http.Request){

	//SetHttpHeader(w,req)

	if req.Method == "POST"{

		//解析请求字符串
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("HomeOrderIndex recv body:", string(result))
		//var m reqConfigEidt
		//err := json.Unmarshal([]byte(result), &m)
		//if err != nil {
		//	glog.Info(err)
		//	retError(err, w)
		//	return
		//}

		var m map[string]string

		parseErr := json.Unmarshal(result, &m)
		if parseErr!=nil {
			fmt.Println(parseErr)
			retError(parseErr, w)
			return
		}

		reqStr:=m["OrderIndexs"]
		fmt.Println(`m["OrderIndex"]`,m["OrderIndex"],m["OrderIndex"]=="")
		if reqStr=="" {
			retValue := &ResponseData{}
			retValue.Success =false
			retValue.Code = 0
			retValue.Data = ""
			retValue.Message = "处理失败，参数格式不对,参数是以OrderIndexs为键"
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}

		split := strings.Split(reqStr, "|")
		for _,v:=range  split {

			idAndIndex := strings.Split(v, ",")
			if(len(idAndIndex)!=2){
				retValue := &ResponseData{}
				retValue.Success =false
				retValue.Code = 0
				retValue.Data = ""
				retValue.Message = "处理失败，参数格式不对"
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
			id:=idAndIndex[0]
			index:=idAndIndex[1]
			stmt, err1 := Db.Prepare(`update proxypro.cb_home set orderIndex=?  where id=? ;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(index, id)
			if err2!=nil {
				fmt.Println(err2)
			}
			glog.Info(err2)
			defer stmt.Close()
			ret, err3 := res.RowsAffected()
			glog.Info("RowsAffected: ", ret, "  ", err3)

		}

		//QueryUser, err := Db.Query(`SELECT orderIndex, id, url,title,imgUrl FROM proxypro.cb_home where img != "" order by orderIndex asc`)
		//if err != nil {
		//	glog.Info(err)
		//}
		//defer QueryUser.Close()
		//
		//tmp_slice := make([]*reqPullGridsData,0)
		//for QueryUser.Next() {
		//	var orderIndex int
		//	var id	int
		//	var url string
		//	var title string
		//	var imgUrl string
		//	err := QueryUser.Scan(&orderIndex, &id, &url, &title, &imgUrl)
		//	if err != nil {
		//		glog.Info(err)
		//	}
		//
		//	tmp:=&reqPullGridsData{}
		//	tmp.OrderIndex = orderIndex
		//	tmp.Id = id
		//	tmp.Url = url
		//	tmp.Title = title
		//	tmp.Img = imgUrl
		//	//tmp.ImgUrl = imgUrl
		//	tmp_slice = append(tmp_slice, tmp)
		//}

		go CachePullGrids()

		retValue := &ResponseData{}
		retValue.Success =true
		retValue.Code = 0
		retValue.Data = ""
		retValue.Message = "处理成功"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success =false
		retValue.Code = 0
		retValue.Data = ""
		retValue.Message = "处理失败，不是POST请求"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}