#ifndef GLOBALS_H
#define GLOBALS_H

#include <SPI.h>
#include <TFT_eSPI.h>
#include <vector>
#include <map>

extern TFT_eSPI tft;
extern int prevBrightness;
extern int brightness;
void setBrightness(int value);

class Screen
{
private:
    std::vector<std::pair<short, short>> longPressCoordArray;
    std::vector<std::pair<short, short>> swipeCoordArray;
    std::vector<std::pair<short, short>> pressCoordArray;

    std::vector<void (*)()> longPressActions;
    std::vector<void (*)()> swipeActions;
    std::vector<void (*)()> pressActions;

public:
    Screen(std::vector<std::pair<short, short>> LPC = {}, std::vector<std::pair<short, short>> SC = {}, std::vector<std::pair<short, short>> PC = {},
           std::vector<void (*)()> LPA = {}, std::vector<void (*)()> SA = {}, std::vector<void (*)()> PA = {})
    {
        longPressCoordArray = LPC;
        swipeCoordArray = SC;
        pressCoordArray = PC;
        longPressActions = LPA;
        swipeActions = SA;
        pressActions = PA;
    }

    void processTouch(short *touchQueuedAction)
    {

        std::vector<std::pair<short, short>> coords;
        std::vector<void (*)()> actions;
        short x, y, devX, devY;
        x = touchQueuedAction[1];
        y = touchQueuedAction[2];

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

extern Screen *activeScreen;

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