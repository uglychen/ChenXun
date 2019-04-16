package server

import (
	"encoding/base64"
	"encoding/json"
	"errors"
	"fmt"
	"github.com/golang/glog"
	"io/ioutil"
	"log"
	"net/http"
	"strconv"
	"sync"
	"time"
)

type ResponseData1 struct {
	Success    bool         `json:"success"`
	Code      int             `json:"code"`
	Data      interface{}       `json:"data"`
	Message    string       `json:"message"`
}

type  reqHomePages struct {
	State       int
	CheckState  int
	IdClient   int
	PageIndex  int
	Token string
}

func HomePages(w http.ResponseWriter, req *http.Request){

	if req.Method  == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("HomePages recv body:", string(result))
		var m reqHomePages
		err := json.Unmarshal([]byte(result), &m)
		if err != nil{
			glog.Info(err)
		}

		//检查token过期情况
		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
				glog.Info(err_token)
				retValue := NewBaseJsonData()
				retValue.Code = 1000
				retValue.Message = err_token.Error()
				retValue.Data = err_token.Error()
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}
		glog.Info("user_uuid: ", user_uuid)


		data_slice := make([]interface{},0)
		rows , err := Db.Query(`SELECT id, title, imgUrl, url FROM proxypro.cb_home where idClient = 0 and state = 1 order by orderIndex asc;`)
		if err !=nil{
			glog.Info(err)
		}

		var id int
		var title string
		var imgUrl string
		var url string

		for rows.Next(){
			err1 := rows.Scan(&id, &title, &imgUrl, &url)
			if err != nil {
				log.Println(err1)
			}
			dataMap := make(map[string]interface{},0)
			dataMap["id"] = id
			dataMap["title"] = title
			dataMap["imgUrl"] = imgUrl
			dataMap["url"] = url
			data_slice = append(data_slice,dataMap)
		}

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = ""
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

/*---------------------------------------------------------------------------------------------------*/
type reqSaveHome struct {
	ImgUrl string
	State int
	Title string
	Url string
	Token string
}

func SaveHome(w http.ResponseWriter, req *http.Request){

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SaveHome recv body:", string(result))
		var m reqSaveHome
		err := json.Unmarshal([]byte(result), &m)
		if err != nil{
			glog.Info(err)
		}

		//检查token过期情况
		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
				glog.Info(err_token)
				retValue := NewBaseJsonData()
				retValue.Code = 1000
				retValue.Message = err_token.Error()
				retValue.Data = err_token.Error()
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}
		glog.Info("user_uuid: ", user_uuid)

		sql := `INSERT INTO proxypro.cb_home (imgUrl, state, title, url) VALUES (?, ?, ?, ?)`
		insert, err := Db.Exec(sql, m.ImgUrl , m.State, m.Title, m.Url)
		if err!=nil {
			glog.Info(insert)
			glog.Info(err)
		}

		go CachePullGrids()

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "success"
		retValue.Data = ""
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

/*---------------------------------------------------------------------------------------------------*/
type EditData struct {
	CheckRemark string
	CheckState string
	CheckTime string
	ClientName string
	Id int
	IdClient int
	Img string
	ImgUrl string
	OrderIndex int
	Selected string
	State int
	Title string
	Url string
	ValidTime string
	Token string
}

func SaveHomeEdit(w http.ResponseWriter, req *http.Request){

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SaveHomeEdit recv body:", string(result))
		var m EditData
		err := json.Unmarshal([]byte(result), &m)
		if err != nil{
			glog.Info(err)
		}

		//检查token过期情况
		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
				glog.Info(err_token)
				retValue := NewBaseJsonData()
				retValue.Code = 1000
				retValue.Message = err_token.Error()
				retValue.Data = err_token.Error()
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}
		glog.Info("user_uuid: ", user_uuid)


		stmt, err := Db.Prepare(`update proxypro.cb_home set title= ?, url= ?, ImgUrl= ?  where id = ?`)
		res, err := stmt.Exec(m.Title, m.Url, m.ImgUrl, m.Id)
		glog.Info("res",res)
		if err!=nil {
			glog.Info(res)
			glog.Info(err)
		}
		defer stmt.Close()

		go CachePullGrids()

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "success"
		retValue.Data = ""
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

/*---------------------------------------------------------------------------------------------------*/
type reqDeleteHome struct {
	Id int
	Token string
}

func DeleteHome(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("DeleteHome recv body:", string(result))
		var m reqDeleteHome
		err := json.Unmarshal([]byte(result), &m)
		if err != nil{
			glog.Info(err)
		}

		//检查token过期情况
		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
				glog.Info(err_token)
				retValue := NewBaseJsonData()
				retValue.Code = 1000
				retValue.Message = err_token.Error()
				retValue.Data = err_token.Error()
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}
		glog.Info("user_uuid: ", user_uuid)

		stmt, err := Db.Prepare(`update proxypro.cb_home set state = ?  where id = ?`)
		res, err := stmt.Exec(0,m.Id)
		defer  stmt.Close()
		if err!=nil {
			glog.Info(res)
			glog.Info(err)

			retValue := &ResponseData{}
			retValue.Success = false
			retValue.Code = 0
			retValue.Message = "failed"
			retValue.Data = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}

		go CachePullGrids()

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "success"
		retValue.Data = ""
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

/*---------------------------------------------------------------------------------------------------*/
func ApkUpdatePages(w http.ResponseWriter, req *http.Request) {

	if req.Method == "GET"{
		glog.Info("ApkUpdatePages api")

		rows , err := Db.Query(`SELECT id, platformCode, state, isForce, versionCode, apkAddress, remark FROM proxypro.cb_apkupdate;`)
		if err != nil{
			glog.Info(err)
		}
		defer  rows.Close()

		data_slice := make([]interface{},0)
		var  id int
		var platformCode int
		var state int
		var isForce int
		var versionCode string
		var apkAddress string
		var remark string
		for rows.Next() {
			err1 := rows.Scan(&id, &platformCode, &state, &isForce,&versionCode,&apkAddress, &remark)
			if err != nil {
				log.Println(err1)
			}
			dataMap := make(map[string]interface{},0)
			dataMap["id"] = id
			dataMap["platformCode"] = platformCode
			dataMap["state"] = state
			dataMap["isForce"] = isForce
			dataMap["versionCode"] = versionCode
			dataMap["apkAddress"] = apkAddress
			dataMap["remark"] = remark
			data_slice = append(data_slice,dataMap)
		}
		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = ""
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
//----------------------------------------------------------------------------------------------------
type reqSaveApkUpdate struct{
	ApkAddress string
	CreateTime string
	Id int
	IdClient string
	IsForce int
	PlatformCode int
	Remark string
	State int
	UpdateTime string
	VersionCode string
	Token string
}

func SaveApkUpdate (w http.ResponseWriter, req *http.Request){
	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SaveApkUpdate recv body:", string(result))
		var m reqSaveApkUpdate
		err := json.Unmarshal([]byte(result), &m)
		if err != nil{
			glog.Info(err)
		}

		{
			stmt, err := Db.Prepare(`update proxypro.cb_apkupdate set apkAddress = ?, isForce= ?, remark= ?, versionCode = ?  where id = ?`)
			res, err := stmt.Exec(m.ApkAddress, m.IsForce, m.Remark, m.VersionCode, m.Id)
			glog.Info("res", res)
			if err != nil {
				glog.Info(res)
				glog.Info(err)
				return
			}
			defer stmt.Close()
		}
		
		 //需要更新ZeusUpdate缓存数据

        {
                       go CacheZeusUpdate()
        }


		//version_code + 1
		{
			stmt, err := Db.Prepare(`update proxypro.config_version set version_code = version_code+1 where  platform = ? `)
			res, err := stmt.Exec(m.PlatformCode)
			if err != nil {
				glog.Info(res)
				glog.Info(err)
				return
			}
			stmt.Close()
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
		retValue.Message = "Request Method error: GET"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}





/*---------------------------------------------------------------------------------------------------*/
type reqFeedbackPages struct {
	CheckState int
	IdClient   int
	IsVideo    int
	PageIndex  int
	PageSize   int
	State      int
}

type FeedbackPagesBody struct {
	Id int                 `json:"id"`
	UpdateTime string      `json:"updateTime"`
	Contact    string      `json:"contact"`//手机号码`json:"updateTime"`
	Img        string `json:"img"`
	Content    string      `json:"content"`
	OsName     string      `json:"osName"`
	OsVersion  string      `json:"osVersion"`
	PhoneModel string      `json:"phoneModel"`
	State      int         `json:"state"`
	CreateTime      int    `json:"createTime"`
}

type resFeedbackPages struct{
	TotalPage int `json:"totalPage"`
	TotalRecord int `json:"totalRecord"`
	List []*FeedbackPagesBody `json:"list"`
}

//缓存feedback
var Feedback_mutex sync.Mutex
var Feedback_tmp_slice []*FeedbackPagesBody
func FeedbackCache(){

	Feedback_mutex.Lock()
	defer Feedback_mutex.Unlock()

	//rows , err := Db.Query(`SELECT id, createTime, updateTime, contact,content, osName, osVersion, phoneModel, state, img
    //        FROM proxypro.cb_feedback order by createTime desc;`)

	rows , err := Db.Query(`SELECT id, createTime, updateTime,content, osName, osVersion, phoneModel, state, contact,img
            FROM proxypro.cb_feedback where state !=1 and content !="" order by createTime desc;`)

	if err !=nil{
		glog.Info(err)
	}

	Feedback_tmp_slice = make([]*FeedbackPagesBody,0)

	for rows.Next(){
		var id int
		var updateTime string
		var contact    string //手机号码
		var img        string
		var content    string
		var osName     string
		var osVersion  string
		var phoneModel string
		var sate      int
		var createTime int
		err := rows.Scan(&id, &createTime, &updateTime, &content, &osName,&osVersion,&phoneModel,&sate, &contact, &img)
		if err != nil {
			glog.Info(err)
		}

		tmp := &FeedbackPagesBody{}
		tmp.Id = id
		tmp.UpdateTime = updateTime
		tmp.Contact = contact
		tmp.Img = img
		tmp.Content = content
		tmp.OsName = osName
		tmp.OsVersion = osVersion
		tmp.PhoneModel = phoneModel
		tmp.State = sate
		tmp.CreateTime = createTime
		Feedback_tmp_slice = append(Feedback_tmp_slice, tmp)
	}
}


func FeedbackPages (w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("FeedbackPages recv body:  ", string(result))


		var fd map[string]interface{}
		var m reqFeedbackPages
		e := json.Unmarshal(result, &m)

		if e!=nil {
			fmt.Println(e,fd)
		}
		glog.Info(m.PageSize)
/*
		var count int
		{
			rows , err := Db.Query(`SELECT count(*) as count FROM proxypro.cb_feedback;`)
			if err !=nil{
				glog.Info(err)
				glog.Info(rows.Columns())
			}

			for rows.Next(){
				rows.Scan(&count)
			}
			rows.Close()
		}
*/
		tmp_response := &resFeedbackPages{}

		//fmt.Println("tmp_slice:",feedback_tmp_slice,"PageIndex:",m.PageIndex,"PageSize",m.PageSize)

		tmp_response.TotalPage = len(Feedback_tmp_slice)/m.PageSize + 1
		tmp_response.TotalRecord = len(Feedback_tmp_slice)
		//tmp_response.List = tmp_slice
		glog.Info("tmp_response.totalPage:", tmp_response.TotalPage)
		glog.Info("tmp_response.totalRecord:", tmp_response.TotalRecord)

		if (m.PageIndex-1)*m.PageSize > tmp_response.TotalRecord{
			retValue := &ResponseData{}
			retValue.Success = false
			retValue.Code = 1
			retValue.Data = strconv.Itoa(tmp_response.TotalPage) + " -1 *"  +  strconv.Itoa(tmp_response.TotalRecord) +"大于总条数"
			retValue.Message = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}

		// TODO 对异常处理
		if len(Feedback_tmp_slice) < m.PageIndex*m.PageSize{
			tmp_response.List = Feedback_tmp_slice[(m.PageIndex-1)*m.PageSize:]
		}else{
			tmp_response.List = Feedback_tmp_slice[(m.PageIndex-1)*m.PageSize:(m.PageIndex)*m.PageSize]
		}
		glog.Info("tmp_response  tmp_list:",tmp_response.List)

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = tmp_response
		retValue.Message = ""
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

type reqpChangeFeedbackState struct {
	Id int
	State int
	Token string
}

func ChangeFeedbackState (w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ChangeFeedbackState recv body:", string(result))
		var m reqpChangeFeedbackState
		json.Unmarshal([]byte(result), &m )
		glog.Info(m)

		stmt, err := Db.Prepare(`update proxypro.cb_feedback set state = ?  where id = ?`)
		res, err := stmt.Exec(m.State, m.Id)
		glog.Info("res",res)
		if err!=nil {
			glog.Info(res)
			glog.Info(err)
			return
		}
		defer stmt.Close()
		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = ""
		retValue.Message = "处理成功"
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



/*---------------------------------------------------------------------------------------------------*/
type reqSaveSiteType struct {
	Content string
	Token string
}

type contentBody struct {
	OrderIndex  int
	Id int
	TypeName string
}

func SaveSiteType (w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SaveSiteType recv body:", string(result))
		var m reqSaveSiteType
		json.Unmarshal([]byte(result), &m)
		glog.Info("content： ", m.Content)

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid,err_token =  CheckToken(m.Token,w, req)
				if err_token != nil{
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}

		var contentBody_slice []*contentBody
		err := json.Unmarshal([]byte(m.Content), &contentBody_slice)
		if err != nil {
			glog.Info("Can't decode json message", err)
		} else {
			glog.Info(contentBody_slice)
		}

		if len(contentBody_slice) > 0 {
			for i := 0; i < len(contentBody_slice); i++ {
				stmt, err := Db.Prepare(`update proxypro.cb_sitetype set typeName= ?, orderIndex= ? where id = ?`)
				res, err := stmt.Exec(contentBody_slice[i].TypeName, contentBody_slice[i].OrderIndex, contentBody_slice[i].Id)
				if err != nil {
					glog.Info(res)
					glog.Info(err)
					retValue := &ResponseData{}
					retValue.Success = false
					retValue.Code = 1
					retValue.Message = err.Error()
					retValue.Data = ""
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
				stmt.Close()
			}

			retValue := &ResponseData{}
			retValue.Success = true
			retValue.Code = 0
			retValue.Message = "处理成功"
			retValue.Data = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
		}

		{
			go CacheCb_sitetype()
		}


	} else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: GET"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqSiteTypeDelete struct {
	Token string
	Id int
}

func SiteTypeDelete (w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqSiteTypeDelete recv body:", string(result))
		var m reqSiteTypeDelete
		json.Unmarshal([]byte(result), &m)
		glog.Info("Token： ", m.Token)
		glog.Info("Id： ", m.Id)

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid, err_token = CheckToken(m.Token, w, req)
				if err_token != nil {
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}

		{
			stmt, _ := Db.Prepare(`update proxypro.cb_sitetype set state = ?  where id = ?`)
			_, err := stmt.Exec(0, m.Id)
			if err != nil {
				glog.Info(err)
				return
			}
			stmt.Close()
		}

		{
			stmt, _ := Db.Prepare(`update proxypro.cb_site set state = ?  where idSiteType = ?`)
			_, err := stmt.Exec(0, m.Id)
			if err != nil {
				glog.Info(err)
				return
			}
			stmt.Close()
		}

		{
			go CacheCb_sitetype()
		}


		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "处理成功"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	} else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}


/*---------------------------------------------------------------------------------------------------*/
type reqAllSiteTypes struct{
	Token string
}

func AllSiteTypes (w http.ResponseWriter, req *http.Request){

	if req.Method == "POST"{

		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesNew1 recv body:", string(result))
		var m reqAllSiteTypes
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			return
		}

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid,err_token =  CheckToken(m.Token,w, req)
				if err_token != nil{
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}

		rows,err := Db.Query("select id, orderIndex, typeName from proxypro.cb_sitetype where state = 1 order by orderIndex;")
		defer  rows.Close()
		if err!=nil{
			glog.Info(err)
			return
		}

		data_slice := make([]interface{},0)
		for rows.Next() {
			var orderIndex int
			var typeName string
			var id int
			err := rows.Scan(&id, &orderIndex, &typeName)
			if err!=nil{
				glog.Info(err)
			}
			data_map := make(map[string]interface{},0)
			data_map["orderIndex"] = orderIndex
			data_map["typeName"] = typeName
			data_map["id"] = id
			data_slice =append(data_slice, data_map)
		}

		go CacheCb_sitetype()

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "处理成功"
		retValue.Data = data_slice
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqSitePages1 struct {
	Token string
	Id int
	SiteName string
}

func SitePages1 (w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SitePages1 recv body:", string(result))
		var m reqSitePages1
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			return
		}

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid,err_token =  CheckToken(m.Token,w, req)
				if err_token != nil{
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}

		var sql string
		if len(m.SiteName) > 0{
			sql = `select id, state, orderIndex, idSiteType, siteUrl, siteImg, introduction, siteName 
								from proxypro.cb_site where idSiteType = ? and state = 1 and  siteName like "%` + m.SiteName +  `%" order by id asc`
		}else{
			sql = `select id, state, orderIndex, idSiteType, siteUrl, siteImg, introduction, siteName 
								from proxypro.cb_site where idSiteType = ? and state = 1 order by id asc`
		}

		rows,err := Db.Query(sql, m.Id)
		if err !=nil{
			glog.Info(err)
			return
		}

		tmp_slice := make([]interface{},0)
		for rows.Next(){
			var state int
			var orderIndex int
			var idSiteType int
			var id int
			//var siteTypeName string
			var siteUrl string
			var siteImg string
			var introduction string
			var siteName string
			err := rows.Scan(&id,&state,&orderIndex,&idSiteType,&siteUrl,&siteImg,&introduction,&siteName)
			if err !=nil{
				glog.Info(err)
			}
			dataMap := make(map[string]interface{},0)
			dataMap["state"]= state
			dataMap["orderIndex"]= orderIndex
			dataMap["idSiteType"]= idSiteType
			dataMap["id"]= id
			dataMap["siteUrl"]= siteUrl
			dataMap["siteImg"]= siteImg
			dataMap["introduction"]= introduction
			dataMap["siteName"]= siteName
			tmp_slice = append(tmp_slice,dataMap)
		}
		rows.Close()

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = tmp_slice
		retValue.Message = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

/*
type reqSitePages struct {
	idSiteType int
	PageIndex  int
	PageSize   int
	SiteName   string
	OrderIndex int
	Token string
}

type resSitePages struct{
	TotalPage int `json:"totalPage"`
	TotalRecord int `json:"totalRecord"`
	List []map[string]interface{} `json:"list"`
}

func SitePages (w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SitePages recv body:", string(result))
		var m reqSitePages
		json.Unmarshal([]byte(result), &m )
		glog.Info(m.PageSize)

		rows,err := Db.Query("select id, state,orderIndex,idSiteType, siteUrl,siteImg,introduction,siteName from proxypro.cb_site where state=1")
		if err !=nil{
			glog.Info(err)
			return
		}

		tmep_resSitePages :=&resSitePages{}
		resSite_slice :=make([]map[string]interface{},0)
		for rows.Next(){
			var state int
			var orderIndex int
			var idSiteType int
			var id int
			//var siteTypeName string
			var siteUrl string
			var siteImg string
			var introduction string
			var siteName string
			err := rows.Scan(&id,&state,&orderIndex,&idSiteType,&siteUrl,&siteImg,&introduction,&siteName)
			if err !=nil{
				glog.Info(err)
			}
			dataMap := make(map[string]interface{},0)
			dataMap["state"]= state
			//dataMap["orderIndex"]= orderIndex
			dataMap["idSiteType"]= idSiteType
			dataMap["id"]= id
			dataMap["siteUrl"]= siteUrl
			dataMap["siteImg"]= siteImg
			dataMap["introduction"]= introduction
			dataMap["siteName"]= siteName
			resSite_slice = append(resSite_slice,dataMap)
		}
		rows.Close()

		glog.Info("SELECT typeName FROM proxypro.cb_sitetype where orderIndex = ?")
		var typeName string
		if len(resSite_slice) > 0{
			for _,v := range resSite_slice{
				glog.Info(v["idSiteType"])
				rows, err := Db.Query(`SELECT typeName FROM proxypro.cb_sitetype where orderIndex = ? `, v["idSiteType"])
				if err!=nil{
					glog.Info(err)
				}
				if rows.Next(){
					rows.Scan(&typeName)
				}
				v["siteTypeName"] = typeName
				rows.Close()
			}
		}

		tmep_resSitePages.TotalPage = len(resSite_slice)/m.PageSize + 1
		tmep_resSitePages.TotalRecord = len(resSite_slice)
		glog.Info("tmep_resSitePages.totalPage:", tmep_resSitePages.TotalPage)
		glog.Info("tmep_resSitePages.totalRecord:", tmep_resSitePages.TotalRecord)

		if len(resSite_slice) < m.PageIndex*m.PageSize{
			tmep_resSitePages.List = resSite_slice[(m.PageIndex-1)*m.PageSize:]
		}else{
			tmep_resSitePages.List = resSite_slice[(m.PageIndex-1)*m.PageSize:(m.PageIndex)*m.PageSize]
		}
		glog.Info("tmp_response  tmp_list:", resSite_slice)

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = tmep_resSitePages
		retValue.Message = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}
*/

type reqsaveSite struct {
	Id string
	IdSiteType int
	Introduction string
	OrderIndex  int
	SiteImg string
	SiteName string
	SiteUrl string
	State int
	Token string
}

func SaveSite (w http.ResponseWriter, req *http.Request){

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqsaveSite recv body:", string(result))
		var m reqsaveSite
		json.Unmarshal([]byte(result), &m )

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid,err_token =  CheckToken(m.Token,w, req)
				if err_token != nil{
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}

		t1 := time.Now().Unix()
		sql := `INSERT INTO proxypro.cb_site (idSiteType, siteName, introduction, siteUrl, siteImg, orderIndex, State, createTime,updateTime)
                        values ( ?, ?, ?, ?, ?, ?, ?, ?, ?)`
		glog.Info("sql: ",sql)

		stmt,err1 := Db.Prepare(sql)
		if err1 !=nil{
			glog.Info("error: ", err1)
		}
		_, err2 := stmt.Exec(m.IdSiteType, m.SiteName , m.Introduction, m.SiteUrl, m.SiteImg, m.OrderIndex-1, 1, t1 ,t1)
		if err2 !=nil{
			glog.Info("err: ",err2)
			return
		}
		stmt.Close()

		go CacheCb_sitetype()

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = ""
		retValue.Message = "处理成功"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqSiteEdit struct {
	Id int
	IdSiteType int
	Introduction string
	OrderIndex  int
	SiteImg string
	SiteName string
	SiteUrl string
	State int
	Token string
}
func SiteEdit (w http.ResponseWriter, req *http.Request){

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqSiteEdit recv body:", string(result))
		var m reqSiteEdit
		json.Unmarshal([]byte(result), &m )

		glog.Info("reqSiteEdit:", m)

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid,err_token =  CheckToken(m.Token,w, req)
				if err_token != nil{
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}


		stmt,err1 := Db.Prepare(`update proxypro.cb_site set siteName =?, introduction =?, siteUrl=?, siteImg = ? where id= ?`)
		if err1 != nil{
			glog.Info(err1)
		}
		_, err2 := stmt.Exec(m.SiteName, m.Introduction, m.SiteUrl, m.SiteImg, m.Id)
		if err2 !=nil{
			glog.Info(err2)
			return
		}
		stmt.Close()

		{
			go CacheCb_sitetype()
		}

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = ""
		retValue.Message = "处理成功"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqSiteDelete struct {
	Token string
	Id int
}

func SiteDelete (w http.ResponseWriter, req *http.Request){

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SiteDelete recv body:", string(result))
		var m reqSiteDelete
		json.Unmarshal([]byte(result), &m )

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid,err_token =  CheckToken(m.Token,w, req)
				if err_token != nil{
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}

		stmt,_ := Db.Prepare(`update proxypro.cb_site set state = ?  where id = ?`)
		_, err := stmt.Exec(0, m.Id)
		if err !=nil{
			glog.Info(err)
			return
		}
		stmt.Close()

		{
			go CacheCb_sitetype()
		}


		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = ""
		retValue.Message = "处理成功"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqSiteOrder struct {
	Token string
	Content string
}

type contentBodySite struct {
	OrderIndex  int
	Id int
}

func SiteOrder (w http.ResponseWriter, req *http.Request){

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqSiteOrder recv body:", string(result))
		var m reqSiteOrder
		json.Unmarshal([]byte(result), &m )
		glog.Info("Content:", m.Content)

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid,err_token =  CheckToken(m.Token,w, req)
				if err_token != nil{
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}

		var contentBody_slice []*contentBodySite
		err := json.Unmarshal([]byte(m.Content), &contentBody_slice)
		if err != nil {
			glog.Info("Can't decode json message", err)
		} else {
			glog.Info(contentBody_slice)
		}

		if len(contentBody_slice) > 0 {
			for i := 0; i < len(contentBody_slice); i++ {
				stmt, err := Db.Prepare(`update proxypro.cb_site set orderIndex= ? where id = ?`)
				res, err := stmt.Exec(contentBody_slice[i].OrderIndex, contentBody_slice[i].Id)
				if err != nil {
					glog.Info(res)
					glog.Info(err)
					retValue := &ResponseData{}
					retValue.Success = false
					retValue.Code = 1
					retValue.Message = err.Error()
					retValue.Data = ""
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
				stmt.Close()
			}
		}else{
			retValue := &ResponseData{}
			retValue.Success = false
			retValue.Code = 1
			retValue.Data = ""
			retValue.Message = "contentBodySite null"
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}


		{
			go CacheCb_sitetype()
		}


		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = ""
		retValue.Message = "处理成功"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}


type reqAddSiteType struct{
	Token string
	TypeName string
	//OrderIndex int
}

func AddSiteType (w http.ResponseWriter, req *http.Request){

	if req.Method == "POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqAddSiteType recv body:", string(result))
		var m reqAddSiteType
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			return
		}

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid,err_token =  CheckToken(m.Token,w, req)
				if err_token != nil{
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}

		var orderIndex int
		{
			rows, err := Db.Query(`SELECT max(orderIndex+ 1) as orderIndex FROM proxypro.cb_sitetype;`)
			if err!=nil{
				glog.Info(err)
			}
			if rows.Next(){
				rows.Scan(&orderIndex)
			}
			rows.Close()
		}

		{
			stmt,err1 := Db.Prepare(`INSERT INTO proxypro.cb_sitetype (typeName,orderIndex) values ( ?,? )`)
			if err1 !=nil{
				glog.Info("error: ", err1)
			}
			_, err2 := stmt.Exec(m.TypeName,orderIndex)
			if err2 !=nil{
				glog.Info("err: ",err2)
				return
			}
			stmt.Close()
		}

		{
			go CacheCb_sitetype()
		}


		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = ""
		retValue.Message = "处理成功"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}



func CheckToken1(token string, w http.ResponseWriter, req *http.Request)(user_uuid string, err error){
	req.ParseForm()
	glog.Info("rec token:", token, " token length:", len(token))

	if len(token)  > 18  {
		s, err1 := base64.StdEncoding.DecodeString(token)
		if err1 != nil {
			glog.Info(err1)
			return  "", err1
		}
		glog.Info("token: ", string(s))
		time_token := s[0:10]
		user_uuid = string(s[18:])
		glog.Info("time_token: ", string(time_token))
		glog.Info("user_uuid: ", user_uuid)
		time_now := time.Now().Unix()
		i, _ := strconv.ParseInt(string(time_token), 10, 64)

		//token过期处理
		if time_now-i >= 24*60*60 {
			glog.Info("token 过期: ", (time.Unix(i, 0).Format("2006-01-02 03:04:05 PM")))
			//过期 退出登录
			stmt, err := Db.Prepare(`update proxypro.userInfo set loginStatus = ?  where uuid = ?`)
			res, err := stmt.Exec(0, user_uuid)
			glog.Info(err)
			glog.Info(res)
			stmt.Close()
			glog.Info("============================================",user_uuid)
			return "", errors.New("token error: token 过期 请重新登")
		} else {
			return  user_uuid, nil
		}
	} else {
		return "", errors.New("token error")
		/*retValue := NewBaseJsonData()
		retValue.Code = 900
		retValue.Message = "token error"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
		return*/
	}
	return "", errors.New("token error: no token")
}

type BaseJsonData1 struct {
	Message string      `json:"message,omitempty"`
	Code    int         `json:"code"`
	Success bool		`json:"success"`
	Data    interface{} `json:"data,omitempty"`
}

func NewBaseJsonData1() *BaseJsonData {
	return &BaseJsonData{}
}