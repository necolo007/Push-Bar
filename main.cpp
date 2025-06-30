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

// Set up text and font for UI
void setupTextEnvironment() {
    // Set font properties
    LOGFONT font;
    gettextstyle(&font);
    font.lfCharSet = DEFAULT_CHARSET;  // Use default character set
    font.lfQuality = ANTIALIASED_QUALITY;  // Anti-aliased text for better display
    _tcscpy(font.lfFaceName, _T("Arial"));  // Use Arial font
    font.lfWeight = FW_NORMAL;  // Normal weight
    settextstyle(&font);
    
    // Set text properties
    setbkmode(TRANSPARENT);  // Transparent background for text
}

int main() {
    // Initialize EasyX graphics window
    initgraph(900, 700);  // Create window
    
    // Set window title
    HWND hwnd = GetHWnd();
    SetWindowText(hwnd, _T("Push Box Game"));
    
    // Set background color
    setbkcolor(RGB(240, 240, 240));
    cleardevice();
    
    // Set text environment
    setupTextEnvironment();
    
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
    
    // Release image resources
    freeUIResources();
    
    // Close graphics window
    closegraph();
    return 0;
}