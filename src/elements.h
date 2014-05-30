#define SETTINGS_KEY 0

Window *window;
TextLayer *info_layer, *time_layer, *date_layer, *beat_layer;

char timeBuffer[] = "00:00";
char beatBuffer[] = "@1000";
char dateBuffer[] = "dd/mm/yy";
int syncStage = 0;
int initialS, second, minute, hour, bmt, beat, utc, h, m, s, valueWritten, valueRead;
time_t utc_test, local;
struct tm *preC;
GFont *walkaway_reg;

typedef struct persist{
	int8_t timezone;
	uint8_t time;
	uint8_t date;
}__attribute__((__packed__)) persist;

persist settings = {
	.timezone = 5,
	.time = 1,
	.date = 1,
};
