/*
 * main.c
 */

#include <pebble.h>
#include "analog-watchface.h"

static Window *s_main_window;
static Layer *s_ticks_layer;
static Layer *s_wall_time_layer;
static Layer *s_stopwatch_layer;

static AppTimer *timer_handle = NULL;

static const GPathInfo MINUTE_HAND_POINTS = {
  4,
  (GPoint []) {
    { -MINUTE_HAND_WIDTH, -MINUTE_RADIUS },
    { -MINUTE_HAND_WIDTH, MINUTE_HAND_WIDTH },
    { MINUTE_HAND_WIDTH, MINUTE_HAND_WIDTH },
    { MINUTE_HAND_WIDTH, -MINUTE_RADIUS }
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  4,
  (GPoint []){
    { -HOUR_HAND_WIDTH, -HOUR_RADIUS },
    { -HOUR_HAND_WIDTH, HOUR_HAND_WIDTH },
    { HOUR_HAND_WIDTH, HOUR_HAND_WIDTH },
    { HOUR_HAND_WIDTH, -HOUR_RADIUS }
  }
};

static GPath *s_minute_arrow, *s_hour_arrow;

static GRect  bounds;
static GPoint center;
static GPoint center1, center2, center3;
static int    radius1, radius2, radius3;

GPoint tick_point (GPoint center, int radius, int degrees) {
  int angle = (int)(TRIG_MAX_ANGLE * degrees / 360.0 + 0.5);
  int x = center.x + (int)(radius * 1.0 * sin_lookup(angle) / TRIG_MAX_RATIO + 0.5);
  int y = center.y - (int)(radius * 1.0 * cos_lookup(angle) / TRIG_MAX_RATIO + 0.5);
  return GPoint(x, y);
}

void draw_ticks (GContext *ctx, GPoint center, int radius, int num_ticks, int ticks_modulo, int thick) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  for (int i = 0; i < num_ticks; i += 1) {
    GPoint p = tick_point(center, radius, i * 360 / num_ticks);
    if (i % ticks_modulo == 0) {
      if (thick) {
	graphics_fill_rect(ctx, GRect(p.x - 1, p.y - 1, 3, 3), 0, GCornerNone);
      } else {
	GPoint p1 = tick_point(center, radius + 1, i * 360 / num_ticks);
	GPoint p2 = tick_point(center, radius - 1, i * 360 / num_ticks);
	graphics_draw_line(ctx, p1, p2);
      }
    } else {
      graphics_draw_pixel(ctx, p);
    }
  }
}

void ticks_update_proc(Layer *layer, GContext *ctx) {
  draw_ticks(ctx, center, TICK_RADIUS, 60, 5, 1);
  draw_ticks(ctx, center1, radius1, 20, 4, 1);
  draw_ticks(ctx, center2, radius2, 60, 5, 0);
  draw_ticks(ctx, center3, radius3, 60, 5, 0);
}

void canvas_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  
  GPoint second = tick_point(center, SECOND_RADIUS, t->tm_sec * 6);
  graphics_draw_line(ctx, center, second);

  int minute_angle = (int)(TRIG_MAX_ANGLE / 3600.0  * (                         t->tm_min * 60 + t->tm_sec) + 0.5);
  int hour_angle   = (int)(TRIG_MAX_ANGLE / 43200.0 * (t->tm_hour % 12 * 3600 + t->tm_min * 60 + t->tm_sec) + 0.5);

  gpath_rotate_to(s_minute_arrow, minute_angle);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  gpath_rotate_to(s_hour_arrow, hour_angle);
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);
}

void stopwatch_update_proc(Layer *layer, GContext *ctx) {
  TimeWithMsec t = stopwatch_time();
  
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);

  GPoint pt_msec   = tick_point(center1, radius1 - 4,
				360.0 * t.msec / 1000);
  GPoint pt_second = tick_point(center2, radius2 - 4,
				t.sec % 60 * 6);
  GPoint pt_minute = tick_point(center3, radius3 - 4,
				(int)((t.sec / 60) % 60  * 6.0 + 0.5));
  GPoint pt_hour   = tick_point(center3, (int)((radius3 - 4.0) * 2.0 / 3.0 + 0.5),
				(int)((t.sec / 60) % 720 * 0.5 + 0.5));

  graphics_draw_line(ctx, center1, pt_msec);
  graphics_draw_line(ctx, center2, pt_second);
  graphics_draw_line(ctx, center3, pt_minute);
  graphics_draw_line(ctx, center3, pt_hour);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_wall_time_layer);
}

void update_stopwatch() {
  timer_handle = app_timer_register(50, (AppTimerCallback) update_stopwatch, NULL);
  layer_mark_dirty(s_stopwatch_layer);
}

void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  bounds = layer_get_bounds(window_layer);
  center = grect_center_point(&bounds);

  center1 = tick_point(center, (int)(TICK_RADIUS * 0.65 + 0.5),   0);
  center2 = tick_point(center, (int)(TICK_RADIUS * 0.5 + 0.5),  90);
  center3 = tick_point(center, (int)(TICK_RADIUS * 0.5 + 0.5), 270);
  radius1 = 20;
  radius2 = 30;
  radius3 = 30;

  window_set_background_color(window, GColorBlack);

  s_ticks_layer = layer_create(bounds);
  layer_set_update_proc(s_ticks_layer, ticks_update_proc);
  layer_add_child(window_layer, s_ticks_layer);

  s_wall_time_layer = layer_create(bounds);
  layer_set_update_proc(s_wall_time_layer, canvas_update_proc);
  layer_add_child(window_layer, s_wall_time_layer);

  s_stopwatch_layer = layer_create(bounds);
  layer_set_update_proc(s_stopwatch_layer, stopwatch_update_proc);
  layer_add_child(window_layer, s_stopwatch_layer);

  layer_mark_dirty(s_wall_time_layer);
  layer_mark_dirty(s_stopwatch_layer);

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  if (stopwatch_load_persist()) {
    timer_handle = app_timer_register(50, (AppTimerCallback) update_stopwatch, NULL);
  }
}

void main_window_unload(Window *window) {
  // Destroy Layer
  layer_destroy(s_wall_time_layer);
  layer_destroy(s_stopwatch_layer);
}

void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (stopwatch_start_stop()) {	/* start */
    if (timer_handle) {
      app_timer_cancel(timer_handle);
    }
    timer_handle = app_timer_register(50, (AppTimerCallback) update_stopwatch, NULL);
  } else {			/* stop */
    if (timer_handle) {
      app_timer_cancel(timer_handle);
    }
    timer_handle = NULL;
    layer_mark_dirty(s_stopwatch_layer);
  }
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (stopwatch_lap_reset()) {	/* lap */
  } else {			/* reset */
  }
  layer_mark_dirty(s_stopwatch_layer);
}

void click_config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_UP,     up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN,   down_single_click_handler);
}

void init(void) {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = main_window_load,
	.unload = main_window_unload,
	});
  window_stack_push(s_main_window, true);
  
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
  
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

  window_set_click_config_provider(s_main_window, (ClickConfigProvider) click_config_provider);
}

void deinit(void) {
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

