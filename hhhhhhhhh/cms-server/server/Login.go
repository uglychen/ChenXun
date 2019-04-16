package server

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"math/rand"
	"net/http"
	"strconv"
	"time"

	"github.com/golang/glog"
	uuid "github.com/satori/go.uuid"
)

type GenerateData struct {
	Vcode   string
	Vcodeid string
}

func CreateCaptcha() string {
	return fmt.Sprintf("%08v", rand.New(rand.NewSource(time.Now().UnixNano())).Int31n(100000000))
}

func Generate(w http.ResponseWriter, req *http.Request) {

	if req.Method == "GET" {
		glog.Info("Generate recv body")

		vcode := CreateCaptcha()
		u1,_ := uuid.NewV4()
		vcodeid := u1.String()
		tmp := &GenerateData{}
		tmp.Vcode = vcode
		tmp.Vcodeid = vcodeid

		sql := `INSERT INTO proxypro.tb_vcode (vcode, vguid, actiontime)VALUES (?, ?, ?)`
		_, err1 := Db.Exec(sql, vcode, vcodeid, time.Now().Unix())
		if err1 != nil {
			glog.Info("error info:", err1)
		}

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Message = "Success"
		retValue.Data = tmp
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	} else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: get"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqLogin struct {
	Password string
	UserName string
	Vcode    string
	Vcodeid  string
}

type ResponseLogin struct {
	AuthCodes string `json:"authCodes"`
	Token     string `json:"token"`
}

func Login(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("Login recv body:", string(result))
		var m reqLogin
		json.Unmarshal([]byte(result), &m)

		if m.UserName == "" || m.Password ==""{
			retValue := &ResponseData{}
			retValue.Success = false
			retValue.Code = 1
			retValue.Message = "LoginName error or Password error"
			retValue.Data = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}

		//检验用户名和用户密码
		var uid string
		{
			var userId int
			var loginName string
			var loginPassword string
			//var uuid string
			sql := "SELECT userId, loginName, loginPassword, uuid FROM proxypro.userInfo  where loginName = ? and loginPassword= ?"
			rows, err := Db.Query(sql, m.UserName, m.Password)
			defer  rows.Close()
			if err != nil {
				glog.Info(err)
				return
			}

			if rows.Next() {
				err := rows.Scan(&userId, &loginName, &loginPassword, &uid)
				if err != nil {
					glog.Info(err)
				}
			}
			glog.Info("LoginName:", loginName)
			glog.Info("LoginPassword:", loginPassword)

			if loginName != m.UserName || loginPassword != m.Password {
				retValue := &ResponseData{}
				retValue.Success = false
				retValue.Code = 0
				retValue.Message = "LoginName error or Password error"
				retValue.Data = ""
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}

		//用户登录
		{

			if len(uid) > 0 {
				glog.Info("uuid: ", uid)
			} else {
				u1,_ := uuid.NewV4()
				uid = u1.String()
				glog.Info("uuid: ", uid)
			}
			t := time.Now().Unix()
			randomNum := CreateCaptcha()
			glog.Info("randomNum: ", randomNum)
			s := strconv.FormatInt(t, 10) + randomNum + uid
			glog.Info("未加密之前的token1： ", s)
			encodeString := base64.StdEncoding.EncodeToString([]byte(s))

			stmt, err1 := Db.Prepare(`update proxypro.userInfo set token= ?, uuid= ?, loginStatus =?  where LoginName = ?;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(encodeString, uid, 1, m.UserName)
			glog.Info(err2)
			defer stmt.Close()
			_, err3 := res.RowsAffected()
			glog.Info(err3)

			tmp := &ResponseLogin{}
			tmp.Token = encodeString
			glog.Info("token: ", encodeString)
			retValue := NewBaseJsonData()
			retValue.Code = 200
			retValue.Message = "success"
			retValue.Success = true
			retValue.Data = tmp
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
		}
	} else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqLoginOut struct {
	Token string
}

func LoginOut(w http.ResponseWriter, req *http.Request) {
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("Login recv body:", string(result))
		var m reqLoginOut
		json.Unmarshal([]byte(result), &m)

		//检查token过期情况
		var user_uuid string
		var err_token error
		{
			user_uuid, err_token = CheckToken(m.Token, w, req)
			if err_token != nil {
				retValue := NewBaseJsonData()
				retValue.Code = 1000
				retValue.Message = err_token.Error()
				retValue.Data = ""
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}
		glog.Info("user_uuid: ", user_uuid)

		{
			stmt, err1 := Db.Prepare(`update proxypro.userInfo set loginStatus =?  where uuid = ?;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(0, user_uuid)
			glog.Info(err2)
			defer stmt.Close()
			_, err3 := res.RowsAffected()
			glog.Info(err3)
			defer stmt.Close()
		}

		retValue := NewBaseJsonData()
		retValue.Success = true
		retValue.Code = 200
		retValue.Message = "success"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	} else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: post"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

func Login1(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("Login recv body:", string(result))
		var m reqLogin
		json.Unmarshal([]byte(result), &m)

		//检验4位验证码
		{
			var vcode string
			rows, err := Db.Query("SELECT Vcode LoginPassword FROM proxypro.tb_vcode where vguid = ? ", m.Vcodeid)
			defer rows.Close()
			if err != nil {
				glog.Info(err)
				return
			}

			if rows.Next() {
				err := rows.Scan(&vcode)
				if err != nil {
					glog.Info(err)
				}
			}

			if vcode != m.Vcode {
				retValue := &ResponseData{}
				retValue.Success = false
				retValue.Code = 0
				retValue.Message = "vcode error"
				retValue.Data = ""
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}

		//检验用户名和用户密码
		{
			var ManagerID int
			var LoginName string
			var LoginPassword string
			rows, err := Db.Query("SELECT ManagerID, LoginName, LoginPassword FROM proxypro.tb_manager where  LoginName = ?", m.UserName)
			if err != nil {
				glog.Info(err)
				return
			}

			if rows.Next() {
				err := rows.Scan(&ManagerID, &LoginName, &LoginPassword)
				if err != nil {
					glog.Info(err)
				}
			}
			glog.Info("LoginName:", LoginName)
			glog.Info("LoginPassword:", LoginPassword)

			if LoginName != m.UserName || LoginPassword != m.Password {
				retValue := &ResponseData{}
				retValue.Success = false
				retValue.Code = 0
				retValue.Message = "LoginName error or Password error"
				retValue.Data = ""
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
			rows.Close()
		}

		//用户登录
		{
			t := time.Now().Unix()
			u1,_ := uuid.NewV4()
			glog.Info(u1.String())
			s := strconv.FormatInt(t, 10) + CreateCaptcha() + u1.String()
			glog.Info("未加密之前的token1： ", s)
			encodeString := base64.StdEncoding.EncodeToString([]byte(s))

			stmt, err1 := Db.Prepare(`update proxypro.tb_manager set token= ? where LoginName = ?;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(encodeString, m.UserName)
			glog.Info(err2)
			defer stmt.Close()
			_, err3 := res.RowsAffected()
			glog.Info(err3)
			defer stmt.Close()

			tmp := &ResponseLogin{}
			tmp.Token = encodeString
			glog.Info("token: ", encodeString)
			retValue := NewBaseJsonData()
			retValue.Code = 200
			retValue.Message = "success"
			retValue.Data = tmp
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
		}
	} else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: get"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

type reqRegister struct {
	Password string
	UserName string
}

func Register(w http.ResponseWriter, req *http.Request) {

	//SetHttpHeader(w, req)
	if req.Method == "POST" {

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

type reqAllManager struct {
	Password string
	UserName string
}

func AllManager(w http.ResponseWriter, req *http.Request) {

	//SetHttpHeader(w, req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("AllManager recv body:", string(result))
		var m reqAllManager
		json.Unmarshal([]byte(result), &m)

		//查询等用户的权限
		var managerName string
		var LoginName string
		var LoginPassword string
		{
			rows, err := Db.Query("SELECT managerName, LoginName, LoginPassword FROM proxypro.userInfo where  LoginName = ?", m.UserName)
			if err != nil {
				glog.Info(err)
				return
			}

			if rows.Next() {
				err := rows.Scan(&managerName, &LoginName, &LoginPassword)
				if err != nil {
					glog.Info(err)
				}
			}
			glog.Info("LoginName:", LoginName)
			glog.Info("LoginPassword:", LoginPassword)

			if LoginName != m.UserName || LoginPassword != m.Password {
				retValue := &ResponseData{}
				retValue.Success = false
				retValue.Code = 0
				retValue.Message = "LoginName error or Password error"
				retValue.Data = ""
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
			rows.Close()
		}

		if managerName == "admin" {

			var userId int
			var managerName string
			var loginName string
			var createTime string
			var lastLoginTime string
			var lastLoginIP string

			rows, err := Db.Query("SELECT userId, managerName, LoginName, createTime, lastLoginTime,lastLoginIP" +
				"  FROM proxypro.userInfo; ")
			if err != nil {
				glog.Info(err)
				return
			}

			DataSlice := make([]interface{}, 0)
			if rows.Next() {
				err := rows.Scan(&userId, &managerName, &loginName, &createTime, &lastLoginTime, &lastLoginIP)
				if err != nil {
					glog.Info(err)
				}
				data_map := make(map[string]interface{}, 0)
				data_map["ManagerID"] = userId
				data_map["managerName"] = managerName
				data_map["LoginName"] = loginName
				data_map["createTime"] = createTime
				data_map["lastLoginTime"] = lastLoginTime
				data_map["lastLoginIP"] = lastLoginIP
				DataSlice = append(DataSlice, data_map)
			}

			retValue := NewBaseJsonData()
			retValue.Code = 200
			retValue.Message = "success"
			retValue.Data = DataSlice
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
		} else {
			retValue := &ResponseData{}
			retValue.Success = true
			retValue.Code = 0
			retValue.Message = "无权限查看"
			retValue.Data = "无权限查看"
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
		}
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

func AddUser(w http.ResponseWriter, req *http.Request) {

	//SetHttpHeader(w, req)
	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("AllManager recv body:", string(result))
		var m reqAllManager
		json.Unmarshal([]byte(result), &m)

		stmt, err := Db.Prepare(`INSERT INTO proxypro.userInfo (typeCode, loginPassword) values (?, ?)`)
		res, err := stmt.Exec(m.UserName, m.Password)
		glog.Info("res", res)
		if err != nil {
			glog.Info(res)
			glog.Info(err)
			return
		}
		stmt.Close()
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
