#ifndef RUM_SERVER_MONITOR_H
#define RUM_SERVER_MONITOR_H
#include "fpnn.h"

using namespace std;
using namespace fpnn;

namespace rum 
{
    class RUMServerMonitor: public IQuestProcessor
    {
        virtual void connected(const ConnectionInfo& connInfo) {}
        virtual void connectionWillClose(const ConnectionInfo& connInfo, bool closeByError) {}
    };
}

#endif
