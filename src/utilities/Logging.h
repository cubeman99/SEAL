#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <wx/log.h>


// NOTE: Logging in SEAL just redirects to wxWidgets logging.


// Log a message, given a format string and a variable number of arguments.
// This function works just like printf
#define SEAL_LOG_MSG(_formatString, ...) \
	wxLogMessage(_formatString, __VA_ARGS__)


#endif // _LOGGING_H_