package main

import (
	handle "cms-server/server"
	"time"

	"flag"
	"net/http"
)

func init() {
	flag.Set("alsologtostderr", "true")
	flag.Set("log_dir", "false")
	//flag.StringVar(&confPath, "conf", "./biz_server.toml", "config path")
}

func main() {

	//开启handle listen
	flag.Parse()
	//defer glog.Flush()
	handle.InitHandlerMap()
	handle.SetListenHandle()

	go handle.CacheConfigJM()
	go handle.CachePullGrids()
	go handle.CacheCb_sitetype()
	go handle.Cache_WebSiteContent()
	go handle.CacheZeusUpdate()

	go handle.SaveWebConfig()
	go handle.FeedbackCache()

	go handle.Subs()

	//go TimeTask()
	if err := http.ListenAndServe("0.0.0.0:5677", nil); err != nil {
		panic(err)
	}
}

func TimeTask() {
	timer2 := time.NewTicker(5*60*time.Second)
	for {
		select {
		case <-timer2.C:
			go handle.SaveWebConfig()
			go handle.FeedbackCache()
			go handle.CachePullGrids()
			go handle.CacheCb_sitetype()
			go handle.Cache_WebSiteContent()
			go handle.CacheConfigJM()
			go handle.CacheZeusUpdate()
		}
	}
}
