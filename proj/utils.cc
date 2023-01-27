#include "utils.h"

//function for fetching the font
string fetchFont(int size){
    switch(size){
        case 1:
            return "../fonts/4x6.bdf";
        case 2:
            return "../fonts/5x7.bdf";
        case 3:
            return "../fonts/5x8.bdf";
        case 4:
            return "../fonts/6x9.bdf";
        case 5:
            return "../fonts/6x10.bdf";
        case 6:
            return "../fonts/6x13.bdf";
        case 7:
            return "../fonts/6x13B.bdf";
        case 8:
            return "../fonts/6x13O.bdf";
        case 9:
            return "../fonts/6x13B.bdf";
        case 10:
            return "../fonts/6x13O.bdf";
        case 11:
            return "../fonts/7x13.bdf";
        case 12:
            return "../fonts/7x13B.bdf";
        case 13:
            return "../fonts/7x13O.bdf";
        case 14:
            return "../fonts/7x14.bdf";
        case 15:
            return "../fonts/7x14B.bdf";
        case 16:
            return "../fonts/8x13.bdf";
        case 17:
            return "../fonts/8x13B.bdf";
        case 18:
            return "../fonts/9x15.bdf";
        case 19:
            return "../fonts/9x15B.bdf";
        case 20:
            return "../fonts/10x20.bdf";
        default:
            return "../fonts/5x8.bdf";
    }
}