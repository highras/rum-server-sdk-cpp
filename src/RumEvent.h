#ifndef RUM_EVENT_H
#define RUM_EVENT_H
#include "fpnn.h"

using namespace std;
using namespace fpnn;

namespace rum 
{
    class RumEvent: public FPWriter
    {
        string _eventName;

    public:
        RumEvent(const string& eventName, uint32_t attrSize): FPWriter(attrSize), _eventName(eventName) {}
		RumEvent(const string& eventName, const string& attrJson): FPWriter(attrJson), _eventName(eventName) {}
        ~RumEvent() {}
        const string& getEventName() const { return _eventName; }
    };

    typedef shared_ptr<RumEvent> RumEventPtr;
}

#endif
