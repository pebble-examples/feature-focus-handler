#include <pebble.h>

#define BUF_LEN 64

static char s_buf[BUF_LEN];
static TextLayer *s_time;
static Window *s_main_window;

static void focus_handler(bool in_focus) {
  static char time_in_str[BUF_LEN];
  static char time_out_str[BUF_LEN];
  time_t t = time(NULL);
  struct tm *lt = localtime(&t);
  strftime((in_focus) ? time_in_str : time_out_str, BUF_LEN, "%H:%M:%S", lt);

  snprintf(s_buf, sizeof(s_buf), "Last in focus:\n%s\nLast out focus:\n%s", time_in_str, time_out_str);
  layer_mark_dirty(text_layer_get_layer(s_time));
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_time = text_layer_create(PBL_IF_RECT_ELSE(bounds, GRect(0, 35, bounds.size.w, bounds.size.h)));
  text_layer_set_text(s_time, s_buf);
  text_layer_set_font(s_time, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_time, PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentCenter));
  layer_add_child(window_layer, text_layer_get_layer(s_time));

  snprintf(s_buf, sizeof(s_buf), "Last in focus:\nNever\nLast out focus:\nNever");

  app_focus_service_subscribe(focus_handler);
}

static void main_window_unload(Window *window) {
  app_focus_service_unsubscribe();
  text_layer_destroy(s_time);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
