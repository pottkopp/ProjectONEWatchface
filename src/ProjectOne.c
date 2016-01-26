/*

   How to use a custom non-system font.
   Screen res: 144*168
 */

#include <pebble.h>

static Window *window;

// Don't forget 2 add the layer here
TextLayer *hour_layer;
TextLayer *minute_layer;
TextLayer *day_layer;
TextLayer *date_layer;
Layer *window_layer;
//BatBar stuff
TextLayer *batlayer;
TextLayer *batlayer_empty;
BatteryChargeState charge_state;
void run_bar();
void init_bar(BatteryChargeState charge_state);

// Does the time pass by?
static void handle_timechanges(struct tm *tick_time, TimeUnits units_changed) {
  static char hour_buffer[5];
  static char minute_buffer[5];
  
  static char day_buffer[5];
  static char date_buffer[10];
  
  strftime(hour_buffer, sizeof(hour_buffer), "%H", tick_time);
  text_layer_set_text(hour_layer, hour_buffer);

  strftime(minute_buffer, sizeof(minute_buffer), "%M", tick_time);
  text_layer_set_text(minute_layer, minute_buffer);  
  
  strftime(day_buffer, sizeof(day_buffer), "%a", tick_time);
  text_layer_set_text(day_layer, day_buffer);

  strftime(date_buffer, sizeof(date_buffer), "%e.%m.", tick_time);
  text_layer_set_text(date_layer, date_buffer);  
}

//BatBar stuff
void start_bar(BatteryChargeState charge_state) {
  run_bar(charge_state);
}

void run_bar() {
  charge_state = battery_state_service_peek();
	init_bar(charge_state);
}

void init_bar(BatteryChargeState charge_state) {
	battery_state_service_subscribe(&start_bar);
  
  charge_state = battery_state_service_peek();
	uint8_t percent = charge_state.charge_percent*1.44;

  batlayer = text_layer_create(GRect(0, 84, percent, 2));
  text_layer_set_background_color(batlayer,GColorWhite);
  layer_add_child(window_layer, text_layer_get_layer(batlayer));
  batlayer_empty = text_layer_create(GRect(percent, 84, 144, 2));
  text_layer_set_background_color(batlayer_empty,GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(batlayer_empty));
  
  if(charge_state.is_charging) {
    window_set_background_color(window, GColorWhite);
    text_layer_set_text_color(hour_layer,GColorBlack);
    text_layer_set_text_color(minute_layer,GColorBlack);
    text_layer_set_text_color(day_layer,GColorBlack);
    text_layer_set_text_color(date_layer,GColorBlack);
    text_layer_set_background_color(batlayer,GColorBlack);
    text_layer_set_background_color(batlayer_empty,GColorWhite);
  } else {
    window_set_background_color(window, GColorBlack);
    text_layer_set_text_color(hour_layer,GColorWhite);
    text_layer_set_text_color(minute_layer,GColorWhite);
    text_layer_set_text_color(day_layer,GColorWhite);
    text_layer_set_text_color(date_layer,GColorWhite);
    text_layer_set_background_color(batlayer,GColorWhite);
    text_layer_set_background_color(batlayer_empty,GColorBlack);
  } 
}

static void init() {
  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_stack_push(window, false /* Animated */);
  window_layer = window_get_root_layer(window);

  // Set font
  GFont clock_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_Clock_60));
  GFont date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_Date_10));

  // Different layers
  hour_layer = text_layer_create(GRect(25, 0, 119, 84));
  text_layer_set_font(hour_layer, clock_font);
  text_layer_set_text_color(hour_layer,GColorWhite);
  text_layer_set_background_color(hour_layer,GColorClear);
  text_layer_set_text_alignment(hour_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(hour_layer));
  
  minute_layer = text_layer_create(GRect(25, 85, 119, 84));
  text_layer_set_font(minute_layer, clock_font);
  text_layer_set_text_color(minute_layer,GColorWhite);
  text_layer_set_background_color(minute_layer,GColorClear);
  text_layer_set_text_alignment(minute_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(minute_layer));
  
  day_layer = text_layer_create(GRect(0, 72, 144, 15));
  text_layer_set_font(day_layer, date_font);
  text_layer_set_text_color(day_layer,GColorWhite);
  text_layer_set_background_color(day_layer,GColorClear);
  text_layer_set_text_alignment(day_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(day_layer));
  
  date_layer = text_layer_create(GRect(0, 86, 144, 15));
  text_layer_set_font(date_layer, date_font);
  text_layer_set_text_color(date_layer,GColorWhite);
  text_layer_set_background_color(date_layer,GColorClear);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));
  
  init_bar(charge_state);
  
  // Start with current time
  time_t now = time(NULL);
  handle_timechanges(localtime(&now), MINUTE_UNIT);
  
  tick_timer_service_subscribe(MINUTE_UNIT, handle_timechanges);
}

void deinit() {
  layer_remove_child_layers(window_get_root_layer(window));
  text_layer_destroy(hour_layer);
  text_layer_destroy(minute_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(day_layer);
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}