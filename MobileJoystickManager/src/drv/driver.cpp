#include "driver.h"
#include "utl/logger.h"
#include "Public.h"
extern "C"
{
    #include <HidSdi.h>
}

namespace drv
{

void Driver::handleMessage(const msg::DriverMessage &message)
{
    if (message.msgType == msg::SET_REPORT)
    {
        SET_JOY_STATE report;
        report.reportId = REPORT_ID_CONTROL;
        report.deviceNo = deviceIndex;
        report.state.xAxis = message.x;
        report.state.yAxis = message.y;
        report.state.buttons = message.buttons;
        DWORD bytesWritten;

        if (FALSE == WriteFile(handle,
                               &report,
                               sizeof(report),
                               &bytesWritten,
                               NULL))
        {
            WLOGGER << "WriteFile failed: " << ::GetLastError() << std::endl;
        }
    }
}

}
