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
  }
}

void stopwatch_stop() {
  if (stopwatch_running) {
    stopwatch_stop_time = stopwatch_time();
    stopwatch_running = 0;
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

