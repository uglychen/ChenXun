package server

import (
	"encoding/json"
	"github.com/garyburd/redigo/redis"
	"github.com/golang/glog"
	"net/http"
	"runtime"
	"sync"
)

type orderName struct {
	id int
	typeName string
}

type siteItemsBoyd struct {
	SiteItems []*siteItems `json:"siteItems"`
	TypeName string `json:"typeName"`
}

type  siteItems struct{
	SiteImg string   `json:"siteImg"`
	SiteUrl string     `json:"siteUrl"`
	Introduction string  `json:"introduction"`
	SiteName string	 `json:"siteName"`
}

func PullSites(w http.ResponseWriter, req *http.Request) {

	//glog.Info(" http://192.168.2.153:5678/ConfigFile/PullSites")
	/*typename_map := make(map[int]string, 0)
	Cache_type_name_slice = make([]*orderName,0)
	{
		Query_typename, err := Db.Query(`SELECT orderIndex, typeName FROM proxypro.cb_sitetype order by orderIndex asc;`)
		if err != nil {
			glog.Info(err)
		}
		defer Query_typename.Close()
		for Query_typename.Next() {
			var tmp_name string
			var orderIndex int
			err := Query_typename.Scan(&orderIndex, &tmp_name)
			if err != nil {
				glog.Info(err)
			}
			typename_map[orderIndex] = tmp_name

			tmp := &orderName{}
			tmp.orderIndex = orderIndex
			tmp.typeName = tmp_name
			type_name_slice = append(type_name_slice, tmp)
		}
	}

	glog.Info("typename_map",Cache_typename_map)
	//mp_siteItemsBoyd_map := make(map[string]*siteItemsBoyd,0)
	tmp_siteItems_map := make(map[string]([]*siteItems),0)
	for _, name := range Cache_typename_map {
		tmp_siteItems_map[name] = []*siteItems{}
	}

	if len(Cache_typename_map) > 0 {
		Query_siteItems, err := Db.Query(`SELECT idSiteType, siteImg, siteUrl, introduction, siteName  FROM proxypro.cb_site where state = 1;`)
		if err != nil {
			glog.Info(err)
		}
		defer Query_siteItems.Close()

		for Query_siteItems.Next() {
			var idSiteType int
			var siteImg string
			var siteUrl string
			var introduction string
			var siteName string
			tmp_siteItems := &siteItems{}

			err := Query_siteItems.Scan(&idSiteType, &siteImg, &siteUrl, &introduction, &siteName)
			if err != nil {
				glog.Info(err)
			}
			tmp_siteItems.SiteImg = siteImg
			tmp_siteItems.SiteUrl = siteUrl
			tmp_siteItems.Introduction = introduction
			tmp_siteItems.SiteName = siteName
			glog.Info("tmp_siteItems: ", tmp_siteItems)

			if _, ok := Cache_typename_map[idSiteType]; ok{
				tmp := Cache_typename_map[idSiteType]
				tmp_siteItems_map[tmp] = append(tmp_siteItems_map[tmp], tmp_siteItems)
				//mp_siteItemsBoyd_map[tmp].TypeName = tmp
			}
		}
	}

	tmp_slice := make([]*siteItemsBoyd,0)
	for key ,value := range tmp_siteItems_map{
		tmp := &siteItemsBoyd{}
		tmp.TypeName = key
		tmp.SiteItems = value
		tmp_slice = append(tmp_slice,tmp)
	}

	//按照顺序 显示
	data_slice := make([]*siteItemsBoyd,0)
	for _, vlaue := range Cache_type_name_slice {
		for _,v := range  tmp_slice{
			if vlaue.typeName == v.TypeName{
				tmp := &siteItemsBoyd{}
				tmp.TypeName = vlaue.typeName
				tmp.SiteItems = v.SiteItems
				data_slice = append(data_slice,tmp)
			}
		}
	}*/

	sitetype_mutex.Lock()
	defer  sitetype_mutex.Unlock()
	//go CacheCb_sitetype()
	glog.Info("cache_PullSites:", cache_PullSites)

	retValue := &ResponseData{}
	retValue.Success = true
	retValue.Code = 0
	retValue.Data = cache_PullSites
	retValue.Message = ""
	bytes, _ := json.Marshal(retValue)
	w.Write([]byte(bytes))
}

var cache_PullSites []*siteItemsBoyd
var  sitetype_mutex sync.Mutex
func CacheCb_sitetype(){
	sitetype_mutex.Lock()
	defer  sitetype_mutex.Unlock()

	Cache_typename_map := make(map[int]string, 0)
	Cache_type_name_slice := make([]*orderName,0)
	Query_typename, err := Db.Query(`SELECT id, typeName FROM proxypro.cb_sitetype where  state = 1 order by orderIndex asc;`)
	if err != nil {
		glog.Info(err)
	}
	defer Query_typename.Close()
	for Query_typename.Next() {
		var tmp_name string
		var id int
		err := Query_typename.Scan(&id, &tmp_name)
		if err != nil {
			glog.Info(err)
		}
		Cache_typename_map[id] = tmp_name

		tmp := &orderName{}
		tmp.id = id
		tmp.typeName = tmp_name
		Cache_type_name_slice = append(Cache_type_name_slice, tmp)
	}

	glog.Info("typename_map",Cache_typename_map)
	//mp_siteItemsBoyd_map := make(map[string]*siteItemsBoyd,0)
	tmp_siteItems_map := make(map[string]([]*siteItems),0)
	for _, name := range Cache_typename_map {
		tmp_siteItems_map[name] = []*siteItems{}
	}

	if len(Cache_typename_map) > 0 {
		Query_siteItems, err := Db.Query(`SELECT idSiteType, siteImg, siteUrl, introduction, siteName  FROM proxypro.cb_site where state = 1 order by orderIndex asc;`)
		if err != nil {
			glog.Info(err)
		}
		defer Query_siteItems.Close()

		for Query_siteItems.Next() {
			var idSiteType int
			var siteImg string
			var siteUrl string
			var introduction string
			var siteName string
			tmp_siteItems := &siteItems{}

			err := Query_siteItems.Scan(&idSiteType, &siteImg, &siteUrl, &introduction, &siteName)
			if err != nil {
				glog.Info(err)
			}
			tmp_siteItems.SiteImg = siteImg
			tmp_siteItems.SiteUrl = siteUrl
			tmp_siteItems.Introduction = introduction
			tmp_siteItems.SiteName = siteName
			glog.Info("tmp_siteItems: ", tmp_siteItems)

			if _, ok := Cache_typename_map[idSiteType]; ok{
				tmp := Cache_typename_map[idSiteType]
				tmp_siteItems_map[tmp] = append(tmp_siteItems_map[tmp], tmp_siteItems)
				//mp_siteItemsBoyd_map[tmp].TypeName = tmp
			}
		}
	}

	tmp_slice := make([]*siteItemsBoyd,0)
	for key ,value := range tmp_siteItems_map{
		tmp := &siteItemsBoyd{}
		tmp.TypeName = key
		tmp.SiteItems = value
		tmp_slice = append(tmp_slice,tmp)
	}

	//按照顺序 显示
	cache_PullSites = make([]*siteItemsBoyd,0)
	for _, vlaue := range Cache_type_name_slice {
		for _,v := range  tmp_slice{
			if vlaue.typeName == v.TypeName{
				tmp := &siteItemsBoyd{}
				tmp.TypeName = vlaue.typeName
				tmp.SiteItems = v.SiteItems
				cache_PullSites = append(cache_PullSites,tmp)
			}
		}
	}

	bytes, err := json.Marshal(cache_PullSites)
	if err!=nil {
		glog.Info(err," json 序列化失败！")
	}
	glog.Info("json 序列化 cache_PullSites=> ", bytes)
	runtime.Gosched()
	conn,_ := redis.Dial("tcp", getRedisMasterAddress())
	conn.Do("set", "PullSites", string(bytes))
	conn.Close()
}