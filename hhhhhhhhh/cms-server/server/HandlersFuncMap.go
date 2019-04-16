package server

import (
	_ "github.com/go-sql-driver/mysql"
	"github.com/golang/glog"
	"net/http"
)

type HandlersFunc func(w http.ResponseWriter, req *http.Request)

var HandlerMap = make(map[string]HandlersFunc)

func InitHandlerMap() {
	//HandlerMap["/ConfigFile/ConfigJM"] = ConfigJM
	HandlerMap["/ConfigFile/ExportConfigJM"] = ExportConfigJM

	HandlerMap["/ConfigFile/SetPlatform"] = SetPlatform
	HandlerMap["/ConfigFile/GetPlatform"] = GetPlatform

	//HandlerMap["/ConfigFile/PullGrids"] = PullGrids

	//HandlerMap["/ConfigFile/ZeusUpdate"] = ZeusUpdate
	//HandlerMap["/ConfigFile/Feedback"] = Feedback
	//HandlerMap["/ConfigFile/WebSiteContent"] = WebSiteContent
	//HandlerMap["/ConfigFile/PullSites"] = PullSites

	//管理后台接口
	HandlerMap["/Config/ConfigFilePages"] = ConfigFilePages
	HandlerMap["/Config/ConfigFilePagesEnable"] = ConfigFilePagesEnable
	HandlerMap["/Config/ConfigFilePagesDelete"] = ConfigFilePagesDelete
	HandlerMap["/Config/ConfigFilePagesExport"] = ConfigFilePagesExport

	//重构配置文件页面(管理后台接口)
	//HandlerMap["/Config/ConfigFilePagesNew"] = ConfigFilePagesNew
	//HandlerMap["/Config/ConfigFilePagesNewEdit"] = ConfigFilePagesNewEdit
	//HandlerMap["/Config/GetCompanyName"] = GetCompanyName
	//HandlerMap["/Config/EditCompanyName"] = EditCompanyName
	//HandlerMap["/Config/DeleteCompanyName"] = DeleteCompanyName
	//HandlerMap["/Config/SvaeCompanyName"] = SvaeCompanyName
	//HandlerMap["/Config/AddConfigFilePages"] = AddConfigFilePages
	//HandlerMap["/Config/EidtConfigFilePages"] = EidtConfigFilePages

	HandlerMap["/Config/EidtTTL"] = EidtTTL
	HandlerMap["/Config/ChangePlatform"] = ChangePlatform
	//官网内容配置 OfficialWebsitePages
	HandlerMap["/Config/OfficialWebsitePages"] = OfficialWebsitePages
	HandlerMap["/Config/SavaeOfficialWebsitePages"] = SavaeOfficialWebsitePages

	HandlerMap["/Chrome/ApkUpdatePages"] = ApkUpdatePages
	HandlerMap["/Chrome/SaveApkUpdate"] = SaveApkUpdate     //version + 1
	HandlerMap["/Chrome/FeedbackPages"] = FeedbackPages
	HandlerMap["/Chrome/ChangeFeedbackState"] = ChangeFeedbackState


	HandlerMap["/Chrome/HomePages"] = HomePages
	HandlerMap["/Chrome/SaveHome"] = SaveHome
	HandlerMap["/Chrome/SaveHomeEdit"] = SaveHomeEdit
	HandlerMap["/Chrome/DeleteHome"] = DeleteHome

	HandlerMap["/Chrome/AllSiteTypes"] = AllSiteTypes
	HandlerMap["/Chrome/SitePages"] = SitePages1 // 编辑子站点 模糊搜素
	HandlerMap["/Chrome/SaveSite"] = SaveSite    // 新增站点
	HandlerMap["/Chrome/SiteEdit"] = SiteEdit    	   // 站点编辑
	HandlerMap["/Chrome/SiteDelete"] =SiteDelete	   //  站点删除
	HandlerMap["/Chrome/SiteOrder"] = SiteOrder	       //  站点排序

	HandlerMap["/Chrome/SaveSiteType"] = SaveSiteType 			//站点类别修改
	HandlerMap["/Chrome/SiteTypeDelete"] = SiteTypeDelete 		//站点类别删除
	HandlerMap["/Chrome/AddSiteType"] = AddSiteType  			//新增站点类别

	//新增新增修改宫格顺序接口
	HandlerMap["/Chrome/HomeOrderIndex"] = HomeOrderIndex

	//HandlerMap["/Login/Generate"] = Generate
	HandlerMap["/Login/Login"] = Login   //用户登陆
	HandlerMap["/Login/LoginOut"] = LoginOut
	HandlerMap["/Login/Register"] = Register

	HandlerMap["/System/AllManager"] = AllManager
	HandlerMap["/System/AddUser"] = AddUser

	HandlerMap["/Config/ConfigFilePagesNew"] = ConfigFilePagesNew1
	HandlerMap["/Config/ConfigEidt"] = ConfigEidt
	HandlerMap["/Config/SetCompanyName"] = SetCompanyName
	HandlerMap["/Config/SetRedirectUrl"] = SetRedirectUrl  // version + 1
	HandlerMap["/Config/SetDNS"] = SetDNS // version + 1
	HandlerMap["/Config/SetSubcompany"] = SetSubcompany  // version + 1
	HandlerMap["/Config/DelelteCompany"] = DelelteCompany  // version + 1
	HandlerMap["/Config/DelelteSubcompany"] = DelelteSubcompany // version + 1
	HandlerMap["/Config/AddCompany"] = AddCompany
	HandlerMap["/Config/AddSubCompany"] = AddSubCompany
	HandlerMap["/Config/ConfigPlatformPages"] = ConfigPlatformPages
}

func SetListenHandle() {
	for key, value := range HandlerMap {
		http.HandleFunc(key, value)
		glog.Info("router:", key, "  handle:", value)
	}
}