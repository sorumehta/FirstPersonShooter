#include "SimpleGameEngine.h"
#include <cmath>
#include <list>

class FirstPerson : public GameEngine {
private:
    float fPlayerX = 16.0f;
    float fPlayerY = 16.0f;
    float fPlayerA = 0.0f;
    int nMapHeight = 32;
    int nMapWidth = 32;
    float fFOV = 3.14159 / 4.0;
    float fDepth = 16.0f;
    std::string map;

public:
    bool onInit() override {
        map += "#########.......#########.......";
        map += "#...............#...............";
        map += "#.......#########.......########";
        map += "#..............##..............#";
        map += "#......##......##......##......#";
        map += "#......##..............##......#";
        map += "#..............##..............#";
        map += "###............####............#";
        map += "##.............###.............#";
        map += "#............####............###";
        map += "#..............................#";
        map += "#..............##..............#";
        map += "#..............##..............#";
        map += "#...........#####...........####";
        map += "#..............................#";
        map += "###..####....########....#######";
        map += "####.####.......######..........";
        map += "#...............#...............";
        map += "#.......#########.......##..####";
        map += "#..............##..............#";
        map += "#......##......##.......#......#";
        map += "#......##......##......##......#";
        map += "#..............##..............#";
        map += "###............####............#";
        map += "##.............###.............#";
        map += "#............####............###";
        map += "#..............................#";
        map += "#..............................#";
        map += "#..............##..............#";
        map += "#...........##..............####";
        map += "#..............##..............#";
        map += "################################";
        return true;
    }

    bool onFrameUpdate(float fElapsedTime) override {
        for (int x=0; x < mWindowWidth; x++){
            // For each column, calculate the projected ray angle into world space
            // if fov=90, we go from playerA - 45 to playerA + 45
            float fRayAngle = (fPlayerA - fFOV/2.0f) + ((float)x / (float)mWindowWidth) * fFOV;
            float fDistanceToWall = 0.0f;
            // for a give theta, sin and cos theta give unit vector (derived from circle geometry)
            float fEyeX = std::sinf(fRayAngle);
            float fEyeY = std::cosf(fRayAngle);
            bool bHitWall = false;
            while(!bHitWall && fDistanceToWall < fDepth){
                fDistanceToWall += 0.1f;
                // now that we have a unit vector in direction of theta, we can create a line of any distance
                // assume the boundaries of the wall are always integers
                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                if (nTestX < 0 || nTestX >= mWindowWidth || nTestY < 0 || nTestY >= mWindowHeight){
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                } else {
                    // we are within bounds of our map
                    // check the map for test position. Here y is rows and x is columns (2d coordinates)
                    if(map[nTestY * nMapWidth + nTestX] == 'X'){
                        // we have hit a wall
                        bHitWall = true;
                    }
                }
            }
            // calculate ceiling and floor positions.
            // ceiling would be position would be middle of screen minus height of the wall
            int nCeiling = (float)(mWindowHeight/2) - (mWindowHeight/(float)fDistanceToWall);
            // and floor would be mirror of the ceiling
            int nFloor = mWindowHeight - nCeiling;

            // draw ceiling, wall and floor
            for (int y=0; y < mWindowHeight; y++){
                if (y < nCeiling){
                    drawPoint(x, y, {0, 0,0 });
                } else if(y > nCeiling && y < nFloor){
                    drawPoint(x, y, {0xFF, 0xFF, 0xFF});
                } else{
                    drawPoint(x, y, {0, 0, 0});
                }
            }
        }

        return true;
    }


};

int main() {
    FirstPerson fps;
    fps.constructConsole(640, 480, "First Person Shooter");
    fps.startGameLoop();
    return 0;
}
