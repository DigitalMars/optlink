
#include <windows.h>

#include "all.h"


void GET_TIME_AND_DATE()
{
    SYSTEMTIME st;

    GetLocalTime(&st);

    CURN_MONTH = st.wMonth;
    CURN_DAY = st.wDay;
    CURN_YEAR = st.wYear;
    CURN_DATE_DWD = (((st.wYear & 0xFFFF) - 1980) << 9) |
		    (st.wMonth << 5) |
		    (st.wDay & 0x1F);

    CURN_HOUR = st.wHour;
    CURN_MINUTE = st.wMinute;
    CURN_SECOND = st.wSecond;
    CURN_HUNDREDTH = st.wMilliseconds / 10;
    CURN_TIME_DWD = (((st.wHour << 6) | st.wMinute) << 5) | (st.wSecond >> 1);
}

