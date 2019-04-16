package server

import (
	"encoding/json"
	"github.com/golang/glog"
	"io/ioutil"
	"net/http"
)

type reqOfficialWebsitePages struct {
	Token string
}

func OfficialWebsitePages(w http.ResponseWriter, req *http.Request){

	//SetHttpHeader(w, req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("OfficialWebsitePages recv body:", string(result))
		var m reqOfficialWebsitePages
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
		}

		Queryconfig, err := Db.Query(`SELECT  id, pageTitle, img, title1, title2, content, updateTime 
										FROM proxypro.cb_officialwebsite`)
		if err != nil {
			glog.Info(err)
		}
		defer Queryconfig.Close()

		var id int
		var pageTitle string
		var img string
		var title1 string
		var title2 string
		var content string
		var updateTime string

		data_slice := make([]interface{},0)
		for Queryconfig.Next(){
			err := Queryconfig.Scan(&id,&pageTitle,&img,&title1,&title2,&content,&updateTime)
			if err!=nil{
				glog.Info(err)
			}

			tmp_map := make(map[string]interface{},0)
			tmp_map["id"] = id
			tmp_map["pageTitle"] = pageTitle
			tmp_map["img"] = img
			tmp_map["title1"] = title1
			tmp_map["content"] = content
			tmp_map["updateTime"] = updateTime
			data_slice = append(data_slice, tmp_map)
		}

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "Success"
		retValue.Data = data_slice
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

type reqSavaeOfficialWebsitePages struct {
	Token string
	Content string
}

type reqWebsitePagesBody struct {
	Id int
	PageTitle string
	Img string
	Title1 string
	Content string
	UpdateTime string
}

func SavaeOfficialWebsitePages(w http.ResponseWriter, req *http.Request){

	//SetHttpHeader(w, req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqSavaeOfficialWebsitePages recv body:", string(result))
		var body reqSavaeOfficialWebsitePages
		err1 := json.Unmarshal([]byte(result), &body)
		if err1 != nil {
			glog.Info(err1)
		}


		var m []*reqWebsitePagesBody
		err2 := json.Unmarshal([]byte(body.Content), &m)
		if err2 != nil {
			glog.Info(err2)
		}

		if len(m) > 0 {
			for _,value := range m{
				glog.Info("value: ", value)
				stmt, err := Db.Prepare(`update proxypro.cb_officialwebsite set pageTitle = ?, img = ?, title1= ?,content= ?  where id = ? `)
				res, err := stmt.Exec(value.PageTitle, value.Img, value.Title1, value.Content, value.Id)
				stmt.Close()
				glog.Info("res", res)
				if err != nil {
					glog.Info(res)
					glog.Info(err)
					return
				}
				stmt.Close()
			}
		}

		go Cache_WebSiteContent()

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