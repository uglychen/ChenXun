package handler

import (
	"net/http"
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



func SetHttpHeader(w http.ResponseWriter, req *http.Request){

	Origin := req.Header.Get("Origin")
	if Origin != "" {
		w.Header().Add("Access-Control-Allow-Origin", Origin)
		w.Header().Add("Access-Control-Allow-Methods", "POST,GET,OPTIONS,DELETE")
		w.Header().Add("Access-Control-Allow-Headers", "x-requested-with,content-type")
		w.Header().Add("Access-Control-Allow-Credentials", "true")
	}
}




