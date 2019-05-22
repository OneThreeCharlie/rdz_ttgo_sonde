#include <U8x8lib.h>
#include <U8g2lib.h>

#include "Display.h"
#include "Sonde.h"

extern Sonde sonde;

extern U8X8_SSD1306_128X64_NONAME_SW_I2C *u8x8;

static unsigned char kmh_tiles[] U8X8_PROGMEM = {
   0x1F, 0x04, 0x0A, 0x11, 0x00, 0x1F, 0x02, 0x04, 0x42, 0x3F, 0x10, 0x08, 0xFC, 0x22, 0x20, 0xF8
   };
static unsigned char ms_tiles[] U8X8_PROGMEM = {
   0x1F, 0x02, 0x04, 0x02, 0x1F, 0x40, 0x20, 0x10, 0x08, 0x04, 0x12, 0xA4, 0xA4, 0xA4, 0x40, 0x00
   };
static unsigned char stattiles[4][4] =  {
   0x00, 0x1F, 0x00, 0x00 ,   // | == ok
   0x00, 0x10, 0x10, 0x00 ,   // . == no header found
   0x1F, 0x15, 0x15, 0x00 ,   // E == decode error
   0x00, 0x00, 0x00, 0x00 };  // ' ' == unknown/unassigned


static uint8_t halfdb_tile[8]={0x80, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00};

static uint8_t halfdb_tile1[8]={0x00, 0x38, 0x28, 0x28, 0x28, 0xC8, 0x00, 0x00};
static uint8_t halfdb_tile2[8]={0x00, 0x11, 0x02, 0x02, 0x02, 0x01, 0x00, 0x00};

static uint8_t empty_tile[8]={0x80, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00};

static uint8_t empty_tile1[8]={0x00, 0xF0, 0x88, 0x48, 0x28, 0xF0, 0x00, 0x00};
static uint8_t empty_tile2[8]={0x00, 0x11, 0x02, 0x02, 0x02, 0x01, 0x00, 0x00};


#define SETFONT(large) u8x8->setFont((large)?u8x8_font_7x14_1x2_r:u8x8_font_chroma48medium8_r);

DispEntry searchLayout[] = {
	{0, 0, FONT_LARGE, disp.drawText, "Scan:"},
	{0, 8, FONT_LARGE, disp.drawType},
	{3, 0, FONT_LARGE, disp.drawFreq, " MHz"},
	{5, 0, FONT_LARGE, disp.drawSite},
	{7, 8, 0, disp.drawIP},	
	{-1, -1, -1, NULL},
};
DispEntry legacyLayout[] = {
	{0, 5, FONT_SMALL, disp.drawFreq, " MHz"},
	{1, 8, FONT_SMALL, disp.drawAFC},
	{0, 0, FONT_SMALL, disp.drawType},
	{1, 0, FONT_SMALL, disp.drawID},
	{2, 0, FONT_LARGE, disp.drawLat},
	{4, 0, FONT_LARGE, disp.drawLon},
	{2, 10, FONT_SMALL, disp.drawAlt},
	{3, 10, FONT_SMALL, disp.drawHS},
	{4, 9, FONT_SMALL, disp.drawVS},
	{6, 0, FONT_LARGE, disp.drawRSSI},
	{6, 7, 0, disp.drawQS},
	{-1, -1, -1, NULL},
};
DispEntry fieldLayout[] = {
	{2, 0, FONT_LARGE, disp.drawLat},
	{4, 0, FONT_LARGE, disp.drawLon},
	{3, 10, FONT_SMALL, disp.drawHS},
	{4, 9, FONT_SMALL, disp.drawVS},
	{0, 0, FONT_LARGE, disp.drawID},
	{6, 0, FONT_LARGE, disp.drawAlt},
	{6, 7, 0, disp.drawQS},
	{-1, -1, -1, NULL},
};
DispEntry fieldLayout2[] = {
	{2, 0, FONT_LARGE, disp.drawLat},
	{4, 0, FONT_LARGE, disp.drawLon},
	{1, 12, FONT_SMALL, disp.drawType},
	{0, 9, FONT_SMALL, disp.drawFreq, ""},
	{3, 10, FONT_SMALL, disp.drawHS},
	{4, 9, FONT_SMALL, disp.drawVS},
	{0, 0, FONT_LARGE, disp.drawID},
	{6, 0, FONT_LARGE, disp.drawAlt},
	{6, 7, 0, disp.drawQS},
	{-1, -1, -1, NULL},
};

DispEntry *layouts[]={searchLayout, legacyLayout, fieldLayout, fieldLayout2};

char Display::buf[17];

Display::Display() {
	setLayout(legacyLayout);
}

void Display::setLayout(DispEntry *newLayout) {
	layout = newLayout;
}

void Display::setLayout(int layoutIdx) {
	layout = layouts[layoutIdx];
}

void Display::drawLat(DispEntry *de) {
	SETFONT(de->fmt);
	if(!sonde.si()->validPos) {
	   u8x8->drawString(de->x,de->y,"<??>      ");
	   return;
	}
	snprintf(buf, 16, "%2.5f", sonde.si()->lat);
	u8x8->drawString(de->x,de->y,buf);
}
void Display::drawLon(DispEntry *de) {
	SETFONT(de->fmt);
	if(!sonde.si()->validPos) {
	   u8x8->drawString(de->x,de->y,"<??>      ");
	   return;
	}
	snprintf(buf, 16, "%2.5f", sonde.si()->lon);
	u8x8->drawString(de->x,de->y,buf);
}
void Display::drawAlt(DispEntry *de) {
	SETFONT(de->fmt);
	if(!sonde.si()->validPos) {
	   u8x8->drawString(de->x,de->y,"     ");
	   return;
	}
	snprintf(buf, 16, sonde.si()->alt>=1000?"   %5.0fm":"   %3.1fm", sonde.si()->alt);
	u8x8->drawString(de->x,de->y,buf+strlen(buf)-6);
}
void Display::drawHS(DispEntry *de) {
	SETFONT(de->fmt);
	if(!sonde.si()->validPos) {
	   u8x8->drawString(de->x,de->y,"     ");
	   return;
	}
	snprintf(buf, 16, sonde.si()->hs>99?" %3.0f":" %2.1f", sonde.si()->hs);
	u8x8->drawString(de->x,de->y,buf+strlen(buf)-4);
	u8x8->drawTile(de->x+4,de->y,2,kmh_tiles);
}
void Display::drawVS(DispEntry *de) {
	SETFONT(de->fmt);
	if(!sonde.si()->validPos) {
	   u8x8->drawString(de->x,de->y,"     ");
	   return;
	}
	snprintf(buf, 16, "  %+2.1f", sonde.si()->vs);
	u8x8->drawString(de->x, de->y, buf+strlen(buf)-5);
	u8x8->drawTile(de->x+5,de->y,2,ms_tiles);

}
void Display::drawID(DispEntry *de) {
	SETFONT((de->fmt&0x01));
	if(!sonde.si()->validID) {
		u8x8->drawString(de->x, de->y, "nnnnnnnn        ");
		return;
	}
	u8x8->drawString(de->x, de->y, sonde.si()->id);
}
void Display::drawRSSI(DispEntry *de) {
	SETFONT(de->fmt);
	snprintf(buf, 16, "-%d   ", sonde.si()->rssi/2);
	int len=strlen(buf)-3;
	buf[5]=0;
	u8x8->drawString(de->x,de->y,buf);
	u8x8->drawTile(de->x+len, de->y, 1, (sonde.si()->rssi&1)?halfdb_tile1:empty_tile1);
	u8x8->drawTile(de->x+len, de->y+1, 1, (sonde.si()->rssi&1)?halfdb_tile2:empty_tile2);
}
void Display::drawQS(DispEntry *de) {
	uint8_t *stat = sonde.si()->rxStat;
	for(int i=0; i<18; i+=2) {
	        uint8_t tile[8];
	        *(uint32_t *)(&tile[0]) = *(uint32_t *)(&(stattiles[stat[i]]));
	        *(uint32_t *)(&tile[4]) = *(uint32_t *)(&(stattiles[stat[i+1]]));
	        u8x8->drawTile(de->x+i/2, de->y, 1, tile);
	}
}
void Display::drawType(DispEntry *de) {
	SETFONT(de->fmt);
        u8x8->drawString(de->x, de->y, sondeTypeStr[sonde.si()->type]);
}
void Display::drawFreq(DispEntry *de) {
	SETFONT(de->fmt);
        snprintf(buf, 16, "%3.3f%s", sonde.si()->freq, de->extra?de->extra:"");
        u8x8->drawString(de->x, de->y, buf);
}
void Display::drawAFC(DispEntry *de) {
 	if(!sonde.config.showafc) return;
	SETFONT(de->fmt);
        snprintf(buf, 15, "     %+3.2fk", sonde.si()->afc*0.001);
        u8x8->drawString(de->x, de->y, buf+strlen(buf)-8);
}
void Display::drawIP(DispEntry *de) {
}
void Display::drawSite(DispEntry *de) {
        SETFONT(de->fmt);
	u8x8->drawString(de->x, de->y, sonde.si()->launchsite);
}
void Display::drawTelemetry(DispEntry *de) {
}
void Display::drawGPSdist(DispEntry *de) {
}
void Display::drawText(DispEntry *de) {
        SETFONT(de->fmt);
	u8x8->drawString(de->x, de->y, de->extra);
}

void Display::updateDisplayPos() {
	for(DispEntry *di=layout; di->func != NULL; di++) {
		if(di->func != disp.drawLat && di->func != disp.drawLon) continue;
		di->func(di);
	}
}
void Display::updateDisplayPos2() {
	for(DispEntry *di=layout; di->func != NULL; di++) {
		if(di->func != disp.drawAlt && di->func != disp.drawHS && di->func != disp.drawVS) continue;
		di->func(di);
	}
}
void Display::updateDisplayID() {
	for(DispEntry *di=layout; di->func != NULL; di++) {
		if(di->func != disp.drawID) continue;
		di->func(di);
	}
}
void Display::updateDisplayRSSI() {
	for(DispEntry *di=layout; di->func != NULL; di++) {
		if(di->func != disp.drawRSSI) continue;
		di->func(di);
	}
}
void Display::updateStat() {
	for(DispEntry *di=layout; di->func != NULL; di++) {
		if(di->func != disp.drawQS) continue;
		di->func(di);
	}
}

void Display::updateDisplayRXConfig() {
       for(DispEntry *di=layout; di->func != NULL; di++) {
                if(di->func != disp.drawQS && di->func != disp.drawAFC) continue;
                di->func(di);
        }
}
void Display::updateDisplayIP() {
}

void Display::updateDisplay() {
	for(DispEntry *di=layout; di->func != NULL; di++) {
		di->func(di);
	}
}

Display disp = Display();
