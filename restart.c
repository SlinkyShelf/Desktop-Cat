#include <windows.h>
#include <stdio.h>
#include <dwmapi.h>

int running = 1;

HDC hdc;

COLORREF TRANSPARENT_COLOR = RGB(255,255,255);

// Cat Variables
int catX = 0;
int catY = 0;
int catDir = 0;
int catAnim = 0;

byte images[6][8][8][32][32][4];

char animations[6][3] = {"SD", "LA", "LD", "Wg", "R1", "R2"};
char directions[8][3] = {"S ", "SW", "W ", "NW", "N ", "NE", "E ", "SE"};

const int SPRITE_SCALE = 10;

void paintFrame(HDC window, int anim, int dir, int frame, int sx, int sy)
{
    RECT prect;
    for (int x = 0; x < 32; x++)
    {
        for (int y = 0; y < 32; y++)
        {
            COLORREF color;

            color   =  RGB(
                images[anim][dir][frame][y][x][0], 
                images[anim][dir][frame][y][x][1], 
                images[anim][dir][frame][y][x][2]);

            if (images[anim][dir][frame][y][x][3] == 0)
                // color = TRANSPARENT_COLOR;
                continue;

            // color |= (images[anim][dir][frame][y][x][3] << 24);


            prect.left = sx + x * SPRITE_SCALE;
            prect.top =  sy + y * SPRITE_SCALE;
            prect.right =  sx + (x+1) * SPRITE_SCALE;
            prect.bottom = sy + (y+1) * SPRITE_SCALE;

            HBRUSH hBrush  = CreateSolidBrush(color);

            FillRect(hdc, &prect, hBrush);
            DeleteObject(hBrush);
        }
    }
}

void paint(HWND window)
{
    // clearFrame(hdc, catX, catY);
    catX++;
    
    paintFrame(hdc, catAnim, catDir, 1, catX, catY);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result;

    static PAINTSTRUCT ps;

    switch (message)
    {
    case WM_CREATE:
        SetTimer(window, 1, 20, NULL); 
        break;
    case WM_CLOSE:
        running = 0;
        break;
    // case WM_ERASEBKGND:
    //     return 1;

    case WM_TIMER:
        // SetWindowPos(window, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
        InvalidateRect(window, NULL, FALSE);
        break;
    case WM_KEYDOWN:
        
        break;
    case WM_PAINT:
        
        break;

    default:
        result = DefWindowProc(window,
                               message,
                               w_param,
                               l_param);
        break;  
    }

    return result;
}

int intfromHex(const char* str)
{
    return (int)strtol(str, NULL, 16);
}

void loadImages()
{
    // Loading anims
    FILE *fptr;
    for (int anim = 0; anim < 6; anim++)
    {
        for (int dir = 0; dir < 8; dir++)
        {
            char filename[] = "./CatAnims/FF-FF.32x32frames";

            // Anim Name
            filename[11] = animations[anim][0];
            filename[12] = animations[anim][1];


            // Direction name
            filename[14] = directions[dir][0];
            filename[15] = directions[dir][1];

            // atoi();

            fptr = fopen(filename, "r");

            char frameChars[2];
            char* ip = frameChars;
            fgets(ip, 2, fptr);

            int frames = intfromHex(ip);

            const int IMAGE_BYTES = 32*32*4;
            const int IMAGE_STRING_SIZE = IMAGE_BYTES*2;

            char* imageP = (char*) malloc(sizeof(char)*IMAGE_STRING_SIZE);
            for (int i = 0; i < frames; i++)
            {
                char* imagePHead = imageP;
                fgets(imageP, IMAGE_STRING_SIZE, fptr);

                // printf("\n");
                // printf(animations[anim]);
                // printf(directions[dir]);
                // printf("%d", i);
                // printf("\n");

                char captureStr[3];

                captureStr[2] = '\0';

                for (int x = 0; x < 32; x++)
                {
                    for (int y = 0; y < 32; y++)
                    {
                        // r value
                        captureStr[0] = *imagePHead;
                        imagePHead++;
                        captureStr[1] = *imagePHead;
                        imagePHead++;
                        images[anim][dir][i][y][x][0] = intfromHex(captureStr);

                        // g value
                        captureStr[0] = *imagePHead;
                        imagePHead++;
                        captureStr[1] = *imagePHead;
                        imagePHead++;
                        images[anim][dir][i][y][x][1] = intfromHex(captureStr);

                        // b value
                        captureStr[0] = *imagePHead;
                        imagePHead++;
                        captureStr[1] = *imagePHead;
                        imagePHead++;
                        images[anim][dir][i][y][x][2] = intfromHex(captureStr);

                        // a value
                        captureStr[0] = *imagePHead;
                        imagePHead++;
                        captureStr[1] = *imagePHead;
                        imagePHead++;
                        images[anim][dir][i][y][x][3] = intfromHex(captureStr);
                    }
                }
            }

            free(imageP);
        }
    }
}

int APIENTRY WinMain(HINSTANCE instance,
                     HINSTANCE prev_instance,
                     LPSTR cmd_line,
                     int cmd_show)
{

    HWND window;

    PIXELFORMATDESCRIPTOR pfd;

    WNDCLASSA window_class = {0};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = "Sample Window Class";
    // window_class.hbrBackground	= NULL;

    RegisterClassA(&window_class);

    HMONITOR hmon = MonitorFromWindow(NULL,
                                    MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hmon, &mi);

    int flags = WS_OVERLAPPED | WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP | WS_DISABLED | WS_EX_TOOLWINDOW;

    window = CreateWindowEx(0,
                            "Sample Window Class",
                            "Game",
                            flags,
                            mi.rcMonitor.left,
                            mi.rcMonitor.top,
                            mi.rcMonitor.right - mi.rcMonitor.left,
                            mi.rcMonitor.bottom - mi.rcMonitor.top,
                            0,
                            0,
                            instance,
                            0);
    hdc = GetDC(window);

    // Transparent
    SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(window, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    loadImages();
    
    while (running)
    {
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    ReleaseDC(window, hdc);
    DestroyWindow(window);

    return 0;
}