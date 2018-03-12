#ifndef WATCH_H
#define WATCH_H

static void main_window_create();
static void main_window_destroy();
static void main_window_load(Window *window);
static void main_window_unload(Window *window);

/* Persistent storage keys */
#define SETTINGS_KEY 1
#define STOPWATCH_DATA_KEY 2

typedef struct WatchSettings {
    bool show_date;
    bool show_battery;
    bool use_bold_font;
    bool use_larger_font;
    bool stopwatch_uses_big_second_hand;
} WatchSettings;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ROUND(a) (int)((a) + 0.5)

typedef struct TimeWithMsec {
    time_t sec;
    uint16_t msec;
} TimeWithMsec;

typedef struct StopwatchData {
    TimeWithMsec start_time;
    TimeWithMsec stop_time;
    bool is_running;
} StopwatchData;

TimeWithMsec stopwatch_time();
int stopwatch_lap_reset();
int stopwatch_start_stop();

int stopwatch_load_persist();
void stopwatch_save_persist();

#endif
