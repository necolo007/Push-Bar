/*
 * Push Box Game - Main Program
 * Use WASD or arrow keys to control character movement and push boxes to target positions
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <graphics.h> // EasyX header
#include "game.h"
#include "ui.h"
#include "map.h"


int main() {
    // Initialize EasyX graphics window
    initgraph(900, 900);  // Create window
    
    // Set window title
    HWND hwnd = GetHWnd();
    SetWindowText(hwnd, _T("Push Box Game"));
    
    // Set background color
    setbkcolor(RGB(240, 240, 240));
    cleardevice();
    
    // Enable batch drawing mode for better rendering efficiency
    BeginBatchDraw();
    
    // Initialize resources and game
    initUIResources();
    initPlayerAnim();  // Initialize player animation
    initGame();
    
    // Show main menu (will now display in the graphics window)
    showMainMenu();
    
    // End batch drawing
    EndBatchDraw();
    
    // Close graphics window
    closegraph();
    return 0;
}