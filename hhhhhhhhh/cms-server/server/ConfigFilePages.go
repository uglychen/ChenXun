package server

import (
	"encoding/json"
	"github.com/golang/glog"
	"io/ioutil"
	"log"
	"net/http"
	"time"
)

type reqConfigFilePages struct{
	State 		int
	PageIndex   int
	PageSize 	int
	Token 		string
}

type response struct{
	TotalPage int `json:"totalPage"`
	TotalRecord int `json:"totalRecord"`
	List []*config_file `json:"list"`
}

type config_file struct{

	UpdateTime int64 `json:"updateTime"`
	CreateTime int64 `json:"createTime"`
	VersionCode int64 `json:"versionCode"`
	LoadTime int `json:"loadTime"`
	State int	`json:"state"`
	Id int64	`json:"id"`
	ConfigName string `json:"configName"`
}

func ConfigFilePages(w http.ResponseWriter, req *http.Request){

	/*url   http://192.168.2.153:5678/Config/ConfigFilePages
	{
    	"state":"-1",
    	"pageIndex":1,
    	"pageSize":15
	}
	*/

	//SetHttpHeader(w,req)
	if req.Method == "POST"{

		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePages recv body:", string(result))
		var m reqConfigFilePages
		err := json.Unmarshal([]byte(result), &m)
		if err != nil{
			glog.Info(err)
		}

		Queryconfig, err := Db.Query(`SELECT  updateTime, createTime, versionCode, loadTime,
							state, id, configName  FROM proxypro.config_file order by versionCode desc, state desc`)
		if err != nil {
			log.Println(err)
		}
		defer Queryconfig.Close()

		tmp_response := &response{}
		var tmp_list  []*config_file

		for Queryconfig.Next(){
			var updateTime string
			var createTime string
			var versionCode int64
			var loadTime int
			var state int
			var id int64
			var configName string
			err := Queryconfig.Scan(&updateTime,&createTime,&versionCode,&loadTime,&state,&id,&configName)
			if err != nil {
				log.Println(err)
			}
			tmp_config := &config_file{ }

			timeLayout := "2006-01-02 15:04:05"
			loc, _ := time.LoadLocation("Local")
			t1, _ := time.ParseInLocation(timeLayout,updateTime,loc)
			t2, _ := time.ParseInLocation(timeLayout, createTime,loc)

			tmp_config.UpdateTime = t1.Unix()
			tmp_config.CreateTime = t2.Unix()
			tmp_config.VersionCode = versionCode
			tmp_config.LoadTime = loadTime
			tmp_config.State = state
			tmp_config.Id = id
			tmp_config.ConfigName = configName
			tmp_list = append(tmp_list,tmp_config)
		}

		tmp_response.TotalPage = len(tmp_list)/m.PageSize + 1
		tmp_response.TotalRecord = len(tmp_list)
		glog.Info("tmp_response.totalPage:", tmp_response.TotalPage)
		glog.Info("tmp_response.totalRecord:", tmp_response.TotalRecord)


		if len(tmp_list) < m.PageIndex*m.PageSize{
			tmp_response.List = tmp_list[(m.PageIndex-1)*m.PageSize:]
		}else{
			tmp_response.List = tmp_list[(m.PageIndex-1)*m.PageSize:(m.PageIndex)*m.PageSize]
		}
		glog.Info("tmp_response  tmp_list:", tmp_list)

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = tmp_response
		retValue.Message = "Success"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}





