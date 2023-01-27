#include "led-matrix.h"
#include "graphics.h"

#include <nlohmann/json.hpp>
#include "widget.h"
using json = nlohmann::json;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

#include <vector>
#include <string>
#include <iostream>
#include <atomic>
#include <thread>

using rgb_matrix::RGBMatrix;
using namespace std;

//Spotify class that inherits from the widget class
class Stock : public Widget{
    public:
        void initWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas);
        void drawWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, atomic_bool& end);
        void layoutWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas);
        void drawChart(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, int text_end);
        void fetchStockData();
        int shouldSwitchWidget();
        Stock(string ticker);
    private:
        vector<double> stock_data{};
        string ticker;
        void fetchApiKey();
        string api_key;
        const int delay_speed_usec = 1000000;
        rgb_matrix::Font loadedFont;
        rgb_matrix::Font loadedFontSmall;
};