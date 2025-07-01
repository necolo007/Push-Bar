#ifndef GAME_H
#define GAME_H

// 地图元素定义
#define WALL        '#'  // 墙
#define PLAYER      '@'  // 玩家
#define BOX         '$'  // 箱子
#define TARGET      '.'  // 目标点
#define FLOOR       ' '  // 空地
#define BOX_TARGET  '*'  // 箱子在目标点上
#define PLAYER_TARGET '+'  // 玩家在目标点上

// 方向定义
#define UP      0
#define DOWN    1
#define LEFT    2
#define RIGHT   3

// 游戏状态
#define GAME_MENU     0  // 主菜单
#define GAME_PLAYING  1  // 游戏中
#define GAME_WON      2  // 游戏胜利
#define GAME_SELECT   3  // 选择关卡
#define GAME_FAILED   4  // 游戏失败

// 最大地图尺寸
#define MAX_MAP_HEIGHT 20
#define MAX_MAP_WIDTH 30

// 最大关卡数
#define MAX_LEVELS 5

#define MAX_STEP 100 // 最大步数

// 玩家位置结构
typedef struct {
    int x;
    int y;
} Position;

// 游戏状态结构
typedef struct {
    char map[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];  // 当前地图
    char originalMap[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];  // 原始地图（重置用）
    char mapHistory[MAX_STEP][MAX_MAP_HEIGHT][MAX_MAP_WIDTH]; //悔棋用
    int mapHeight;  // 地图高度
    int mapWidth;   // 地图宽度
    Position player; // 玩家位置
    int steps;      // 移动步数
    int score;      // 得分
    int boxCount;   // 箱子数量
    int boxOnTarget; // 已放到目标点的箱子数
    int currentLevel; // 当前关卡
    int gameState;   // 游戏状态
} GameState;

// 游戏初始化
void initGame();

// 加载关卡
int loadLevel(int level);

// 重置当前关卡
void resetLevel();

// 处理输入
void handleInput(int input);

// 移动玩家
int movePlayer(int direction);

// 检查游戏是否胜利
int checkWin();

int checkFail();

void undoMove(); // 撤销上一步操作

// 获取游戏状态
GameState* getGameState();

#endif // GAME_H 