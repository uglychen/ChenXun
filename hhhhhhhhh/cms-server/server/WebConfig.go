package server

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"github.com/golang/glog"
	"io/ioutil"
	"net/http"
	"strconv"
	"strings"
)


type reqConfigFilePagesNew1 struct{
	Token string
	CompanyName string
}

func ConfigFilePagesNew1(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigFilePagesNew1 recv body:", string(result))
		var m reqConfigFilePagesNew1
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err, w)
			return
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
		
		//查询公司名称
		companyName_slice := make([](map[string]string), 0)
		{
			var Queryconfig *sql.Rows
			if len(m.CompanyName) > 0{
				var err error
				sql :=`SELECT id, companyName FROM proxypro.company where estatus = 1 and companyName like "%` + m.CompanyName  + `%";`
				Queryconfig, err = Db.Query(sql)
				if err != nil {
					glog.Info(err)
				}
			}else{
				var err error
				sql :=`SELECT id, companyName FROM proxypro.company where estatus = 1;`
				Queryconfig, err = Db.Query(sql)
				if err != nil {
					glog.Info(err)
				}
			}

			for Queryconfig.Next(){
				var id string
				var companyName string
				err := Queryconfig.Scan(&id, &companyName)
				if err!=nil{
					glog.Info(err)
				}

				tmp_map := make(map[string]string,0)
				tmp_map["id"] = id
				tmp_map["companyName"] = companyName
				companyName_slice = append(companyName_slice, tmp_map)
			}
			Queryconfig.Close()
		}
		glog.Info(companyName_slice)

		/*
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
*/
		type webcount struct {
			CompanyNameID		int
			CompanyName    		string
			SubCompanyCount 	int64
			DomainNameCount     int64
		}

		mutex.Lock()
		defer mutex.Unlock()
		//data_slice := make([](map[string]*webcount), 0)
		data_map := make(map[string]*webcount)  //公司名称作为key
  		for _,value := range WebConfig_tmp_map_slice {
  			for  _,v := range companyName_slice{
				if v["id"]   == value["companyNameID"]{
					tmp := &webcount{}
					if  _,ok:=  data_map[v["companyName"]]; ok {
						tmp_name := v["companyName"]
						data_map[v["companyName"]].SubCompanyCount ++
						slice := strings.Split(value["redirectUrl"], "|")
						if  len(slice) ==1  && slice[0] == "0"{
						}else{
							data_map[tmp_name].DomainNameCount =  data_map[tmp_name].DomainNameCount + int64(len(slice))
						}
					}else{
						tmp_name := v["companyName"]
						tmp.CompanyName = tmp_name
						tmp.CompanyNameID,_ = strconv.Atoi(v["id"])
						tmp.SubCompanyCount = 1
						slice := strings.Split(value["redirectUrl"], "|")
						if len(slice) ==1 && slice[0] == "0"{
							tmp.DomainNameCount = 0
						}else{
							tmp.DomainNameCount = int64(len(slice))
						}
						data_map[tmp_name] = tmp
					}
				}
			}
		}

		for  _,v := range companyName_slice{
			if _,ok := data_map[v["companyName"]]; ok{

			}else{
				tmp := &webcount{}
				tmp.CompanyName = v["companyName"]
				tmp.CompanyNameID,_ = strconv.Atoi(v["id"])
				data_map[v["companyName"]] = tmp
			}
		}

		data_slice := make([]interface{},0)
  		if len(data_map) > 0 {
			/*if len(m.CompanyName)> 0 {
				if _,ok := data_map[m.CompanyName]; ok{
					data_slice = append(data_slice, data_map[m.CompanyName])
				}
			}else{
				for  _,value := range data_map {
					data_slice = append(data_slice, value)
				}
			}*/

			for _, value := range data_map {
				data_slice = append(data_slice, value)
			}

			retValue := &ResponseData{}
			retValue.Success = true
			retValue.Code = 0
			retValue.Message = "Success"
			retValue.Data = data_slice
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
		}

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

type reqConfigEidt struct {
	Id int      //companyNameID
	Token string
	SubcompanyName string
}

func ConfigEidt(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("ConfigEidt recv body:", string(result))
		var m reqConfigEidt
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err, w)
			return
		}

		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
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


		//查询公司名称
		var id string
		var companyName string
		companyName_slice := make([](map[string]string), 0)
		{
			Queryconfig, err := Db.Query(`SELECT id, companyName FROM proxypro.company where estatus = 1;`)
			if err != nil {
				glog.Info(err)
			}

			for Queryconfig.Next(){
				err := Queryconfig.Scan(&id, &companyName)
				if err!=nil{
					glog.Info(err)
				}

				tmp_map := make(map[string]string,0)
				tmp_map["id"] = id
				tmp_map["companyName"] = companyName
				companyName_slice = append(companyName_slice, tmp_map)
			}
			Queryconfig.Close()
		}
		glog.Info(companyName_slice)


		tmp_map_slice := make([](map[string]string),0)
		{
			var Queryconfig *sql.Rows
			var err error
			if len(m.SubcompanyName) > 0{
				sql := `SELECT id, companyNameID, subcompany, platform, createTime, URL, redirectUrl, DNS 
								FROM proxypro.webConfig where companyNameID = ? and estatus = 1 and subcompany like "%` +
								m.SubcompanyName  + `%" order by createTime desc;`
				glog.Info("sql:", sql)
				Queryconfig, err = Db.Query(sql, m.Id)
			}else{
				Queryconfig, err = Db.Query(`SELECT id, companyNameID, subcompany, platform, createTime, URL, redirectUrl, DNS 
								FROM proxypro.webConfig where companyNameID = ? and estatus = 1 order by createTime desc ;`, m.Id)
			}

			if err != nil {
				glog.Info(err)
			}

			for Queryconfig.Next(){
				var id int64
				var companyNameID string
				var subcompany string
				var redirectUrl string
				var DNS string
				var platform string
				var createTime string
				var URL string

				err3 := Queryconfig.Scan(&id, &companyNameID, &subcompany, &platform, &createTime, &URL,&redirectUrl,&DNS)
				if err!=nil{
					glog.Info(err3)
				}else{

				}

				tmp_map := make(map[string]string,0)
				tmp_map["id"] = strconv.FormatInt(id,10)
				tmp_map["companyNameID"] = companyNameID
				tmp_map["subcompanyName"] = subcompany
				tmp_map["redirectUrl"] = redirectUrl
				tmp_map["DNS"] = DNS
				tmp_map["platform"] = platform
				tmp_map["createTime"] = createTime
				tmp_map["URL"] = URL
				tmp_map_slice = append(tmp_map_slice, tmp_map)
			}
			Queryconfig.Close()
		}

		//if len(tmp_map_slice) > 0

			data_slice := make([]interface{},0)
			for  _,value := range tmp_map_slice{
				slice := strings.Split(value["redirectUrl"], "|")
				if  len(slice) ==1  && slice[0] == "0"{
					value["DomainNameCount"] = strconv.Itoa(0)
					value["redirectUrl"] = ""
				}else{
					count := strconv.Itoa(len(slice))
					value["DomainNameCount"] = count
				}
				data_slice = append(data_slice, value)
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

type reqCompanyName struct {
	CompanyNameID int
	CompanyName string
	Token string
}

func SetCompanyName(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SetRedirectUrl recv body:", string(result))
		var m reqCompanyName
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err, w)
			return
		}

		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
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

		//查询公司名称重复
		{
			Queryconfig, err := Db.Query(`SELECT companyName FROM proxypro.company where companyName=? ;`,
				m.CompanyName)
			if err != nil {
				glog.Info(err)
			}
			defer  Queryconfig.Close()

			if Queryconfig.Next(){
				retValue := &ResponseData{}
				retValue.Success = false
				retValue.Code = 300
				retValue.Message = "公司名称重复"
				retValue.Data = "公司名称重复"
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}
		
		{
			stmt, err1 := Db.Prepare(`update proxypro.company set companyName=?  where id=?`)
			glog.Info(err1)
			res, err2 := stmt.Exec(m.CompanyName, m.CompanyNameID)
			glog.Info(err2)
			defer stmt.Close()
			ret , err3 := res.RowsAffected()
			glog.Info("RowsAffected: ",ret,"  ",err3)
		}

		//更新缓存
		{
			go SaveWebConfig()
			go CacheConfigJM()
		}

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

type reqSetRedirectUrl struct {
	RedirectUrl string
	CompanyNameID int
	SubcompanyName string
	Platform 	int
	Token string
}

func SetRedirectUrl(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SetRedirectUrl recv body:", string(result))
		var m reqSetRedirectUrl
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err, w)
			return
		}

		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
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
			stmt, err1 := Db.Prepare(`update proxypro.webConfig set redirectUrl=?  where companyNameID=? and subcompany = ?;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(m.RedirectUrl, m.CompanyNameID,m.SubcompanyName)
			glog.Info(err2)
			defer stmt.Close()
			ret , err3 := res.RowsAffected()
			glog.Info("RowsAffected: ",ret,"  ",err3)
		}

		AddVsersion(m.Platform)
		//更新缓存
		{
			go SaveWebConfig()
			go CacheConfigJM()
		}

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

type reqSetDNS struct {
	DNS string
	CompanyNameID int
	SubcompanyName string
	Platform int
	Token string
}

func SetDNS(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("SetDNS recv body:", string(result))
		var m reqSetDNS
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err, w)
			return
		}

		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
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
			stmt, err1 := Db.Prepare(`update proxypro.webConfig set DNS=?  where companyNameID=? and subcompany = ?;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(m.DNS, m.CompanyNameID,m.SubcompanyName)
			glog.Info(err2)
			defer stmt.Close()
			ret , err3 := res.RowsAffected()
			glog.Info("RowsAffected: ",ret,"  ",err3)
		}

		AddVsersion(m.Platform)
		//更新缓存
		{
			go SaveWebConfig()
			go CacheConfigJM()
		}

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

//-------------------------------------------------------------------------------------------------
//编辑站点
type reqSetSubcompany struct {
	Id int64
	CompanyNameID int
	SubcompanyName string
	SetSubcompanyName string
	URL string
	Platform int
	Token string
}

func SetSubcompany(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqSetSubcompany recv body:", string(result))
		var m reqSetSubcompany
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err, w)
			return
		}

		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
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

		if m.Id <= 0{

		}

		if m.SubcompanyName == m.SetSubcompanyName{
			glog.Info("SetSubcompany: 此时未修改子站点名称")
		}else{
			glog.Info("SetSubcompany: 此时修改子站点名称")
		}

		//查询子站点名称重复
		var subcompany string
		var flag bool = false
		{
			Queryconfig, err := Db.Query(`SELECT subcompany FROM proxypro.webConfig where companyNameID=? and id != ?;`,
				m.CompanyNameID, m.Id)
			if err != nil {
				glog.Info(err)
			}
			defer  Queryconfig.Close()

			for  Queryconfig.Next(){
				err := Queryconfig.Scan(&subcompany)
				glog.Info(err)

				if subcompany == m.SetSubcompanyName{
					flag = true
					break
				}
			}
		}

		if flag == true{
			glog.Info("SetSubcompany: 子站点名称与本公司其他子站点 重复")
			retValue := &ResponseData{}
			retValue.Success = false
			retValue.Code = 300
			retValue.Message = "子站点名称重复"
			retValue.Data = "子站点名称重复"
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}else{
			glog.Info("SetSubcompany: 子站点名称与本公司其他子站点 不重复")
		}

		//如果url变了version + 1
		{
			var companyNameID string
			var subcompany string
			var URL string
			{
				Queryconfig, err := Db.Query(`SELECT companyNameID, subcompany, URL FROM proxypro.webConfig where companyNameID=? and id = ?;`,
					m.CompanyNameID, m.Id)
				if err != nil {
					glog.Info(err)
				}
				defer  Queryconfig.Close()

				if  Queryconfig.Next(){
					err := Queryconfig.Scan(&companyNameID, &subcompany, &URL)
					glog.Info(err)
				}
			}

			{
				if URL == m.URL{

				}else{
					go AddVsersion(m.Platform)
				}
			}
		}

		//更新缓存
		{
			go SaveWebConfig()
			go CacheConfigJM()
		}

		{
			stmt, err1 := Db.Prepare(`update proxypro.webConfig set URL=?, subcompany = ? where companyNameID=? and subcompany = ?;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(m.URL, m.SetSubcompanyName, m.CompanyNameID,m.SubcompanyName)
			glog.Info(err2)
			defer stmt.Close()
			ret , err3 := res.RowsAffected()
			glog.Info("RowsAffected: ",ret,"  ",err3)
		}

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

type reqDelelteCompany struct {
	CompanyNameID int
	Token string
}

func DelelteCompany(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("DelelteSubcompany recv body:", string(result))
		var m reqDelelteCompany
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err.Error(), w)
			return
		}

		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
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
			stmt, err1 := Db.Prepare(`update proxypro.company set estatus=?  where id=? ;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(0, m.CompanyNameID)
			glog.Info(err2)
			defer stmt.Close()
			ret, err3 := res.RowsAffected()
			glog.Info("RowsAffected: ", ret, "  ", err3)
		}

		{
			stmt, err1 := Db.Prepare(`update proxypro.webConfig set estatus=?  where companyNameID=? ;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(0, m.CompanyNameID)
			glog.Info(err2)
			defer stmt.Close()
			ret, err3 := res.RowsAffected()
			glog.Info("RowsAffected: ", ret, "  ", err3)
		}

		// version_code 需要加１　
		{
			AddVsersion(-1)

		}

		//更新缓存
		{
			go SaveWebConfig()
			go CacheConfigJM()
		}

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

type reqDelelteSubcompany struct {
	CompanyNameID int
	SubcompanyName string
	Token string
}

func DelelteSubcompany(w http.ResponseWriter, req *http.Request) {
	fmt.Println("--------DelelteSubcompany start----------")

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("DelelteSubcompany recv body:", string(result))
		var m reqDelelteSubcompany
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err, w)
			return
		}

		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
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



		var flag bool = false
		{

			Queryconfig, err := Db.Query(`SELECT redirectUrl,DNS FROM proxypro.webConfig where companyNameID=? and subcompany=?;`,
				m.CompanyNameID, m.SubcompanyName)
			if err != nil {
				glog.Info(err)
			}
			defer  Queryconfig.Close()

			for  Queryconfig.Next(){
				var redirectUrl string
				var DNS string
				err := Queryconfig.Scan(&redirectUrl,&DNS)
				fmt.Println("redirectUrl,DNS:",redirectUrl,DNS)
				glog.Info(err)

				if redirectUrl == "0" && DNS==""{
					flag = true
					//break
				}

				// version_code 需要加１　 如果 redirectUrl =0 和 DNS 是都空的，则不用 version+1，其他情况 +1
				if flag==false {
					fmt.Println("------DelelteSubcompany----AddVsersion(-1)-----------")
					go AddVsersion(-1)

				}
			}
		}




		{
			stmt, err1 := Db.Prepare(`update proxypro.webConfig set estatus= ?  where companyNameID=? and subcompany=?;`)
			glog.Info(err1)
			res, err2 := stmt.Exec(0, m.CompanyNameID, m.SubcompanyName)
			glog.Info(err2)
			defer stmt.Close()
			ret, err3 := res.RowsAffected()
			glog.Info("RowsAffected: ", ret, "  ", err3)
		}



		//更新缓存
		{
			go SaveWebConfig()
			go CacheConfigJM()
		}

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


type reqAddCompany struct {
	CompanyName string
	Token string
}

func AddCompany(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqAddCompany recv body:", string(result))
		var m reqAddCompany
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err, w)
			return
		}

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

		if len(m.CompanyName) == 0 {
			retValue := &ResponseData{}
			retValue.Success = false
			retValue.Code = 300
			retValue.Message = "CompanyName null"
			retValue.Data = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}

		//查询公司名称重复
		{
			Queryconfig, err := Db.Query(`SELECT companyName FROM proxypro.company where companyName=? and estatus=1;`,
				m.CompanyName)
			if err != nil {
				glog.Info(err)
			}
			defer  Queryconfig.Close()

			if Queryconfig.Next(){
				retValue := &ResponseData{}
				retValue.Success = false
				retValue.Code = 300
				retValue.Message = "公司名称重复"
				retValue.Data = "公司名称重复"
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}

		{
			stmt, err := Db.Prepare(`INSERT INTO proxypro.company (companyName) values (?)`)
			res, err := stmt.Exec(m.CompanyName)
			glog.Info("res", res)
			if err != nil {
				glog.Info(res)
				glog.Info(err)
				return
			}
			stmt.Close()
		}

		//更新缓存
		{
			go SaveWebConfig()
			go CacheConfigJM()
		}

		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 200
		retValue.Message = "Success"
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

type reqAddSubCompany struct {
	CompanyNameID int
	Subcompany string
	URL string
	Token string
}

func AddSubCompany(w http.ResponseWriter, req *http.Request) {

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqAddSubCompany recv body:", string(result))
		var m reqAddSubCompany
		err := json.Unmarshal([]byte(result), &m)
		if err != nil {
			glog.Info(err)
			retError(err, w)
			return
		}

		var user_uuid string
		var err_token error
		{
			user_uuid,err_token =  CheckToken(m.Token,w, req)
			if err_token != nil{
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

		if m.CompanyNameID == 0 || len(m.Subcompany) == 0 || len(m.URL) == 0{
			retValue := &ResponseData{}
			retValue.Success = false
			retValue.Code = 300
			retValue.Message = "Subcompany  or URL null"
			retValue.Data = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}

		//查询子站点名称重复
		{
			Queryconfig, err := Db.Query(`SELECT companyNameID, subcompany FROM proxypro.webConfig where companyNameID=? and subcompany = ? and  estatus=1;`,
											m.CompanyNameID, m.Subcompany)
			if err != nil {
				glog.Info(err)
			}
			defer  Queryconfig.Close()

			if Queryconfig.Next(){
				retValue := &ResponseData{}
				retValue.Success = false
				retValue.Code = 300
				retValue.Message = "子站点名称重复"
				retValue.Data = "子站点名称重复"
				bytes, _ := json.Marshal(retValue)
				w.Write([]byte(bytes))
				return
			}
		}

		{
			stmt, err := Db.Prepare(`INSERT INTO proxypro.webConfig (companyNameID,subcompany,URL,redirectUrl) values (?,?,?,?)`)
			res, err := stmt.Exec(m.CompanyNameID, m.Subcompany, m.URL,0)
			glog.Info("res",res)
			if err!=nil {
				glog.Info(res)
				glog.Info(err)
				return
			}
			stmt.Close()
		}

		//取消version+1
		//// version_code 需要加１　
		//{
		//	AddVsersion(-1)
		//
		//}

		//更新缓存
		{
			go SaveWebConfig()
			go CacheConfigJM()
		}

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