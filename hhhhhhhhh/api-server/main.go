package main

import (
	"api-server/router"
	"api-server/utils"
	"flag"
	"github.com/golang/glog"
	"net/http"
)



func init() {
	flag.Set("alsologtostderr", "true")
	flag.Set("log_dir", "false")
}

func main() {

	//开启handle listen
	flag.Parse()
	//defer glog.Flush()
	router.InitHandlerMap()
	router.SetListenHandle()


	glog.Info("address::::",utils.ThisAddress)


	if err := http.ListenAndServe(utils.ThisAddress, nil); err != nil {
		panic(err)
	}

}
