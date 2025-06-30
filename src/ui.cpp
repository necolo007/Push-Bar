#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "ui.h"
#include "game.h"
#include "map.h"
#include <graphics.h> // EasyX 头文件
#include <string.h>

// 图片资源变量
IMAGE img_wall, img_box, img_player, img_goal, img_box_on_goal, img_player_on_goal, img_floor;
IMAGE img_player_front, img_player_back, img_player_left, img_player_right;
IMAGE img_player_front_walk1, img_player_front_walk2;
IMAGE img_player_back_walk1, img_player_back_walk2;
IMAGE img_player_left_walk, img_player_right_walk;

// 玩家动画状态变量
PlayerAnim playerAnim;


// 加载图片资源
void initUIResources() {
    loadimage(&img_wall, _T("res/wall.png"));
    loadimage(&img_box, _T("res/box.png"));
    loadimage(&img_player, _T("res/player.png"));
    loadimage(&img_goal, _T("res/goal.png"));
    loadimage(&img_box_on_goal, _T("res/box_on_goal.png"));
    loadimage(&img_player_on_goal, _T("res/player_on_goal.png"));
    loadimage(&img_floor, _T("res/floor.png"));
    loadimage(&img_player_front, _T("res/player_front.png"));
    loadimage(&img_player_back, _T("res/player_back.png"));
    loadimage(&img_player_left, _T("res/player_left.png"));
    loadimage(&img_player_right, _T("res/player_right.png"));
    loadimage(&img_player_front_walk1, _T("res/player_front_walk1.png"));
    loadimage(&img_player_front_walk2, _T("res/player_front_walk2.png"));
    loadimage(&img_player_back_walk1, _T("res/player_back_walk1.png"));
    loadimage(&img_player_back_walk2, _T("res/player_back_walk2.png"));
    loadimage(&img_player_left_walk, _T("res/player_left_walk.png"));
    loadimage(&img_player_right_walk, _T("res/player_right_walk.png"));
}

// 释放图片资源（EasyX 无需手动释放，但可留空以便扩展）
void freeUIResources() {}

// 清屏函数
void clearScreen() {
    system("cls");
}

// 设置控制台光标位置
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 隐藏控制台光标
void hideCursor() {
    CONSOLE_CURSOR_INFO cursor_info = {1, 0};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// 显示控制台光标
void showCursor() {
    CONSOLE_CURSOR_INFO cursor_info = {1, 1};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// 显示主菜单
void showMainMenu() {
    clearScreen();
    hideCursor();
    
    printf("\n\n");
    printf("        ★★★ 推箱子游戏 ★★★\n\n");
    printf("        1. 开始游戏\n");
    printf("        2. 选择关卡\n");
    printf("        3. 游戏说明\n");
    printf("        4. 退出游戏\n\n");
    printf("        请选择 (1-4): ");

    // 等待用户输入
    int input;
    while (1) {
        input = _getch();
        handleMenuInput(input);
    }
}

// 处理菜单输入
void handleMenuInput(int input) {
    GameState* gameState = getGameState();
    
    switch (gameState->gameState) {
        case GAME_MENU:
            switch (input) {
                case '1': // 开始游戏
                    if (loadLevel(1)) {
                        showGame();
                    }
                    break;
                    
                case '2': // 选择关卡
                    gameState->gameState = GAME_SELECT;
                    showLevelSelect();
                    break;
                    
                case '3': // 游戏说明
                    showInstructions();
                    break;
                    
                case '4': // 退出游戏
                    clearScreen();
                    showCursor();
                    exit(0);
                    break;
            }
            break;
            
        case GAME_SELECT:
            // 在showLevelSelect中处理
            break;
        
        // 其他状态的输入处理已在game.c中的handleInput函数中完成
    }
}

// 显示游戏界面
void showGame() {
    GameState* gameState = getGameState();
    int cellSize = 64; // 格子像素大小
    int offsetX = 50, offsetY = 80; // 地图绘制起点
    int goalSize = 32; // 目标点图片尺寸

    while (gameState->gameState == GAME_PLAYING) {
        cleardevice();
        setbkmode(TRANSPARENT);
        settextcolor(RGB(0,0,0));
        TCHAR info[128];
        _stprintf(info, _T("第 %d 关  步数: %d  得分: %d"), gameState->currentLevel, gameState->steps, gameState->score);
        outtextxy(offsetX, 20, info);
        outtextxy(offsetX, 40, _T("操作: WASD/方向键-移动 R-重置关卡 ESC-返回菜单"));

        for (int y = 0; y < gameState->mapHeight; y++) {
            for (int x = 0; x < gameState->mapWidth; x++) {
                int px = offsetX + x * cellSize;
                int py = offsetY + y * cellSize;
                char cell = gameState->map[y][x];
                // 1. 画地板
                putimage(px, py, &img_floor);
                // 2. 画目标点（居中）
                if (cell == TARGET || cell == BOX_TARGET || cell == PLAYER_TARGET) {
                    int gx = px + (cellSize - goalSize) / 2;
                    int gy = py + (cellSize - goalSize) / 2;
                    drawAlpha(&img_goal,gx, gy);
                }
                // 3. 画箱子/墙壁
                if (cell == WALL) {
                    putimage(px, py, &img_wall);
                } else if (cell == BOX || cell == BOX_TARGET) {
                    drawAlpha(&img_box, px, py); // PNG透明
                }
                // 4. 画人物（底对齐居中，PNG透明）
                IMAGE* playerImg = nullptr;
                if (cell == PLAYER || cell == PLAYER_TARGET) {
                    switch (playerAnim.dir) {
                        case DIR_UP:
                            if (playerAnim.walkFrame == 0) playerImg = &img_player_back;
                            else if (playerAnim.walkFrame == 1) playerImg = &img_player_back_walk1;
                            else playerImg = &img_player_back_walk2;
                            break;
                        case DIR_DOWN:
                            if (playerAnim.walkFrame == 0) playerImg = &img_player_front;
                            else if (playerAnim.walkFrame == 1) playerImg = &img_player_front_walk1;
                            else playerImg = &img_player_front_walk2;
                            break;
                        case DIR_LEFT:
                            playerImg = (playerAnim.walkFrame == 0) ? &img_player_left : &img_player_left_walk;
                            break;
                        case DIR_RIGHT:
                            playerImg = (playerAnim.walkFrame == 0) ? &img_player_right : &img_player_right_walk;
                            break;
                    }
                    if (playerImg) {
                        int pw = playerImg->getwidth();
                        int ph = playerImg->getheight();
                        int px2 = px + (cellSize - pw) / 2;
                        int py2 = py + cellSize - ph;
                        drawAlpha(playerImg, px2, py2); // PNG透明
                    }
                }
            }
        }
        // 等待用户输入
        int input = _getch();
        if (input == 224) input = _getch();
        handleInput(input);
        if (gameState->gameState == GAME_WON) {
            showWinScreen();
        }
    }
}

// 显示游戏胜利界面
void showWinScreen() {
    GameState* gameState = getGameState();
    
    clearScreen();
    
    printf("\n\n\n");
    printf("        ★★★ 恭喜过关！★★★\n\n");
    printf("        关卡: %d\n", gameState->currentLevel);
    printf("        步数: %d\n", gameState->steps);
    printf("        得分: %d\n\n", gameState->score);
    
    if (gameState->currentLevel < MAX_LEVELS) {
        printf("        按回车键或空格键继续下一关...\n");
    } else {
        printf("        恭喜你完成了所有关卡！\n");
        printf("        按回车键或空格键返回主菜单...\n");
    }
    
    // 等待用户输入
    int input;
    while (1) {
        input = _getch();
        handleInput(input);
        
        if (gameState->gameState != GAME_WON) {
            // 状态已经改变，退出等待
            if (gameState->gameState == GAME_PLAYING) {
                // 进入下一关
                showGame();
            } else if (gameState->gameState == GAME_MENU) {
                // 返回主菜单
                showMainMenu();
            }
            break;
        }
    }
}

// 显示关卡选择界面
void showLevelSelect() {
    GameState* gameState = getGameState();
    int availableLevels = getAvailableLevels();
    
    clearScreen();
    
    printf("\n\n");
    printf("        ★★★ 选择关卡 ★★★\n\n");
    
    for (int i = 1; i <= availableLevels; i++) {
        printf("        %d. 第 %d 关\n", i, i);
    }
    
    printf("\n        ESC. 返回主菜单\n\n");
    printf("        请选择关卡 (1-%d): ", availableLevels);
    
    // 等待用户输入
    int input;
    while (1) {
        input = _getch();
        
        if (input == 27) { // ESC
            gameState->gameState = GAME_MENU;
            showMainMenu();
            break;
        } else if (input >= '1' && input <= '0' + availableLevels) {
            int level = input - '0';
            if (loadLevel(level)) {
                showGame();
            }
            break;
        }
    }
}

// 从文件加载并显示操作说明
void showInstructions() {
    clearScreen();
    
    printf("\n");
    printf("        ★★★ 游戏说明 ★★★\n\n");
    
    // 尝试从文件读取说明
    FILE *file = fopen("instructions.txt", "r");
    if (file) {
        char line[100];
        while (fgets(line, sizeof(line), file)) {
            printf("        %s", line);
        }
        fclose(file);
    } else {
        // 如果文件不存在，显示默认说明
        printf("        游戏元素说明：\n");
        printf("        ■ - 墙壁\n");
        printf("        ♀ - 玩家\n");
        printf("        □ - 箱子\n");
        printf("        ★ - 目标点\n");
        printf("        ☆ - 箱子放在目标点上\n");
        printf("        ♂ - 玩家站在目标点上\n\n");
        
        printf("        操作说明：\n");
        printf("        WASD或方向键 - 控制玩家移动\n");
        printf("        R键 - 重置当前关卡\n");
        printf("        ESC键 - 返回主菜单\n\n");
        
        printf("        游戏规则：\n");
        printf("        1. 通过控制人物移动，将所有箱子推到目标点上\n");
        printf("        2. 箱子只能被推，不能被拉\n");
        printf("        3. 一次只能推动一个箱子\n");
        printf("        4. 当所有箱子都放到目标点上时，过关成功\n");
        
        // 创建说明文件以供将来使用
        file = fopen("instructions.txt", "w");
        if (file) {
            fprintf(file, "游戏元素说明：\n");
            fprintf(file, "■ - 墙壁\n");
            fprintf(file, "♀ - 玩家\n");
            fprintf(file, "□ - 箱子\n");
            fprintf(file, "★ - 目标点\n");
            fprintf(file, "☆ - 箱子放在目标点上\n");
            fprintf(file, "♂ - 玩家站在目标点上\n\n");
            
            fprintf(file, "操作说明：\n");
            fprintf(file, "WASD或方向键 - 控制玩家移动\n");
            fprintf(file, "R键 - 重置当前关卡\n");
            fprintf(file, "ESC键 - 返回主菜单\n\n");
            
            fprintf(file, "游戏规则：\n");
            fprintf(file, "1. 通过控制人物移动，将所有箱子推到目标点上\n");
            fprintf(file, "2. 箱子只能被推，不能被拉\n");
            fprintf(file, "3. 一次只能推动一个箱子\n");
            fprintf(file, "4. 当所有箱子都放到目标点上时，过关成功\n");
            fclose(file);
        }
    }
    
    printf("\n\n        按任意键返回主菜单...");
    _getch();
    showMainMenu();
}

// 初始化玩家动画
void initPlayerAnim() {
    playerAnim.dir = DIR_DOWN;
    playerAnim.walkFrame = 0;
    playerAnim.moving = false;
}

// 玩家移动时更新动画帧
void updatePlayerAnim(PlayerDir dir, bool moving) {
    if (playerAnim.dir != dir) {
        playerAnim.dir = dir;
        playerAnim.walkFrame = 0;
    }
    playerAnim.moving = moving;
}

// 玩家移动一格后切换帧
void nextPlayerAnimFrame() {
    switch (playerAnim.dir) {
        case DIR_UP:
        case DIR_DOWN:
            playerAnim.walkFrame = (playerAnim.walkFrame == 0) ? 1 : (playerAnim.walkFrame == 1 ? 2 : 0);
            if (playerAnim.walkFrame > 2) playerAnim.walkFrame = 0;
            break;
        case DIR_LEFT:
        case DIR_RIGHT:
            playerAnim.walkFrame = (playerAnim.walkFrame == 0) ? 1 : 0;
            break;
    }
}

// 手动实现PNG alpha混合的绘制函数
void drawAlpha(IMAGE* picture, int picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
{
    DWORD *dst = GetImageBuffer();    // 目标显存指针
    DWORD *draw = GetImageBuffer();
    DWORD *src = GetImageBuffer(picture); //图片显存指针
    int picture_width = picture->getwidth();
    int picture_height = picture->getheight();
    int graphWidth = getwidth();
    int graphHeight = getheight();
    int dstX = 0;
    for (int iy = 0; iy < picture_height; iy++)
    {
        for (int ix = 0; ix < picture_width; ix++)
        {
            int srcX = ix + iy * picture_width;
            int sa = ((src[srcX] & 0xff000000) >> 24);
            int sr = ((src[srcX] & 0xff0000) >> 16);
            int sg = ((src[srcX] & 0xff00) >> 8);
            int sb = src[srcX] & 0xff;
            if (ix + picture_x >= 0 && ix + picture_x < graphWidth && iy + picture_y >= 0 && iy + picture_y < graphHeight)
            {
                dstX = (ix + picture_x) + (iy + picture_y) * graphWidth;
                int dr = ((dst[dstX] & 0xff0000) >> 16);
                int dg = ((dst[dstX] & 0xff00) >> 8);
                int db = dst[dstX] & 0xff;
                draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)
                    | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)
                    | (sb * sa / 255 + db * (255 - sa) / 255);
            }
        }
    }
} 