#ifndef ANALOG_WATCHFACE_H
#define ANALOG_WATCHFACE_H

static void main_window_create();
static void main_window_destroy();
static void main_window_load(Window *window);
static void main_window_unload(Window *window);

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ROUND(a) (int)((a) + 0.5)

typedef struct TimeWithMsec {
    time_t sec;
    uint16_t msec;
} TimeWithMsec;

TimeWithMsec stopwatch_time();
int stopwatch_lap_reset();
int stopwatch_start_stop();

int stopwatch_load_persist();
void stopwatch_save_persist();

#endif
