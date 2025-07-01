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

// 菜单状态变量
MenuState menuState;

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


// 初始化文本设置
void initTextSettings() {
    // 设置背景模式为透明
    setbkmode(TRANSPARENT);
}

// 清屏函数 - 使用EasyX的cleardevice代替
void clearScreen() {
    cleardevice();
}

// 处理EasyX消息循环
void processWindowMessage() {
    // 检查是否有消息
    ExMessage msg;
    if (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
        if (msg.message == WM_KEYDOWN) {
            // 将EasyX按键消息转换为字符输入
            int input;
            switch (msg.vkcode) {
                case VK_UP:    input = 72; break;
                case VK_DOWN:  input = 80; break;
                case VK_LEFT:  input = 75; break;
                case VK_RIGHT: input = 77; break;
                case VK_ESCAPE: input = 27; break;
                case VK_RETURN: input = '\r'; break;
                case VK_SPACE:  input = ' '; break;
                default: input = msg.vkcode; break;
            }
            // 处理输入
            handleInput(input);
        } else if (msg.message == WM_LBUTTONDOWN) {
            // 根据鼠标点击位置转换为方向键输入
            GameState* gameState = getGameState();
            int cellSize = 64;
            int offsetX = 50, offsetY = 80;
            
            // 计算玩家在屏幕上的像素位置
            int playerScreenX = offsetX + gameState->player.x * cellSize + cellSize/2;
            int playerScreenY = offsetY + gameState->player.y * cellSize + cellSize/2;
            
            // 根据鼠标点击的方向决定移动方向
            int dx = msg.x - playerScreenX;
            int dy = msg.y - playerScreenY;
            
            // 判断主要移动方向（水平或垂直）
            if (abs(dx) > abs(dy)) {
                // 水平移动为主
                if (dx > 0) {
                    handleInput('d'); // 向右
                } else {
                    handleInput('a'); // 向左
                }
            } else {
                // 垂直移动为主
                if (dy > 0) {
                    handleInput('s'); // 向下
                } else {
                    handleInput('w'); // 向上
                }
            }
        }
    }
}

// 绘制菜单项
void drawMenuItem(int x, int y, const char* text, bool selected) {
    // 设置文本样式
    settextstyle(28, 0, _T("Arial"));
    setbkmode(TRANSPARENT);
    
    if (selected) {
        // 选中项的样式
        settextcolor(RGB(255, 0, 0));  // 红色
        TCHAR buffer[256];
        _stprintf(buffer, _T("> %s <"), text);
        outtextxy(x - 20, y, buffer);
    } else {
        // 未选中项的样式
        settextcolor(RGB(0, 0, 0));    // 黑色
        outtextxy(x, y, text);
    }
}

// 显示主菜单
void showMainMenu() {
    // 初始化菜单状态
    menuState.selectedItem = 0;
    menuState.menuItemCount = 4;
    menuState.menuActive = true;
    
    // 初始化文本设置
    initTextSettings();
    
    // 菜单循环
    while (menuState.menuActive) {
        // 清屏并设置背景
        cleardevice();
        setbkcolor(RGB(220, 220, 220));
        cleardevice();
        
        // 绘制标题
        settextstyle(40, 0, _T("Arial"));
        settextcolor(RGB(0, 0, 180));
        setbkmode(TRANSPARENT);
        outtextxy(300, 100, _T("PUSH BOX GAME"));
        
        // 绘制菜单项
        const char* menuItems[] = {"Start Game", "Select Level", "Instructions", "Exit Game"};
        for (int i = 0; i < menuState.menuItemCount; i++) {
            drawMenuItem(360, 200 + i * 60, menuItems[i], i == menuState.selectedItem);
        }
        
        // 绘制操作提示
        settextstyle(20, 0, _T("Arial"));
        settextcolor(RGB(100, 100, 100));
        outtextxy(300, 500, _T("Up/Down to select, Enter to confirm"));
        
        // 刷新屏幕
        FlushBatchDraw();
        
        // 处理输入 - 同时获取键盘和鼠标消息
        ExMessage msg;
        // 使用EM_MOUSE | EM_KEY获取所有类型的消息
        if (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
            if (msg.message == WM_KEYDOWN) {
                // 键盘处理逻辑保持不变
                switch (msg.vkcode) {
                    case VK_UP:
                        menuState.selectedItem = (menuState.selectedItem + menuState.menuItemCount - 1) % menuState.menuItemCount;
                        break;
                    case VK_DOWN:
                        menuState.selectedItem = (menuState.selectedItem + 1) % menuState.menuItemCount;
                        break;
                    case VK_RETURN:
                        executeMenuAction(menuState.selectedItem);
                        break;
                }
            } else if (msg.message == WM_MOUSEMOVE) {
                // 鼠标移动时更新选中项（实现悬停效果）
                for (int i = 0; i < menuState.menuItemCount; i++) {
                    int itemY = 200 + i * 60;
                    // 扩大检测区域以提高响应性
                    if (msg.x >= 260 && msg.x <= 660 && msg.y >= itemY - 25 && msg.y <= itemY + 25) {
                        menuState.selectedItem = i;
                        break;  // 找到后立即退出循环
                    }
                }
            } else if (msg.message == WM_LBUTTONDOWN) {
                // 鼠标左键点击处理
                for (int i = 0; i < menuState.menuItemCount; i++) {
                    int itemY = 200 + i * 60;
                    if (msg.x >= 260 && msg.x <= 660 && msg.y >= itemY - 25 && msg.y <= itemY + 25) {
                        menuState.selectedItem = i;
                        executeMenuAction(menuState.selectedItem);
                        break;  // 找到后立即退出循环
                    }
                }
            }
        }
        // 控制帧率
        Sleep(20);
    }
}

// 处理菜单输入 - 在新的消息处理中不需要这个函数，但保留接口
void handleMenuInput(int input) {
    // 此函数不再需要，但保留接口兼容性
    GameState* gameState = getGameState();
    // 将直接在菜单绘制函数中处理输入
}

// 执行菜单项动作
void executeMenuAction(int itemIndex) {
    switch (itemIndex) {
        case 0: // 开始游戏
            menuState.menuActive = false;
            if (loadLevel(1)) {
                showGame();
            }
            break;
        case 1: // 选择关卡
            menuState.menuActive = false;
            getGameState()->gameState = GAME_SELECT;
            showLevelSelect();
            break;
        case 2: // 游戏说明
            menuState.menuActive = false;
            showInstructions();
            break;
        case 3: // 退出游戏
            exit(0);
            break;
    }
}

// 显示游戏界面
void showGame() {
    GameState* gameState = getGameState();
    int cellSize = 64; // 格子像素大小
    int offsetX = 50, offsetY = 80; // 地图绘制起点
    int goalSize = 32; // 目标点图片尺寸

    // 初始化文本设置
    initTextSettings();

    // 开始批量绘制
    BeginBatchDraw();
    
    while (gameState->gameState == GAME_PLAYING) {
        cleardevice();
        
        settextstyle(24, 0, _T("Arial"));
        settextcolor(RGB(0,0,0));
        TCHAR info[128];
        _stprintf(info, _T("Level %d  Steps: %d  Score: %d"), gameState->currentLevel, gameState->steps, gameState->score);
        outtextxy(offsetX, 20, info);
        outtextxy(offsetX, 40, _T("Controls: WASD/Arrows-Move R-Reset ESC-Menu"));

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
        
        // 刷新屏幕
        FlushBatchDraw();
        
        // 处理输入
        processWindowMessage();
        
        // 如果游戏状态改变，退出游戏循环
        if (gameState->gameState != GAME_PLAYING) {
            break;
        }
        
        // 控制帧率
        Sleep(30);
    }
    
    // 结束批量绘制
    EndBatchDraw();
    
    // 根据游戏状态转到对应界面
    if (gameState->gameState == GAME_WON) {
        showWinScreen();
    } else if (gameState->gameState == GAME_MENU) {
        resetGameToMenu();  // 重置游戏状态到主菜单
        showMainMenu();
    } else if (gameState->gameState == GAME_FAILED) {
        showFailScreen();  // 显示游戏失败界面
    }
}

// 显示游戏胜利界面
void showWinScreen() {
    GameState* gameState = getGameState();
    bool winScreenActive = true;
    
    // 初始化文本设置
    initTextSettings();
    
    while (winScreenActive && gameState->gameState == GAME_WON) {
        cleardevice();
        setbkcolor(RGB(240, 240, 200));
        cleardevice();
        
        // 绘制胜利信息
        settextstyle(40, 0, _T("Arial"));
        settextcolor(RGB(0, 180, 0));
        setbkmode(TRANSPARENT);
        outtextxy(300, 150, _T("Level Complete!"));
        
        // 显示统计信息
        settextstyle(28, 0, _T("Arial"));
        settextcolor(RGB(0, 0, 0));
        TCHAR info[128];
        _stprintf(info, _T("Total Steps: %d  Total Score: %d"), gameState->steps, gameState->score);
        outtextxy(310, 240, info);
        
        // 下一步提示
        settextstyle(24, 0, _T("Arial"));
        if (gameState->currentLevel < MAX_LEVELS) {
            outtextxy(260, 320, _T("Press Enter/Space for next level"));
        } else {
            outtextxy(280, 320, _T("All levels complete!"));
            outtextxy(290, 360, _T("Press Enter to return to menu"));
        }
        
        // 刷新屏幕
        FlushBatchDraw();
        
        // 处理输入
        ExMessage msg;
        if (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
            if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_RETURN || msg.vkcode == VK_SPACE) {
                    winScreenActive = false;
                    
                    if (gameState->currentLevel < MAX_LEVELS) {
                        // 进入下一关
                        loadLevel(gameState->currentLevel + 1);
                        showGame();
                    } else {
                        // 所有关卡完成，强制返回主菜单
                        resetGameToMenu();  // 重置游戏状态
                        showMainMenu();     // 显示主菜单
                    }
                } else if (msg.vkcode == VK_ESCAPE) {
                    // ESC键也可以返回主菜单
                    winScreenActive = false;
                    resetGameToMenu();  // 重置游戏状态
                    showMainMenu();
                }
            } else if (msg.message == WM_LBUTTONDOWN) {
                // 处理鼠标点击
                if (gameState->currentLevel < MAX_LEVELS) {
                    // 点击任意位置进入下一关
                    winScreenActive = false;
                    loadLevel(gameState->currentLevel + 1);
                    showGame();
                } else {
                    // 点击任意位置返回主菜单
                    winScreenActive = false;
                    resetGameToMenu();
                    showMainMenu();
                }
            }
        }
        
        // 控制帧率
        Sleep(50);
    }
}

// 显示关卡选择界面
void showLevelSelect() {
    GameState* gameState = getGameState();
    int selectedLevel = 1;
    bool levelSelectActive = true;
    
    // 初始化文本设置
    initTextSettings();
    
    while (levelSelectActive && gameState->gameState == GAME_SELECT) {
        cleardevice();
        setbkcolor(RGB(220, 240, 240));
        cleardevice();
        
        // 绘制标题
        settextstyle(36, 0, _T("Arial"));
        settextcolor(RGB(0, 0, 150));
        setbkmode(TRANSPARENT);
        outtextxy(330, 100, _T("Select Level"));
        
        // 绘制关卡选项
        settextstyle(28, 0, _T("Arial"));
        for (int i = 1; i <= MAX_LEVELS; i++) {
            TCHAR levelText[20];
            _stprintf(levelText, _T("Level %d"), i);
            
            if (i == selectedLevel) {
                settextcolor(RGB(255, 0, 0));
                outtextxy(380 - 20, 180 + (i - 1) * 50, _T(">"));
                outtextxy(380, 180 + (i - 1) * 50, levelText);
                outtextxy(480 + 10, 180 + (i - 1) * 50, _T("<"));
            } else {
                settextcolor(RGB(0, 0, 0));
                outtextxy(380, 180 + (i - 1) * 50, levelText);
            }
        }
        
        // 绘制操作提示
        settextstyle(20, 0, _T("Arial"));
        settextcolor(RGB(100, 100, 100));
        outtextxy(260, 500, _T("Up/Down to select, Enter to confirm, ESC to return"));
        
        // 刷新屏幕
        FlushBatchDraw();
        
        // 处理输入 - 同时获取键盘和鼠标消息
        ExMessage msg;
        if (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
            if (msg.message == WM_KEYDOWN) {
                switch (msg.vkcode) {
                    case VK_UP:
                        if (selectedLevel > 1)
                            selectedLevel--;
                        break;
                    case VK_DOWN:
                        if (selectedLevel < MAX_LEVELS)
                            selectedLevel++;
                        break;
                    case VK_RETURN:
                        levelSelectActive = false;
                        loadLevel(selectedLevel);
                        showGame();
                        break;
                    case VK_ESCAPE:
                        levelSelectActive = false;
                        resetGameToMenu();  // 重置游戏状态
                        showMainMenu();
                        break;
                }
            } else if (msg.message == WM_MOUSEMOVE) {
                // 鼠标悬停选择关卡
                for (int i = 1; i <= MAX_LEVELS; i++) {
                    int itemY = 180 + (i - 1) * 50;
                    if (msg.x >= 360 && msg.x <= 500 && msg.y >= itemY - 15 && msg.y <= itemY + 15) {
                        selectedLevel = i;
                        break;
                    }
                }
            } else if (msg.message == WM_LBUTTONDOWN) {
                // 鼠标点击选择关卡
                for (int i = 1; i <= MAX_LEVELS; i++) {
                    int itemY = 180 + (i - 1) * 50;
                    if (msg.x >= 360 && msg.x <= 500 && msg.y >= itemY - 15 && msg.y <= itemY + 15) {
                        selectedLevel = i;
                        levelSelectActive = false;
                        loadLevel(selectedLevel);
                        showGame();
                        break;
                    }
                }
                
                // 检查"返回"按钮区域
                if (msg.x >= 260 && msg.x <= 520 && msg.y >= 490 && msg.y <= 520) {
                    levelSelectActive = false;
                    resetGameToMenu();
                    showMainMenu();
                }
            }
        }
        
        // 控制帧率
        Sleep(25);
    }
}

void showFailScreen() {
    GameState* gameState = getGameState();
    bool failScreenActive = true;
    
    // 初始化文本设置
    initTextSettings();
    
    while (failScreenActive && gameState->gameState == GAME_FAILED) {
        cleardevice();
        setbkcolor(RGB(240, 200, 200));
        cleardevice();
        
        // 绘制失败信息
        settextstyle(40, 0, _T("Arial"));
        settextcolor(RGB(180, 0, 0));
        setbkmode(TRANSPARENT);
        outtextxy(300, 150, _T("Game Over!"));
        
        // 显示统计信息
        settextstyle(28, 0, _T("Arial"));
        settextcolor(RGB(0, 0, 0));
        TCHAR info[128];
        _stprintf(info, _T("Total Steps: %d  Total Score: %d"), gameState->steps, gameState->score);
        outtextxy(310, 240, info);
        
        // 返回提示
        settextstyle(24, 0, _T("Arial"));
        outtextxy(260, 320, _T("Press Enter to return to main menu"));
        
        // 刷新屏幕
        FlushBatchDraw();
        
        // 处理输入
        ExMessage msg;
        if (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
            if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_RETURN) {
                    failScreenActive = false;
                    resetGameToMenu();  // 重置游戏状态
                    showMainMenu();     // 显示主菜单
                } else if (msg.vkcode == VK_ESCAPE) {
                    failScreenActive = false;
                    resetGameToMenu();  // 重置游戏状态
                    showMainMenu();
                }
            } else if (msg.message == WM_LBUTTONDOWN) {
                // 点击任何位置返回主菜单
                failScreenActive = false;
                resetGameToMenu();  // 重置游戏状态
                showMainMenu();
            }
        }
        
        // 控制帧率
        Sleep(50);
    }
}

// 显示操作说明
void showInstructions() {
    bool instructionsActive = true;
    
    // 初始化文本设置
    initTextSettings();
    
    while (instructionsActive) {
        cleardevice();
        setbkcolor(RGB(240, 240, 240));
        cleardevice();
        
        // 绘制标题
        settextstyle(36, 0, _T("Arial"));
        settextcolor(RGB(0, 100, 150));
        setbkmode(TRANSPARENT);
        outtextxy(330, 80, _T("Instructions"));
        
        // 绘制说明内容
        settextstyle(24, 0, _T("Arial"));
        settextcolor(RGB(0, 0, 0));
        int y = 150;
        int lineHeight = 36;
        
        outtextxy(200, y, _T("Game Objective:")); y += lineHeight;
        outtextxy(200, y, _T("Push all boxes to the target positions")); y += lineHeight * 1.5;
        
        outtextxy(200, y, _T("Controls:")); y += lineHeight;
        outtextxy(200, y, _T("Up/W: Move up")); y += lineHeight;
        outtextxy(200, y, _T("Down/S: Move down")); y += lineHeight;
        outtextxy(200, y, _T("Left/A: Move left")); y += lineHeight;
        outtextxy(200, y, _T("Right/D: Move right")); y += lineHeight;
        outtextxy(200, y, _T("R: Reset current level")); y += lineHeight;
        outtextxy(200, y, _T("ESC: Return to main menu")); y += lineHeight * 1.5;
        
        // 绘制返回提示
        settextstyle(20, 0, _T("Arial"));
        settextcolor(RGB(100, 100, 100));
        outtextxy(330, 500, _T("Press ESC to return to main menu"));
        
        // 刷新屏幕
        FlushBatchDraw();
        
        // 处理输入
        ExMessage msg;
        if (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
            if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                instructionsActive = false;
                resetGameToMenu();  // 重置游戏状态
                showMainMenu();
            } else if (msg.message == WM_LBUTTONDOWN) {
                // 检查"返回"按钮区域
                if (msg.x >= 330 - 100 && msg.x <= 330 + 300 && msg.y >= 500 - 10 && msg.y <= 500 + 30) {
                    instructionsActive = false;
                    resetGameToMenu();  // 重置游戏状态
                    showMainMenu();
                }
            }
        }
        
        // 控制帧率
        Sleep(50);
    }
}

// 初始化玩家动画
void initPlayerAnim() {
    playerAnim.dir = DIR_DOWN;
    playerAnim.walkFrame = 0;
    playerAnim.moving = false;
}

// 更新玩家动画方向和移动状态
void updatePlayerAnim(PlayerDir dir, bool moving) {
    playerAnim.dir = dir;
    playerAnim.moving = moving;
    if (!moving) {
        playerAnim.walkFrame = 0; // 站立状态
    }
}

// 切换动画帧
void nextPlayerAnimFrame() {
    if (playerAnim.moving) {
        playerAnim.walkFrame = (playerAnim.walkFrame + 1) % 3;
        if (playerAnim.walkFrame == 0) playerAnim.walkFrame = 1; // 跳过静止帧
    }
}

// PNG带alpha通道手动混合绘制函数
void drawAlpha(IMAGE* picture, int picture_x, int picture_y) {
    // 实现PNG透明图层的绘制
    // 获取图像的宽和高
    DWORD *dst = GetImageBuffer();
    DWORD *src = GetImageBuffer(picture);
    int src_width = picture->getwidth();
    int src_height = picture->getheight();
    int dst_width = getwidth();
    int dst_height = getheight();

    // 计算贴图的实际长宽
    int real_width = (picture_x + src_width > dst_width) ? dst_width - picture_x : src_width;
    int real_height = (picture_y + src_height > dst_height) ? dst_height - picture_y : src_height;

    // 修正负坐标
    if (picture_x < 0) { src += -picture_x;  real_width -= -picture_x;  picture_x = 0; }
    if (picture_y < 0) { src += -picture_y * src_width;  real_height -= -picture_y;  picture_y = 0; }

    // 计算指针偏移量
    dst += picture_y * dst_width + picture_x;

    // 实现透明贴图
    for (int iy = 0; iy < real_height; ++iy) {
        for (int ix = 0; ix < real_width; ++ix) {
            DWORD sc = src[ix];
            BYTE sa = ((sc & 0xff000000) >> 24);
            if (sa > 0) {
                // 像素点不全透明
                BYTE sr = ((sc & 0xff0000) >> 16); // 源像素RGB分量
                BYTE sg = ((sc & 0xff00) >> 8);
                BYTE sb = sc & 0xff;
                DWORD dc = dst[ix]; // 目标像素
                BYTE dr = ((dc & 0xff0000) >> 16); // 目标像素RGB分量
                BYTE dg = ((dc & 0xff00) >> 8);
                BYTE db = dc & 0xff;

                // 混合
                float alpha = sa / 255.0f;
                BYTE r = sr * alpha + dr * (1 - alpha);
                BYTE g = sg * alpha + dg * (1 - alpha);
                BYTE b = sb * alpha + db * (1 - alpha);

                // 新像素写回
                dst[ix] = ((r << 16) | (g << 8) | b);
            }
        }
        dst += dst_width;
        src += src_width;
    }
}

// 重置游戏状态，确保回到主菜单时状态正确
void resetGameToMenu() {
    GameState* gameState = getGameState();
    gameState->gameState = GAME_MENU;
    
    // 清除可能影响菜单的状态
    cleardevice();
    setbkcolor(RGB(220, 220, 220));
    cleardevice();
    
    // 重置菜单状态
    menuState.selectedItem = 0;
    menuState.menuItemCount = 4;
    menuState.menuActive = true;
}