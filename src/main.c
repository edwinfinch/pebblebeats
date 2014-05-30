#include <pebble.h>
#include "elements.h"
	
static TextLayer* textLayerInit(GRect location, int font)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, GColorWhite);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_text_alignment(layer, GTextAlignmentCenter);
	if(font == 1){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
	}
	else if(font == 2){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	}
	else if(font == 3){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	}
	return layer;
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	second = tick_time->tm_sec;
	minute = tick_time->tm_min;
	hour = tick_time->tm_hour;
	
	if(clock_is_24h_style()){
		strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", tick_time);
	}
	else{
		strftime(timeBuffer, sizeof(timeBuffer), "%I:%M", tick_time);
	}
	
	strftime(dateBuffer, sizeof(dateBuffer), "%d/%m/%y", tick_time);
	
	text_layer_set_text(time_layer, timeBuffer);
	text_layer_set_text(date_layer, dateBuffer);

	time(&local);
	preC = gmtime(&local);
	h = preC->tm_hour+5;
	m = preC->tm_min;
	s = preC->tm_sec;
	
    utc = 3600 * h + 60 * m + s;
    bmt = utc + 3600;  //Biel Mean Time (BMT)

    beat = bmt / 86.4;

	if(beat > 1000){
		beat -= 1000;
	}
	//Debug
	//APP_LOG(APP_LOG_LEVEL_INFO, "%d beats, %d hour, %d min, %d sec(s), %d utc", beat, h, m, s, utc);
	snprintf(beatBuffer, sizeof(beatBuffer), "@%d", beat);
	text_layer_set_text(beat_layer, beatBuffer);
	
}

void window_load(Window *window){
	info_layer = textLayerInit(GRect(0, 0, 144, 168), 3);
	beat_layer = textLayerInit(GRect(0, 30, 144, 168), 1);
	time_layer = textLayerInit(GRect(0, 90, 144, 168), 2);
	date_layer = textLayerInit(GRect(0, 120, 144, 168), 2);
	text_layer_set_text(info_layer, "Configured to EST");
	text_layer_set_text(date_layer, "Date...");
	text_layer_set_text(time_layer, "Time...");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(beat_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(info_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}

void window_unload(Window *window){
	text_layer_destroy(info_layer);
	text_layer_destroy(date_layer);
	text_layer_destroy(time_layer);
}

void init(void) {
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
	});
	tick_timer_service_subscribe(SECOND_UNIT, &tick_handler);
	//PUSH, M8!
	window_stack_push(window, true);
}

void deinit(void) {
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
