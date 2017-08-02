package main

import (
    //"database/sql"
    "encoding/json"
    "fmt"
    _ "github.com/go-sql-driver/mysql"
    "log"
    "net/http"
)

type info struct {
    Image_url string
    Icon_name string
    Url       string
}

type icon_data struct {
    Version string      `json:"version"`
    Data    interface{} `json:"data"`
}

func NewIconData() *icon_data {
    return &icon_data{}
}

func icon(w http.ResponseWriter, req *http.Request) {

    str_sql := "select version, image_url, icon_name, url from icon"
    rows, err := Db.Query(str_sql)
    if err != nil {
        log.Println("err:", err)
    }

    var version string
    var image_url string
    var icon_name string
    var url string

    slice := make([]*info, 0)
    for rows.Next() {

        err := rows.Scan(&version, &image_url, &icon_name, &url)
        if err != nil {
            log.Println("err:", err)
        }

        tmp := &info{image_url, icon_name, url}
        slice = append(slice, tmp)
    }

    retData := NewIconData()
    retData.Version = version
    retData.Data = slice
    bytes, err := json.Marshal(retData)
    fmt.Fprint(w, string(bytes), "\n")

}
