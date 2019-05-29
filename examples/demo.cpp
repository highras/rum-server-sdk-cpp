#include <iostream>
#include "rum.h"

using namespace std;
using namespace rum;

int main(int argc, const char** argv)
{
    RUMServerClientPtr client(new RUMServerClient(41000015, "xxx-xxxxx-xxxxxxxx-xxxxx", "52.83.220.166:13609", true, 5000));

    {
        RumEventPtr event(new RumEvent("error", 3));
        event->param("aaa", 111);
        event->param("bbb", "cccc");
        event->param("ccc", true);

        QuestResult result = client->sendCustomEvent(event);
        if (result.isError())
            cout << "error: " << result.errorCode << " " << result.errorInfo << endl;
        else 
            cout << "sendCustomEvent sync ok" << endl;

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

        QuestResult result = client->sendCustomEvents(events);
        if (result.isError())
            cout << "error: " << result.errorCode << " " << result.errorInfo << endl;
        else 
            cout << "sendCustomEvents sync ok" << endl;
        
        client->sendCustomEvents(events, [](QuestResult result) {
            if (result.isError())
                cout << "error: " << result.errorCode << " " << result.errorInfo << endl;
            else 
                cout << "sendCustomEvents async ok" << endl;
        });
    }

    sleep(10);

    return 0;
}
