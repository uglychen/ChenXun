package server

import (
	"encoding/json"
	"github.com/golang/glog"
	"io/ioutil"
	"net/http"
)


type reqEdit struct {
	Id int
	Token string
}

func ConfigFilePagesEnable(w http.ResponseWriter, req *http.Request){

	//SetHttpHeader(w,req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqEdit
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
		}

       //查找最大的versionCode
		var max_versionCode int64
		{
			sql := "SELECT max(versionCode) FROM proxypro.config_file"
			queryversionCode, err:=  Db.Query(sql)
			defer  queryversionCode.Close()
			if err != nil {
				glog.Info(err)
			}

			if queryversionCode.Next(){
				err = queryversionCode.Scan(&max_versionCode)
				if err != nil {
					glog.Info("error info:", err)
				}
			}
		}
		glog.Info("max_versionCode: ",max_versionCode)

		{
			update_sql, err1 := Db.Prepare(`update proxypro.config_file set state = 0 where state = 1`)
			defer update_sql.Close()
			if err != nil {
				glog.Info(err1)
			}
			update, err2 := update_sql.Exec()
			if err != nil {
				glog.Info(err2)
			}
			glog.Info(update)
		}

		//跟新启用的  versionCode
		{
			update_sql, err1 := Db.Prepare(`update proxypro.config_file set versionCode = ?, state= ?  where id = ?;`)
			if err != nil {
				glog.Info(err1)
			}
			_, err2 := update_sql.Exec(max_versionCode + 1, 1, m.Id)
			if err != nil {
				glog.Info(err2)
			}
		}

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "Success"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))

	}else {

		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: GET"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

func ConfigFilePagesDelete(w http.ResponseWriter, req *http.Request){

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesDelete recv body:", string(result))
		var m reqEdit
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
		}

		delete, err1 := Db.Prepare(`delete from proxypro.config_file where id = ?`)
		defer delete.Close()
		if err != nil {
			glog.Info(err1)
		}
		_, err2 := delete.Exec(m.Id)
		if err != nil {
			glog.Info(err2)
			return
		}

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "Success"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

func ConfigFilePagesExport(w http.ResponseWriter, req *http.Request){

	req.ParseForm()
	param_id, _ := req.Form["id"]
	id := param_id[0]
	glog.Info(id)
	param_token, _ := req.Form["token"]
	token := param_token[0]
	glog.Info(token)

	if req.Method == "GET"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqEdit
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
		}

		data_slice := &DataSlice{}
		{
			sql := "SELECT typeCode, configValue FROM proxypro.config_value where idConfigFile = ?"
			Query_value, _ := Db.Query(sql, id)
			if err != nil {
				glog.Info(err)
			}
			defer Query_value.Close()

			var typeCode string
			var configValue string
			for Query_value.Next() {
				err = Query_value.Scan(&typeCode, &configValue)
				if err != nil {
					glog.Info("error info:", err)
				}

				if typeCode == "redirect" {
					data_slice.Redirect = append(data_slice.Redirect, configValue)
				}
				if typeCode == "dns-proxy" {
					data_slice.Dnsproxy = append(data_slice.Dnsproxy, configValue)
				}
			}
		}

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "Success"
		retValue.Data = data_slice
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: GET"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}