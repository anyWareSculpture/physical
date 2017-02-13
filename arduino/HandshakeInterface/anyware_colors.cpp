#include "anyware_colors.h"

const CRGB locationColor[3] = {
  MYBLUE,
  MYYELLOW,
  MYPINK
};

void getRGB(const CRGB &col, uint8_t &r, uint8_t &g, uint8_t &b)
{
  r = col.r;
  g = col.g;
  b = col.b;
}

CRGB applyIntensity(const CRGB &col, uint8_t intensity)
{
  return CRGB(uint8_t(col.r * intensity / 100),
              uint8_t(col.g * intensity / 100),
              uint8_t(col.b * intensity / 100));
}

bool getColor(const char *colorstr, CRGB &col)
{
  if (!strcmp(colorstr, "user0")) {
    col = locationColor[0];
  }
  else if (!strcmp(colorstr, "user1")) {
    col = locationColor[1];
  }
  else if (!strcmp(colorstr, "user2")) {
    col = locationColor[2];
  }
  else if (!strcmp(colorstr, "error")) {
    col = RED;
  }
  else if (!strcmp(colorstr, "success")) {
    col = GREEN;
  }
  else if (!strcmp(colorstr, "black")) {
    col = BLACK;
  }
  else if (!strcmp(colorstr, "white")) {
    col = WHITE;
  }
  else if (!strcmp(colorstr, "rgb0")) {
    col = RGB0;
  }
  else if (!strcmp(colorstr, "rgb1")) {
    col = RGB1;
  }
  else {
    return false;
  }
  return true;
}
