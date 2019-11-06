#pragma once

#ifdef USE_DBGPRINT

#	include "Driver.h"
#	define TRACE_INIT(X,Y)
#	define TRACE_CLEANUP(X)
#	define TRACE_INFO(MSG, ...) DbgPrint(MSG ## "\n", ##__VA_ARGS__)
#	define TRACE_ERROR(MSG, ...) DbgPrint(MSG ## "\n", ##__VA_ARGS__)

#elif USE_WPP

#	include <evntrace.h>

//
// Define the tracing flags.
//
// Tracing GUID - a7f22900-1caa-47f4-82d6-2a64e3aa2468
//

#	define WPP_CONTROL_GUIDS											\
    WPP_DEFINE_CONTROL_GUID(											\
        MobileJoystickTraceGuid, (a7f22900,1caa,47f4,82d6,2a64e3aa2468),\
                                                                        \
        WPP_DEFINE_BIT(MYDRIVER_ALL_INFO)								\
        WPP_DEFINE_BIT(TRACE_DRIVER)									\
        WPP_DEFINE_BIT(TRACE_DEVICE)									\
        WPP_DEFINE_BIT(TRACE_QUEUE)										\
        )                             

#	define WPP_FLAG_LEVEL_LOGGER(flag, level)                           \
    WPP_LEVEL_LOGGER(flag)

#	define WPP_FLAG_LEVEL_ENABLED(flag, level)                          \
    (WPP_LEVEL_ENABLED(flag) &&                                         \
     WPP_CONTROL(WPP_BIT_ ## flag).Level >= level)

#	define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) \
	WPP_LEVEL_LOGGER(flags)
               
#	define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) \
    (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

//
// This comment block is scanned by the trace preprocessor to define our
// Trace function.
//
// begin_wpp config
// FUNC Trace{FLAG=MYDRIVER_ALL_INFO}(LEVEL, MSG, ...);
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// FUNC TraceError{FLAG=MYDRIVER_ALL_INFO,LEVEL=TRACE_LEVEL_ERROR}(MSG, ...);
// FUNC TraceInfo{FLAG=MYDRIVER_ALL_INFO,LEVEL=TRACE_LEVEL_INFORMATION}(MSG, ...);
// end_wpp
//

#	define TRACE_INIT		WPP_INIT_TRACING
#	define TRACE_CLEANUP	WPP_CLEANUP
#	define TRACE_ERROR		TraceError
#	define TRACE_INFO		TraceInfo

#else

#	define TRACE_INIT(X,Y)
#	define TRACE_CLEANUP(X)
#	define TRACE_INFO(...)
#	define TRACE_ERROR(...)

#endif

