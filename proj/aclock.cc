#include "aclock.h"
#include "utils.h"
#include <time.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

//init function for the clock
void Clock::initWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas){
    //load the font
    if(!loadedFont.LoadFont(fetchFont(20).c_str())){
        cout << "Could not load font" << endl;
    }
}

int Clock::shouldSwitchWidget(){
    return 0;
}

//draw function for the clock
void Clock::drawWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, atomic_bool& end){
    cout << "Drawing clock" << endl;
    canvas->Clear();
    canvas->Fill(0, 0, 0);
    //get the time
    time_t now = time(0);
    tm *ltm = localtime(&now);
    //get the time in string format
    char *time = asctime(ltm);
    char *time_to_draw = new char[6];
    char colon_char = show_colon ? ':' : ' ';
    time_to_draw[0] = time[11];
    time_to_draw[1] = time[12];
    time_to_draw[2] = colon_char;
    time_to_draw[3] = time[14];
    time_to_draw[4] = time[15];
    time_to_draw[5] = '\0';
    cout << time_to_draw << endl;
    //draw the time
    rgb_matrix::DrawText(canvas, loadedFont, 7, 22, rgb_matrix::Color(255, 0, 0), NULL, time_to_draw, 0, 0, 64, 0, false);
    canvas = matrix->SwapOnVSync(canvas);
    show_colon = !show_colon;
    usleep(delay_speed_usec);
}