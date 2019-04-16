package server

import (
	"encoding/json"
	"fmt"
	"github.com/golang/glog"
	"net/http"
	"sync"
)

type BaseJsonData struct {
	Message string      `json:"message,omitempty"`
	Code    int         `json:"code"`
	Success bool		`json:"success"`
	Data    interface{} `json:"data,omitempty"`
}

type ResponseData struct {
	Success 	bool 			`json:"success"`
	Code		int				`json:"code"`
	Data		interface{}		`json:"data"`
	Message 	string			`json:"message"`
}

func NewBaseJsonData() *BaseJsonData {
	return &BaseJsonData{}
}

func CheckError(err error){
	if err !=nil{
		fmt.Println("error info: ", err)
	}
}

func SetHttpHeader(w http.ResponseWriter, req *http.Request){

	Origin := req.Header.Get("Origin")
	if Origin != "" {
		w.Header().Add("Access-Control-Allow-Origin", Origin)
		w.Header().Add("Access-Control-Allow-Methods", "POST,GET,OPTIONS,DELETE")
		w.Header().Add("Access-Control-Allow-Headers", "x-requested-with,content-type")
		w.Header().Add("Access-Control-Allow-Credentials", "true")
	}
}

func retError(info interface{}, w http.ResponseWriter){
	retValue := &ResponseData{}
	retValue.Success = false
	retValue.Code = 0
	retValue.Message = "failed"
	retValue.Data = info
	bytes, _ := json.Marshal(retValue)
	w.Write([]byte(bytes))
}

func AddVsersion(platform int){

	glog.Info("start AddVsersion")

	if  platform > 0{
		stmt, err := Db.Prepare(`update proxypro.config_version set version_code = version_code+1 where  platform = ? `)
		res, err := stmt.Exec(platform)


		{
			go CacheConfigJM()
		}

		if err != nil {
			glog.Info(res)
			glog.Info(err)
			return
		}
		stmt.Close()
	}else{
		stmt, err := Db.Prepare(`update proxypro.config_version set version_code = version_code+1`)
		res, err := stmt.Exec()
		if err != nil {
			glog.Info(res)
			glog.Info(err)
			return
		}
		stmt.Close()
	}
}

var mutex sync.Mutex
var WebConfig_tmp_map_slice [](map[string]string)

func SaveWebConfig(){
	mutex.Lock()
	defer mutex.Unlock()
	glog.Info("==========================SaveWebConfig=========================")
	tmp_map_slice := make([](map[string]string),0)
	{
		Queryconfig, err := Db.Query(`SELECT companyNameID, subcompany, redirectUrl, DNS, platform, createTime  
											FROM proxypro.webConfig where estatus = 1;`)
		if err != nil {
			glog.Info(err)
		}

		for Queryconfig.Next(){
			var companyNameID string
			var subcompany string
			var redirectUrl string
			var DNS string
			var platform string
			var createTime string
			err := Queryconfig.Scan(&companyNameID, &subcompany, &redirectUrl, &DNS, &platform, &createTime)
			if err!=nil{
				glog.Info(err)
			}

			tmp_map := make(map[string]string,0)
			tmp_map["companyNameID"] = companyNameID
			tmp_map["subcompany"] = subcompany
			tmp_map["redirectUrl"] = redirectUrl
			tmp_map["DNS"] = DNS
			tmp_map["platform"] = platform
			tmp_map["createTime"] = createTime
			tmp_map_slice = append(tmp_map_slice, tmp_map)
		}
		Queryconfig.Close()
	}

	WebConfig_tmp_map_slice = make([](map[string]string),0)
	WebConfig_tmp_map_slice = tmp_map_slice
	glog.Info("WebConfig_tmp_map_slice")
}

