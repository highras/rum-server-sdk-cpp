#include <fstream>
#include <streambuf>
#include "md5.h"
#include "hex.h"
#include "MidGenerator.h"
#include "RUMServerClient.h"
#include "FPLog.h"
#include "msec.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

using namespace rum;

const string SDK_SOURCE = "cpp";

RUMServerClient::RUMServerClient(int32_t pid, const string& secret, const string& endpoint, bool reconnect, int32_t timeout): _pid(pid), _secret(secret), _rid(""), _sid(0)
{
    MidGenerator::init(slack_real_msec());
    _client = TCPClient::createClient(endpoint, reconnect);
    if (_client) {
        _client->setQuestTimeout(timeout);
    }
}

RUMServerClient::~RUMServerClient()
{

}

bool RUMServerClient::connected()
{
    return _client->connected();
}

const string& RUMServerClient::endpoint()
{
    return _client->endpoint();
}

void RUMServerClient::setQuestTimeout(int seconds)
{
    _client->setQuestTimeout(seconds);
}

void RUMServerClient::setAutoReconnect(bool autoReconnect)
{
    _client->setAutoReconnect(autoReconnect);
}

bool RUMServerClient::enableEncryptorByDerData(const string &derData, bool packageMode, bool reinforce)
{
    return _client->enableEncryptorByDerData(derData, packageMode, reinforce);
}

bool RUMServerClient::enableEncryptorByPemData(const string &PemData, bool packageMode, bool reinforce)
{
    return _client->enableEncryptorByPemData(PemData, packageMode, reinforce);
}

bool RUMServerClient::enableEncryptorByDerFile(const char *derFilePath, bool packageMode, bool reinforce)
{
    return _client->enableEncryptorByDerFile(derFilePath, packageMode, reinforce);
}

bool RUMServerClient::enableEncryptorByPemFile(const char *pemFilePath, bool packageMode, bool reinforce)
{
    return _client->enableEncryptorByPemFile(pemFilePath, packageMode, reinforce);
}

void RUMServerClient::enableEncryptor(const string& curve, const string& peerPublicKey, bool packageMode, bool reinforce)
{
    _client->enableEncryptor(curve, peerPublicKey, packageMode, reinforce);
}

string RUMServerClient::_calcMD5(const string& content)
{
    unsigned char digest[16];
    md5_checksum(digest, content.c_str(), content.size());
    char hexstr[32 + 1];
    Hexlify(hexstr, digest, sizeof(digest));
    return string(hexstr);
}

void RUMServerClient::setRumId(const string& rid)
{
    _rid = rid;
}

void RUMServerClient::setSessionId(int64_t sid)
{
    _sid = sid;
}

FPQuestPtr RUMServerClient::_getRumQuest(const vector<RumEventPtr>& events)
{

    if (_rid.empty())
        _rid = to_string(MidGenerator::genMid());
    if (_sid == 0)
        _sid = MidGenerator::genMid();

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Value v;
    v.SetInt(_pid);
    document.AddMember("pid", v, document.GetAllocator());

    int32_t salt = slack_real_sec();
    string sign = _calcMD5(to_string(_pid) + ":" + _secret + ":" + to_string(salt));

    v.SetInt(salt);
    document.AddMember("salt", v, document.GetAllocator());
    v.SetString(sign.c_str(), sign.length(), document.GetAllocator());
    document.AddMember("sign", v, document.GetAllocator());

    v.SetArray();
    for (auto event: events) {
        rapidjson::Value obj, vv;
        obj.SetObject();

        string ev = event->getEventName();
        vv.SetString(ev.c_str(), ev.length(), document.GetAllocator());
        obj.AddMember("ev", vv, document.GetAllocator());
        vv.SetInt64(_sid);
        obj.AddMember("sid", vv, document.GetAllocator());
        vv.SetString(_rid.c_str(), _rid.length(), document.GetAllocator());
        obj.AddMember("rid", vv, document.GetAllocator());
        vv.SetInt(slack_real_sec());
        obj.AddMember("ts", vv, document.GetAllocator());
        vv.SetInt64(MidGenerator::genMid());
        obj.AddMember("eid", vv, document.GetAllocator());
        vv.SetString(SDK_SOURCE.c_str(), SDK_SOURCE.length(), document.GetAllocator());
        obj.AddMember("source", vv, document.GetAllocator());
        
        rapidjson::Document attrsDoc;
        string json = event->json();
        if (!attrsDoc.Parse(json.c_str()).HasParseError()) {
            rapidjson::Value copyValue(attrsDoc, document.GetAllocator());
            obj.AddMember("attrs", copyValue, document.GetAllocator());
        }
        v.PushBack(obj, document.GetAllocator());
    }
    document.AddMember("events", v, document.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    FPQWriter qw("adds", buffer.GetString()); 
    return qw.take();
}

bool RUMServerClient::_checkAnswerError(FPAnswerPtr answer, QuestResult& result, int32_t errorCode)
{
	if (!answer)
    {
        result.errorCode = -1;
        result.errorInfo = "answer is null";
        return true;
    }
    if (errorCode > 0 || answer->status())
    {
        FPAReader ar(answer);
        result.errorCode = ar.getInt("code");
        result.errorInfo = ar.getString("ex");
        return true;
    }
    return false;
}

QuestResult RUMServerClient::sendCustomEvent(RumEventPtr event, int32_t timeout)
{
    FPQuestPtr quest = _getRumQuest({ event });
    FPAnswerPtr answer = _client->sendQuest(quest, timeout);
    QuestResult result;
    _checkAnswerError(answer, result);
    return result;
}

void RUMServerClient::sendCustomEvent(RumEventPtr event, std::function<void (QuestResult result)> callback, int32_t timeout)
{
	FPQuestPtr quest = _getRumQuest({ event });
	bool status = _client->sendQuest(quest, [this, callback](FPAnswerPtr answer, int32_t errorCode) {
		QuestResult result;
        if (errorCode != FPNN_EC_OK)
            _checkAnswerError(answer, result);
        callback(result);
	}, timeout);

	if (!status)
    {
        QuestResult result;
        result.errorCode = -1;
        result.errorInfo = "socket maybe closed";
        callback(result);
    }
}

QuestResult RUMServerClient::sendCustomEvents(const vector<RumEventPtr>& events, int32_t timeout)
{
	FPQuestPtr quest = _getRumQuest(events);
	FPAnswerPtr answer = _client->sendQuest(quest, timeout);
    QuestResult result;
    _checkAnswerError(answer, result);
    return result;
}

void RUMServerClient::sendCustomEvents(const vector<RumEventPtr>& events, std::function<void (QuestResult result)> callback, int32_t timeout)
{
	FPQuestPtr quest = _getRumQuest(events);
	bool status = _client->sendQuest(quest, [this, callback](FPAnswerPtr answer, int32_t errorCode) {
		QuestResult result;
        if (errorCode != FPNN_EC_OK)
            _checkAnswerError(answer, result);
        callback(result);
	}, timeout);

	if (!status)
    {
        QuestResult result;
        result.errorCode = -1;
        result.errorInfo = "socket maybe closed";
        callback(result);
    }
}

