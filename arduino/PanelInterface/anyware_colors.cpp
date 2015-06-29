#include "anyware_colors.h"

const uint32_t locationColor[3] = {
  MYBLUE,
  MYYELLOW,
  MYPINK
};

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

void getRGB(uint32_t col, uint8_t &r, uint8_t &g, uint8_t &b)
{
  r = (uint8_t)(col >> 16);
  g = (uint8_t)(col >>  8);
  b = (uint8_t)col;
}

uint32_t scaleColor(uint32_t from, uint32_t to, uint8_t ratio) {
  uint8_t a[3], b[3];
  getRGB(from, a[0], a[1], a[2]);
  getRGB(to, b[0], b[1], b[2]);

  uint32_t c = Color(uint8_t(a[0] + (b[0] - a[0]) * ratio / 255),
                     uint8_t(a[1] + (b[1] - a[1]) * ratio / 255),
                     uint8_t(a[2] + (b[2] - a[2]) * ratio / 255));

  return c;
}

uint32_t applyIntensity(uint32_t col, uint8_t intensity)
{
  uint8_t rgb[3];
  getRGB(col, rgb[0], rgb[1], rgb[2]);
  return Color(uint8_t(rgb[0] * intensity / 100),
               uint8_t(rgb[1] * intensity / 100),
               uint8_t(rgb[2] * intensity / 100));
}

bool getColor(const char *colorstr, uint32_t &col)
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
  else {
    return false;
  }
  return true;
}
