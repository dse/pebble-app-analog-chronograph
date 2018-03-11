/*
 * stopwatch.c
 *
 * Stopwatch routines.
 */

#include <pebble.h>
#include "analog-watchface.h"

static TimeWithMsec stopwatch_start_point = {0, 0};
static TimeWithMsec stopwatch_stop_time   = {0, 0};
static int          stopwatch_running     = 0;

#define PERSIST_STOPWATCH_START_POINT_SEC  0
#define PERSIST_STOPWATCH_START_POINT_MSEC 1
#define PERSIST_STOPWATCH_STOP_TIME_SEC    2
#define PERSIST_STOPWATCH_STOP_TIME_MSEC   3
#define PERSIST_STOPWATCH_RUNNING          4

int stopwatch_load_persist() {
    stopwatch_start_point.sec = 0;
    stopwatch_start_point.msec = 0;
    stopwatch_stop_time.sec = 0;
    stopwatch_stop_time.msec = 0;
    stopwatch_running = 0;
    if (persist_exists(PERSIST_STOPWATCH_START_POINT_SEC)) {
        stopwatch_start_point.sec = persist_read_int(PERSIST_STOPWATCH_START_POINT_SEC);
    }
    if (persist_exists(PERSIST_STOPWATCH_START_POINT_MSEC)) {
        stopwatch_start_point.msec = persist_read_int(PERSIST_STOPWATCH_START_POINT_MSEC);
    }
    if (persist_exists(PERSIST_STOPWATCH_STOP_TIME_SEC)) {
        stopwatch_stop_time.sec = persist_read_int(PERSIST_STOPWATCH_STOP_TIME_SEC);
    }
    if (persist_exists(PERSIST_STOPWATCH_STOP_TIME_MSEC)) {
        stopwatch_stop_time.msec = persist_read_int(PERSIST_STOPWATCH_STOP_TIME_MSEC);
    }
    if (persist_exists(PERSIST_STOPWATCH_RUNNING)) {
        stopwatch_running = persist_read_int(PERSIST_STOPWATCH_RUNNING);
    }
    return stopwatch_running;
}

void stopwatch_save_persist() {
    persist_write_int(PERSIST_STOPWATCH_START_POINT_SEC,  stopwatch_start_point.sec  );
    persist_write_int(PERSIST_STOPWATCH_START_POINT_MSEC, stopwatch_start_point.msec );
    persist_write_int(PERSIST_STOPWATCH_STOP_TIME_SEC,    stopwatch_stop_time.sec    );
    persist_write_int(PERSIST_STOPWATCH_STOP_TIME_MSEC,   stopwatch_stop_time.msec   );
    persist_write_int(PERSIST_STOPWATCH_RUNNING,          stopwatch_running          );
}

TimeWithMsec timewithmsec_minus(TimeWithMsec a, TimeWithMsec b) {
    TimeWithMsec result;
    if (a.msec < b.msec) {
        a.msec += 1000;
        a.sec -= 1;
    }
    if (a.sec < b.sec) {
        result.sec = 0;
        result.msec = 0;
        return result;
    }
    result.sec = a.sec - b.sec;
    result.msec = a.msec - b.msec;
    return result;
}

TimeWithMsec timewithmsec_plus(TimeWithMsec a, TimeWithMsec b) {
    TimeWithMsec result;
    result.sec = a.sec + b.sec;
    result.msec = a.msec + b.msec;
    if (result.msec > 1000) {
        result.sec = result.sec + result.msec / 1000;
        result.msec %= 1000;
    }
    return result;
}

void stopwatch_start() {
    if (!stopwatch_running) {
        stopwatch_running = 1;
        time_ms(&(stopwatch_start_point.sec), &(stopwatch_start_point.msec));
        stopwatch_save_persist();
    }
}

void stopwatch_stop() {
    if (stopwatch_running) {
        stopwatch_stop_time = stopwatch_time();
        stopwatch_running = 0;
        stopwatch_save_persist();
    }
}

int stopwatch_start_stop() {
    if (stopwatch_running) {
        stopwatch_stop();
        return 0;
    } else {
        stopwatch_start();
        return 1;
    }
}

void stopwatch_reset() {
    stopwatch_start_point.sec = 0;
    stopwatch_start_point.msec = 0;
    stopwatch_stop_time.sec = 0;
    stopwatch_stop_time.msec = 0;
    stopwatch_running = 0;
    stopwatch_save_persist();
}

int stopwatch_lap_reset() {
    if (stopwatch_running) {
        return 1;
    } else {
        stopwatch_reset();
        return 0;
    }
}

TimeWithMsec stopwatch_time() {
    if (stopwatch_running) {
        TimeWithMsec now;
        time_ms(&(now.sec), &(now.msec));
        return timewithmsec_plus(stopwatch_stop_time,
                                 timewithmsec_minus(now, stopwatch_start_point));
    } else {
        return stopwatch_stop_time;
    }
}

