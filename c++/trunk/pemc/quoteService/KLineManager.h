#ifndef _KLINE_MANAGER_H_
#define _KLINE_MANAGER_H_
#include "QuoteDataDef.h"
#include "Sharingline.h"
#include <string>
#include <map>
#include <memory>
#include <jsoncpp/json.h>
using namespace std;
class KLineBase;
class SharingLine;

/** k线管理器
*/
class KLineManager 
{
public:
    KLineManager();
    ~KLineManager();

    //运行实时行情
    string runQuoteReal(const QuoteMsg& quoteMsg);

    void Init();
    void UnInit();

    void PushQuoteMsg(const QuoteMsg& quoteMsg);

    void GetKLineData(const string& productId, const string& kLineType, const string& startTime, Json::Value& data);

    void GetShareLineDate(const string& productId, std::map<int, ShareLineData>& tmp_map);

private:
    typedef map<string, shared_ptr<KLineBase>> KLineList;
    KLineList _kLineList;

    //分时线
    shared_ptr<SharingLine> _ptrShareLine;
};
#endif