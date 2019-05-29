#ifndef Quest_Result_H
#define Quest_Result_H

using namespace std;

namespace rum
{
	struct QuestResult
    {
        int32_t errorCode;
        string errorInfo;

        QuestResult()
        {
            errorCode = 0;
            errorInfo = "";
        }

        bool isError()
        {
            return errorCode > 0 || !errorInfo.empty();
        }
    };
}

#endif
