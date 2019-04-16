 package server

/*

url   http://192.168.2.153:5678/ConfigFile/ConfigJM?version=1

*/
/*
type ResponseConfigJM struct {
	Success 	string 			`json:"success"`
	Code		int				`json:"code"`
	Data		interface{}		`json:"data"`
	Message 	string			`json:"message"`
}

type DataSlice struct {
	LoadTime  []string   `json:"load-time"`
	Redirect  []string   `json:"redirect"`
	Dnsproxy  []string   `json:"dns-proxy"`
}

func ConfigJM(w http.ResponseWriter, req *http.Request){

	req.ParseForm()
	SetHttpHeader(w,req)

	if req.Method == "GET"{
		req.ParseForm()
		param_type, _ := req.Form["version"]
		req_version := param_type[0]
		glog.Info("ConfigJM version:", req_version)

		QueryUser, err := Db.Query("SELECT id,state,loadTime FROM proxypro.config_file where versionCode = ?", req_version)
		if err != nil {
			glog.Info(err)
		}
		defer QueryUser.Close()

		var flag bool = false
		var state int
		var id int64
		var loadTime string
		var old_id int64

		data_slice := &DataSlice{}
		id_slice := make([]int64,0)
		for QueryUser.Next() {
			err := QueryUser.Scan(&id, &state,&loadTime)
			id_slice = append(id_slice,id)
			data_slice.LoadTime = append(data_slice.LoadTime, loadTime)
			if err != nil {
				glog.Info("error info:", err)
			}
			if state == 1 {
				old_id = id
				flag = true
				glog.Info("loginStatus")
			}
		}

		glog.Info("id_slice:", id_slice)
		if flag == false{
			w.Write([]byte("no need change"))
		}else{
			if len(id_slice) > 0{
				for _,id_value := range id_slice{
					sql :="SELECT typeCode, configValue FROM proxypro.config_value where idConfigFile = ?"
					Query_value,_:= Db.Query(sql, id_value)
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

						if typeCode == "redirect"{
							data_slice.Redirect = append(data_slice.Redirect, configValue)
						}
						if typeCode == "dns-proxy"{
							data_slice.Dnsproxy = append(data_slice.Dnsproxy, configValue)
						}
					}
				}
			}
			glog.Info("redirect_slice:", data_slice.Redirect)
			glog.Info("dnsproxy_slice:", data_slice.Dnsproxy)

			if len(id_slice) > 0{
				update(id_slice[0], old_id)
			}

			retValue := &ResponseConfigJM{}
			retValue.Success = "success"
			retValue.Code = 0
			retValue.Data = data_slice
			retValue.Message = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
		}
	}
}


func update(id int64, old_id int64){

	sql := "SELECT max(versionCode) FROM proxypro.config_file"
	queryversionCode, err:=   Db.Query(sql)
	defer  queryversionCode.Close()
	if err != nil {
		glog.Info(err)
	}

	var max_versionCode int64
	if queryversionCode.Next(){
		err = queryversionCode.Scan(&max_versionCode)
		if err != nil {
			glog.Info("error info:", err)
		}


		update_sql, err1 := Db.Prepare(`update proxypro.config_file set versionCode= ? where id = ?`)
		if err != nil {
			glog.Info(err1)
		}
		_, err2 := update_sql.Exec(max_versionCode + 1, id)
		if err != nil {
			glog.Info(err2)
		}
	}

	update_sql, err1 := Db.Prepare(`update proxypro.config_file set state= ? where id = ?`)
	if err != nil {
		glog.Info(err1)
	}
	_, err2 := update_sql.Exec(0, old_id)
	if err != nil {
		glog.Info(err2)
	}
}

*/
