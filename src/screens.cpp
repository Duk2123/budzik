#include <globals.h>

class Screen
{
public:
    String name;
    std::map<std::pair<short, short>, void (*)()> actionDictionary[3];

    Screen(String val1, std::map<std::pair<short, short>, void (*)()> val2 = {}, std::map<std::pair<short, short>, void (*)()> val3 = {}, std::map<std::pair<short, short>, void (*)()> val4 = {})
    {
        name = val1;
        actionDictionary[0] = val2;
        actionDictionary[1] = val3;
        actionDictionary[2] = val4;
    }
};

Screen screens[2] = {
    // "nameOfScreen", {dictionary of {x,y}, action for actionType 0'long press'}, -//- for swipe, -//- for press
    {"menu", {}, {}, {{{110, 210}, clockScreen}}},
    {"clock", {}, {{{0, -40}, menuScreen}}, {}}};

/* TODO do dokończenia i przetestowania:
        -po wykryciu dotyku w handleTouch sprawdzić który ekran jest aktywny i dla którego przeszukiwać akcje ->
            -> sprawdzić czy koordynaty dla odpowiedniej akcji są w zakresie błędu od któregoś z podanych ? uruchomić akcje odpowiednią akcje : nic
        -dodać zmiane aktywnego ekranu do akcji
*/

String activeScreen;

void clockScreen()
{
    tft.fillScreen(0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 20, 4);
    tft.print("CLOCK");
}

void menuScreen()
{
    tft.fillScreen(0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 20, 4);
    tft.print("MENU");
    tft.setTextSize(1);

    tft.setCursor(68, 141, 4);
    tft.print("Opcja 1");

    tft.setCursor(198, 141, 4);
    tft.print("Opcja 2");

    tft.setCursor(328, 141, 4);
    tft.print("Opcja 3");

    tft.fillRect(90, 190, 40, 40, TFT_RED);
    tft.fillRect(220, 190, 40, 40, TFT_GREEN);
    tft.fillRect(350, 190, 40, 40, TFT_BLUE);
}