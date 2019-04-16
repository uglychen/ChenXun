package server

import (
	"encoding/json"
	"fmt"
	"github.com/garyburd/redigo/redis"
	"github.com/golang/glog"
	"time"
)

type Feedbackdata struct{
	Content string`json:"content"`
	Img string`json:"img"`
	Contact string`json:"contact"`
	OsName string`json:"osName"`
	OsVersion string`json:"osVersion"`
	PhoneModel string`json:"phoneModel"`
}

func Feedback(result []byte){

	//result, _ := ioutil.ReadAll(req.Body)
	//req.Body.Close()
	//glog.Info("LoginAndOut recv body:", string(result))
	var m Feedbackdata
	json.Unmarshal(result, &m)
	glog.Info("result:", m)

	t:= time.Now().Unix()
	sql := `INSERT INTO proxypro.cb_feedback (content, img, contact, osName, osVersion ,phoneModel,createTime,updateTime)
                   VALUES (?, ?, ?, ?, ?, ?, ?, ?)`
	insert, err1 := Db.Exec(sql, m.Content, m.Img, m.Contact, m.OsName, m.OsVersion, m.PhoneModel, t, t)
	glog.Info(err1)
	if err1 != nil{
		glog.Error("error info: ", err1)
		return
	}

	insert_id,_ := insert.LastInsertId()
	glog.Info("lastinsertid: ", insert_id)

	Feedback_mutex.Lock()
	defer  Feedback_mutex.Unlock()
	go FeedbackCache()
}

func Subs() {  //订阅者
	conn := RedisPool.Get()
	psc := redis.PubSubConn{conn}
	psc.Subscribe("Feedback")
	for {
		switch v := psc.Receive().(type) {
		case redis.Message: //单个订阅
			glog.Info(" 订阅者　收到的消息　Message　%s: message: %s\n", v.Channel, v.Data)
			Feedback(v.Data)
		case redis.Subscription:
			fmt.Printf("订阅者　Subscription　%s: %s %d\n", v.Channel, v.Kind, v.Count)
		case error:
			fmt.Println(v)
			return
		}
	}
}