#define SETTINGS_KEY 0
	
#define TIMEZONE_KEY 0
#define TIMEENABLED_KEY 1
#define DATEENABLED_KEY 2
#define DEBUG_KEY 3
	
Window *window;
TextLayer *info_layer, *time_layer, *date_layer, *beat_layer;

char timeBuffer[] = "00:00";
char beatBuffer[] = "@1000";
char dateBuffer[] = "dd/mm/yy";
int syncStage = 0;
int initialS, second, minute, hour, bmt, beat, utc, h, m, s, valueWritten, valueRead;
int timezoneFixHour, timezoneFixMin;
time_t utc_test, local;
struct tm *preC;
GFont *walkaway_reg;

int currentAppVer = 2;
AppTimer *glanceTimer;

char infoBuffer[] = "Configured to TZ1/TZ2";

typedef struct zonepack {
	char pack[30][7];
} zonepack;

zonepack timezone = {
	.pack = {
		"MIT", "HST", "AST", "PST", "PNT/MST", "CST", "EST/IET", "PRT", "CNT", "AGT/BET",  "CAT",
		"GMT/UTC", "ETC", "EET/ART", "EAT", "NET", "PLT", "BST", "VST",
		"CTT", "JST", "AET", "SST", "NST",
    },
};

typedef struct persist{
	double timezone;
	uint8_t time;
	uint8_t date;
	bool debug;
}__attribute__((__packed__)) persist;

persist settings = {
	.timezone = 5,
	.time = 1,
	.date = 1,
	.debug = 0,
};

char *glanceChar[1][11] = {
	{
		"Settings updated.",
	}
};

GRect start02, finish01;
char glance_buffer[160];
//Glance variables
int holdUpSentence;
bool holdUpVibrate;
int holdUpVibrateNum;
int holdUpAnimationLength, intro, glanceTimerCalled, value;
int holdUpFullNotify;
bool currentlyGlancing = 0;
TextLayer *update_at_a_glance;

void glance_this(int sentence, bool vibrate, int vibrateNum, int animationLength, int fullNotify);
