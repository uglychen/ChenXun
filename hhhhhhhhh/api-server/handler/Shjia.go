package handler

import (
	"encoding/json"
	"github.com/golang/glog"
	"net/http"
)

func Shjia(w http.ResponseWriter, req *http.Request) {

	if req.Method == "GET"{
		ret_data := make(map[string]interface{},0)
		ret_data["ztai"] = 1
		ret_data["version"] = "1.3.0"
		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = ret_data
		retValue.Message = ""
		bytes, err := json.Marshal(retValue)
		if err != nil {
			ret := FailRet("json　Marshal出错")
			w.Write(ret)
			glog.Error("json　Marshal出错", err)
			return
		}
		w.Write(bytes)

	}else{
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 1
		retValue.Data = ""
		retValue.Message = ""
	}
}