#ifndef SCREENS_H
#define SCREENS_H

#ifndef GLOBALS_H
#include <globals.h>
#endif

class ScreenObject
{
private:
    std::vector<std::pair<int, int>> longPressCoordArray;
    std::vector<std::pair<int, int>> swipeCoordArray;
    std::vector<std::pair<int, int>> pressCoordArray;

    std::vector<void (*)()> longPressActions;
    std::vector<void (*)()> swipeActions;
    std::vector<void (*)()> pressActions;

public:
    ScreenObject(std::vector<std::pair<int, int>> LPC = {}, std::vector<std::pair<int, int>> SC = {}, std::vector<std::pair<int, int>> PC = {},
                 std::vector<void (*)()> LPA = {}, std::vector<void (*)()> SA = {}, std::vector<void (*)()> PA = {})
    {
        longPressCoordArray = LPC;
        swipeCoordArray = SC;
        pressCoordArray = PC;
        longPressActions = LPA;
        swipeActions = SA;
        pressActions = PA;
    }

    void processTouch(int *touchQueuedAction)
    {

        std::vector<std::pair<int, int>> coords;
        std::vector<void (*)()> actions;
        int x, y, devX, devY;
        x = touchQueuedAction[1];
        y = touchQueuedAction[2];

        // TODO usunąć printy
        Serial.println();
        Serial.println(touchQueuedAction[0]);
        Serial.println(x);
        Serial.println(y);

        devX = 20;
        devY = 20;

        // Select arrays to corresponding touch type
        switch (touchQueuedAction[0])
        {
            {
            case 0:
                coords = longPressCoordArray;
                actions = longPressActions;
                break;
            case 1:
                coords = swipeCoordArray;
                actions = swipeActions;
                devX = 480;
                devY = 360;
                break;
            case 2:
                coords = pressCoordArray;
                actions = pressActions;
                break;
            }
        }

        for (int i = 0; i < coords.size(); i++)
        {
            // Check for whole display action
            if (coords[i].first == -999 && coords[i].second == 999)
            {
                actions[i]();
                break;
            }
            /* Check touched coordinates or deviation
                Parameters for deviation:
                    {-540, 0}   Right swipe
                    {540, 0}    Left swipe
                    {0, 390}    Up swipe
                    {0, -390}   Down swipe
            */
            if (abs(coords[i].first - x) <= devX && abs(coords[i].second - y) <= devY)
            {
                actions[i]();
                break;
            }
        }
    }
};

extern ScreenObject *activeScreenElement;

void displaySleep();
void menuScreen();
void clockScreen();
void test1Screen();
void test2Screen();
void testR();
void testL();
void testU();
void testD();

#endif