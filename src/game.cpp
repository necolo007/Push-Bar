#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "ui.h"
#include "map.h"

// 全局游戏状态
static GameState gameState;

// 游戏初始化
void initGame() {
    // 初始化游戏状态
    memset(&gameState, 0, sizeof(GameState));
    gameState.gameState = GAME_MENU;
    gameState.currentLevel = 1;
}

// 加载关卡
int loadLevel(int level) {
    if (level < 1 || level > MAX_LEVELS) {
        return 0;
    }

    // 从文件加载地图
    if (!loadMap(level, gameState.map, &gameState.mapHeight, &gameState.mapWidth)) {
        return 0;
    }

    // 保存原始地图状态
    memcpy(gameState.originalMap, gameState.map, sizeof(gameState.map));

    // 重置游戏状态
    gameState.steps = 0;
    gameState.score = 1000; // 初始分数
    gameState.boxCount = 0;
    gameState.boxOnTarget = 0;
    gameState.currentLevel = level;
    gameState.gameState = GAME_PLAYING;

    // 查找玩家位置和统计箱子数量
    for (int y = 0; y < gameState.mapHeight; y++) {
        for (int x = 0; x < gameState.mapWidth; x++) {
            if (gameState.map[y][x] == PLAYER || gameState.map[y][x] == PLAYER_TARGET) {
                gameState.player.x = x;
                gameState.player.y = y;
            }
            if (gameState.map[y][x] == BOX || gameState.map[y][x] == BOX_TARGET) {
                gameState.boxCount++;
            }
        }
    }

    return 1;
}

// 重置当前关卡
void resetLevel() {
    // 恢复原始地图
    memcpy(gameState.map, gameState.originalMap, sizeof(gameState.map));

    // 重置游戏状态
    gameState.steps = 0;
    gameState.boxOnTarget = 0;

    // 重新查找玩家位置
    for (int y = 0; y < gameState.mapHeight; y++) {
        for (int x = 0; x < gameState.mapWidth; x++) {
            if (gameState.map[y][x] == PLAYER || gameState.map[y][x] == PLAYER_TARGET) {
                gameState.player.x = x;
                gameState.player.y = y;
                break;
            }
        }
    }
}

void undoMove() {
    if (gameState.steps > 1) {
        // 回退到上一步的地图状态
        gameState.steps--;
        memcpy(gameState.map, gameState.mapHistory[gameState.steps], sizeof(gameState.map));

        // 重新查找玩家位置
        for (int y = 0; y < gameState.mapHeight; y++) {
            for (int x = 0; x < gameState.mapWidth; x++) {
                if (gameState.map[y][x] == PLAYER || gameState.map[y][x] == PLAYER_TARGET) {
                    gameState.player.x = x;
                    gameState.player.y = y;
                    break;
                }
            }
        }
    }
}

// 处理输入
void handleInput(int input) {
    switch (gameState.gameState) {
        case GAME_MENU:
            // 菜单输入现在直接在showMainMenu中处理
            // 此处保留接口，但内部不需要实现
            break;

        case GAME_PLAYING:
            switch (input) {
                case 'W': // 上 (注意大小写都支持)
                case 'w':
                case 72:
                case VK_UP:  // 使用VK_UP等Windows虚拟键码
                    movePlayer(UP);
                    break;
                case 'S': // 下
                case 's':
                case 80:
                case VK_DOWN:
                    movePlayer(DOWN);
                    break;
                case 'A': // 左
                case 'a':
                case 75:
                case VK_LEFT:
                    movePlayer(LEFT);
                    break;
                case 'D': // 右
                case 'd':
                case  77:
                case VK_RIGHT:
                    movePlayer(RIGHT);
                    break;
                case 'R': // 重置关卡
                case 'r':
                    resetLevel();
                    break;
                case VK_ESCAPE:  // ESC键，返回主菜单
                    gameState.gameState = GAME_MENU;
                    // 游戏逻辑中只负责改变状态，不直接调用UI函数
                    break;
                case 'v':
                case 'V':
                    undoMove(); // 撤销上一步操作
                    // 这里可以添加暂停逻辑
                    break;
            }
            
            // 检查游戏是否胜利
            if (checkWin()) {
                gameState.gameState = GAME_WON;
            }
            if (checkFail()) {
                gameState.gameState = GAME_FAILED; // 游戏失败状态
            }
            break;

        case GAME_WON:
            // 胜利界面的输入现在在showWinScreen中处理
            break;

        case GAME_SELECT:
            // 关卡选择的输入现在在showLevelSelect中处理
            break;
        case GAME_FAILED:
            // 游戏失败界面的输入现在在showFailScreen中处理
            break;
    }
}

// 移动玩家
int movePlayer(int direction) {
    int dx = 0, dy = 0;
    PlayerDir dir;
    switch (direction) {
        case UP: dy = -1; dir = DIR_UP; break;
        case DOWN: dy = 1; dir = DIR_DOWN; break;
        case LEFT: dx = -1; dir = DIR_LEFT; break;
        case RIGHT: dx = 1; dir = DIR_RIGHT; break;
        default: return 0;
    }

    // 计算新位置
    int newX = gameState.player.x + dx;
    int newY = gameState.player.y + dy;
    
    // 检查新位置是否有效
    if (newX < 0 || newX >= gameState.mapWidth || newY < 0 || newY >= gameState.mapHeight) {
        return 0;
    }

    // 检查新位置的内容
    char newCell = gameState.map[newY][newX];

    if (newCell == WALL) {
        // 撞墙，不能移动
        return 0;
    } else if (newCell == BOX || newCell == BOX_TARGET) {
        // 推箱子
        int boxNewX = newX + dx;
        int boxNewY = newY + dy;
        
        // 检查箱子新位置是否有效
        if (boxNewX < 0 || boxNewX >= gameState.mapWidth || boxNewY < 0 || boxNewY >= gameState.mapHeight) {
            return 0;
        }

        // 检查箱子新位置的内容
        char boxNewCell = gameState.map[boxNewY][boxNewX];

        if (boxNewCell == WALL || boxNewCell == BOX || boxNewCell == BOX_TARGET) {
            // 箱子不能移动，玩家也不能移动
            return 0;
        }

        // 移动箱子
        if (boxNewCell == TARGET) {
            // 箱子移到目标点上
            gameState.map[boxNewY][boxNewX] = BOX_TARGET;
            gameState.boxOnTarget++;
            gameState.score += 10; // 加分
        } else {
            // 箱子移到普通地板
            gameState.map[boxNewY][boxNewX] = BOX;
        }

        // 更新原箱子位置
        if (newCell == BOX_TARGET) {
            // 箱子原来在目标点上
            gameState.map[newY][newX] = TARGET;
            gameState.boxOnTarget--;
        } else {
            // 箱子原来在普通地板上
            gameState.map[newY][newX] = FLOOR;
        }
    }

    gameState.score -= 10; // 减分
    // 更新玩家原位置
    if (gameState.map[gameState.player.y][gameState.player.x] == PLAYER_TARGET) {
        gameState.map[gameState.player.y][gameState.player.x] = TARGET;
    } else {
        gameState.map[gameState.player.y][gameState.player.x] = FLOOR;
    }

    // 更新玩家新位置
    if (gameState.map[newY][newX] == TARGET) {
        gameState.map[newY][newX] = PLAYER_TARGET;
    } else {
        gameState.map[newY][newX] = PLAYER;
    }

    // 更新玩家坐标
    gameState.player.x = newX;
    gameState.player.y = newY;

    // 增加步数
    gameState.steps++;
    memcpy(gameState.mapHistory[gameState.steps], gameState.map, sizeof(gameState.map));

    // 玩家实际移动时，更新动画
    updatePlayerAnim(dir, true); // 设置方向和移动状态
    nextPlayerAnimFrame();       // 切换动画帧

    return 1;
}

// 检查游戏是否胜利
int checkWin() {
    return gameState.boxOnTarget == gameState.boxCount;
}

int checkFail() {
    // 游戏失败的条件可以根据需要定义
    // 例如：如果步数超过某个限制，或者分数低于0等
    if (gameState.score < 0) {
        return 1; // 游戏失败
    }
    return 0; // 游戏未失败
}

// 获取游戏状态
GameState* getGameState() {
    return &gameState;
}