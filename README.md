# RUM Server C++ SDK

### 安装

* 本SDK依赖 [FPNN C++ SDK](https://github.com/highras/fpnn-sdk-cpp)
* 头文件默认搜索路径为 **../fpnn-sdk-cpp/release/include** 
* 如在其他位置可自行修改Makefile中 **FPNN_RELEASE_INCLUDE_PATH**

1. 编译

		cd <rtm-C++-SDK-folder>
		make

1. release

		sh release.sh
    
### 开发
```
  #include "rum.h"
  using namespace rum;
  
  RUMServerClientPtr client(new RUMServerClient(41000015, "xxx-xxxxx-xxxxxxxx-xxxxx", "52.83.220.166:13609", true, 5000));
  
  {
        RumEventPtr event(new RumEvent("error", 3));
        event->param("aaa", 111);
        event->param("bbb", "cccc");
        event->param("ccc", true);

        // 同步方式发送单条事件
        QuestResult result = client->sendCustomEvent(event);
        if (result.isError())
            cout << "error: " << result.errorCode << " " << result.errorInfo << endl;
        else
            cout << "sendCustomEvent sync ok" << endl;

        // 异步方式发送单条事件
        client->sendCustomEvent(event, [](QuestResult result) {
            if (result.isError())
                cout << "error: " << result.errorCode << " " << result.errorInfo << endl;
            else
                cout << "sendCustomEvent async ok" << endl;
        });
  }

  {
        vector<RumEventPtr> events;
        events.push_back(make_shared<RumEvent>("error", "{\"aaa\": 123, \"bbb\":\"ccc\", \"ccc\":true}"));
        events.push_back(make_shared<RumEvent>("error", "{\"aaa\": 456, \"bbb\":\"ddd\", \"ccc\":false}"));
        events.push_back(make_shared<RumEvent>("info", "{\"aaa\": 789, \"bbb\":\"ddd\", \"ccc\":false}"));

        // 同步方式发送多条事件
        QuestResult result = client->sendCustomEvents(events);
        if (result.isError())
            cout << "error: " << result.errorCode << " " << result.errorInfo << endl;
        else
            cout << "sendCustomEvents sync ok" << endl;

        // 异步方式发送多条事件
        client->sendCustomEvents(events, [](QuestResult result) {
            if (result.isError())
                cout << "error: " << result.errorCode << " " << result.errorInfo << endl;
            else
                cout << "sendCustomEvents async ok" << endl;
        });
  }  
```

### API

#### RTMServerClient构造函数
* `RUMServerClient(int32_t pid, const string& secret, const string& endpoint, bool reconnect, int32_t timeout)` 
    * `pid`: 应用编号, RUM提供
    * `secret`: 应用密钥, RUM提供
    * `endpoint`: 服务端网关地址
    * `reconnect`: 是否重连
    * `timeout`: 请求超时时间(s)
   
#### 设置全局请求超时时间  
* `void setQuestTimeout(int seconds)`  

#### 设置是否自动重连
* `void setAutoReconnect(bool autoReconnect)` 

#### 配置链接加密 请参考 [FPNN Client Advanced Tutorial](https://github.com/highras/fpnn/blob/master/doc/zh-cn/fpnn-client-advanced-tutorial.md#-%E5%8A%A0%E5%AF%86%E9%93%BE%E6%8E%A5)
* `bool enableEncryptorByDerData(const string &derData, bool packageMode = true, bool reinforce = false)`
* `bool enableEncryptorByPemData(const string &PemData, bool packageMode = true, bool reinforce = false)`
* `bool enableEncryptorByDerFile(const char *derFilePath, bool packageMode = true, bool reinforce = false)`
* `bool enableEncryptorByPemFile(const char *pemFilePath, bool packageMode = true, bool reinforce = false)`
* `void enableEncryptor(const string& curve, const string& peerPublicKey, bool packageMode = true, bool reinforce = false)`

#### 设置RUM ID (不设置将自动生成随机值)
* `void setRumId(const string& rid)`

#### 设置SESSION ID (不设置将自动生成随机值)
* `void setSessionId(int64_t sid)`

#### 发送单条事件 (同步)
* `QuestResult sendCustomEvent(RumEventPtr event, int32_t timeout = 0)`

#### 发送单条事件 (异步)
* `void sendCustomEvent(RumEventPtr event, std::function<void (QuestResult result)> callback, int32_t timeout = 0)`

#### 发送多条事件 (同步)
* `QuestResult sendCustomEvents(const vector<RumEventPtr>& events, int32_t timeout = 0)`

#### 发送多条事件 (异步)
* `void sendCustomEvents(const vector<RumEventPtr>& events, std::function<void (QuestResult result)> callback, int32_t timeout = 0)`

#### 构造事件两种方式
```
// 构造时指定attrs字段长度，逐条添加
RumEventPtr event(new RumEvent("eventName", 3));
event->param("key1", "value1");
event->param("key2", "value2");
event->param("key3", "value3");
	
// 通过json描述的attrs方式添加
RumEventPtr event(new RumEvent("eventName", "{\"key\":\"value\"}"));
```



