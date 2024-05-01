#ifndef SCREENS_H
#define SCREENS_H

#ifndef GLOBALS_H
#include <globals.h>
#endif

class ScreenObject
{
private:
    std::vector<std::array<int, 4>> longPressCoordArray;
    std::vector<std::array<int, 4>> swipeCoordArray;
    std::vector<std::array<int, 4>> pressCoordArray;

    std::vector<void (*)(int *touchQueuedAction)> longPressActions;
    std::vector<void (*)(int *touchQueuedAction)> swipeActions;
    std::vector<void (*)(int *touchQueuedAction)> pressActions;

public:
    ScreenObject(std::vector<std::array<int, 4>> LPC = {}, std::vector<std::array<int, 4>> SC = {}, std::vector<std::array<int, 4>> PC = {},
                 std::vector<void (*)(int *touchQueuedAction)> LPA = {}, std::vector<void (*)(int *touchQueuedAction)> SA = {}, std::vector<void (*)(int *touchQueuedAction)> PA = {})
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

        std::vector<std::array<int, 4>> coords;
        std::vector<void (*)(int *touchQueuedAction)> actions;
        int x, y, devX, devY;
        x = touchQueuedAction[1];
        y = touchQueuedAction[2];

        // TODO usunąć printy
        Serial.println();
        Serial.println(touchQueuedAction[0]);
        Serial.println(x);
        Serial.println(y);

        // devX = 20;
        // devY = 20;

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
                // devX = 480;
                // devY = 360;
                break;
            case 2:
                coords = pressCoordArray;
                actions = pressActions;
                break;
            }
        }

        for (int i = 0; i < coords.size(); i++)
        {

            /* Check touched coordinates or deviation
                Parameters for deviation:
                    {-540, 0}   Right swipe
                    {540, 0}    Left swipe
                    {0, 390}    Up swipe
                    {0, -390}   Down swipe
            */
            if (coords[i][0] < x && x < coords[i][2] && coords[i][1] < y && y < coords[i][3])
            {
                actions[i](touchQueuedAction);
                break;
            }
        }
    }
};

extern ScreenObject *activeScreenElement;

void displaySleep(int *touchQueuedAction);
void menuScreen(int *touchQueuedAction);
void clockScreen(int *touchQueuedAction);
void test1Screen(int *touchQueuedAction);
void test2Screen(int *touchQueuedAction);
void testDirection(int *touchQueuedAction);

#endif