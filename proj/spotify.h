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

using rgb_matrix::RGBMatrix;
using namespace std;

//Spotify class that inherits from the widget class
class Spotify : public Widget{
    public:
        void initWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas);
        void drawWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, atomic_bool& end);
        static Spotify& getInstance(){
            static Spotify instance;
            return instance;
        }
        Spotify(Spotify const&) = delete;
        void operator=(Spotify const&) = delete;
        int shouldSwitchWidget();
    private:
        Spotify();
        string fetchAccessToken();
        vector<string> fetchCurrentSong();
        void fetchWidgetData(atomic_bool& request_end);
        void layoutWidgetFields(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas);
        void fetchAlbumArt(string url);
        void drawAlbumArt(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas);
        string access_token;
        int delay_speed_usec = 1000000;
        atomic_bool new_data{false};
        string song = "";
        string album = "";
        string artist = "";
        vector<unsigned char> album_art = {};
        int song_x = 1;
        int album_x = 32;
        int artist_x = 32;
        bool direction_left = true;
        rgb_matrix::Font loadedFont;
        rgb_matrix::Font loadedFontSmall;
};