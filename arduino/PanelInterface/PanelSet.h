#ifndef PANELSET_H_
#define PANELSET_H_

class PanelSet
{
public:
  PanelSet() {}
  bool get(uint8_t panel) const { return (panels >> panel) & 0x01; }
  void set(uint8_t panel) { panels |= (static_cast<uint16_t>(1) << panel); }
  void clear(uint8_t panel) { panels &= ~(static_cast<uint16_t>(1) << panel); }
  
  uint16_t panels{0};
};

#endif
