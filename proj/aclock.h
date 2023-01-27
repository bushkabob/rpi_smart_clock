#include "led-matrix.h"
#include "graphics.h"
#include "widget.h"
#include <condition_variable>

#include <atomic>

using rgb_matrix::RGBMatrix;

//Clock class that inherits from the widget class
class Clock : public Widget{
    public:
        void initWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas);
        void drawWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, atomic_bool& end);
        int shouldSwitchWidget();
    private:
        int delay_speed_usec = 1000000;
        bool show_colon = true;
        rgb_matrix::Font loadedFont;
};