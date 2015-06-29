#include "AnywareEasing.h"

bool AnywareEasing::getEasing(const char *easingstr, AnywareEasing::EasingType &easing)
{
  if (!strcmp(easingstr, "easein")) {
    easing = AnywareEasing::IN_CUBIC;
  }
  else if (!strcmp(easingstr, "pulse")) {
    easing = AnywareEasing::PULSE;
  }
  else if (!strcmp(easingstr, "pop")) {
    easing = AnywareEasing::POP;
  }
  else {
    return false;
  }

  return true;
}

void AnywareEasing::start(uint8_t type)
{
  this->type = type;
  this->active = true;
  this->startTime = millis();
  
  switch (type) {
  case BINARY:
    duration = 0;
    break;
  case LINEAR:
  case SINE:
  case IN_CUBIC:
  case OUT_CUBIC:
  case PULSE:
    duration = 2000;
    break;
  case POP:
    duration = 500;
    break;
  default:
    break;
  }
  
}

uint8_t AnywareEasing::apply()
{
  uint32_t dt = millis()-startTime;
  float v = 0;
  switch (type) {
  case BINARY:
    v = 255;
    break;
  case LINEAR:
    v = Easing::linearTween(dt, 0, 255, duration);
    break;
  case SINE:
    v = Easing::easeInOutSine(dt, 0, 255, duration);
    break;
  case IN_CUBIC:
    v = Easing::easeInCubic(dt, 0, 255, duration);
    break;
  case OUT_CUBIC:
    v = Easing::easeOutCubic(dt, 0, 255, duration);
    break;
  case PULSE:
    v = Easing::easeOutCubic(dt, 255, -255, duration);
    break;
  case POP:
    v = Easing::easeOutCubic(dt, 128, -128, duration);
    break;
  default:
    break;
  }
  
  if (dt >= duration) active = false;
  
  uint8_t val = (uint8_t)v;
  return val;
}
