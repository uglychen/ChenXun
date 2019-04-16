package server

import (
	"encoding/base64"
	"errors"
	"github.com/golang/glog"
	"net/http"
	"strconv"
	"time"
)

func CheckToken(token string, w http.ResponseWriter, req *http.Request)(user_uuid string, err error){
	req.ParseForm()
	glog.Info("rec token:", token, " token length:", len(token))

	if len(token)  > 18  {
		s, err1 := base64.StdEncoding.DecodeString(token)
		if err1 != nil {
			glog.Info(err1)
			return  "", err1
		}
		glog.Info("token: ", string(s))
		time_token := s[0:10]
		user_uuid = string(s[18:])
		glog.Info("time_token: ", string(time_token))
		glog.Info("user_uuid: ", user_uuid)
		time_now := time.Now().Unix()
		i, _ := strconv.ParseInt(string(time_token), 10, 64)

		//token过期处理
		if time_now-i >= 24*60*60 {
			glog.Info("token 过期: ", (time.Unix(i, 0).Format("2006-01-02 03:04:05 PM")))
			//过期 退出登录
			stmt, err := Db.Prepare(`update proxypro.userInfo set loginStatus = ?  where uuid = ?`)
			res, err := stmt.Exec(0, user_uuid)
			glog.Info(err)
			glog.Info(res)
			stmt.Close()
			glog.Info("============================================",user_uuid)
			return "", errors.New("token error: token 过期 请重新登")
		} else {
			return  user_uuid, nil
		}
	} else {
		return "", errors.New("token error")
		/*retValue := NewBaseJsonData()
		retValue.Code = 900
		retValue.Message = "token error"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
		return*/
	}
	return "", errors.New("token error: no token")
}