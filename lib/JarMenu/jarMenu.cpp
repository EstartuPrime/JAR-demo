#include <Zumo32U4.h>
#include <jarMenu.h>
#include <jarButton.h>

#ifndef MENU_CPP
#define MENU_CPP

JarMenu::JarMenu(JarMenuItem *items, uint8_t itemCount, Zumo32U4LCD *lcd)
{
    this->items = items;
    this->itemCount = itemCount;
    this->lcdRef = lcd;
    lcdItemIndex = 0;
}

void JarMenu::lcdUpdate(uint8_t index)
{
    this->lcdRef->clear();
    this->lcdRef->print(items[index].name);
    this->lcdRef->gotoXY(0, 1);
    this->lcdRef->print(F("\x7f"
                          "A \xa5"
                          "B C\x7e"));
}

void JarMenu::action(uint8_t index)
{
    items[index].action();
}

// Prompts the user to choose one of the menu items,
// then runs it, then returns.
void JarMenu::select()
{
    lcdUpdate(lcdItemIndex);

    while (1)
    {
        switch (JarButton::monitor())
        {
        case 'A':
            // The A button was pressed so decrement the index.
            if (lcdItemIndex == 0)
            {
                lcdItemIndex = itemCount - 1;
            }
            else
            {
                lcdItemIndex--;
            }
            lcdUpdate(lcdItemIndex);
            break;

        case 'C':
            // The C button was pressed so increase the index.
            if (lcdItemIndex >= itemCount - 1)
            {
                lcdItemIndex = 0;
            }
            else
            {
                lcdItemIndex++;
            }
            lcdUpdate(lcdItemIndex);
            break;

        case 'B':
            // The B button was pressed so run the item and return.
            action(lcdItemIndex);
            return;
        }
    }
}

#endif
