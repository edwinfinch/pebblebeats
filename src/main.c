#include <pebble.h>
#include "elements.h"
#include "appkeys.h"	

void refresh_settings(){
	if(settings.time == 0){
		layer_set_hidden(text_layer_get_layer(time_layer), true);
	}
	else{
		layer_set_hidden(text_layer_get_layer(time_layer), false);
	}
	
	if(settings.date == 0){
		layer_set_hidden(text_layer_get_layer(date_layer), true);
	}
	else{
		layer_set_hidden(text_layer_get_layer(date_layer), false);
	}
}
	
void on_animation_stopped(Animation *anim, bool finished, void *context)
{
    property_animation_destroy((PropertyAnimation*) anim);
}
	
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)
{
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
	AnimationHandlers handlers = {
    .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
    animation_schedule((Animation*) anim);
}

void process_tuple(Tuple *t)
{
	//Get key
	int key = t->key;

	//Get integer value
	int value = t->value->int32;

	//Get string value
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Extracting key: %d with value: %d", key, value);
  switch (key) {
	  case TIMEZONE_KEY:
	  	settings.timezone = value;
	  	break;
	  case TIMEENABLED_KEY:
	  	settings.time = value;
	  	break;
	  case DATEENABLED_KEY:
	  	settings.date = value;
	  	refresh_settings();
	  	break;
  }
}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
	(void) context;

	//Get data
	Tuple *t = dict_read_first(iter);
	if(t)
	{
		process_tuple(t);
	}
	//Get next
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)
		{
			//Process that data
			process_tuple(t);
		}
	}
}


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
	else if(font == 4){
		text_layer_set_font(layer, walkaway_reg);		
	}
	return layer;
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	second = tick_time->tm_sec;
	minute = tick_time->tm_min;
	hour = tick_time->tm_hour;
	if(initialS == 1){
		GRect finish = GRect(0, 0, 144, 168);
		GRect start = GRect(0, -100, 144, 168);
		animate_layer(text_layer_get_layer(info_layer), &start, &finish, 1000, 400);
	}
	if(initialS < 5){
		initialS++;
	}
	else if(initialS == 5){
		GRect start = GRect(0, 0, 144, 168);
		GRect finish = GRect(0, -100, 144, 168);
		animate_layer(text_layer_get_layer(info_layer), &start, &finish, 1000, 400);
		initialS = 6;
	}
	
	if(clock_is_24h_style()){
		strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", tick_time);
	}
	else{
		strftime(timeBuffer, sizeof(timeBuffer), "%I:%M", tick_time);
	}
	
	strftime(dateBuffer, sizeof(dateBuffer), "%d-%m", tick_time);
	
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
	Layer *window_layer = window_get_root_layer(window);
	walkaway_reg = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WALKAWAY_35));
	info_layer = textLayerInit(GRect(0, -100, 144, 168), 3);
	beat_layer = textLayerInit(GRect(0, 30, 144, 168), 1);
	time_layer = textLayerInit(GRect(0, 90, 144, 168), 4);
	date_layer = textLayerInit(GRect(0, 120, 144, 168), 4);
	text_layer_set_text(info_layer, "Configured to EST");
	text_layer_set_text(date_layer, "Date...");
	text_layer_set_text(time_layer, "Time...");
	layer_add_child(window_layer, text_layer_get_layer(beat_layer));
	layer_add_child(window_layer, text_layer_get_layer(info_layer));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	refresh_settings();
}

void window_unload(Window *window){
	text_layer_destroy(info_layer);
	text_layer_destroy(beat_layer);
	text_layer_destroy(date_layer);
	text_layer_destroy(time_layer);
	animation_unschedule_all();
	fonts_unload_custom_font(walkaway_reg);
}

void init(void) {
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
	});
	if(persist_exists(SETTINGS_KEY)){
		valueRead = persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
		APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes read from settings", valueRead);
	}
	else{
		settings.time = 1;
		settings.date = 1;
	}
	app_message_register_inbox_received(in_received_handler);
	app_message_open(1028, 512);
	tick_timer_service_subscribe(SECOND_UNIT, &tick_handler);
	//PUSH, M8!
	window_stack_push(window, true);
}

void deinit(void) {
	valueWritten = persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
	APP_LOG(APP_LOG_LEVEL_INFO, "Wrote %d bytes to settings", valueWritten);
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
