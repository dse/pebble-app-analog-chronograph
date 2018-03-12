/*
 * stopwatch.c
 *
 * Stopwatch routines.
 */

#include <pebble.h>
#include "watch.h"

static StopwatchData stopwatch_data;

int stopwatch_load_persist() {
    stopwatch_data.start_time.sec = 0;
    stopwatch_data.start_time.msec = 0;
    stopwatch_data.stop_time.sec = 0;
    stopwatch_data.stop_time.msec = 0;
    stopwatch_data.is_running = 0;
    if (persist_exists(STOPWATCH_DATA_KEY)) {
        persist_read_data(STOPWATCH_DATA_KEY, &stopwatch_data,
                          sizeof(stopwatch_data));
    }
    return stopwatch_data.is_running;
}

void stopwatch_save_persist() {
    persist_write_data(STOPWATCH_DATA_KEY, &stopwatch_data,
                       sizeof(stopwatch_data));
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
    if (!stopwatch_data.is_running) {
        stopwatch_data.is_running = 1;
        time_ms(&(stopwatch_data.start_time.sec), &(stopwatch_data.start_time.msec));
        stopwatch_save_persist();
    }
}

void stopwatch_stop() {
    if (stopwatch_data.is_running) {
        stopwatch_data.stop_time = stopwatch_time();
        stopwatch_data.is_running = 0;
        stopwatch_save_persist();
    }
}

int stopwatch_start_stop() {
    if (stopwatch_data.is_running) {
        stopwatch_stop();
        return 0;
    } else {
        stopwatch_start();
        return 1;
    }
}

void stopwatch_reset() {
    stopwatch_data.start_time.sec = 0;
    stopwatch_data.start_time.msec = 0;
    stopwatch_data.stop_time.sec = 0;
    stopwatch_data.stop_time.msec = 0;
    stopwatch_data.is_running = 0;
    stopwatch_save_persist();
}

int stopwatch_lap_reset() {
    if (stopwatch_data.is_running) {
        return 1;
    } else {
        stopwatch_reset();
        return 0;
    }
}

TimeWithMsec stopwatch_time() {
    if (stopwatch_data.is_running) {
        TimeWithMsec now;
        time_ms(&(now.sec), &(now.msec));
        return timewithmsec_plus(stopwatch_data.stop_time,
                                 timewithmsec_minus(now, stopwatch_data.start_time));
    } else {
        return stopwatch_data.stop_time;
    }
}

