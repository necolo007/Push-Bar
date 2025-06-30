#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "map.h"
#include "game.h"

// 默认地图
static const char* defaultMaps[] = {
    // 第一关 - 简单
    "#########\n"
    "#   #   #\n"
    "# $ # . #\n"
    "#       #\n"
    "# $   . #\n"
    "#   #   #\n"
    "#@  #   #\n"
    "#########\n",

    // 第二关 - 中等
    "############\n"
    "#          #\n"
    "#  #####   #\n"
    "#       $  #\n"
    "#  # $ #   #\n"
    "# .      . #\n"
    "#  #####   #\n"
    "#      @   #\n"
    "############\n",

    // 第三关 - 困难
    "###############\n"
    "#      #      #\n"
    "# $$   #   .. #\n"
    "#             #\n"
    "### ##### ### #\n"
    "#      #      #\n"
    "# $    #    . #\n"
    "#   @         #\n"
    "###############\n"
};

// 地图文件夹和文件名格式
static const char* MAP_FOLDER = "maps";
static const char* MAP_FILE_FORMAT = "maps/level%d.map";

// 创建地图目录和地图文件
void createMapFiles() {
    // 创建地图目录
    _mkdir(MAP_FOLDER);

    // 创建默认地图文件
    for (int i = 0; i < sizeof(defaultMaps) / sizeof(defaultMaps[0]); i++) {
        char filename[50];
        sprintf(filename, MAP_FILE_FORMAT, i + 1);
        
        FILE *file = fopen(filename, "w");
        if (file) {
            fputs(defaultMaps[i], file);
            fclose(file);
        }
    }
}

// 加载地图
int loadMap(int level, char map[MAX_MAP_HEIGHT][MAX_MAP_WIDTH], int *height, int *width) {
    char filename[50];
    sprintf(filename, MAP_FILE_FORMAT, level);

    FILE *file = fopen(filename, "r");
    if (!file) {
        // 如果文件不存在，创建地图文件
        createMapFiles();
        
        // 重新尝试打开
        file = fopen(filename, "r");
        if (!file) {
            return 0; // 仍然无法打开
        }
    }

    // 读取地图
    int y = 0;
    int maxWidth = 0;
    char line[MAX_MAP_WIDTH + 2]; // +2为了存放可能的换行符和终止符

    while (fgets(line, sizeof(line), file) && y < MAX_MAP_HEIGHT) {
        // 去掉换行符
        int len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (len > 0 && line[len - 1] == '\r') {
            line[len - 1] = '\0';
            len--;
        }

        // 复制到地图数组
        strncpy(map[y], line, MAX_MAP_WIDTH);

        // 确保字符串终止
        if (len < MAX_MAP_WIDTH) {
            map[y][len] = '\0';
        } else {
            map[y][MAX_MAP_WIDTH - 1] = '\0';
            len = MAX_MAP_WIDTH - 1;
        }

        // 记录最大宽度
        if (len > maxWidth) {
            maxWidth = len;
        }

        y++;
    }

    fclose(file);

    // 设置地图尺寸
    *height = y;
    *width = maxWidth;

    return 1;
}

// 获取可用关卡数量
int getAvailableLevels() {
    int count = 0;
    char filename[50];
    FILE *file;

    // 检查每个关卡文件是否存在
    for (int i = 1; i <= MAX_LEVELS; i++) {
        sprintf(filename, MAP_FILE_FORMAT, i);
        file = fopen(filename, "r");
        if (file) {
            count++;
            fclose(file);
        } else {
            break;
        }
    }

    // 如果没有找到任何关卡文件，创建默认地图
    if (count == 0) {
        createMapFiles();
        count = sizeof(defaultMaps) / sizeof(defaultMaps[0]);
    }

    return count;
} 