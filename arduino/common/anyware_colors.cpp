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

uint32_t scaleColor(uint32_t from, uint32_t to, uint8_t ratio) {
  uint8_t a[3] = {
    (uint8_t)(from >> 16),
    (uint8_t)(from >>  8),
    (uint8_t)from };

  uint8_t b[3] = {
    (uint8_t)(to >> 16),
    (uint8_t)(to >>  8),
    (uint8_t)to };

  uint32_t c = Color(uint8_t(a[0] + (b[0] - a[0]) * ratio / 255),
                     uint8_t(a[1] + (b[1] - a[1]) * ratio / 255),
                     uint8_t(a[2] + (b[2] - a[2]) * ratio / 255));

  return c;
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
