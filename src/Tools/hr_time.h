#ifndef HR_TIME_H
#define HR_TIME_H

#include <windows.h>

typedef struct {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
} stopWatch;

class CStopWatch {

private:
        stopWatch timer;
        LARGE_INTEGER frequency;
        double LIToSecs( LARGE_INTEGER & L);
public:
        CStopWatch();
        void startTimer( );
        void stopTimer( );
        double getElapsedTime();
};

#endif // HR_TIME_H
