#include "raylib.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "rlgl.h"
#include "math.h"
#include "time.h"


// definitions

// window size
const int screenWidth = 800;
const int screenHeight = 600;

// setup state machine
static int baseState;
static int hoverState;

enum baseList
{
    splash,
    title,
    game,
    over
};

enum hoverList
{
    help,
    none,
    map,
    battle,
    upgradeState,
    sign
};

// compass
enum compDir
{
    north,
    east,
    south,
    west
};

// timer
static int fpsCounter;
static int secOneCounter;
static int secTenCounter;
static int minOneCounter;
static int minTenCounter;
static bool timeOverload = false;

// player
struct player
{
    int maxhp;
    int hp;
    int regen;
    int speed;
    int accuracy;
    int damage;
    int dir;
    bool charging;
    Vector2 pos;
    char location[11];
};

struct core
{
    int regen;
    int speed;
    int acc;
    int damage;
};

// player struct
struct player player = {100, 100, 0, 0, 0, 0, north, false, {0, 0}, "TEST"};

struct core baseCore = {10, 50, 75, 25};

// map

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

struct mapGrid
{
    Vector2 pos;
    int type;
};

// demo maps
int demoMap[6][6] =
    {
        // x  0  1  2  3  4  5  //y
        {1, 1, 1, 1, 1, 1},  // 0
        {1, 0, 1, 0, 72, 1}, // 1
        {1, 0, 0, 0, 0, 1},  // 2
        {1, 0, 1, 0, 1, 1},  // 3
        {1, 0, 1, 0, 0, 1},  // 4
        {1, 1, 1, 1, 1, 1}}; // 5

int demoObjMap[6][6] =
    {
        {0, 0, 0, 0, 0, 0},
        {0, 3, 0, 0, 0, 0},
        {0, 5, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 2, 0},
        {0, 0, 0, 0, 0, 0}};

// y/x
Vector2 mapSpawn = {4, 1};
Vector2 doorSpawn = {1, 1};
int compDoor = south;
int compSpawn = west;

int testMap[6][6] =
    {
        // x  0  1  2  3  4  5  //y
        {1, 1, 1, 1, 1, 1},  // 0
        {1, 0, 1, 1, 0, 1}, // 1
        {1, 0, 0, 0, 0, 1},  // 2
        {1, 0, 1, 0, 0, 1},  // 3
        {1, 72, 1, 0, 0, 1},  // 4
        {1, 1, 1, 1, 1, 1}}; // 5

int testObjMap[6][6] =
    {
        {0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 2, 0},
        {0, 5, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 3, 0, 0, 4, 0},
        {0, 0, 0, 0, 0, 0}};

Vector2 testSpawn = {1, 4};
Vector2 testSpawn2 = {4, 4};
int compTest = north;

int test2Map[6][6] =
    {
        // x  0  1  2  3  4  5  //y
        {1, 1, 1, 1, 1, 1},  // 0
        {1, 0, 0, 1, 72, 1}, // 1
        {1, 0, 0, 1, 0, 1},  // 2
        {1, 0, 1, 1, 0, 1},  // 3
        {1, 0, 0, 0, 0, 1},  // 4
        {1, 1, 1, 1, 1, 1}}; // 5

int test2ObjMap[6][6] =
    {
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 4, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0}};

Vector2 test2Spawn = {4, 1};
int compTest2 = south;

enum mapList
{
    demo,
    test,
    test2
};

int curMap;

// current map
int rpgMap[6][6];

int objMap[6][6];

// 1 = default wall 99 = nothing 72 = player spawn
int blockers[2] = {1, 99};

// 1 = battery, 2 = upgrade,
int drawers[2] = {1, 2};

struct mapGrid uiMap[6][6];

static const int xCellSize = 51;
static const int yCellSize = 30;

Vector2 mapOffset = {85, 250};

Vector2 gridToMap(int x, int y)
{
    return (Vector2){mapOffset.x + (x * xCellSize), mapOffset.y + (y * yCellSize)};
};

// rotational axis
int rotX[4] = {0, 1, 0, -1};
int rotY[4] = {-1, 0, 1, 0};

bool canMove = true;

// modulo
int mod(int i, int j)
{
    int r = i % j;
    return r < 0 ? r + j : r;
}

// viewport
int maxDepth = 4;
int maxPer = 3;

// max battery
int maxOneSec = 0;
int maxTenSec = 0;
int maxOneMin = 1;
int maxTenMin = 0;

// randomize
int Random(int max)
{
    return (rand() % max) + 1;
}

// enemy

struct enemy
{
    int hp;
    int atk;
    int acc;
};

struct enemy skull = {100, 10, 50};

bool needSpeed;
bool checkFirst;

bool newDrop;
struct core newCore;

bool wait;
bool loot()
{
    if (25 >= Random(100))
    {
        newDrop = true;
    }
    else
    {
        printf(" noget\n");
    }
    return false;
}

int main(void)
{
    // initial copy
    memcpy(rpgMap, demoMap, sizeof(int) * 6 * 6);
    memcpy(objMap, demoObjMap, sizeof(int) * 6 * 6);
    curMap = demo;

    // seed
    srand(time(NULL));

    // initialization

    // debug log
    SetTraceLogLevel(LOG_ERROR);
    // actual window creation, size and name
    InitWindow(screenWidth, screenHeight, "WonderRPG");

    SetTargetFPS(60); // target fps

    // texture init
    // splash (800x600)
    Texture2D splashImage = LoadTexture("assets/splash.png");
    // title (800x600)
    Texture2D titleImage = LoadTexture("assets/title.png");
    // map (800x600)
    Texture2D mapImage = LoadTexture("assets/MAP.png");
    // ui atlas (800x600)
    Texture2D hpAtlas = LoadTexture("assets/hp.png");

    Texture2D compAtlas = LoadTexture("assets/compass.png");

    Texture2D helpImage = LoadTexture("assets/HELP.png");

    Texture2D tileAtlas = LoadTexture("assets/tiles.png");

    Texture2D mapDotImage = LoadTexture("assets/mapdot.png");

    // segment textures
    Texture2D segNFront = LoadTexture("assets/NearFront.png");
    Texture2D segCFront = LoadTexture("assets/CloseFront.png");
    Texture2D segMFront = LoadTexture("assets/MidFront.png");

    Texture2D segNLeft = LoadTexture("assets/NearLeft.png");
    Texture2D segCLeft = LoadTexture("assets/CloseLeft.png");
    Texture2D segMLeft = LoadTexture("assets/MidLeft.png");

    Texture2D segNRight = LoadTexture("assets/NearRight.png");
    Texture2D segCRight = LoadTexture("assets/CloseRight.png");
    Texture2D segMRight = LoadTexture("assets/MidRight.png");

    Texture2D segNFloor = LoadTexture("assets/NearBot.png");
    Texture2D segCFloor = LoadTexture("assets/CloseBot.png");
    Texture2D segMFloor = LoadTexture("assets/MidBot.png");

    Texture2D segNTop = LoadTexture("assets/NearTop.png");
    Texture2D segCTop = LoadTexture("assets/CloseTop.png");
    Texture2D segMTop = LoadTexture("assets/MidTop.png");

    Texture2D objAtlas = LoadTexture("assets/objects.png");

    Texture2D overImage = LoadTexture("assets/gameover.png");

    Texture2D coreImage = LoadTexture("assets/corepanel.png");
    Texture2D upgradeImage = LoadTexture("assets/upgradepanel.png");

    Texture2D mapObjAtlas = LoadTexture("assets/mapobj.png");

    Texture2D signImage = LoadTexture("assets/translate.png");

    // map objects
    Rectangle mapObjRec = {0, 0, mapObjAtlas.width / 3, mapObjAtlas.height};

    // object size 380 x 470
    Rectangle objRec = {0, 0, objAtlas.width / 7, objAtlas.height};
    Vector2 objPos = {200, 200};

    // tile rec
    Rectangle tileRec = {0, 0, tileAtlas.width / 17, tileAtlas.height};

    // ui init
    Rectangle hpRec = {0, 0, hpAtlas.width / 5, hpAtlas.height};
    Vector2 hpPos = {0, 0};

    Rectangle compRec = {0, 0, compAtlas.width / 4, compAtlas.height};
    Vector2 compPos = {0, 0};

    // overworld turns
    int ovTurn = 0;
    int lastovTurn = 0;

    int bTurn = 0;
    int lastbTurn = 0;

    int upTime = 0;
    int maxupTime = 10;

    // close window
    bool exitWindow = false;

    while (!exitWindow)
    {
        if (WindowShouldClose())
        {
            exitWindow = true;
        }
        // update loop

        if (fpsCounter > 60)
        {
            fpsCounter = 0;
        }

        // axis
        // main axis
        int mainX = rotX[player.dir];
        int mainY = rotY[player.dir];
        // side axis
        int altX = rotX[mod((player.dir - 1), 4)];
        int altY = rotY[mod((player.dir - 1), 4)];

        switch (baseState)
        {
        case splash:
            if (fpsCounter == 60)
                baseState = title;
            break;
        case title:
            if (IsKeyReleased(KEY_SPACE) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                baseState = game;

            secOneCounter = maxOneSec;
            secTenCounter = maxTenSec;
            minOneCounter = maxOneMin;
            minTenCounter = maxTenMin;

            player.pos.x = mapSpawn.x;
            player.pos.y = mapSpawn.y;
            player.dir = compSpawn;

            player.hp = player.maxhp;
            timeOverload = false;

            // swap core
            player.regen = baseCore.regen;
            player.speed = baseCore.speed;
            player.accuracy = baseCore.acc;
            player.damage = baseCore.damage;

            break;
        case game:

            if (timeOverload == true)
            {
                player.hp = 0;
            }

            if (player.hp == 0)
            {
                baseState = over;
                break;
            }

            if (newDrop)
            {
                hoverState = upgradeState;
            }

            // sync ovturn
            lastovTurn = ovTurn;

            // inputs
            // b
            switch (hoverState)
            {
            case help:
                if (IsKeyReleased(KEY_ENTER) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
                {
                    if (hoverState == none)
                    {
                        hoverState = map;
                    }
                    else
                    {
                        hoverState = none;
                    }
                }
                break;
            case sign:
                if (IsKeyReleased(KEY_SPACE) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                {
                    hoverState = none;
                }
                break;
            case upgradeState:
                if (newDrop)
                {
                    printf(" dropget\n");
                    printf("current: r:%d s:%d a:%d d:%d\n", player.regen, player.speed, player.accuracy, player.damage);
                    newCore.regen = Random(100);
                    newCore.speed = Random(100);
                    newCore.acc = Random(100);
                    newCore.damage = Random(100);
                    printf("new: r:%d s:%d a:%d d:%d\n", newCore.regen, newCore.speed, newCore.acc, newCore.damage);
                    printf("press a to upgrade or b to keep\n");
                    newDrop = false;
                    wait = true;
                }

                if (wait)
                {
                    if (IsKeyReleased(KEY_SPACE) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                    {
                        player.regen = newCore.regen;
                        player.speed = newCore.speed;
                        player.accuracy = newCore.acc;
                        player.damage = newCore.damage;
                        wait = false;
                        printf("current: r:%d s:%d a:%d d:%d\n", player.regen, player.speed, player.accuracy, player.damage);
                    }
                    if (IsKeyReleased(KEY_ENTER) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
                    {
                        wait = false;
                        printf("current: r:%d s:%d a:%d d:%d\n", player.regen, player.speed, player.accuracy, player.damage);
                    }
                }
                else
                {
                    hoverState = none;
                }
                break;
            case battle:
                // DEBUG BATTLE
                int spRoll;
                bool pFirst;

                if (needSpeed)
                {
                    spRoll = Random(100);
                    printf("roll: %d", spRoll);
                    needSpeed = false;
                }

                if (!checkFirst)
                {
                    if (player.speed > spRoll)
                    {
                        pFirst = true;
                        printf(" outsped");
                    }
                    else
                    {
                        pFirst = false;
                        printf(" slow");
                    }
                    checkFirst = true;
                }

                if (pFirst)
                {
                    lastbTurn = bTurn;

                    if (IsKeyReleased(KEY_SPACE) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                    {
                        printf(" Attack");
                        bTurn++;
                    } else if (IsKeyReleased(KEY_ENTER) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
                    {
                        printf("run\n");
                        hoverState = none;
                    }
                    if (bTurn != lastbTurn)
                    {
                        loot();
                        hoverState = none;
                    }
                }
                else
                {
                    lastbTurn = bTurn;
                    if (IsKeyReleased(KEY_SPACE) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                    {
                        printf(" Counter");
                        bTurn++;
                    } else if (IsKeyReleased(KEY_ENTER) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
                    {
                        printf(" run\n");
                        hoverState = none;
                    }
                    if (bTurn != lastbTurn)
                    {
                        loot();
                        hoverState = none;
                    }
                }

                break;
            case map:
                if (IsKeyReleased(KEY_ENTER) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
                {
                    if (hoverState == none)
                    {
                        hoverState = map;
                    }
                    else
                    {
                        hoverState = none;
                    }
                }
                break;
            case none:

                // object handling
                int ox = player.pos.x;
                int oy = player.pos.y;

                if (IsKeyReleased(KEY_ENTER) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
                {
                    if (hoverState == none)
                    {
                        hoverState = map;
                    }
                    else
                    {
                        hoverState = none;
                    }
                }
                // a (interact)
                if (IsKeyReleased(KEY_SPACE) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                {
                    if (objMap[oy][ox] == 2)
                    {
                        if (upTime == 0)
                        {
                            upTime = maxupTime;
                            newDrop = true;
                            hoverState = upgradeState;
                        }
                        else
                        {
                            printf("cooldown");
                        }
                    } else if (objMap[oy][ox] == 3)
                    {
                        if (curMap == demo)
                        {
                            player.dir = compTest;
                            player.pos.x = testSpawn.x;
                            player.pos.y = testSpawn.y;
                            curMap = test;
                            // copy
                            memcpy(rpgMap, testMap, sizeof(int) * 6 * 6);
                            memcpy(objMap, testObjMap, sizeof(int) * 6 * 6);
                        } else if (curMap == test)
                        {
                            player.dir = compDoor;
                            player.pos.x = doorSpawn.x;
                            player.pos.y = doorSpawn.y;
                            curMap = demo;
                            // copy
                            memcpy(rpgMap, demoMap, sizeof(int) * 6 * 6);
                            memcpy(objMap, demoObjMap, sizeof(int) * 6 * 6);                            
                        }

                    } else if (objMap[oy][ox] == 4)
                    {
                        if (curMap == demo)
                        {

                        } else if (curMap == test)
                        {
                            player.dir = compTest2;
                            player.pos.x = test2Spawn.x;
                            player.pos.y = test2Spawn.y;
                            curMap = test2;
                            // copy
                            memcpy(rpgMap, test2Map, sizeof(int) * 6 * 6);
                            memcpy(objMap, test2ObjMap, sizeof(int) * 6 * 6);                            
                        } else if (curMap == test2)
                        {
                            player.dir = compTest;
                            player.pos.x = testSpawn2.x;
                            player.pos.y = testSpawn2.y;
                            curMap = test;
                            // copy
                            memcpy(rpgMap, testMap, sizeof(int) * 6 * 6);
                            memcpy(objMap, testObjMap, sizeof(int) * 6 * 6);                            
                        }

                    } else if (objMap[oy][ox] == 5)
                    {
                        hoverState = sign;
                    }
                }

                // forward
                if (IsKeyReleased(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
                {

                    int xMove = player.pos.x + rotX[player.dir];
                    int yMove = player.pos.y + rotY[player.dir];

                    // temp collision

                    for (int i = 0; i < 2; i++)
                    {
                        if (rpgMap[yMove][xMove] == blockers[i])
                        {
                            canMove = false;
                            break;
                        }
                        else
                        {
                            canMove = true;
                        }
                    };

                    // check out of bounds
                    if (xMove > 5 || xMove < 0 || yMove > 5 || yMove < 0)
                    {
                        canMove = false;
                    }

                    if (canMove)
                    {
                        player.pos.x = xMove;
                        player.pos.y = yMove;
                        ovTurn++;
                    }
                }
                // back
                if (IsKeyReleased(KEY_DOWN) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
                {
                    int backdir;

                    if (player.dir == north)
                    {
                        backdir = south;
                    }
                    else if (player.dir == east)
                    {
                        backdir = west;
                    }
                    else if (player.dir == south)
                    {
                        backdir = north;
                    }
                    else
                    {
                        backdir = east;
                    }

                    int xMove = player.pos.x + rotX[backdir];
                    int yMove = player.pos.y + rotY[backdir];

                    // temp collision

                    for (int i = 0; i < 2; i++)
                    {
                        if (rpgMap[yMove][xMove] == blockers[i])
                        {
                            canMove = false;
                            break;
                        }
                        else
                        {
                            canMove = true;
                        }
                    };

                    // check out of bounds
                    if (xMove > 5 || xMove < 0 || yMove > 5 || yMove < 0)
                    {
                        canMove = false;
                    }

                    if (canMove)
                    {
                        player.pos.x = xMove;
                        player.pos.y = yMove;
                        ovTurn++;
                    }
                }
                // left
                if (IsKeyReleased(KEY_LEFT) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
                {
                    if (player.dir > 0)
                    {
                        player.dir--;
                    }
                    else
                    {
                        player.dir = 3;
                    }
                }
                // right
                if (IsKeyReleased(KEY_RIGHT) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
                {
                    if (player.dir < 3)
                    {
                        player.dir++;
                    }
                    else
                    {
                        player.dir = 0;
                    }
                }
                break;
            }

            if (ovTurn != lastovTurn)
            {
                // player heal
                if (player.hp < player.maxhp)
                {
                    player.hp += player.regen;
                }

                if (player.hp > player.maxhp)
                {
                    player.hp = player.maxhp;
                }

                // object handling
                int ox = player.pos.x;
                int oy = player.pos.y;

                // check for battle
                if (objMap[oy][ox] == 0)
                {
                    int chance = Random(100);
                    if (chance <= 25)
                    {
                        bTurn = 0;
                        lastbTurn = 0;
                        checkFirst = false;
                        needSpeed = true;
                        hoverState = battle;
                    }
                }

                // upgrade timer
                if (upTime != 0)
                {
                    upTime--;
                }
            }

            // object handling
            int ox = player.pos.x;
            int oy = player.pos.y;

            // charging
            if (objMap[oy][ox] == 1)
            {
                player.charging = true;
            }
            else
            {
                player.charging = false;
            }

            // updates
            if (hoverState == none)
            {
                if (!player.charging)
                {
                    if (fpsCounter == 60)
                    {
                        // update timer
                        if (minTenCounter == 0 && minOneCounter == 0 && secOneCounter == 0 && secTenCounter == 0)
                        {
                            timeOverload = true;
                        }
                        else
                        {
                            if (minTenCounter >= 0)
                            {
                                if (minOneCounter >= 0)
                                {
                                    if (secTenCounter >= 0)
                                    {
                                        if (secOneCounter > 0)
                                        {
                                            secOneCounter--;
                                        }
                                        else
                                        {
                                            secOneCounter = 9;
                                            if (secTenCounter != 0)
                                            {
                                                secTenCounter--;
                                            }
                                            else
                                            {
                                                if (minOneCounter != 0)
                                                {
                                                    minOneCounter--;
                                                }
                                                else
                                                {
                                                    minOneCounter = 9;
                                                    if (minTenCounter != 0)
                                                    {
                                                        minTenCounter--;
                                                    }
                                                }
                                                secTenCounter = 5;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    secOneCounter = maxOneSec;
                    secTenCounter = maxTenSec;
                    minOneCounter = maxOneMin;
                    minTenCounter = maxTenMin;
                }
            }

            // update health
            if (player.hp >= 50)
            {
                if (fpsCounter >= 30)
                {
                    hpRec.x = hpAtlas.width / 5 * 0;
                }
                else
                {
                    hpRec.x = hpAtlas.width / 5 * 1;
                }
            }
            else if (player.hp <= 0)
            {
                hpRec.x = hpAtlas.width / 5 * 4;
            }
            else
            {
                if (fpsCounter <= 30)
                {
                    hpRec.x = hpAtlas.width / 5 * 2;
                }
                else
                {
                    hpRec.x = hpAtlas.width / 5 * 3;
                }
            }
            // update compass
            switch (player.dir)
            {
            case north:
                compRec.x = compAtlas.width / 4 * 0;
                break;
            case east:
                compRec.x = compAtlas.width / 4 * 1;
                break;
            case south:
                compRec.x = compAtlas.width / 4 * 2;
                break;
            case west:
                compRec.x = compAtlas.width / 4 * 3;
                break;
            }
            break;
        case over:
            if (IsKeyReleased(KEY_SPACE) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || WindowShouldClose())
            {
                baseState = splash;
            }
            else if (IsKeyReleased(KEY_ENTER) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
            {
                exitWindow = true;
                break;
            }
            break;
        }

        // draw loop
        BeginDrawing();
        ClearBackground(BLACK);

        // draw screen
        switch (baseState)
        {
        case splash:
            DrawTexture(splashImage, 0, 0, WHITE);
            break;
        case title:
            DrawTexture(titleImage, 0, 0, WHITE);
            DrawText("PRESS SPACE / A TO PLAY", 230, 280, 20, WHITE);
            break;
        case over:
            DrawTexture(overImage, 0, 0, WHITE);
            DrawText("PRESS SPACE / A TO RESTART", 290, 500, 30, RED);
            DrawText("PRESS ENTER / B TO EXIT", 350, 450, 30, RED);
            break;
        case game:;

            // draw viewport
            int depth = maxDepth;

            // steps forward
            int stepsForward = -1;

            for (int d = 0; d < depth + 1; d++)
            {
                // xy offset
                int x = player.pos.x + (mainX * d);
                int y = player.pos.y + (mainY * d);

                int cell = rpgMap[y][x];
                // check for blocker
                for (int i = 0; i < 2; i++)
                {
                    if (cell == blockers[i])
                    {
                        stepsForward++;
                        break;
                    }
                }

                stepsForward++;
            }

            DrawTexture(segMFloor, 0, 0, WHITE);
            DrawTexture(segCFloor, 0, 0, WHITE);
            DrawTexture(segNFloor, 0, 0, WHITE);
            DrawTexture(segMTop, 0, 0, WHITE);
            DrawTexture(segCTop, 0, 0, WHITE);
            DrawTexture(segNTop, 0, 0, WHITE);

            // test
            for (int d = 4; d-- > 0;)
            {
                for (int p = -1; p < 2; p += 2)
                {
                    int sx = player.pos.x + (mainX * d) + (altX * p);
                    int sy = player.pos.y + (mainY * d) + (altY * p);

                    int sCell = rpgMap[sy][sx];

                    if (p == -1)
                    {
                        // right
                        // far
                        if (d == 2)
                        {
                            if (sCell == 1)
                            {
                                DrawTexture(segMRight, 0, 0, WHITE);
                            }
                            // close
                        }
                        else if (d == 1)
                        {
                            if (sCell == 1)
                            {
                                DrawTexture(segCRight, 0, 0, WHITE);
                            }
                            // near
                        }
                        else if (d == 0)
                        {
                            if (sCell == 1)
                            {
                                DrawTexture(segNRight, 0, 0, WHITE);
                            }
                        }
                    }
                    else
                    {
                        // left
                        if (d == 2)
                        {
                            if (sCell == 1)
                            {
                                DrawTexture(segMLeft, 0, 0, WHITE);
                            }
                            // close
                        }
                        if (d == 1)
                        {
                            if (sCell == 1)
                            {
                                DrawTexture(segCLeft, 0, 0, WHITE);
                            }
                            // near
                        }
                        if (d == 0)
                        {
                            if (sCell == 1)
                            {
                                DrawTexture(segNLeft, 0, 0, WHITE);
                            }
                        }
                    }
                }

                int fx = player.pos.x + (mainX * d);
                int fy = player.pos.y + (mainY * d);

                int faceCell = rpgMap[fy][fx];

                // face
                // far

                if (d == 3)
                {
                    if (faceCell == 1)
                    {
                        DrawTexture(segMFront, 0, 0, WHITE);
                    }
                    // med
                }
                if (d == 2)
                {
                    if (faceCell == 1)
                    {
                        DrawTexture(segCFront, 0, 0, WHITE);
                    }
                    // close
                }
                if (d == 1)
                {
                    if (faceCell == 1)
                    {
                        DrawTexture(segNFront, 0, 0, WHITE);
                    }
                }
            }

            // draw sprite for object

            // update object check coords
            int ox = player.pos.x;
            int oy = player.pos.y;

            if (objMap[oy][ox] == 0)
            {
                objRec.x = objAtlas.width / 7 * 0;
                DrawTextureRec(objAtlas, objRec, objPos, WHITE);
            }
            else if (objMap[oy][ox] == 1)
            {
                objRec.x = objAtlas.width / 7 * 1;
                DrawTextureRec(objAtlas, objRec, objPos, WHITE);
            }
            else if (objMap[oy][ox] == 2)
            {
                objRec.x = objAtlas.width / 7 * 3;
                DrawTextureRec(objAtlas, objRec, objPos, WHITE);
                if (upTime != 0)
                {
                    DrawText("COOLDOWN", 370, 475, 30, RED);
                }
            } else if (objMap[oy][ox] == 3)
            {
                objRec.x = objAtlas.width / 7 * 4;
                DrawTextureRec(objAtlas, objRec, objPos, WHITE);
            } else if (objMap[oy][ox] == 4)
            {
                objRec.x = objAtlas.width / 7 * 4;
                DrawTextureRec(objAtlas, objRec, objPos, WHITE);
            } else if (objMap[oy][ox] == 5)
            {
                objRec.x = objAtlas.width / 7 * 5;
                DrawTextureRec(objAtlas, objRec, objPos, WHITE);
            } 

            // draw hp
            DrawTextureRec(hpAtlas, hpRec, hpPos, WHITE);
            DrawText(TextFormat("%i", player.hp), 15, 10, 30, RED);

            // draw core
            DrawTexture(coreImage, 0, 0, WHITE);
            DrawText(TextFormat("R:%d S:%d A:%d D:%d", player.regen, player.speed, player.accuracy, player.damage), 585, 10, 20, WHITE);

            // hover state
            switch (hoverState)
            {
            case none:
                break;
            case help:
                DrawTexture(helpImage, 0, 0, WHITE);
                break;
            case sign:

                DrawTexture(signImage, 0, 0, WHITE);

                if (curMap == demo)
                {
                    DrawText("THIS IS A SIGN", 70, 300, 50, WHITE);
                } else if (curMap == test)
                {
                    DrawText("DIFFERENT SIGN", 70, 300, 50, WHITE);
                }
                break;
            case upgradeState:
                DrawTexture(upgradeImage, 0, 0, WHITE);

                DrawText(TextFormat("%d", player.regen), 230, 230, 30, WHITE);
                DrawText(TextFormat("%d", player.speed), 230, 290, 30, WHITE);
                DrawText(TextFormat("%d", player.accuracy), 230, 340, 30, WHITE);
                DrawText(TextFormat("%d", player.damage), 230, 400, 30, WHITE);

                DrawText(TextFormat("%d", newCore.regen), 560, 230, 30, WHITE);
                DrawText(TextFormat("%d", newCore.speed), 560, 290, 30, WHITE);
                DrawText(TextFormat("%d", newCore.acc), 560, 340, 30, WHITE);
                DrawText(TextFormat("%d", newCore.damage), 560, 400, 30, WHITE);

                DrawText("SPACE/A UPG ENTER/B KEEP", 160, 480, 30, WHITE);
                break;
            case battle:
                objRec.x = objAtlas.width / 7 * 2;
                DrawTextureRec(objAtlas, objRec, objPos, WHITE);
                break;
            case map:
                DrawTexture(mapImage, 0, 0, WHITE);
                DrawTextureRec(compAtlas, compRec, compPos, WHITE);
                DrawText(TextFormat("%s", player.location), 280, 80, 60, WHITE);

                // draw map
                for (int i = 0; i < 6; i++)
                {
                    for (int j = 0; j < 6; j++)
                    {
                        int cellType = rpgMap[j][i];
                        int tileType;
                        bool nEnt = true;
                        bool eEnt = true;
                        bool sEnt = true;
                        bool wEnt = true;

                        for (int k = 0; k < 4; k++)
                        {
                            int tempX = i + rotX[k];
                            int tempY = j + rotY[k];

                            for (int m = 0; m < 2; m++)
                            {
                                if (rpgMap[tempY][tempX] == blockers[m])
                                {
                                    if (k == 0)
                                    {
                                        nEnt = false;
                                    }
                                    else if (k == 1)
                                    {
                                        eEnt = false;
                                    }
                                    else if (k == 2)
                                    {
                                        sEnt = false;
                                    }
                                    else
                                    {
                                        wEnt = false;
                                    }
                                }
                            }
                        }

                        if (nEnt && eEnt && sEnt && wEnt)
                        {
                            tileType = empty;
                        }
                        else if (nEnt && eEnt && sEnt)
                        {
                            tileType = left;
                        }
                        else if (nEnt && eEnt && wEnt)
                        {
                            tileType = bot;
                        }
                        else if (nEnt && sEnt && wEnt)
                        {
                            tileType = right;
                        }
                        else if (sEnt && wEnt && eEnt)
                        {
                            tileType = top;
                        }
                        else if (nEnt && eEnt)
                        {
                            tileType = botleft;
                        }
                        else if (nEnt && wEnt)
                        {
                            tileType = botright;
                        }
                        else if (sEnt && eEnt)
                        {
                            tileType = topleft;
                        }
                        else if (sEnt && wEnt)
                        {
                            tileType = topright;
                        }
                        else if (wEnt && eEnt)
                        {
                            tileType = horz;
                        }
                        else if (nEnt && sEnt)
                        {
                            tileType = vert;
                        }
                        else if (nEnt)
                        {
                            tileType = closebot;
                        }
                        else if (eEnt)
                        {
                            tileType = closeleft;
                        }
                        else if (sEnt)
                        {
                            tileType = closetop;
                        }
                        else if (wEnt)
                        {
                            tileType = closeright;
                        }

                        if (cellType == 1)
                        {
                            tileType = block;
                        }
                        if (cellType == 99)
                        {
                            tileType = nothing;
                        }

                        tileRec.x = tileAtlas.width / 17 * tileType;
                        DrawTextureRec(tileAtlas, tileRec, gridToMap(i, j), WHITE);
                        if (fpsCounter > 10)
                        {
                            DrawTextureV(mapDotImage, gridToMap(player.pos.x, player.pos.y), WHITE);
                        }

                        // draw objects
                        if (objMap[j][i] == 1)
                        {
                            mapObjRec.x = mapObjAtlas.width / 3 * 0;
                            DrawTextureRec(mapObjAtlas, mapObjRec, gridToMap(i, j), WHITE);
                        }
                        else if (objMap[j][i] == 2)
                        {
                            mapObjRec.x = mapObjAtlas.width / 3 * 1;
                            DrawTextureRec(mapObjAtlas, mapObjRec, gridToMap(i, j), WHITE);
                        }
                        else if (objMap[j][i] == 3)
                        {
                            mapObjRec.x = mapObjAtlas.width / 3 * 2;
                            DrawTextureRec(mapObjAtlas, mapObjRec, gridToMap(i, j), WHITE);
                        } else if (objMap[j][i] == 4)
                        {
                            mapObjRec.x = mapObjAtlas.width / 3 * 2;
                            DrawTextureRec(mapObjAtlas, mapObjRec, gridToMap(i, j), WHITE);
                        }
                    }
                }
                break;
            }

            // timer
            if (!timeOverload)
            {
                if (player.charging)
                {
                    DrawText(TextFormat("%i%i:%i%i", minTenCounter, minOneCounter, secTenCounter, secOneCounter), 360, 0, 40, YELLOW);
                }
                else if (minTenCounter <= 0 && minOneCounter <= 0)
                {
                    DrawText(TextFormat("%i%i:%i%i", minTenCounter, minOneCounter, secTenCounter, secOneCounter), 360, 0, 40, RED);
                }
                else
                {
                    DrawText(TextFormat("%i%i:%i%i", minTenCounter, minOneCounter, secTenCounter, secOneCounter), 360, 0, 40, WHITE);
                }
            }
            else
            {
                DrawText("XX:XX", 360, 0, 40, RED);
            }
            break;
        }

        EndDrawing();

        fpsCounter++;
    }

    // unload textures
    UnloadTexture(hpAtlas);
    UnloadTexture(compAtlas);
    UnloadTexture(tileAtlas);
    UnloadTexture(splashImage);
    UnloadTexture(mapDotImage);
    UnloadTexture(titleImage);
    UnloadTexture(helpImage);
    UnloadTexture(mapImage);

    // De initialization
    CloseWindow();

    return 0;
}