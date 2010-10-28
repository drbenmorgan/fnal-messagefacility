/*
 * FormatTime.cc
 *
 *  Created on: Jun 11, 2010
 *      Author: qlu
 */

#include "messagefacility/Utilities/FormatTime.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

char * mf::formatTime(const timeval t, bool milli )
{

	static char tm[] = "dd-Mon-yyyy hh:mm:ss.mmm TZN     ";
	static char ts[] = "dd-Mon-yyyy hh:mm:ss TZN     ";

	#ifdef AN_ALTERNATIVE_FOR_TIMEZONE
	  char * c  = ctime( &t.tv_sec );                      // 6/14/99 mf Can't be static!
	  if(milli)
	  {
	    char m[10];
	    sprintf(m, "%03d", (int)(t.tv_usec/1000));
	    strncpy( tm+ 0, c+ 8, 2 );  // dd
	    strncpy( tm+ 3, c+ 4, 3 );  // Mon
	    strncpy( tm+ 7, c+20, 4 );  // yyyy
	    strncpy( tm+12, c+11, 8 );  // hh:mm:ss
	    strncpy( tm+21, m   , 3 );  // mmm
	    strncpy( tm+25, tzname[localtime(&t.tv_sec)->tm_isdst], 8 );  // CST
	  }
	  else
	  {
	    strncpy( ts+ 0, c+ 8, 2 );  // dd
	    strncpy( ts+ 3, c+ 4, 3 );  // Mon
	    strncpy( ts+ 7, c+20, 4 );  // yyyy
	    strncpy( ts+12, c+11, 8 );  // hh:mm:ss
	    strncpy( ts+21, tzname[localtime(&t.tv_sec)->tm_isdst], 8 );  // CST
	  }
	#endif
	  if(milli)
	  {
	    char tc[35];
	    strftime( tc, 30, "%d-%b-%Y %H:%M:%S %Z", localtime(&t.tv_sec) );
	    char m[10];
	    sprintf(m, "%03d", (int)(t.tv_usec/1000));
	    strncpy( tm+0,  tc+0 , 20 );
	    strncpy( tm+21, m +0 , 3  );
	    strncpy( tm+25, tc+21, 8  );
	  }
	  else
	  {
	    strftime( ts, strlen(ts)+1, "%d-%b-%Y %H:%M:%S %Z", localtime(&t.tv_sec) );
	                // mf 4-9-04
	  }

	#ifdef STRIP_TRAILING_BLANKS_IN_TIMEZONE
	  // strip trailing blanks that would come when the time zone is not as
	  // long as the maximum allowed - probably not worth the time
	  if(milli)
	  {
	    unsigned int b = strlen(tm);
	    while (tm[--b] == ' ') {tm[b] = 0;}
	  }
	  else
	  {
	    unsigned int b = strlen(ts);
	    while (ts[--b] == ' ') {ts[b] = 0;}
	  }
	#endif

	return milli ? tm : ts;
}

