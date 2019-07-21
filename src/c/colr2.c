#include <pebble.h>
#include <math.h>

static Window *mainWindow;
static Layer *pixelLayer;
static TextLayer *hrsLayer;
static TextLayer *hrsLayerShadow;
static TextLayer *minsLayer;
static TextLayer *minsLayerShadow;
static TextLayer *secsLayer;
static TextLayer *secsLayerShadow;
static TextLayer *dateLayer;
static TextLayer *dateLayerShadow;
static GFont timeFont;
static GFont timeFontSmall;
static GFont timeFontTiny;

#define WIDTH 30
#define HEIGHT 30
#define SIZE 6

int min(int x, int y) {
	return y ^ ((x ^ y) & -(x < y));
}

int max(int x, int y) { 
	return x ^ ((x ^ y) & -(x < y));  
}

void strupr(char s[]) {
   int c = 0;
   
   while (s[c] != '\0') {
      if (s[c] >= 'a' && s[c] <= 'z') {
         s[c] = s[c] - 32;
      }
      c++;
   }
}

static int palette = 0;
static bool rands[WIDTH][HEIGHT] = {{0}};
static int finals[WIDTH][HEIGHT] = {{0}};
static void updateRands() {
	for(int y = 0; y < HEIGHT; y++) {
		for(int x = 0; x < WIDTH; x++) {
			rands[x][y] = rand() % 2;
		}
	}

	for(int y = 1; y < HEIGHT-1; y++) {
		for(int x = 1; x < WIDTH-1; x++) {
			finals[x-1][y-1] = min(
				floor((
					rands[x-1][y-1] + rands[x+1][y-1] +
					rands[x][y] + rands[x][y] + 
					rands[x-1][y+1] + rands[x+1][y+1]
					)
					/1.67)
				, 2
			);
		}
	}

	palette = rand() % 4;
}

static int seconds = 0;
static bool firstInit = false;

static void pixelUpdateProc(Layer *layer, GContext *ctx) {
	/* can't define these globally, compiler spits out "initializer element is not constant" errors */
	GColor colors[][3] = {
		{GColorCobaltBlue, GColorVividCerulean, GColorElectricBlue},
		{GColorElectricBlue, GColorVividCerulean, GColorCobaltBlue},
		{GColorIndigo, GColorVividViolet, GColorShockingPink},
		{GColorShockingPink, GColorVividViolet, GColorIndigo}
	};

	GColor dateColors[4] = {
		GColorRichBrilliantLavender,
		GColorRichBrilliantLavender,
		GColorCeleste,
		GColorCeleste
	};

	GRect bounds = layer_get_bounds(layer);

	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, bounds, 0, GCornerNone);

	if(!seconds || !firstInit) {
		updateRands();
		text_layer_set_text_color(dateLayer, dateColors[palette]);
	}
	firstInit = true;

	for(int y = 0; y < HEIGHT-2; y++) {
		for(int x = 0; x < WIDTH-2; x++) {
			graphics_context_set_fill_color(ctx, colors[palette][finals[x][y]]);
			graphics_fill_rect(ctx, GRect((x)*SIZE, (y)*SIZE, SIZE, SIZE), 0, 0);
		}
	}
}

#define HRS_POS_X -7
#define HRS_POS_Y -5
#define MINS_POS_X -7
#define MINS_POS_Y 55
#define TEXT_HEIGHT 80
#define SHADOW_OFFSET 3
#define SECS_POS_X 93
#define SECS_POS_Y 45
#define SECS_TEXT_HEIGHT 40
#define SECS_SHADOW_OFFSET 2
#define DATE_POS_X 0
#define DATE_POS_Y 135
#define DATE_SHADOW_OFFSET 1
#define DATE_TEXT_HEIGHT 24

static void mainWindowLoad(Window *window) {
	GColor dateColors[4] = {
		GColorRichBrilliantLavender,
		GColorRichBrilliantLavender,
		GColorCeleste,
		GColorCeleste
	};

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	pixelLayer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(pixelLayer, pixelUpdateProc);
	layer_add_child(window_get_root_layer(mainWindow), pixelLayer);

	timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MANIA_73));
	timeFontSmall = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MANIA_40));
	timeFontTiny = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MANIA_24));

	hrsLayer = text_layer_create(GRect(HRS_POS_X, HRS_POS_Y, floor(bounds.size.w/1.5), TEXT_HEIGHT));
	hrsLayerShadow = text_layer_create(GRect(HRS_POS_X + SHADOW_OFFSET, HRS_POS_Y + SHADOW_OFFSET, floor(bounds.size.w/1.5), TEXT_HEIGHT));
	text_layer_set_background_color(hrsLayer, GColorClear);
	text_layer_set_background_color(hrsLayerShadow, GColorClear);
	text_layer_set_text_color(hrsLayer, GColorWhite);
	text_layer_set_text_color(hrsLayerShadow, GColorBlack);
	text_layer_set_font(hrsLayer, timeFont);
	text_layer_set_font(hrsLayerShadow, timeFont);
	text_layer_set_text_alignment(hrsLayer, GTextAlignmentRight);
	text_layer_set_text_alignment(hrsLayerShadow, GTextAlignmentRight);

	layer_add_child(window_layer, text_layer_get_layer(hrsLayerShadow));
	layer_add_child(window_layer, text_layer_get_layer(hrsLayer));

	minsLayer = text_layer_create(GRect(MINS_POS_X, MINS_POS_Y, floor(bounds.size.w/1.5), TEXT_HEIGHT));
	minsLayerShadow = text_layer_create(GRect(MINS_POS_X + SHADOW_OFFSET, MINS_POS_Y + SHADOW_OFFSET, floor(bounds.size.w/1.5), TEXT_HEIGHT));
	text_layer_set_background_color(minsLayer, GColorClear);
	text_layer_set_background_color(minsLayerShadow, GColorClear);
	text_layer_set_text_color(minsLayer, GColorWhite);
	text_layer_set_text_color(minsLayerShadow, GColorBlack);
	text_layer_set_font(minsLayer, timeFont);
	text_layer_set_font(minsLayerShadow, timeFont);
	text_layer_set_text_alignment(minsLayer, GTextAlignmentRight);
	text_layer_set_text_alignment(minsLayerShadow, GTextAlignmentRight);

	layer_add_child(window_layer, text_layer_get_layer(minsLayerShadow));
	layer_add_child(window_layer, text_layer_get_layer(minsLayer));

	secsLayer = text_layer_create(GRect(SECS_POS_X, SECS_POS_Y, 40, SECS_TEXT_HEIGHT));
	secsLayerShadow = text_layer_create(GRect(SECS_POS_X + SECS_SHADOW_OFFSET, SECS_POS_Y + SECS_SHADOW_OFFSET, 40, SECS_TEXT_HEIGHT));
	text_layer_set_background_color(secsLayer, GColorClear);
	text_layer_set_background_color(secsLayerShadow, GColorClear);
	text_layer_set_text_color(secsLayer, GColorWhite);
	text_layer_set_text_color(secsLayerShadow, GColorBlack);
	text_layer_set_font(secsLayer, timeFontSmall);
	text_layer_set_font(secsLayerShadow, timeFontSmall);
	text_layer_set_text_alignment(secsLayer, GTextAlignmentLeft);
	text_layer_set_text_alignment(secsLayerShadow, GTextAlignmentLeft);

	layer_add_child(window_layer, text_layer_get_layer(secsLayerShadow));
	layer_add_child(window_layer, text_layer_get_layer(secsLayer));

	dateLayer = text_layer_create(GRect(DATE_POS_X, DATE_POS_Y, bounds.size.w, DATE_TEXT_HEIGHT));
	dateLayerShadow = text_layer_create(GRect(DATE_POS_X + DATE_SHADOW_OFFSET, DATE_POS_Y + DATE_SHADOW_OFFSET, bounds.size.w, DATE_TEXT_HEIGHT));
	text_layer_set_background_color(dateLayer, GColorClear);
	text_layer_set_background_color(dateLayerShadow, GColorClear);
	text_layer_set_text_color(dateLayer, dateColors[palette]);
	text_layer_set_text_color(dateLayerShadow, GColorBlack);
	text_layer_set_font(dateLayer, timeFontTiny);
	text_layer_set_font(dateLayerShadow, timeFontTiny);
	text_layer_set_text_alignment(dateLayer, GTextAlignmentCenter);
	text_layer_set_text_alignment(dateLayerShadow, GTextAlignmentCenter);

	layer_add_child(window_layer, text_layer_get_layer(dateLayerShadow));
	layer_add_child(window_layer, text_layer_get_layer(dateLayer));
}

static void mainWindowUnload(Window *window) {
	layer_destroy(pixelLayer);
	text_layer_destroy(hrsLayer);
	text_layer_destroy(hrsLayerShadow);
	text_layer_destroy(minsLayer);
	text_layer_destroy(minsLayerShadow);
	text_layer_destroy(secsLayer);
	text_layer_destroy(secsLayerShadow);
	text_layer_destroy(dateLayer);
	text_layer_destroy(dateLayerShadow);
	fonts_unload_custom_font(timeFont);
	fonts_unload_custom_font(timeFontSmall);
	fonts_unload_custom_font(timeFontTiny);
}

static void update_time() {
	// Get a tm structure
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	// Write the current hours into a buffer
	static char hrsBuffer[4];
	static int hrsint;
	strftime(hrsBuffer, sizeof(hrsBuffer), clock_is_24h_style() ? "%H" : "%I", tick_time);
	hrsint = atoi(hrsBuffer);
	snprintf(hrsBuffer, sizeof(hrsBuffer), "%d", hrsint);

	// Display this time on the hrsLayer
	text_layer_set_text(hrsLayer, hrsBuffer);
	text_layer_set_text(hrsLayerShadow, hrsBuffer);

	static char minsBuffer[4];
	strftime(minsBuffer, sizeof(minsBuffer), "%M", tick_time);

	text_layer_set_text(minsLayer, minsBuffer);
	text_layer_set_text(minsLayerShadow, minsBuffer);

	static char secsBuffer[4];
	strftime(secsBuffer, sizeof(secsBuffer), "%S", tick_time);
	seconds = atoi(secsBuffer);

	text_layer_set_text(secsLayer, secsBuffer);
	text_layer_set_text(secsLayerShadow, secsBuffer);

	static char dateBuffer[20];
	strftime(dateBuffer, sizeof(dateBuffer), "%a %b %d", tick_time);
	strupr(dateBuffer);

	text_layer_set_text(dateLayer, dateBuffer);
	text_layer_set_text(dateLayerShadow, dateBuffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

static void init() {
	mainWindow = window_create();

	window_set_window_handlers(mainWindow, (WindowHandlers) {
		.load = mainWindowLoad,
		.unload = mainWindowUnload
	});

	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

	window_stack_push(mainWindow, true);

	update_time();
}

static void deinit() {
	tick_timer_service_unsubscribe();
	window_destroy(mainWindow);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}