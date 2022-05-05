#include <raylib.h>
#include <string.h>

// WORLD DATA
enum mapList
{
    demo
};

#define mapX 10
#define mapY 6

// demo

int spawnMap = demo;

// collision map
int demoColMap[mapX][mapY] =
    {
        // x  0  1  2  3  4  5  //y
        {1, 1, 1, 1, 1, 1},  // 0
        {1, 0, 1, 0, 72, 1}, // 1
        {1, 0, 0, 0, 0, 1},  // 2
        {1, 0, 1, 0, 1, 1},  // 3
        {1, 0, 1, 0, 0, 1},  // 4
        {1, 1, 1, 1, 1, 1}, // 5
        {99,99,99,99,99,99},
        {99,99,99,99,99,99},
        {99,99,99,99,99,99},
        {99,99,99,99,99,99},
        };
// object map

int demoObjMap[mapX][mapY] =
    {
        {0, 0, 0, 0, 0, 0},
        {0, 3, 0, 0, 0, 0},
        {0, 5, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 2, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        };

enum tileList
{
    block,
    empty,
    top,
    right,
    bot,
    left,
    topright,
    botright,
    botleft,
    topleft,
    closeright,
    closebot,
    closeleft,
    closetop,
    vert,
    horz,
    nothing,
};

// tile types
int blockers[2] = {1, 99};
int blockersSize = sizeof blockers / sizeof *blockers;

// MAP
#define maxDepth 4
#define maxPer 3

// map tiles
#define xCellSize 51
#define yCellSize 30

// map
// current map
int curMap;
int lastMap;

// array copies of map
int colMap[mapX][mapY];
int objMap[mapX][mapY];

struct mapGrid
{
    Vector2 pos;
    int type;
};

struct mapGrid uiMap[mapX][mapY];

Vector2 mapOffset = {85, 200};

// translate grid to map
Vector2 gridToMap(int x, int y)
{
    return (Vector2){mapOffset.x + (x * xCellSize), mapOffset.y + (y * yCellSize)};
};

bool mapCopy(void)
{
    switch(curMap)
    {
        case demo:
            memcpy(colMap, demoColMap, sizeof(int) * mapX * mapY);
            memcpy(objMap, demoObjMap, sizeof(int) * mapX * mapY);
            break;
    }

    return false;
}
