//
// Created by Admin on 13/08/2024.
//

#ifndef SHREKWATCHY_FACE_H
#define SHREKWATCHY_FACE_H

#include <Watchy.h>
#include "frames.h"

class ShrekWatchy : public Watchy {
    using Watchy::Watchy;

public:
    void drawWatchFace();
};
#endif //SHREKWATCHY_FACE_H
