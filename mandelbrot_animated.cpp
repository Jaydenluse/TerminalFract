#include <ncurses.h>
#include <complex>
#include <cmath>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

enum class FractalType { Mandelbrot, Julia };

int main(int argc, char* argv[]) {
    FractalType fractalType = FractalType::Mandelbrot;
    complex<double> juliaC(-0.4, 0.6);  // Default Julia set parameter

    if (argc > 1) {
        string arg = argv[1];
        if (arg == "julia") {
            fractalType = FractalType::Julia;
            if (argc > 3) {
                juliaC = complex<double>(stod(argv[2]), stod(argv[3]));
            }
        } else if (arg != "mandelbrot") {
            cout << "Usage: " << argv[0] << " [mandelbrot|julia] [real imag]" << endl;
            return 1;
        }
    }

    initscr();
    start_color();
    curs_set(0);
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    
    for (int i = 1; i <= 64; ++i) {
        init_pair(i, i % 8, COLOR_BLACK);
    }
    
    int width = COLS;
    int height = LINES;
    
    WINDOW *buffer = newwin(height, width, 0, 0);
    
    double xmin = -2.0, xmax = 2.0;
    double ymin = -2.0, ymax = 2.0;
    
    const int maxIterations = 1000;

    const char* asciiFrames[] = {
        " .,~:;+%@#",
        ".,~:;+%@# ",
        ",~:;+%@# .",
        "~:;+%@# .,",
        ":;+%@# .,~",
        ";+%@# .,~:",
        "+%@# .,~:;",
        "%@# .,~:;+",
        "%@# .,~:;+",
        "@# .,~:;+%",
        "# .,~:;+*%@"
    };
    const int numAsciiFrames = sizeof(asciiFrames) / sizeof(asciiFrames[0]);
    const int asciiLen = strlen(asciiFrames[0]);
    
    const int frames = 1000000;  
    double zoomFactor = 1.0;  
    double panX = 0.0, panY = 0.0;
    double panSpeed = 0.0008;
    
    for (int frame = 0; frame < frames; ++frame) {
        wclear(buffer);
        
        int ch = getch();
        switch(ch) {
            case KEY_UP:
                panY -= panSpeed;
                break;
            case KEY_DOWN:
                panY += panSpeed;
                break;
            case KEY_LEFT:
                panX -= panSpeed;
                break;
            case KEY_RIGHT:
                panX += panSpeed;
                break;
            case '+':
            case '=':
                zoomFactor *= 0.95;  
                break;
            case '-':
            case '_':
                zoomFactor /= 0.95;  
                break;
            case '[':
                panSpeed *= 0.9;
                break;
            case ']':
                panSpeed *= 1.1;
                break;
            case 'q':
            case 'Q':
                goto end;  
        }
        
        double centerX = (xmin + xmax) / 2;
        double centerY = (ymin + ymax) / 2;
        xmin = centerX + (xmin - centerX) * zoomFactor;
        xmax = centerX + (xmax - centerX) * zoomFactor;
        ymin = centerY + (ymin - centerY) * zoomFactor;
        ymax = centerY + (ymax - centerY) * zoomFactor;
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double real = xmin + (xmax - xmin) * x / width + panX;
                double imag = ymin + (ymax - ymin) * y / height + panY;
                complex<double> c(real, imag);
                complex<double> z;

                if (fractalType == FractalType::Mandelbrot) {
                    z = 0;
                } else {  // Julia set
                    z = c;
                    c = juliaC;
                }
                
                int iterations = 0;
                while (abs(z) < 2.0 && iterations < maxIterations) {
                    z = z * z + c;
                    ++iterations;
                }
                
                if (iterations == maxIterations) {
                    mvwaddch(buffer, y, x, ' ');
                } else {
                    int colorPair = (iterations + frame) % 64 + 1;
                    wattron(buffer, COLOR_PAIR(colorPair));
                    int asciiIndex = (iterations * asciiLen) / maxIterations;
                    char asciiChar = asciiFrames[(frame / 5) % numAsciiFrames][asciiIndex];
                    mvwaddch(buffer, y, x, asciiChar);
                    wattroff(buffer, COLOR_PAIR(colorPair));
                }
            }
        }

        mvwprintw(buffer, 0, 0, "Pan Speed: %.6f", panSpeed);
        if (fractalType == FractalType::Julia) {
            mvwprintw(buffer, 1, 0, "Julia C: %.3f + %.3fi", real(juliaC), imag(juliaC));
        }
        
        overwrite(buffer, stdscr);
        refresh();
        
        usleep(30000);
    }
    
end:
    delwin(buffer);
    endwin();
    return 0;
}