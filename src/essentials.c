#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_weekday_layer;
static TextLayer *s_week_number_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_connection_layer;
// static GContext *s_my_top_line = null; //I have lines. The best lines.
static Layer *s_my_top_line_layer;

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "chrg");
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  }
  text_layer_set_text(s_battery_layer, battery_text);
}

//%R corresponds to %H:%M
//%F corresponds to %Y-%m-%d
//%A full textual representation of the day
//%W week number starting on the first monday of the year
static void update_time(struct tm* tick_time, TimeUnits units_changed){
  // Create long lived buffer and show the time:
  static char s_time_text[] = "00:00";
  strftime(s_time_text, sizeof(s_time_text), "%R", tick_time); //%R corresponds to %H:%M
  text_layer_set_text(s_time_layer, s_time_text);
  
  // Set the date:
  static char date_buffer[] = "1970-01-01"; //%F corresponds to %Y-%m-%d
  strftime(date_buffer, sizeof(date_buffer), "%F", tick_time);
  text_layer_set_text(s_date_layer, date_buffer);
  
  // Set the day:
  static char day_buffer[] = "Wednesday";
  strftime(day_buffer, sizeof(day_buffer), "%A", tick_time);
  text_layer_set_text(s_weekday_layer, day_buffer);
  
  // Set the week number:
  static char week_buffer[] = "W52";
  strftime(week_buffer, sizeof(week_buffer), "W%W", tick_time);
  text_layer_set_text(s_week_number_layer, week_buffer);
}

// static void handle_bluetooth(bool connected) {
//   text_layer_set_text(s_connection_layer, connected ? "connected" : "disconnected");
// }

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // Set the line color
  graphics_context_set_stroke_color(ctx, GColorMediumSpringGreen);
  // Set the fill color
  graphics_context_set_fill_color(ctx, GColorBlue);
  // Set the stroke width (must be an odd integer value)
  graphics_context_set_stroke_width(ctx, 5);
  // Disable antialiasing (enabled by default where available)
  graphics_context_set_antialiased(ctx, false);
  graphics_draw_line(ctx, GPoint(0, 37), GPoint(layer_get_bounds(s_my_top_line_layer).size.w, 37));
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
//   GRect line_layer_bounds = layer_get_bounds(window_get_root_layer(window));
  s_my_top_line_layer = layer_create(bounds);//GRect(0,20,bounds.size.w,25)
//   layer_set_background_color(s_my_top_line_layer, GColorWhite);
  
  #ifdef PBL_ROUND
  s_week_number_layer = text_layer_create(GRect(10, 10, bounds.size.w/2, 34));
  text_layer_set_font(s_week_number_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_week_number_layer, GTextAlignmentCenter);
  #else
  s_week_number_layer = text_layer_create(GRect(5, 1, bounds.size.w/2, 34));
  text_layer_set_font(s_week_number_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_week_number_layer, GTextAlignmentLeft);
  #endif
  text_layer_set_text_color(s_week_number_layer, GColorWhite);
  text_layer_set_background_color(s_week_number_layer, GColorClear);
  
  
  #ifdef PBL_ROUND
  s_battery_layer = text_layer_create(GRect(bounds.size.w/2, 10, bounds.size.w/2 - 15, 34));
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  #else
  s_battery_layer = text_layer_create(GRect(bounds.size.w/2, 1, bounds.size.w/2 - 5, 34));
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  #endif
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  
  text_layer_set_text(s_battery_layer, "100%");

  s_weekday_layer = text_layer_create(GRect(0, 40, bounds.size.w, 34));
  text_layer_set_text_color(s_weekday_layer, GColorWhite);
  text_layer_set_background_color(s_weekday_layer, GColorClear);
  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  
  s_time_layer = text_layer_create(GRect(0, 70, bounds.size.w, 50));
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  s_date_layer = text_layer_create(GRect(0, 125, bounds.size.w, 34));
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorClear);
  #ifdef PBL_ROUND
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  #else
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  #endif
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

//   s_connection_layer = text_layer_create(GRect(0, 90, bounds.size.w, 34));
//   text_layer_set_text_color(s_connection_layer, GColorWhite);
//   text_layer_set_background_color(s_connection_layer, GColorClear);
//   text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
//   text_layer_set_text_alignment(s_connection_layer, GTextAlignmentCenter);
//   handle_bluetooth(connection_service_peek_pebble_app_connection());

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  update_time(current_time, MINUTE_UNIT);

  tick_timer_service_subscribe(MINUTE_UNIT, update_time);
  battery_state_service_subscribe(handle_battery);

//   connection_service_subscribe((ConnectionHandlers) {
//     .pebble_app_connection_handler = handle_bluetooth
//   });

  layer_add_child(window_layer, text_layer_get_layer(s_week_number_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weekday_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  //   layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  
  // Assign the custom drawing procedure
  layer_set_update_proc(s_my_top_line_layer, canvas_update_proc);
  // Add to Window
  layer_add_child(window_get_root_layer(window), s_my_top_line_layer);

//   handle_battery(battery_state_service_peek());
}

static void main_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
//   connection_service_unsubscribe();
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_connection_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_week_number_layer);
  layer_destroy(s_my_top_line_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
