package server

import (
	"encoding/json"
	"github.com/golang/glog"
	"io/ioutil"
	"log"
	"net/http"
)

type ConfigFilePagesData struct {
	Dns_proxy int `json:"dns_proxy"`
	Redirect int `json:"redirect"`
	Subcompany string `json:"subcompany"`
	CompanyName string `json:"companyName"`
}

type reqConfigFilePagesNew struct {
	Token string  `json:"omitempty`
}

func ConfigFilePagesNew(w http.ResponseWriter, req *http.Request) {

	//SetHttpHeader(w,req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqConfigFilePagesNew
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err,w)
			return
		}

		Queryconfig, err := Db.Query(`SELECT  id, typeCode, configValue, companyName,
			subcompany, platform FROM proxypro.config_value1 where estatus = 1`)
		if err != nil {
			log.Println(err)
		}
		defer Queryconfig.Close()

		data_slice := make([]interface{},0)
		companyName_map := make(map[string]([](map[string]string)),0)

		for Queryconfig.Next(){
			var id string
			var typeCode string
			var configValue string
			var companyName string
			var subcompany string
			var platform string
			err := Queryconfig.Scan(&id, &typeCode, &configValue, &companyName, &subcompany, &platform)
			if err!=nil{
				glog.Info(err)
				//continue
			}

			tmp_map := make(map[string]string,0)
			tmp_map["id"] = id
			tmp_map["typeCode"] = typeCode
			tmp_map["configValue"] = configValue
			tmp_map["companyName"] = companyName
			tmp_map["subcompany"] = subcompany
			tmp_map["platform"] = platform
			glog.Info(typeCode)
			if   _, ok := companyName_map[companyName]; ok{
				companyName_map[companyName] = append(companyName_map[companyName], tmp_map)
			}else{
				tmp_slice := make([]map[string]string,0)
				tmp_slice = append(tmp_slice, tmp_map)
				companyName_map[companyName] = tmp_slice
			}
		}
		data_slice = append(data_slice, companyName_map)
		data_slice1 := make([](map[string]*ConfigFilePagesData),0)
		glog.Info(len(companyName_map))

		for key , value := range companyName_map {
			tmp_map := make(map[string]*ConfigFilePagesData, 0) //子公司name作为key
			glog.Info(key, len(value), value)
			for _, v := range value {
				if _, ok := tmp_map[v["subcompany"]]; ok {
					if v["typeCode"] == "dns_proxy" {
						t := (tmp_map[v["subcompany"]]).Dns_proxy
						(tmp_map[v["subcompany"]]).Dns_proxy = t + 1
					}
					if v["typeCode"] == "redirect" {
						t := (tmp_map[v["subcompany"]]).Redirect
						(tmp_map[v["subcompany"]]).Redirect = t + 1
					}
				} else {
					tmp_map[v["subcompany"]] = &ConfigFilePagesData{}
					(tmp_map[v["subcompany"]]).CompanyName = key
					(tmp_map[v["subcompany"]]).Dns_proxy = 0
					(tmp_map[v["subcompany"]]).Redirect = 0
					(tmp_map[v["subcompany"]]).Subcompany = v["subcompany"]
					if v["typeCode"] == "dns_proxy" {
						t := (tmp_map[v["subcompany"]]).Dns_proxy
						(tmp_map[v["subcompany"]]).Dns_proxy = t + 1
					}
					if v["typeCode"] == "redirect" {
						t := (tmp_map[v["subcompany"]]).Redirect
						(tmp_map[v["subcompany"]]).Redirect = t + 1
					}
				}
			}
			data_slice1 = append(data_slice1, tmp_map)
		}

		tmp_slice := make([]interface{},0)
		for _, value := range data_slice1{
			if len(value) > 0{
				for _,value := range value {
					tmp_slice = append(tmp_slice, value)
				}
			}
		}

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "Success"
		retValue.Data = tmp_slice
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))

	}else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqConfigFilePagesNewEdit struct {
	Token string
	CompanyName string
}

func ConfigFilePagesNewEdit(w http.ResponseWriter, req *http.Request){

	//SetHttpHeader(w,req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqConfigFilePagesNewEdit
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err,w)
			return
		}

		//查询TTL
		ttl_map :=make(map[int]int,0)
		{
			QueryTTL, err := Db.Query(`SELECT  platform,loadTime  FROM proxypro.config_version `)
			if err != nil {
				glog.Info(err)
			}

			for QueryTTL.Next(){
				var platform int
				var loadTime int
				err := QueryTTL.Scan(&platform,&loadTime)
				if err != nil{
					glog.Info(err)
				}
				ttl_map[platform] = loadTime
			}
			QueryTTL.Close()
		}
		glog.Info("ttl_map: ", ttl_map)

		Queryconfig, err := Db.Query(`SELECT  id, typeCode, configValue, companyName,
			subcompany, platform FROM proxypro.config_value1 where estatus = 1`)
		if err != nil {
			log.Println(err)
		}
		defer Queryconfig.Close()

		companyName_map := make(map[string]([](map[string]interface{})),0)
		for Queryconfig.Next(){
			var id int
			var typeCode string
			var configValue string
			var companyName string
			var subcompany string
			var platform int
			err := Queryconfig.Scan(&id, &typeCode, &configValue, &companyName, &subcompany, &platform)
			if err!=nil{
				glog.Info(err)
			}

			tmp_map := make(map[string]interface{},0)
			tmp_map["id"] = id
			tmp_map["typeCode"] = typeCode
			tmp_map["configValue"] = configValue
			tmp_map["companyName"] = companyName
			tmp_map["subcompany"] = subcompany
			tmp_map["platform"] = platform

			if _,ok := ttl_map[platform]; ok{
				tmp_map["ttl"] = ttl_map[platform]
			}else {

				tmp_map["ttl"] = 0
			}

			if   _, ok := companyName_map[companyName]; ok{
				companyName_map[companyName] = append(companyName_map[companyName], tmp_map)
			}else{
				tmp_slice := make([]map[string]interface{},0)
				tmp_slice = append(tmp_slice, tmp_map)
				companyName_map[companyName] = tmp_slice
			}
		}

		type DataSlice struct {
			//LoadTime  string   `json:"load-time"`
			Redirect  []interface{}   `json:"redirect"`
			Dnsproxy  []interface{}   `json:"dns_proxy"`
		}
		ret_data := &DataSlice{}

       if len(companyName_map) > 0 {
		   for _, value1 := range companyName_map {
			   if len(m.CompanyName) > 0 && len(value1) > 0 {
				   for _, value2 := range value1 {
					   if value2["typeCode"] == "dns_proxy" && value2["companyName"] == m.CompanyName {
						   ret_data.Dnsproxy = append(ret_data.Dnsproxy, value2)
					   }

					   if value2["typeCode"] == "redirect" && value2["companyName"] == m.CompanyName {
						   ret_data.Redirect = append(ret_data.Redirect, value2)
					   }
				   }
			   } else if len(value1) > 0 {
				   for _, value2 := range value1 {
					   if value2["typeCode"] == "dns_proxy" {
						   ret_data.Dnsproxy = append(ret_data.Dnsproxy, value2)
					   }

					   if value2["typeCode"] == "redirect" {
						   ret_data.Redirect = append(ret_data.Redirect, value2)
					   }
				   }
			   }
		   }
	   }
		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "Success"
		retValue.Data = ret_data
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else {

		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqGetCompanyName struct {
	Token string
}

func GetCompanyName(w http.ResponseWriter, req *http.Request) {

	//SetHttpHeader(w, req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqGetCompanyName
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err,w)
			return
		}

		Queryconfig, err := Db.Query(`SELECT  id, typeCode, configValue, companyName,
			subcompany, platform FROM proxypro.config_value1 where estatus = 1`)
		if err != nil {
			glog.Info(err)
		}
		defer Queryconfig.Close()
		companyName_map := make(map[string]([]string), 0)
		for Queryconfig.Next() {
			var id string
			var typeCode string
			var configValue string
			var companyName string
			var subcompany string
			var platform string
			err := Queryconfig.Scan(&id, &typeCode, &configValue, &companyName, &subcompany, &platform)
			if err != nil {
				glog.Info(err)
			}
			companyName_map[companyName] = append(companyName_map[companyName], subcompany)
		}

		tmp_companyName_map := make(map[string]interface{}, 0)
		if len(companyName_map) > 0 {
			for key, value1 := range companyName_map {
				tmp_map := make(map[string]bool, 0)
				if len(value1) > 0 {
					for _, value2 := range value1 {
						tmp_map[value2] = true
					}
				}
				tmp_companyName_map[key] = tmp_map
			}
		}
		glog.Info(tmp_companyName_map)
		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "Success"
		retValue.Data = tmp_companyName_map
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))

	}else {

		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqEditCompanyName struct {
	Token string
	CompanyName string
	Subcompany  string
	EiditCompanyName string
	EditSubcompany   string
}

func EditCompanyName(w http.ResponseWriter, req *http.Request){
	//SetHttpHeader(w, req)
	if req.Method == "POST" {

		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqEditCompanyName
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err,w)
			return
		}

		stmt, err := Db.Prepare(`update proxypro.config_value1 set companyName = ?, subcompany= ? 
								where companyName = ? and subcompany= ? and estatus = 1`)
		res, err := stmt.Exec(m.EiditCompanyName, m.EditSubcompany, m.CompanyName, m.Subcompany)
		glog.Info("res",res)
		if err!=nil {
			glog.Info(res)
			glog.Info(err)
			return
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
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

func DeleteCompanyName(w http.ResponseWriter, req *http.Request){
	//SetHttpHeader(w, req)
	if req.Method == "POST" {

		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqEditCompanyName
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
		}

		stmt, err := Db.Prepare(`update proxypro.config_value1 set estatus = 0 where companyName = ? and subcompany= ? `)
		res, err := stmt.Exec(m.CompanyName, m.Subcompany)
		glog.Info("res",res)
		if err!=nil {
			glog.Info(res)
			glog.Info(err)
			return
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
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

//
func SvaeCompanyName(w http.ResponseWriter, req *http.Request){
	//SetHttpHeader(w, req)
	if req.Method == "POST" {

		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqEditCompanyName
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
		}

		stmt, err := Db.Prepare(`INSERT INTO proxypro.config_value1 (companyName, subcompany,configValue) values (?, ?,?)`)
		res, err := stmt.Exec(m.CompanyName, m.Subcompany,"")
		glog.Info("res",res)
		if err!=nil {
			glog.Info(res)
			glog.Info(err)
			return
		}
		stmt.Close()

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
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}


type reqAddConfigFilePages struct {
	URL string
	TO string
	TypeCode string
	CompanyName string
	Subcompany string
}

func AddConfigFilePages(w http.ResponseWriter, req *http.Request){

	//SetHttpHeader(w, req)
	if req.Method == "POST" {

		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqAddConfigFilePages
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
		}


		configValue := m.URL +"->" +m.TO
		stmt, err := Db.Prepare(`INSERT INTO proxypro.config_value1 (typeCode, configValue, companyName, subcompany, estatus) 
                                 values (?, ?, ?, ?, ?)`)
		res, err := stmt.Exec(m.TypeCode,configValue,m.CompanyName, m.Subcompany,1)
		glog.Info("res",res)
		if err!=nil {
			glog.Info(res)
			glog.Info(err)
			return
		}
		stmt.Close()

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
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqEidtConfigFilePages struct {
	Id  int
	URL string
	TO string
	TypeCode string
	CompanyName string
	Subcompany string
}

func EidtConfigFilePages(w http.ResponseWriter, req *http.Request){

	//SetHttpHeader(w, req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesEdit recv body:", string(result))
		var m reqEidtConfigFilePages
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err,w)
			return
		}

		configValue := m.URL +"->" +m.TO
		stmt, err := Db.Prepare(`update proxypro.config_value1 set configValue = ?, companyName = ?, subcompany= ? 
								where  id = ? and estatus = 1`)
		res, err := stmt.Exec(configValue, m.CompanyName, m.Subcompany, m.Id)
		defer stmt.Close()
		glog.Info("res",res)
		if err!=nil {
			glog.Info(res)
			glog.Info(err)
			return
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
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqEidtTTL struct {
	Token string
	Id int
	Platform int
	LoadTime int
}

func EidtTTL(w http.ResponseWriter, req *http.Request){

	//SetHttpHeader(w, req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqEidtTTL recv body:", string(result))
		var m reqEidtTTL
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err,w)
			return
		}
		glog.Info("66666666666666666666666666666666666666")
		{
			stmt, err := Db.Prepare(`update proxypro.config_version set loadTime = ? where  id = ? `)
			res, err := stmt.Exec(m.LoadTime, m.Id)
			stmt.Close()
			if err != nil {
				glog.Info(res)
				glog.Info(err)
				return
			}
			stmt.Close()

			//添加当loadtime变化的时候　修改缓存
			{
				go CacheConfigJM()
			}

		}
		glog.Info("66666666666666666666666666666666666666")
		{
			//当loadtime变化的时候version_code+1
			stmt2, err2 := Db.Prepare(`update proxypro.config_version set version_code = version_code+1 where  id = ? `)
			res2, err2 := stmt2.Exec(m.Id)
			if err2 != nil {
				glog.Info(res2)
				glog.Info(err2)
				return
			}
			stmt2.Close()
		}

		glog.Info("66666666666666666666666666666666666666")

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 200
		retValue.Message = "Success"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))

	}else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}


type reqChangePlatform struct {
	Token string
	Platform int
	Status int
}
func ChangePlatform(w http.ResponseWriter, req *http.Request) {
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqChangePlatform recv body:", string(result))
		var m reqChangePlatform
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err,w)
			return
		}

		stmt, err := Db.Prepare(`update proxypro.config_version set status = ? where  platform = ? `)
		res, err := stmt.Exec(m.Status, m.Platform)
		if err!=nil {
			glog.Info(res)
			glog.Info(err)
			return
		}
		stmt.Close()

		{
			go CacheConfigJM()
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
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}