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
  else if (!strcmp(easingstr, "sleep")) {
    easing = AnywareEasing::SLEEP;
  }
  else {
    return false;
  }

  return true;
}

void AnywareEasing::start(uint8_t type, uint16_t duration)
{
  this->type = type;
  this->active = true;
  this->startTime = millis();
  this->currval = 0;

  if (duration > 0) {
    this->duration = duration;
  }
  else {
    switch (type) {
    case BINARY:
      this->duration = 0;
      break;
    case LINEAR:
    case SINE:
    case IN_CUBIC:
    case OUT_CUBIC:
    case PULSE:
      this->duration = 2000;
      break;
    case POP:
      this->duration = 500;
      break;
    case SLEEP:
      this->duration = 4000;
      break;
    case BINARY_PULSE:
      this->duration = 200;
      break;
    default:
      break;
    }
  }
}

uint32_t AnywareEasing::end()
{
  active = false;
  return apply(startTime + this->duration);
}

uint8_t AnywareEasing::apply(uint32_t t)
{
  uint32_t dt = t-startTime;
  float v = 0;
  switch (type) {
  case BINARY:
    v = 255;
    break;
  case LINEAR:
    v = Easing::linearTween(dt, 0, 255, this->duration);
    break;
  case SINE:
    v = Easing::easeInOutSine(dt, 0, 255, this->duration);
    break;
  case IN_CUBIC:
    v = Easing::easeInCubic(dt, 0, 255, this->duration);
    break;
  case OUT_CUBIC:
    v = Easing::easeOutCubic(dt, 0, 255, this->duration);
    break;
  case PULSE:
    v = Easing::easeOutCubic(dt, 255, -255, this->duration);
    break;
  case POP:
    v = Easing::easeOutCubic(dt, 128, -128, this->duration);
    break;
  case SLEEP:
    v = Easing::pulseSleep(dt, 128, -128, this->duration);
    break;
  case BINARY_PULSE:
    v = (dt < this->duration) ? 255.0 : 0.0;
    break;
  default:
    break;
  }
  
  if (dt >= this->duration) active = false;
  
  this->currval = (v > 255.0) ? 255 : uint8_t(v);
  return this->currval;
}
