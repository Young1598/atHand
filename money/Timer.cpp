#include <Windows.h>
#define buttonClick 1001
#define buttonWid   50
#define buttonHei   40
#define timeText    1002
#define textWid     100
#define textHei     80

int g_seconds = 0;
bool g_isRunning = false;


//                          窗口句柄    消息代码     包含消息的其他数据
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_COMMAND: {
      // LOWORD(wParam) 就是按钮的 ID
      if (LOWORD(wParam) == buttonClick) {
        if (!g_isRunning) {
          SetTimer(hwnd, 100, 1000, NULL);
          g_isRunning = true;
          SetWindowText(GetDlgItem(hwnd, buttonClick), L"Stop");
        }
        else {
          KillTimer(hwnd, 100);
          g_isRunning = false;
          SetWindowText(GetDlgItem(hwnd, buttonClick), L"Continue");
        }
      }
      return 0;
    }
    case WM_TIMER: {
      if (wParam == 100) {
        g_seconds++;
        wchar_t buffer[64];
        wsprintfW(buffer, L"%d", g_seconds);
        SetWindowText(GetDlgItem(hwnd, timeText), buffer);
      }

      return 0;
    }
    case WM_SIZE: {
      int wid = LOWORD(lParam);
      int hei = HIWORD(lParam);

      HWND hText = GetDlgItem(hwnd, timeText);
      if (hText) {
        int x = (wid - textWid) / 2;
        int y = (hei - textHei) / 2;

        MoveWindow(hText, x, y, textWid, textHei, true);
      }

      HWND hButton = GetDlgItem(hwnd, buttonClick);
      if (hButton) {
        int x = (wid - buttonWid) / 2;
        int y = (hei - buttonHei) / 2 + hei / 3;

        MoveWindow(hButton, x, y, buttonWid, buttonHei, true);
      }

      return 0;
    }
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow) {
  // 1. 注册窗口类 (注意使用 W 版本)
  WNDCLASSW wc = {};
  wc.lpfnWndProc = WindowProc;         // 窗口过程
  wc.hInstance = hInstance;            // 窗口句柄
  wc.lpszClassName = L"SampleWindow";  // 窗口标识
  // 窗口类必须设置的结构成员
  RegisterClassW(&wc);   // 传入窗口类的地址，将窗口类注册到操作系统
    
  // 2. 创建窗口
  // CreateWindowExW 创建成功会返回窗口句柄，否则返回0
  HWND hwnd = CreateWindowExW(
    1,                    // window style
    L"SampleWindow",      // class : 定义要创建的窗口类型
    L"Clock",       // text  : 不同窗口的text的表现形式不同
    WS_OVERLAPPEDWINDOW,  // style : 提供标题栏，最小化，边框等样式

    100, // x
    100, // y
    400, // wid
    300, // hei

    NULL,                 // parent 
    NULL,                 // menu
    hInstance,            // handle
    NULL                  // application data
  );

  HWND hButton = CreateWindowEx( 
    2,
    L"BUTTON",  // Predefined class; Unicode assumed 
    L"Click",      // Button text 
    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
    175,         // x position 
    200,         // y position 
    50,        // Button width
    40,        // Button height
    hwnd,       // Parent window
    (HMENU)buttonClick,       // No menu.
    (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
    NULL        // Pointer not needed.
  );      

  HWND hText = CreateWindowEx(
    0,
    L"Static",  // Static text
    L"0",
    WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
    100, 50,
    100, 80,
    hwnd,
    (HMENU)timeText, 
    GetModuleHandle(NULL),
    NULL
  );
    
  // 3. 显示并更新窗口
  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);
  
  // 4. 消息循环 (使用 GetMessageW)
  MSG msg;
  while (GetMessageW(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
  return 0;
}