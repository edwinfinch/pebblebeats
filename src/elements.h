Window *window;
TextLayer *info_layer, *time_layer, *date_layer, *beat_layer;

char timeBuffer[] = "00:00";
char beatBuffer[] = "@1000";
char dateBuffer[] = "dd/mm/yy";
int syncStage = 0;
int dotDotDot, second, minute, hour, bmt, beat, utc, h, m, s;
time_t utc_test, local;
struct tm *preC;