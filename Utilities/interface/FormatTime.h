#ifndef UTILITIES_FORMATTIME_H
#define UTILITIES_FORMATTIME_H

#include <sys/time.h>

namespace mf {

char * formatTime( const timeval t, bool milli = false );

}

#endif
