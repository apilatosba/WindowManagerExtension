#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

HHOOK mouseHook;
HWND windowUnderCursor = NULL;
POINT initialMousePos = { 0 };
BOOL isDragging = FALSE;
BOOL isResizing = FALSE;

HWND GetDesktopListView() {
   HWND hShellWnd = GetShellWindow();
   if (hShellWnd == NULL) {
      return NULL;
   }

   HWND hDefView = FindWindowEx(hShellWnd, NULL, L"SHELLDLL_DefView", NULL);
   if (hDefView == NULL) {
      return NULL;
   }

   HWND folderView = FindWindowEx(hDefView, NULL, L"SysListView32", NULL);
   return folderView;
}

BOOL IsDesktopWindow(HWND hWnd) {
   return hWnd == GetDesktopWindow();
}

//BOOL IsDesktopWindow(HWND hWnd) {
//   const int classNameSize = 256;
//   char className[classNameSize];
//
//   GetClassNameA(hWnd, className, classNameSize);
//
//   return (strcmp(className, "Progman") == 0 || strcmp(className, "WorkerW") == 0);
//}

//BOOL IsDesktopWindow(HWND hWnd) {
//   LONG_PTR style = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
//   return ((style & WS_EX_TOOLWINDOW) == 0);
//}

//BOOL IsDesktopWindow(HWND hWnd) {
//   LONG_PTR style = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
//   return ((style & WS_EX_LAYERED) == 0);
//}

//BOOL IsDesktopWindow(HWND hWnd) {
//   return hWnd == GetShellWindow();
//}

//BOOL IsDesktopWindow(HWND hWnd) {
//   HWND parent = GetParent(hWnd);
//
//   // If the window has no parent and is visible, it might be the desktop window
//   if (parent == NULL && IsWindowVisible(hWnd)) {
//      return TRUE;
//   }
//
//   return FALSE;
//}

//BOOL IsDesktopWindow(HWND hWnd) {
//   HWND hShellWnd = GetShellWindow();
//   HWND hDefView = FindWindowEx(hShellWnd, NULL, _T("SHELLDLL_DefView"), NULL);
//   HWND folderView = FindWindowEx(hDefView, NULL, _T("SysListView32"), NULL);
//   return hWnd == folderView;
//}

//BOOL IsDesktopWindow(HWND hWnd) {
//   return hWnd == GetDesktopListView();
//}

// todo bug i can reposition desktop. icons move
void RepositionWindow() {
   if (windowUnderCursor != NULL && !IsDesktopWindow(windowUnderCursor)) {
      POINT currentMousePos;
      GetCursorPos(&currentMousePos);

      int deltaX = currentMousePos.x - initialMousePos.x;
      int deltaY = currentMousePos.y - initialMousePos.y;

      RECT windowRect;
      GetWindowRect(windowUnderCursor, &windowRect);

      int newX = windowRect.left + deltaX;
      int newY = windowRect.top + deltaY;

      SetWindowPos(windowUnderCursor, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

      initialMousePos = currentMousePos;
   }
}

void ResizeWindow() {
   if (windowUnderCursor != NULL && !IsDesktopWindow(windowUnderCursor)) {
      POINT currentMousePos;
      GetCursorPos(&currentMousePos);

      int deltaX = currentMousePos.x - initialMousePos.x;
      int deltaY = currentMousePos.y - initialMousePos.y;

      RECT windowRect;
      GetWindowRect(windowUnderCursor, &windowRect);

      int newWidth = windowRect.right - windowRect.left + deltaX;
      int newHeight = windowRect.bottom - windowRect.top + deltaY;

      SetWindowPos(windowUnderCursor, NULL, windowRect.left, windowRect.top, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);
      initialMousePos = currentMousePos;
   }
}


LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
   if (nCode >= 0) {
      MSLLHOOKSTRUCT* hookStruct = (MSLLHOOKSTRUCT*)lParam;

      if (wParam == WM_LBUTTONDOWN) {
         windowUnderCursor = WindowFromPoint(hookStruct->pt);
         GetCursorPos(&initialMousePos);
         isDragging = TRUE;
         printf("Starting drag...\n");
      } else if (wParam == WM_LBUTTONUP) {
         windowUnderCursor = NULL;
         isDragging = FALSE;
      } else if (wParam == WM_RBUTTONDOWN) {
         windowUnderCursor = WindowFromPoint(hookStruct->pt);
         GetCursorPos(&initialMousePos);
         isResizing = TRUE;
         printf("Starting resize...\n");
         /*if (windowUnderCursor != NULL && windowUnderCursor != GetDesktopWindow()) {
            printf("Ctrl + Right Click: Resizing Window\n");
         }*/
      } else if (wParam == WM_RBUTTONUP) {
         windowUnderCursor = NULL;
         isResizing = FALSE;
      }

      if (!IsDesktopWindow(windowUnderCursor)) {
         if (isDragging && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            // Continuously reposition the window while dragging and Ctrl is pressed
            RepositionWindow();
            //printf("Repositioning window...\n");
         }

         if (isResizing && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            ResizeWindow();
         }
      }
   }

   return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

int main() {
   mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);

   if (mouseHook == NULL) {
      fprintf(stderr, "Failed to install mouse hook\n");
      return 1;
   }

   MSG msg;
   while (GetMessage(&msg, NULL, 0, 0) != 0) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   UnhookWindowsHookEx(mouseHook);

   return 0;
}

/*
#include <stdio.h>
#include <Windows.h>

HHOOK mouseHook;
HWND windowUnderCursor = NULL;

void RepositionWindow() {
    // Add your repositioning code here using the global variable windowUnderCursor
    // You might want to use functions like SetWindowPos to change the window position
    // Example: SetWindowPos(windowUnderCursor, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    printf("Repositioning Window...\n");
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSLLHOOKSTRUCT* hookStruct = (MSLLHOOKSTRUCT*)lParam;

        // Check if Ctrl key is pressed
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
            // Get the window under the mouse cursor
            windowUnderCursor = WindowFromPoint(hookStruct->pt);

            // Check for left mouse button down
            if (wParam == WM_LBUTTONDOWN) {
                // Start repositioning when the left mouse button is pressed
                RepositionWindow();
            }
            // Check for left mouse button up
            else if (wParam == WM_LBUTTONUP) {
                // Stop repositioning when the left mouse button is released
                windowUnderCursor = NULL;
            }
            // Check for right mouse button down
            else if (wParam == WM_RBUTTONDOWN) {
                // Implement resizing logic here if the mouse is on a window
                if (windowUnderCursor != NULL && windowUnderCursor != GetDesktopWindow()) {
                    printf("Ctrl + Right Click: Resizing Window\n");
                    // Add your resizing code here using windowUnderCursor
                }
            }
        }
    }

    // Call the next hook in the chain
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

int main() {
    // Set up the mouse hook
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);

    if (mouseHook == NULL) {
        fprintf(stderr, "Failed to install mouse hook\n");
        return 1;
    }

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook the mouse hook
    UnhookWindowsHookEx(mouseHook);

    return 0;
}

*/