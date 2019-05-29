#ifndef RUM_SERVER_CLIENT_H
#define RUM_SERVER_CLIENT_H
#include "fpnn.h"
#include "RumEvent.h"
#include "QuestResult.h"
#include "RUMServerMonitor.h"

using namespace std;
using namespace fpnn;

namespace rum 
{

    class RUMException : exception
    {
    public:
        RUMException(const std::string& msg) { _msg = msg; }
        ~RUMException() throw() {}
        const char* what() const throw() { return _msg.c_str(); }
    private:
        std::string _msg;
    };

    class RUMServerClient
    {
    public:
        RUMServerClient(int32_t pid, const string& secret, const string& endpoint, bool reconnect, int32_t timeout);
        ~RUMServerClient();

        bool connected();
        const string& endpoint();
        void setQuestTimeout(int seconds);
        void setAutoReconnect(bool autoReconnect);
        void setServerMonitor(shared_ptr<RUMServerMonitor> serverMonitor);

        bool enableEncryptorByDerData(const string &derData, bool packageMode = true, bool reinforce = false);
        bool enableEncryptorByPemData(const string &PemData, bool packageMode = true, bool reinforce = false);
        bool enableEncryptorByDerFile(const char *derFilePath, bool packageMode = true, bool reinforce = false);
        bool enableEncryptorByPemFile(const char *pemFilePath, bool packageMode = true, bool reinforce = false);
        void enableEncryptor(const string& curve, const string& peerPublicKey, bool packageMode = true, bool reinforce = false);

        void setRumId(const string& rid);
        void setSessionId(int64_t sid);

        QuestResult sendCustomEvent(RumEventPtr event, int32_t timeout = 0);
        void sendCustomEvent(RumEventPtr event, std::function<void (QuestResult result)> callback, int32_t timeout = 0);
        
        QuestResult sendCustomEvents(const vector<RumEventPtr>& events, int32_t timeout = 0);
        void sendCustomEvents(const vector<RumEventPtr>& events, std::function<void (QuestResult result)> callback, int32_t timeout = 0);

    private:
        string _calcMD5(const string& content);
        FPQuestPtr _getRumQuest(const vector<shared_ptr<RumEvent> >& events);
        bool _checkAnswerError(FPAnswerPtr answer, QuestResult& result, int32_t errorCode = -1);

        TCPClientPtr _client;
        int32_t _pid;
        string _secret;
        string _rid;
        int64_t _sid;
    };

    typedef shared_ptr<RUMServerClient> RUMServerClientPtr;
}

#endif
