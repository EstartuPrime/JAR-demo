#include <jarMenuItem.h>

class JarMenu
{
public:
  JarMenu(JarMenuItem *items, uint8_t itemCount, Zumo32U4LCD *lcd);
  void lcdUpdate(uint8_t index);
  void action(uint8_t index);
  void select();

private:
  JarMenuItem *items;
  uint8_t itemCount;
  uint8_t lcdItemIndex;
  Zumo32U4LCD *lcdRef;
};
