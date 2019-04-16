package server

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"github.com/golang/glog"
	"io/ioutil"
	"net/http"
	"net/url"
	"strconv"
	"strings"
)

type responseConfigJM struct {
	Success 	string 			`json:"success"`
	Code		int				`json:"code"`
	Data		interface{}		`json:"data"`
	Message 	string			`json:"message"`
}

type dataSlice struct {
	LoadTime  int   `json:"load-time"`
	Redirect  []string   `json:"redirect"`
	Dnsproxy  []string   `json:"dns-proxy"`
	Version  int  `json:"version"`
}

type reqexportConfigJM struct {
	PlatformCode int
	Token string
}


//修改后的ConfigJM
func ExportConfigJM(w http.ResponseWriter, req *http.Request) {

	glog.Info("ExportConfigJM")

	if req.Method == "POST" {
		result, _ := ioutil.ReadAll(req.Body)
		req.Body.Close()
		glog.Info("reqsaveSite recv body:", string(result))
		var m reqexportConfigJM
		e := json.Unmarshal([]byte(result), &m)
		if e != nil {
			retValue := &ResponseData{}
			retValue.Success = false
			retValue.Code = 0
			retValue.Message = "Request Method error: POST"
			retValue.Data = ""
			bytes, _ := json.Marshal(retValue)
			w.Write([]byte(bytes))
			return
		}

		{
			//检查token过期情况
			var user_uuid string
			var err_token error
			{
				user_uuid, err_token = CheckToken(m.Token, w, req)
				if err_token != nil {
					glog.Info(err_token)
					retValue := NewBaseJsonData()
					retValue.Code = 1000
					retValue.Message = err_token.Error()
					retValue.Data = err_token.Error()
					bytes, _ := json.Marshal(retValue)
					w.Write([]byte(bytes))
					return
				}
			}
			glog.Info("user_uuid: ", user_uuid)
		}

		var version_code string
		var loadTime string
		{

			rows, err := Db.Query("SELECT version_code, loadTime FROM proxypro.config_version where platform = ?", m.PlatformCode)
			glog.Info(err)
			for rows.Next() {
				err := rows.Scan(&version_code, &loadTime)
				if err != nil {
					glog.Info(err)
				}
			}
			rows.Close()
		}

		//判断 platformCode 对应的 平台表 中的 status 状态是启用还是禁用
		tmp_data := &DataSlice{}
		{
			tmp_data.Dnsproxy = make([]string, 0)
			tmp_data.Redirect = make([]string, 0)
			tmp_data.LoadTime, _ = strconv.Atoi(loadTime)
			tmp_data.Version, _ = strconv.Atoi(version_code)

			all_tmp_map_slice := make([](map[string]string), 0)
			{
				// 从数据库中查询中所有的 webConfig 　redirecturl dns url
				//all_tmp_map_slice := make([](map[string]string), 0)
				{
					var Queryconfig *sql.Rows
					var err error

					var tmp_PlatformCode int
					if m.PlatformCode == 1{
						tmp_PlatformCode = 0
					}

					Queryconfig, err = Db.Query(`SELECT companyNameID, subcompany, platform, createTime, URL, redirectUrl, 
							DNS FROM proxypro.webConfig  where estatus=1 and platform =?;`, tmp_PlatformCode)
					if err != nil {
						glog.Info(err)
					}

					for Queryconfig.Next() {
						var companyNameID string
						var subcompany string
						var redirectUrl string
						var DNS string
						var platform string
						var createTime string
						var URL string
						err := Queryconfig.Scan(&companyNameID, &subcompany, &platform, &createTime, &URL, &redirectUrl, &DNS)
						if err != nil {
							glog.Info(err)
						}
						tmp_map := make(map[string]string, 0)
						tmp_map["companyNameID"] = companyNameID
						tmp_map["subcompanyName"] = subcompany
						tmp_map["redirectUrl"] = redirectUrl
						tmp_map["DNS"] = DNS
						tmp_map["platform"] = platform
						tmp_map["createTime"] = createTime
						tmp_map["URL"] = URL
						all_tmp_map_slice = append(all_tmp_map_slice, tmp_map)
					}
					Queryconfig.Close()
				}
			}

			glog.Info("all_tmp_map_slice length", len(all_tmp_map_slice))

			for _, value := range all_tmp_map_slice {
				var httpstr []string
				var httpsstr []string
				nottwostrss := map[string][]string{}
				twostrss := map[string][]string{}

				//如果重定向 　并且　　DNS里为空，则该条为空，不用塞入最后的data里
				if len(value["redirectUrl"]) == 0 && len(value["DNS"]) == 0 {
					continue
				}
				line := value["redirectUrl"]
				domainname := strings.Split(line, "|")
				for _, v1 := range domainname {
					if strings.HasPrefix(v1, "http:") || strings.HasPrefix(v1, "https:") {

						if strings.HasPrefix(v1, "http://") {
							httpstr = append(httpstr, v1[len("http://"):])
						} else if strings.HasPrefix(v1, "https://") {
							httpsstr = append(httpsstr, v1[len("https://"):])
						}
					} else {
						//处理非http开头
						split := strings.Split(v1, ".")
						//处理两段
						if len(split) == 2 {
							twostrss[split[1]] = append(twostrss[split[1]], split[0])
						} else if len(split) == 3 && strings.HasPrefix(v1, "www.") {
							// 处理以 `www.` 开头的3段
							twostrss[split[2]] = append(twostrss[split[2]], split[1])

						} else {
							//处理非两段
							if len(split) > 1 {
								//fmt.Println(`split[len(split)-1],line[:strings.LastIndex(line, ".")]::`, split[len(split)-1], v1[:strings.LastIndex(v1, ".")])
								nottwostrss[split[len(split)-1]] = append(nottwostrss[split[len(split)-1]], v1[:strings.LastIndex(v1, ".")])
							}
						}
					}

				}

				//---------------http  begin----------------
				myhttp := ""
				index := 0
				for _, v := range httpstr {
					index++
					if len(httpstr) == 1 {
						myhttp += v
					} else if len(httpstr) > 1 {
						if len(httpstr) == index {
							myhttp += "(" + v + ")"
						} else {
							myhttp += "(" + v + ")" + "|"
						}
					}
				}
				myhttp1 := "http://" + "(" + myhttp + ")"
				myhttp1 = strings.Replace(myhttp1, ".", "\\.", -1)
				if len(myhttp) > 0 {
					tmp_data.Redirect = append(tmp_data.Redirect, myhttp1+`->`+value["URL"])
				}

				//---------------http end----------------

				//---------------https 　begin----------------
				myHttps := ""
				index1 := 0
				for _, v := range httpsstr {
					index1++
					if len(myHttps) == 1 {
						myHttps += v
					} else if len(myHttps) > 1 {
						if len(myHttps) == index1 {
							myHttps += "(" + v + ")"
						} else {
							myHttps += "(" + v + ")" + "|"
						}
					}
				}
				myHttps1 := "https://" + "(" + myHttps + ")"
				myHttps1 = strings.Replace(myHttps1, ".", "\\.", -1)
				if len(myHttps) > 0 {
					tmp_data.Redirect = append(tmp_data.Redirect, myHttps1+`->`+value["URL"])

				}

				//---------------https 　end----------------

				//---------------handle 2段　begin----------------
				twostrs := ""
				//>0
				if len(twostrss) > 0 {
					indextwostr := 0
					for k, v := range twostrss {
						indextwostr++
						tmpst := ""
						for i, j := range v {
							if i == len(v)-1 {
								tmpst += j
							} else {
								tmpst += j + "|"
							}
						}

						if indextwostr == len(twostrss) {
							if len(v) == 1 {
								twostrs += "(" + tmpst + `.` + k + ")"
							} else {
								twostrs += "(" + `(` + tmpst + `).` + k + ")"
							}

						} else {
							if len(v) == 1 {
								twostrs += "(" + tmpst + `.` + k + ")" + "|"
							} else {
								twostrs += "(" + `(` + tmpst + `).` + k + ")" + "|"
							}
						}
					}
					if len(strings.Split(twostrs, "|")) > 1 {
						twostrs = `(www.)?(` + twostrs + ")"
					} else {
						twostrs = `(www.)?` + twostrs

					}
					twostrs = strings.Replace(twostrs, ".", "\\.", -1)
				}
				//---------------handle 2段　end----------------

				//---------------handle not 2段　begin----------------

				nottwostrs := ""
				//>0
				if len(nottwostrss) > 0 {
					indexnottwostr := 0
					for k, v := range nottwostrss {
						indexnottwostr++
						tmpst := ""
						for i, j := range v {
							if i == len(v)-1 {
								tmpst += j
							} else {
								tmpst += j + "|"
							}
						}

						if indexnottwostr == len(nottwostrss) {
							//TODO　按照这种
							if len(v) == 1 {
								nottwostrs += "(" + tmpst + `.` + k + ")"
							} else {
								nottwostrs += "(" + `(` + tmpst + `).` + k + ")"
							}
						} else {
							if len(v) == 1 {
								nottwostrs += "(" + tmpst + `.` + k + ")" + "|"
							} else {
								nottwostrs += "(" + `(` + tmpst + `).` + k + ")" + "|"
							}
						}
					}
					if len(strings.Split(nottwostrs, "|")) > 1 {
						nottwostrs = "(" + nottwostrs + ")"
					} else {
						nottwostrs = nottwostrs
					}
					nottwostrs = strings.Replace(nottwostrs, ".", "\\.", -1)
				}

				twoAndNotTwo := ""
				if len(twostrs) > 0 && len(nottwostrs) > 0 {
					twoAndNotTwo = `((` + twostrs + `)|(` + nottwostrs + `))`

				} else if len(twostrs) == 0 {
					twoAndNotTwo = nottwostrs
				} else if len(nottwostrs) == 0 {
					twoAndNotTwo = twostrs
				}
				if len(twoAndNotTwo) != 0 {
					twoAndNotTwo = twoAndNotTwo + `->` + value["URL"]
					tmp_data.Redirect = append(tmp_data.Redirect, twoAndNotTwo)
				}

				// 对于hostname进行处理
				u, err := url.Parse(value["URL"])
				if err != nil {
					panic(err)
				}

				// 带有端口号的情况
				if strings.Contains(u.Host, ":") {
					split := strings.Split(u.Host, ":")

					if len(split) > 1 {

						if strings.HasPrefix(split[0], `www.`) && len(split) == 3 {

							split[0] = strings.Replace(split[0], `.`, `\.`, -1)
							split[0] = strings.Replace(split[0], `www.`, `(www\\.)?`, -1)
							tmp_Dnsproxy := split[0] + ":" + value["DNS"]
							tmp_data.Dnsproxy = append(tmp_data.Dnsproxy, tmp_Dnsproxy)
						} else {

							split[0] = strings.Replace(split[0], `.`, `\.`, -1)
							tmp_Dnsproxy := `(www\.)?` + split[0] + ":" + value["DNS"]
							tmp_data.Dnsproxy = append(tmp_data.Dnsproxy, tmp_Dnsproxy)
						}
					}
				} else {
					//不带有端口号的情况
					if strings.HasPrefix(u.Host, `www.`) {
						u.Host = strings.Replace(u.Host, `www.`, `(www.)?`, -1)
						u.Host = strings.Replace(u.Host, `.`, `\.`, -1)
						tmp_data.Dnsproxy = append(tmp_data.Dnsproxy, u.Host+":"+value["DNS"])
					} else {
						u.Host = strings.Replace(u.Host, `.`, `\.`, -1)
						tmp_Dnsproxy := "(www\\.)?" + u.Host + ":" + value["DNS"]
						tmp_data.Dnsproxy = append(tmp_data.Dnsproxy, tmp_Dnsproxy)
					}
				}
			}
		}
		//tmp_Redirect := value["redirectUrl"]+ `/?->` + value["URL"]
		//tmp_data.Redirect = append(tmp_data.Redirect, tmp_Redirect)
		///
		//Aeskey := "9XPghLnqdLFTro5o"
		//data_json, _ := json.Marshal(tmp_data)
		//content := strings.Replace(string(data_json), "\\u003e", ">", -1)
		//glog.Info("data_json: ", string(content))
		//crypted := aescrypto.Encrypt(aescrypto.PKCS7Pad(data_json), key)
		//crypted, _ := aescrypto.AesEcbPkcs5Encrypt([]byte(content), []byte(Aeskey))
		//glog.Info("aes: ", string(crypted))
		//encodeString := base64.StdEncoding.EncodeToString(crypted)
		//glog.Info("aes ecb base64 encrypt: ", encodeString)


		retValue := &ResponseData{}
		retValue.Success = true
		retValue.Code = 0
		retValue.Data = tmp_data
		retValue.Message = ""
		bytes2, _ := json.Marshal(retValue)
		w.Write(bytes2)
	} else {
		retValue := &ResponseData{}
		retValue.Success = false
		retValue.Code = 0
		retValue.Message = "Request Method error: POST"
		retValue.Data = ""
		bytes, _ := json.Marshal(retValue)
		w.Write([]byte(bytes))
	}
}

func handleNotStartWithHttp(v1 string, twostrss map[string][]string, nottwostrss map[string][]string) {
	split := strings.Split(v1, ".")
	//fmt.Println(reflect.TypeOf(split),split)
	//处理两段
	if len(split) == 2 {
		twostrss[split[1]] = append(twostrss[split[1]], split[0])
	} else if len(split) == 3 && strings.HasPrefix(v1, "www.") {
		// 处理以 `www.` 开头的3段
		twostrss[split[2]] = append(twostrss[split[2]], split[1])

	} else {
		//处理非两段
		if len(split) > 1 {
			fmt.Println(`split[len(split)-1],line[:strings.LastIndex(line, ".")]::`, split[len(split)-1], v1[:strings.LastIndex(v1, ".")])
			nottwostrss[split[len(split)-1]] = append(nottwostrss[split[len(split)-1]], v1[:strings.LastIndex(v1, ".")])
		}
	}
}

func handleStartWithHttp(v1 string, httpstr []string, httpsstr []string) ([]string, []string) {
	if strings.HasPrefix(v1, "http://") {
		fmt.Println("http://")
		fmt.Println(`v1[len("http://"):]:`, v1[len("http://"):])
		httpstr = append(httpstr, v1[len("http://"):])
	} else if strings.HasPrefix(v1, "https://") {
		fmt.Println("https://")
		fmt.Println(`v1[len("https://"):]`, v1[len("https://"):])
		httpsstr = append(httpsstr, v1[len("https://"):])
	}
	if len(v1) == 0 {
		fmt.Println("*****************************************")
	}
	return httpstr, httpsstr
}

func handleNotTwoStr(nottwostrss map[string][]string) string {
	nottwostrs := ""
	fmt.Println(len(nottwostrss))
	//>0
	if len(nottwostrss) > 0 {
		fmt.Println("nottwostrss:", nottwostrss)
		//twostrs := `(www\\.)?`

		indexnottwostr := 0
		for k, v := range nottwostrss {
			indexnottwostr++
			tmpst := ""
			for i, j := range v {
				if i == len(v)-1 {
					tmpst += j
					//fmt.Println(j)
				} else {
					tmpst += j + "|"
					//fmt.Println(j)
				}
			}

			fmt.Println("len(twostrss):", indexnottwostr, "--", len(nottwostrss))
			if indexnottwostr == len(nottwostrss) {
				fmt.Println(tmpst, len(v))
				fmt.Println("if tmpst:", tmpst)

				//TODO　按照这种
				if len(v) == 1 {
					nottwostrs += "(" + tmpst + `.` + k + ")"
				} else {
					nottwostrs += "(" + `(` + tmpst + `).` + k + ")"
				}

			} else {
				fmt.Println("else tmpst:", tmpst, " len(v):", len(v))
				if len(v) == 1 {
					nottwostrs += "(" + tmpst + `.` + k + ")" + "|"
				} else {
					nottwostrs += "(" + `(` + tmpst + `).` + k + ")" + "|"
				}
			}
		}
		if len(strings.Split(nottwostrs, "|")) > 1 {

			nottwostrs = "(" + nottwostrs + ")"
			if nottwostrs == `(aaa.xasdf.com)|(asd.44.abc)|(a.b.c.xyz))` {
				fmt.Println(`1111(aaa.xasdf.com)|(asd.44.abc)|(a.b.c.xyz))****`)
			}
		} else {

			nottwostrs = nottwostrs
			if nottwostrs == `(aaa.xasdf.com)|(asd.44.abc)|(a.b.c.xyz))` {
				fmt.Println(`222(aaa.xasdf.com)|(asd.44.abc)|(a.b.c.xyz))****`)
			}

		}
		//twostrs=twostrs+")"
		nottwostrs = strings.Replace(nottwostrs, ".", "\\.", -1)
		fmt.Println("nottwostrs:||::", nottwostrs)
		if nottwostrs == `(aaa\.xasdf\.com)|(asd\.44\.abc)|(a\.b\.c\.xyz))` {
			fmt.Println(`(aaa\.xasdf\.com)|(asd\.44\.abc)|(a\.b\.c\.xyz))----------`)
		}

	}
	return nottwostrs
}

func handleTwoStr(twostrss map[string][]string) string {
	fmt.Println(len(twostrss))
	twostrs := ""
	//>0
	if len(twostrss) > 0 {
		fmt.Println("twostrss:", twostrss)
		//twostrs := `(www\\.)?`

		indextwostr := 0
		for k, v := range twostrss {
			indextwostr++
			tmpst := ""
			for i, j := range v {
				if i == len(v)-1 {
					tmpst += j
					//fmt.Println(j)
				} else {
					tmpst += j + "|"
					//fmt.Println(j)
				}
			}

			fmt.Println("len(twostrss):", indextwostr, "--", len(twostrss))
			if indextwostr == len(twostrss) {
				fmt.Println(tmpst, len(v))
				fmt.Println("if tmpst:", tmpst)

				if len(v) == 1 {
					twostrs += "(" + tmpst + `.` + k + ")"
				} else {
					twostrs += "(" + `(` + tmpst + `).` + k + ")"
				}

			} else {
				fmt.Println("else tmpst:", tmpst, " len(v):", len(v))
				if len(v) == 1 {
					twostrs += "(" + tmpst + `.` + k + ")" + "|"
				} else {
					twostrs += "(" + `(` + tmpst + `).` + k + ")" + "|"
				}
			}
		}
		if len(strings.Split(twostrs, "|")) > 1 {
			twostrs = `(www.)?(` + twostrs + ")"
		} else {
			twostrs = `(www.)?` + twostrs

		}
		//twostrs=twostrs+")"
		twostrs = strings.Replace(twostrs, ".", "\\.", -1)
		fmt.Println("twostrs:||::", twostrs)

	}
	return twostrs
}

func handleHttps(httpsstr []string, tmp_data *DataSlice, value map[string]string) {
	myHttps := ""
	index1 := 0
	for _, v := range httpsstr {
		index1++
		if len(myHttps) == 1 {
			myHttps += v
		} else if len(myHttps) > 1 {
			if len(myHttps) == index1 {
				myHttps += "(" + v + ")"
			} else {
				myHttps += "(" + v + ")" + "|"
			}
		}
	}
	myHttps1 := "https://" + "(" + myHttps + ")"
	myHttps1 = strings.Replace(myHttps1, ".", "\\.", -1)
	if len(myHttps) > 0 {
		tmp_data.Redirect = append(tmp_data.Redirect, myHttps1+`->`+value["URL"])

	}
}

func handleHttp(httpstr []string, tmp_data *DataSlice, value map[string]string) {
	myhttp := ""
	index := 0
	fmt.Println("httpstr:::", httpstr)
	for _, v := range httpstr {
		index++
		if len(httpstr) == 1 {
			myhttp += v
		} else if len(httpstr) > 1 {
			if len(httpstr) == index {
				myhttp += "(" + v + ")"
			} else {
				myhttp += "(" + v + ")" + "|"
			}
		}
	}
	myhttp1 := "http://" + "(" + myhttp + ")"
	myhttp1 = strings.Replace(myhttp1, ".", "\\.", -1)
	if len(myhttp) > 0 {
		tmp_data.Redirect = append(tmp_data.Redirect, myhttp1+`->`+value["URL"])

	}
}