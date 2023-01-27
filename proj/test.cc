#include "led-matrix.h"
#include "graphics.h"
// #include "spotify.h"
// #include "widget.h"
// #include "utils.h"
// #include "aclock.h"

#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

#include <vector>
#include <string>
#include <iostream>
#include <atomic>
#include <thread>
#include <condition_variable>

using namespace std;
using rgb_matrix::RGBMatrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

// void runDrawFunc(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, Widget *widget, atomic_bool& end){
//     while(!end){
//         cout << "draw loop run" << endl;
//         widget->drawWidget(matrix, canvas, ref(end));
//     }
// }

int main(int argc, char **argv) {
    RGBMatrix::Options my_defaults;
    rgb_matrix::RuntimeOptions runtime_defaults;
    my_defaults.cols = 64;
    my_defaults.rows = 32;
    my_defaults.hardware_mapping = "adafruit-hat-pwm";
    if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv, &my_defaults, &runtime_defaults)) {
        cout << "issues" << endl;
    }
    // Set some defaults
    //   my_defaults.chain_length = 1;
    //   my_defaults.show_refresh_rate = true;
    //my_defaults.show_refresh_rate = true;
    //my_defaults.limit_refresh_rate_hz = 60;
    //runtime_defaults.gpio_slowdown = 2;
    // If you drop privileges, the root user you start the program with
    // to be able to initialize the hardware will be switched to an unprivileged
    // user to minimize a potential security attack surface.
    //runtime_defaults.drop_privileges = 1;
    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(my_defaults, runtime_defaults);

    if (matrix == NULL) {
        PrintMatrixFlags(stderr, my_defaults, runtime_defaults);
        return 1;
    }
    // int current_widget = 1;
    // //array of widgets including the clock
    // Widget *widgets[2];
    // widgets[0] = new Clock();
    // Spotify *spotify = &Spotify::getInstance();
    // widgets[1] = spotify;
    // //initialize the widgets
    // for(int i = 0; i < 2; i++){
    //     widgets[i]->initWidget(matrix, matrix->CreateFrameCanvas());
    // }
    rgb_matrix::FrameCanvas *canvas = matrix->CreateFrameCanvas();
    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    int i = 0;
    while(!interrupt_received){
        //create an atomic bool
        //atomic_bool end{false};
        // thread draw_thread(runDrawFunc, matrix, canvas, widgets[current_widget], ref(end));
        // usleep(600000000);
        // end = true;
        // draw_thread.join();
        //runDrawFunc(matrix, canvas, widgets[current_widget], ref(end));
        // current_widget < 1 ? current_widget++ : current_widget = 0;
        //runDrawFunc(matrix, canvas, widgets[current_widget]);
        //usleep(3000000);
        //create a loop that will increment through all passible 8bit rgb colors and display them on the matrix
        i++;
        cout << "incrementing i to " << i << endl;
        //canvas->Clear();
        canvas->Fill(i, i, i);
        canvas = matrix->SwapOnVSync(canvas);
        usleep(10000000);
    }
    delete matrix;
}