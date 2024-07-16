#include <ncurses.h>
#include <complex>
#include <cmath>
#include <unistd.h>

using namespace std;

int main() {
    initscr();
    start_color();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);  // Enable keypad input
    
    for (int i = 1; i <= 64; ++i) {
        init_pair(i, i % 8, COLOR_BLACK);
    }
    
    int width = COLS;
    int height = LINES;
    
    double xmin = -2.0, xmax = 1.0;
    double ymin = -1.5, ymax = 1.5;
    
    const int maxIterations = 100;

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
    
    // const char* asciiFrames[] = {
    //     " .,~:;+*%@",
    //     ".,~:;+*%@#",
    //     "~:;+*%@#$&",
    //     ":;+*%@#$&A",
    //     ";+*%@#$&AB",
    //     "+*%@#$&ABC",
    //     "*%@#$&ABCd",
    //     "%@#$&ABCde",
    //     "@#$&ABCdef",
    //     "#$&ABCdefg",
    //     "$&ABCdefgh",
    //     "&ABCdefghi",
    //     "ABCdefghij",
    //     "BCdefghijk",
    //     "Cdefghijkl",
    //     "defghijklm",
    //     "efghijklm0",
    //     "fghijklm01",
    //     "ghijklm012",
    //     "hijklm0123",
    //     "ijklm01234",
    //     "jklm012345",
    //     "klm0123456",
    //     "lm01234567",
    //     "m012345678",
    //     "0123456789"
    // };
    const int numAsciiFrames = sizeof(asciiFrames) / sizeof(asciiFrames[0]);
    const int asciiLen = strlen(asciiFrames[0]);
    
    const int frames = 1000000;  
    double zoomFactor = 1.0;  
    double panX = 0.0, panY = 0.0;
    double panSpeed = 0.0008;  // Adjust for faster/slower navigation
    
    for (int frame = 0; frame < frames; ++frame) {
        clear();
        
        // Handle user input
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
                panSpeed *= 0.9;  // Decrease pan speed
                break;
            case ']':
                panSpeed *= 1.1;  // Increase pan speed
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
                complex<double> z(0, 0);
                
                int iterations = 0;
                while (abs(z) < 2.0 && iterations < maxIterations) {
                    z = z * z + c;
                    ++iterations;
                }
                
                if (iterations == maxIterations) {
                    mvaddch(y, x, ' ');
                } else {
                    int colorPair = (iterations + frame) % 64 + 1;
                    attron(COLOR_PAIR(colorPair));
                    int asciiIndex = (iterations * asciiLen) / maxIterations;
                    char asciiChar = asciiFrames[(frame / 5) % numAsciiFrames][asciiIndex];
                    mvaddch(y, x, asciiChar);
                    attroff(COLOR_PAIR(colorPair));
                }
            }
        }

        mvprintw(0, 0, "Pan Speed: %.6f", panSpeed);
        refresh();
        usleep(30000);  // Adjust speed as needed
    }
    
end:
    endwin();
    return 0;
}