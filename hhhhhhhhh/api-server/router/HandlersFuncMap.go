package router

import (
	"api-server/handler"
	_ "github.com/go-sql-driver/mysql"
	"github.com/golang/glog"
	"net/http"
)

type HandlersFunc func(w http.ResponseWriter, req *http.Request)

var HandlerMap = make(map[string]HandlersFunc)

func InitHandlerMap() {
	HandlerMap["/ConfigFile/ConfigJM"] = handler.ConfigJM
	HandlerMap["/ConfigFile/PullGrids"] = handler.PullGrids
	HandlerMap["/ConfigFile/PullSites"] = handler.PullSites
	HandlerMap["/ConfigFile/ZeusUpdate"] = handler.ZeusUpdate
	HandlerMap["/ConfigFile/WebSiteContent"] = handler.WebSiteContent
	HandlerMap["/ConfigFile/Feedback"] = handler.Feedback
	HandlerMap["/ConfigFile/Shjia"] = handler.Shjia
}

func SetListenHandle() {
	for key, value := range HandlerMap {
		http.HandleFunc(key, value)
		glog.Info("router:", key, "  handle:", value)
	}
}
