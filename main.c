/*
    Wonder Dungeon is an old school grid based Dungeon crawler

    it aims to be customizable, allowing the creation of custom maps,
    stats and items, and enemies

*/

// includes

// raylib libs
#include <raylib.h>
#include <rlgl.h>

// standard libs
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// private libraries
#include "src/switch.h"
#include "src/mapHandler.h"
#include "src/timer.h"

// defines
#define screenWidth 800
#define screenHeight 600

#define maxFps 60

// player
struct player
{
    int dir;
    Vector2 pos;
};

struct player player = {};

// compass
enum dir
{
    north,
    east,
    south,
    west
};

// rotational axis
int rotX[4] = {0, 1, 0, -1};
int rotY[4] = {-1, 0, 1, 0};

// overworld turn counter
struct OT
{
    int current;
    int last;
};

struct OT oTurn = {};

bool newGame(void)
{
    baseState = game;
    hoverState = help;
    curMap = spawnMap;
    mapCopy();
    resetTime();

    return false;
}

bool gameOver(void)
{
    baseState = over;
    return false;
}

// modulo
int modulo(int i, int j)
{
    int r = i % j;
    return r < 0 ? r + j : r;
}

int main(void)
{
    // init

    // default states
    baseState = splash;
    hoverState = none;

    // resize window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // debug log
    SetTraceLogLevel(LOG_ERROR);

    // init window
    InitWindow(screenWidth, screenHeight, "WonderRPG");

    // fps
    SetTargetFPS(maxFps);

    // textures

    // static images
    Texture2D splashImage = LoadTexture("assets/splash.png");
    Texture2D titleImage = LoadTexture("assets/title.png");
    Texture2D overImage = LoadTexture("assets/gameover.png");
    
    // ui elements
    Texture2D mapUI = LoadTexture("assets/MAP.png");
    Texture2D helpUI = LoadTexture("assets/HELP.png");
    Texture2D hpAtlas = LoadTexture("assets/hp.png");
    Texture2D signUI = LoadTexture("assets/translate.png");
    Texture2D compAtlas = LoadTexture("assets/compass.png");

    // sprites
    Texture2D mapDotImage = LoadTexture("assets/mapdot.png");
    Texture2D objAtlas = LoadTexture("assets/objects.png");
    Texture2D mapObjAtlas = LoadTexture("assets/mapobj.png");
    Texture2D tileAtlas = LoadTexture("assets/tiles.png");

    // viewport segments
    // near, close, mid (player, player+1, player+2)
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

    // rectangles
    // atlas pointers
    Rectangle mapObjRec = {0, 0, mapObjAtlas.width / 3, mapObjAtlas.height};

    // object size 380 x 470
    //Rectangle objRec = {0, 0, objAtlas.width / 7, objAtlas.height};
    //Vector2 objPos = {200, 200};

    // tile rec
    Rectangle tileRec = {0, 0, tileAtlas.width / 17, tileAtlas.height};

    // ui init
    //Rectangle hpRec = {0, 0, hpAtlas.width / 5, hpAtlas.height};
    //Vector2 hpPos = {0, 0};

    Rectangle compRec = {0, 0, compAtlas.width / 4, compAtlas.height};
    Vector2 compPos = {0, 0};

    // window flag
    bool exitWindow = false;

    while (!exitWindow)
    {
        // close window
        if (WindowShouldClose())
        {
            exitWindow = true;
        }

        // update loop

        // baseState updates
        switch (baseState)
        {
        case splash:
            StopWatch(maxFps, 1, &splashTimer);
            break;
        case title:

            if (IsKeyReleased(KEY_SPACE) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
            {
                newGame();
            }
            break;
        case game:

            // sync turn
            oTurn.last = oTurn.current;

            if (lastMap != curMap)
            {
                mapCopy();
            }

            // hoverState updates
            switch (hoverState)
            {
            case help:
                clearHover();
                break;
            case none:
                if (IsKeyReleased(KEY_ENTER) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
                {
                    hoverState = map;
                }
                break;
            case map:
                switch (player.dir)
                {
                case north:
                    compRec.x = compAtlas.width / 4 * north;
                    break;
                case east:
                    compRec.x = compAtlas.width / 4 * east;
                    break;
                case south:
                    compRec.x = compAtlas.width / 4 * south;
                    break;
                case west:
                    compRec.x = compAtlas.width / 4 * west;
                    break;
                }
                    break;
                clearHover();
                break;
            case sign:
                clearHover();
                break;
            }
            break;
        }

        // increment fps and count time
        CountUp(maxFps);

        // draw loop
        BeginDrawing();

        // draw states
        switch (baseState)
        {
        case splash:
            DrawTexture(splashImage, 0, 0, WHITE);
            break;
        case title:
            DrawTexture(titleImage, 0, 0, WHITE);
            // display underneath title
            DrawText("PRESS SPACE / A TO PLAY", 230, 260, 20, WHITE);
            break;
        case game:

            // hoverState updates
            switch (hoverState)
            {
            case help:
                DrawTexture(helpUI, 0, 0, WHITE);
                break;
            case sign:
                DrawTexture(signUI, 0, 0, WHITE);
                break;
            case map:
                DrawTexture(mapUI, 0, 0, WHITE);
                DrawTextureRec(compAtlas, compRec, compPos, WHITE);
                
                // draw map
                for (int i = 0; i < mapY; i++)
                {
                    for (int j = 0; j < mapX; j++)
                    {
                        int cellType = colMap[j][i];
                        int tileType;
                        bool nEnt = true;
                        bool eEnt = true;
                        bool sEnt = true;
                        bool wEnt = true;

                        for (int k = 0; k < 4; k++)
                        {
                            int tempX = i + rotX[k];
                            int tempY = j + rotY[k];

                            for (int m = 0; m < blockersSize; m++)
                            {
                                if (colMap[tempY][tempX] == blockers[m])
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

                        if (cellType == tileWall)
                        {
                            tileType = block;
                        }
                        if (cellType == tileNothing)
                        {
                            tileType = nothing;
                        }

                        tileRec.x = tileAtlas.width / 17 * tileType;
                        DrawTextureRec(tileAtlas, tileRec, gridToMap(i, j), WHITE);
                        if (timer.fps > 10)
                        {
                            DrawTextureV(mapDotImage, gridToMap(player.pos.x, player.pos.y), WHITE);
                        }

                        // draw objects
                        if (objMap[j][i] == tileHealth)
                        {
                            mapObjRec.x = mapObjAtlas.width / 3 * mapHealth;
                            DrawTextureRec(mapObjAtlas, mapObjRec, gridToMap(i, j), WHITE);
                        }
                        else if (objMap[j][i] == tileShop)
                        {
                            mapObjRec.x = mapObjAtlas.width / 3 * mapShop;
                            DrawTextureRec(mapObjAtlas, mapObjRec, gridToMap(i, j), WHITE);
                        }
                        else if (objMap[j][i] == tileDoor)
                        {
                            mapObjRec.x = mapObjAtlas.width / 3 * mapDoor;
                            DrawTextureRec(mapObjAtlas, mapObjRec, gridToMap(i, j), WHITE);
                        }
                    }
                }
                break;
            }
            break;
        case over:
            DrawTexture(overImage, 0, 0, WHITE);
            DrawText("PRESS SPACE / A TO RESTART", 290, 500, 30, RED);
            DrawText("PRESS ENTER / B TO EXIT", 350, 450, 30, RED);
            break;
        }

        // background colour
        ClearBackground(BLACK);

        EndDrawing();
    }

    // unload textures
    UnloadTexture(splashImage);
    UnloadTexture(titleImage);
    UnloadTexture(overImage);
    UnloadTexture(signUI);
    UnloadTexture(mapUI);
    UnloadTexture(helpUI);
    UnloadTexture(hpAtlas);
    UnloadTexture(compAtlas);
    UnloadTexture(mapDotImage);
    UnloadTexture(objAtlas);
    UnloadTexture(mapObjAtlas);
    UnloadTexture(segNFront);
    UnloadTexture(segCFront);
    UnloadTexture(segMFront);
    UnloadTexture(segNLeft);
    UnloadTexture(segCLeft);
    UnloadTexture(segMLeft);
    UnloadTexture(segNRight);
    UnloadTexture(segCRight);
    UnloadTexture(segMRight);
    UnloadTexture(segNFloor);
    UnloadTexture(segCFloor);
    UnloadTexture(segMFloor);
    UnloadTexture(segNTop);
    UnloadTexture(segCTop);
    UnloadTexture(segMTop);

    // de init
    CloseWindow();

    return 0;
}