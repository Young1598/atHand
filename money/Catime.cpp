#include <Windows.h>
#include <stdio.h>
LARGE_INTEGER g_freq = {0};
LARGE_INTEGER g_st = {0};
LONGLONG g_baseCount = 0;
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

      // FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
      FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));

      // Text color
      SetTextColor(hdc, RGB(255, 255, 255));

      // Text background mode
      SetBkMode(hdc, TRANSPARENT);

      HFONT hFont = CreateFont(
        120,              // 字体高度（像素）
        0,                // 字体宽度（0 表示自动按比例）
        0, 0,             // 旋转角度（0 表示不旋转）
        FW_BOLD,          // 粗体
        FALSE, FALSE, FALSE, // 斜体、下划线、删除线（都不需要）
        DEFAULT_CHARSET,  // 字符集（默认）
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Arial"          // 字体名称
      );

      SelectObject(hdc, hFont);

      LONGLONG totalCount = g_baseCount;
      if (g_isRunning) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        totalCount += (now.QuadPart - g_st.QuadPart);
      }

      double textSeconds = (double)totalCount / g_freq.QuadPart;

      wchar_t buffer[64];
      swprintf_s(buffer, sizeof(buffer) / sizeof(wchar_t), L"%.2f", textSeconds);

      RECT rect;
      GetClientRect(hwnd, &rect);

      DrawTextW(
        hdc,
        buffer,
        -1,     // the length of text
        &rect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
      );

      DeleteObject(hFont);

      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_KEYDOWN: {
      if (wParam == VK_SPACE) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);

        if (!g_isRunning) {
          if (g_st.QuadPart == 0) {
            g_baseCount = 0;
          }

          g_st = now;
          g_isRunning = true;
          InvalidateRect(hwnd, NULL, TRUE);
        }
        else {
          g_baseCount += (now.QuadPart - g_st.QuadPart);
          g_isRunning = false;
        }
      }
      return 0;
    }
    case WM_TIMER: {
      if (wParam == 100) {
        if (g_isRunning) InvalidateRect(hwnd, NULL, TRUE); // 强制重绘
        //                             窗口   区域  是否擦除旧值
      }
      return 0;
    }
    case WM_CREATE: {
      SetTimer(hwnd, 100, 50, NULL);
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
  wc.lpszClassName = L"Catime";  // 窗口标识
  // 窗口类必须设置的结构成员
  RegisterClassW(&wc);   // 传入窗口类的地址，将窗口类注册到操作系统

  QueryPerformanceFrequency(&g_freq);
    
  // 2. 创建窗口
  // CreateWindowExW 创建成功会返回窗口句柄，否则返回0
  HWND hwnd = CreateWindowExW(
    WS_EX_LAYERED | WS_EX_TOPMOST,   // window style : 分层窗口 + 保持顶层
    L"Catime",                 // class : 定义要创建的窗口类型
    L"Clock",                        // text  : 不同窗口的text的表现形式不同
    WS_POPUP,                        // style : 提供标题栏，最小化，边框等样式

    100,                             // x
    100,                             // y
    400,                             // wid
    300,                             // hei

    NULL,                            // parent 
    NULL,                            // menu
    hInstance,                       // handle
    NULL                             // application data
  );

  SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    
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