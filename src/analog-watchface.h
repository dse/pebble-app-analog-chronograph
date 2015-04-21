#ifndef ANALOG_WATCHFACE_H
#define ANALOG_WATCHFACE_H

#define TICK_RADIUS       68
#define SECOND_RADIUS     64
#define MINUTE_RADIUS     56
#define HOUR_RADIUS       36
#define MINUTE_HAND_WIDTH 2
#define HOUR_HAND_WIDTH   2

typedef struct TimeWithMsec {
  time_t sec;
  uint16_t msec;
} TimeWithMsec ;

TimeWithMsec stopwatch_time();
int stopwatch_lap_reset();
int stopwatch_start_stop();

#endif /* ANALOG_WATCHFACE_H */
