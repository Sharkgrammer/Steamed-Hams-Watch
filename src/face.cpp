//
// Created by Admin on 13/08/2024.
//

#include "face.h"
void ShrekWatchy::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, f3, 200, 200, GxEPD_BLACK);
}
