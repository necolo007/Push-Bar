/*
 * 推箱子游戏 - 主程序
 * 使用WASD或方向键控制人物移动，推动箱子到目标位置
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <graphics.h> // EasyX 头文件
#include "game.h"
#include "ui.h"
#include "map.h"

int main() {
    // 设置控制台编码为UTF-8
    system("chcp 65001");
    
    // 初始化 EasyX 图形窗口
    initgraph(900, 700); // 可根据地图实际大小调整
    initUIResources();
    initPlayerAnim(); // 初始化玩家动画
    // 初始化游戏
    initGame();
    // 显示主菜单
    showMainMenu();
    // 释放图片资源
    freeUIResources();
    // 关闭图形窗口
    closegraph();
    return 0;
} 