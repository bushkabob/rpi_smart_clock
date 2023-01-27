#ifndef WIDGET_H
#define WIDGET_H
#include "led-matrix.h"
#include "graphics.h"
#include <atomic>
#include <condition_variable>

using rgb_matrix::RGBMatrix;
using namespace std;

//create an abstract class for a widget, it should have an init function and a draw function and should be a singleton
class Widget{
    public:
        virtual void initWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas);
        virtual void drawWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, atomic_bool& end);
        virtual int shouldSwitchWidget();
};
#endif