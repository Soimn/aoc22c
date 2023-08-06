#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define STRICT
#define UNICODE
#define WIN32_LEAN_AND_MEAN 1
#define NOMINMAX            1

#include <windows.h>
#include <timeapi.h>

#undef STRICT
#undef UNICODE
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#undef far
#undef near

#include "rin.h"

R_u32* Backbuffer;
R_u32* Frontbuffer;
R_u32 BufferWidth;
R_u32 BufferHeight;
HBRUSH BackgroundBrush;

LRESULT
WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
  LRESULT result;

  if (msg == WM_QUIT || msg == WM_CLOSE || msg == WM_DESTROY)
  {
    PostQuitMessage(0);
    result = 0;
  }
  else if (msg == WM_PAINT)
  {
    PAINTSTRUCT p = {0};
    HDC dc = BeginPaint(window, &p);

    R_ASSERT(BufferWidth  < R_S32_MAX);
    R_ASSERT(BufferHeight < R_S32_MAX);
    R_ASSERT(BufferWidth*BufferHeight < R_U32_MAX);
    BITMAPINFO bmp_info = {
      .bmiHeader = {
        .biSize          = sizeof(BITMAPINFOHEADER),
        .biWidth         = (LONG)BufferWidth,
        .biHeight        = -(LONG)BufferHeight,
        .biPlanes        = 1,
        .biBitCount      = 32,
        .biCompression   = BI_RGB,
      },
      .bmiColors = {0},
    };

    RECT rect;
    GetClientRect(window, &rect);
    R_u32 window_width  = rect.right - rect.left;
    R_u32 window_height = rect.bottom - rect.top;

    R_f32 aspect = (R_f32)BufferWidth/BufferHeight;

    R_u32 height = R_MIN(window_height, (R_u32)(window_width/aspect));
    R_u32 width  = (R_u32)(height*aspect);

    FillRect(dc, &rect, BackgroundBrush);
    StretchDIBits(dc, (window_width - width)/2, (window_height - height)/2, width, height, 0, 0, BufferWidth, BufferHeight, Frontbuffer, &bmp_info, DIB_RGB_COLORS, SRCCOPY);

    EndPaint(window, &p);

    result = 0;
  }
  else result = DefWindowProcW(window, msg, wparam, lparam);

  return result;
}

int
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd)
{
  HANDLE input_file_handle = CreateFileA("day14_input.txt", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

  R_String input;
  input.size = GetFileSize(input_file_handle, &(R_u32){0});
  input.data = VirtualAlloc(0, input.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  ReadFile(input_file_handle, input.data, input.size, 0, 0);
  CloseHandle(input_file_handle);

  R_u64 min_x = R_U64_MAX;
  R_u64 max_x = 500; // NOTE: Must at least include x = 500
  R_u64 min_y = 0;   // NOTE: Must at least include y = 0
  R_u64 max_y = 0;
  for (R_String scan = input; scan.size != 0;)
  {
    R_u64 x, y;
    scan = R_String_EatN(scan, R_String_PatternMatch(scan, "%u64,%u64", &x, &y));

    min_x = R_MIN(min_x, x);
    max_x = R_MAX(max_x, x);
    min_y = R_MIN(min_y, y);
    max_y = R_MAX(max_y, y);

    R_smm eat_arrow   = R_String_PatternMatch(scan, " -> ");
    R_smm eat_newline = R_String_PatternMatch(scan, "\r\n");
    if (eat_arrow   != -1) scan = R_String_EatN(scan, eat_arrow);
    else
    {
      R_ASSERT(eat_newline != -1);
      scan = R_String_EatN(scan, eat_newline);
    }
  }

  BufferWidth  = (max_x + 1) - min_x;
  BufferHeight = (max_y + 1) - min_y;
  R_V2S sand_source = R_V2S(500 - min_x, 0);

  R_u32* backbuffer_memory = VirtualAlloc(0, 2*BufferWidth*BufferHeight*sizeof(R_u32), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  Backbuffer  = backbuffer_memory;
  Frontbuffer = (R_u32*)((R_u8*)backbuffer_memory + (BufferWidth*BufferHeight*sizeof(R_u32)));

  if (backbuffer_memory == 0) OutputDebugStringA("Failed to allocate memory\n");
  else
  {
    for (R_String scan = input; scan.size != 0;)
    {
      R_u64 prev_x, prev_y;
      scan = R_String_EatN(scan, R_String_PatternMatch(scan, "%u64,%u64", &prev_x, &prev_y));
      for (;;)
      {
        R_u64 curr_x, curr_y;
        scan = R_String_EatN(scan, R_String_PatternMatch(scan, " -> %u64,%u64", &curr_x, &curr_y));

        for (R_u64 j = R_MIN(prev_y, curr_y); j <= R_MAX(prev_y, curr_y); ++j)
        {
          for (R_u64 i = R_MIN(prev_x, curr_x); i <= R_MAX(prev_x, curr_x); ++i)
          {
            Backbuffer[(j - min_y)*BufferWidth + (i - min_x)] = 0xFF444444;
          }
        }

        prev_x = curr_x;
        prev_y = curr_y;

        R_smm eat_newline = R_String_PatternMatch(scan, "\r\n");
        if (eat_newline == -1) continue;
        else
        {
          scan = R_String_EatN(scan, eat_newline);
          break;
        }
      }
    }

    BackgroundBrush = CreateSolidBrush(RGB(0, 0, 0));

    WNDCLASSEXW window_class_info = {
      .cbSize        = sizeof(WNDCLASSEXW),
      .style         = CS_OWNDC,
      .lpfnWndProc   = &WndProc,
      .hInstance     = instance,
      .hIcon         = LoadIcon(0, IDI_APPLICATION),
      .hCursor       = LoadCursor(0, IDC_ARROW),
      .lpszClassName = L"Sand",
      .hbrBackground = BackgroundBrush,
    };

    if (!RegisterClassExW(&window_class_info)) OutputDebugStringA("Failed to register window class\n");
    else
    {
      HWND window = CreateWindowW(window_class_info.lpszClassName, L"Sand", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024, 0, 0, 0, 0);
      if (window == 0) OutputDebugStringA("Failed to create window\n");
      else
      {
        LARGE_INTEGER perf_counter_freq = {0};
        timeBeginPeriod(1);
        QueryPerformanceFrequency(&perf_counter_freq);

        LARGE_INTEGER flip_time;
        QueryPerformanceCounter(&flip_time);

        ShowWindow(window, SW_SHOW);

        R_V2S curr_sand = sand_source;
        R_u32 sand_move_counter = 0;
        R_u32 sand_at_rest = 0;

        R_bool is_running = R_true;
        while (is_running)
        {
          MSG msg;
          while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
          {
            if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
            {
              is_running = R_false;
              break;
            }
            else DispatchMessage(&msg);
          }

          R_u32 sand_color = 0xFFBBBB46;

          if ((sand_move_counter = ++sand_move_counter % 1) == 0)
          {
            curr_sand.y += 1;

            if (curr_sand.y < BufferHeight && Backbuffer[curr_sand.y*BufferWidth + curr_sand.x] != 0)
            {
              curr_sand.x -= 1;

              if (curr_sand.x >= 0 && Backbuffer[curr_sand.y*BufferWidth + curr_sand.x] != 0)
              {
                curr_sand.x += 2;

                if (curr_sand.x < BufferWidth && Backbuffer[curr_sand.y*BufferWidth + curr_sand.x] != 0)
                {
                  curr_sand.x -= 1;
                  curr_sand.y -= 1;
                  if (curr_sand.x >= 0 && curr_sand.x <= BufferWidth && curr_sand.y <= BufferHeight)
                  {
                    Backbuffer[curr_sand.y*BufferWidth + curr_sand.x] = sand_color;
                    curr_sand = sand_source;
                    sand_at_rest += 1;
                  }
                }
              }
            }
          }

          if (curr_sand.x < 0 || curr_sand.x >= BufferWidth || curr_sand.y < 0 || curr_sand.y >= BufferHeight)
          {
            CHAR buffer[256];
            wsprintfA(buffer, "Result: %u", sand_at_rest);
            MessageBoxA(window, buffer, "Part 1", MB_OK);
            ExitProcess(0);
          }

          for (R_uint i = 0; i < BufferWidth*BufferHeight; ++i)
          {
            Frontbuffer[i] = Backbuffer[i];
          }

          Frontbuffer[curr_sand.y*BufferWidth + curr_sand.x] = sand_color;

          InvalidateRect(window, 0, 0);

          LARGE_INTEGER end_time;
          R_f32 frame_time;
          R_f32 frame_time_ms;

          for (;;)
          {
            QueryPerformanceCounter(&end_time);
            frame_time = (R_f32)(end_time.QuadPart - flip_time.QuadPart) / (R_f32)perf_counter_freq.QuadPart;
            frame_time_ms = frame_time*1000;

            if (frame_time_ms >= 2) break;
            else
            {
              Sleep(2 - (R_u32)(frame_time*1000));
              continue;
            }
          }

          flip_time = end_time;

          WCHAR window_title_buffer[256];
          wsprintfW(window_title_buffer, L"Tracer - %u ms, %u fps", (unsigned int)(frame_time_ms), (unsigned int)(1/frame_time));
          SetWindowTextW(window, window_title_buffer);
        }
      }
    }
  }

  ExitProcess(0);
}
