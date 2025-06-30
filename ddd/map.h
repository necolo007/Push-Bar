#ifndef MAP_H
#define MAP_H

#include "game.h"

// 加载地图
int loadMap(int level, char map[MAX_MAP_HEIGHT][MAX_MAP_WIDTH], int *height, int *width);

// 创建地图目录和地图文件
void createMapFiles();

// 获取可用关卡数量
int getAvailableLevels();

#endif // MAP_H 