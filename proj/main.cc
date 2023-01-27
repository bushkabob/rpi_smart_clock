#include "led-matrix.h"
#include "graphics.h"
#include "spotify.h"
#include "widget.h"
#include "utils.h"
#include "aclock.h"
#include "stock.h"

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

void runDrawFunc(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, Widget *widget, atomic_bool& end){
    while(!end){
        cout << "draw loop run" << endl;
        widget->drawWidget(matrix, canvas, ref(end));
    }
}

int main(int argc, char **argv) {
    cout << "Starting" << endl;
    // Set some defaults
    RGBMatrix::Options my_defaults;
    //   my_defaults.chain_length = 1;
    //   my_defaults.show_refresh_rate = true;
    rgb_matrix::RuntimeOptions runtime_defaults;
    my_defaults.cols = 64;
    my_defaults.rows = 32;
    my_defaults.hardware_mapping = "adafruit-hat-pwm";
    runtime_defaults.gpio_slowdown = 2;
    // If you drop privileges, the root user you start the program with
    // to be able to initialize the hardware will be switched to an unprivileged
    // user to minimize a potential security attack surface.
    runtime_defaults.drop_privileges = 1;
    cout << "Creating matrix" << endl;
    RGBMatrix *matrix = RGBMatrix::CreateFromFlags(&argc, &argv, &my_defaults);
    if (matrix == NULL) {
        PrintMatrixFlags(stderr, my_defaults, runtime_defaults);
        return 1;
    }
    cout << "Matrix created" << endl;
    int current_widget = 1;
    //array of widgets including the clock
    Widget *widgets[3];
    widgets[0] = new Clock();
    Spotify *spotify = &Spotify::getInstance();
    widgets[1] = spotify;
    widgets[2] = new Stock("AAPL");
    //initialize the widgets
    cout << "Initializing widgets" << endl;
    for(int i = 0; i < 3; i++){
        widgets[i]->initWidget(matrix, matrix->CreateFrameCanvas());
    }
    cout << "Widgets initialized" << endl;
    
    rgb_matrix::FrameCanvas *canvas = matrix->CreateFrameCanvas();
    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);
    cout << "Starting loop" << endl;
    while(!interrupt_received){
        cout << "run" << endl;
        //create an atomic bool
        atomic_bool end{false};
        thread draw_thread(runDrawFunc, matrix, canvas, widgets[current_widget], ref(end));
        // string inpt;
        // cin >>  inpt;      
        // if(inpt == "prev" || inpt == "p"){
        //     current_widget == 1 ? current_widget = 0 : current_widget++;
        // }
        usleep(10000000);
        // cout << "end = true" << endl;
        end = true;
        draw_thread.join();
        // canvas->Clear();
        // //iterate through the matrix and set each pixel to white
        // for(int i = 0; i < 64; i++){
        //     for(int j = 0; j < 32; j++){
        //         canvas->SetPixel(i, j, 255, 255, 255);
        //     }
        // }
        // //canvas->Fill(255, 255, 255);
        // canvas = matrix->SwapOnVSync(canvas);
        // usleep(1000000);
        //current_widget < 1 ? current_widget++ : current_widget = 0;
    }
    delete matrix;
    cout << "Exiting" << endl;
}