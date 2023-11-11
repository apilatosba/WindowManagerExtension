#include <stdio.h>
#include <Windows.h>

HHOOK mouseHook;
HWND windowUnderCursor = NULL;
POINT initialMousePos = { 0 };
BOOL isDragging = FALSE;

void RepositionWindow() {
   if (windowUnderCursor != NULL) {
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

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
   if (nCode >= 0) {
      MSLLHOOKSTRUCT* hookStruct = (MSLLHOOKSTRUCT*)lParam;

      if (wParam == WM_LBUTTONDOWN) {
         windowUnderCursor = WindowFromPoint(hookStruct->pt);
         GetCursorPos(&initialMousePos);
         //ScreenToClient(windowUnderCursor, &initialMousePos);
         printf("Starting drag...\n");
         isDragging = TRUE;
      } else if (wParam == WM_LBUTTONUP) {
         windowUnderCursor = NULL;
         isDragging = FALSE;
      } else if (wParam == WM_RBUTTONDOWN) {
         if (windowUnderCursor != NULL && windowUnderCursor != GetDesktopWindow()) {
            printf("Ctrl + Right Click: Resizing Window\n");
         }
      }

      if (isDragging && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
         // Continuously reposition the window while dragging and Ctrl is pressed
         RepositionWindow();
         //printf("Repositioning window...\n");
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