#ifndef UI_H
#define UI_H
#include <graphics.h>
// 清屏函数
void clearScreen();

// 设置控制台光标位置
void gotoxy(int x, int y);

// 显示主菜单
void showMainMenu();

// 处理菜单输入
void handleMenuInput(int input);

// 显示游戏界面
void showGame();

// 显示游戏胜利界面
void showWinScreen();

// 显示关卡选择界面
void showLevelSelect();

// 显示操作说明
void showInstructions();

// EasyX 图片资源管理
void initUIResources(); // 加载图片资源
void freeUIResources(); // 释放图片资源

// 玩家动画方向
typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } PlayerDir;
// 玩家动画状态
typedef struct {
    PlayerDir dir;   // 当前朝向
    int walkFrame;   // 当前帧（0=站立，1/2=移动帧）
    bool moving;     // 是否正在移动
} PlayerAnim;

// 玩家动画全局变量
extern PlayerAnim playerAnim;

// 初始化玩家动画
void initPlayerAnim();
// 玩家移动时更新动画帧
void updatePlayerAnim(PlayerDir dir, bool moving);
// 玩家移动一格后切换帧
void nextPlayerAnimFrame();

// PNG带alpha通道手动混合绘制函数声明
void drawAlpha(IMAGE* picture, int picture_x, int picture_y);

#endif // UI_H 