package handler

import (
	. "api-server/utils"
	"encoding/json"
	"github.com/golang/glog"
	"io/ioutil"
	"net/http"
)


func Feedback(w http.ResponseWriter, req *http.Request){
	glog.Info("req.Method:",req.Method)
	glog.Info("-----------Feedback-----------------")


	w.Header().Set("Content-Type", "application/json;charset=UTF-8")

	if req.Method=="POST"{
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("Feedback recv body:", string(result))

		err :=Push(string(result))
		if err != nil{
			retValue := &ResponseData{}
			retValue.Success = false
			retValue.Code = 1
			retValue.Data = err
			retValue.Message = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}else {
			retValue := &ResponseData{}
			retValue.Success = true
			retValue.Code = 0
			retValue.Data = ""
			retValue.Message = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}
	}else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 1
		retValue.Data = ""
		retValue.Message = "请求的资源不支持 http 方法“GET”。"
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}

}


func Push(message string) error  { //发布者

	conn:= NewMasterRedisPool().Get()
	//defer conn.Close()


	_,err1 := conn.Do("PUBLISH", "Feedback", message)
	if err1 != nil {
		glog.Info("pub err: ", err1)
		return err1
	}

	return nil
}