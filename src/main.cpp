#include "SimpleGameEngine.h"
#include <cmath>
#include <list>

class FirstPerson : public GameEngine {
private:
    float fPlayerX = 4.0f;
    float fPlayerY = 4.0f;
    float fPlayerA = 0.0f;
    int nMapHeight = 32;
    int nMapWidth = 32;
    float fFOV = 3.14159 / 4.0;
    float fDepth = 32.0f;
    std::string map;

public:
    float clamp(float value, float min_value, float max_value) {
        return std::max(min_value, std::min(value, max_value));
    }

    Color distanceToRGB(float dot_product, float min_dot_product, float max_dot_product) {
        // Clamp the dot product value between min_dot_product and max_dot_product
        dot_product = clamp(dot_product, min_dot_product, max_dot_product);

        // Normalize the dot product value to a range of 0 to 1
        float normalized_dot_product = (dot_product - min_dot_product) / (max_dot_product - min_dot_product);

        // Convert the normalized dot product value to an 8-bit integer value
        uint8_t color_value = static_cast<uint8_t>((1-normalized_dot_product) * 255);

        // Set the RGB color channels
        Color color;
        color.r = color_value;
        color.g = color_value;
        color.b = color_value;

        return color;
    }
    void onUserInputEvent(int eventType, int button, int mousePosX, int mousePosY, float secPerFrame) {

        if (eventType == SDL_KEYDOWN) {
            if(button == SDLK_a){
                fPlayerA -= 0.6f * secPerFrame;
            } else if(button == SDLK_d){
                fPlayerA += 0.6f * secPerFrame;
            } else if(button == SDLK_w){
                fPlayerX += std::sinf(fPlayerA) * 5.0f * secPerFrame;
                fPlayerY += std::cosf(fPlayerA) * 5.0f * secPerFrame;
                if(map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#'){
                    fPlayerX -= std::sinf(fPlayerA) * 5.0f * secPerFrame;
                    fPlayerY -= std::cosf(fPlayerA) * 5.0f * secPerFrame;
                }
            } else if(button == SDLK_s){
                fPlayerX -= std::sinf(fPlayerA) * 5.0f * secPerFrame;
                fPlayerY -= std::cosf(fPlayerA) * 5.0f * secPerFrame;
                if(map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#'){
                    fPlayerX += std::sinf(fPlayerA) * 5.0f * secPerFrame;
                    fPlayerY += std::cosf(fPlayerA) * 5.0f * secPerFrame;
                }
            }
        }
    }
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

        auto onUserInputFn = [this](int eventType, int buttonCode, int mousePosX, int mousePosY, float secPerFrame) {
            onUserInputEvent(eventType, buttonCode, mousePosX, mousePosY, secPerFrame);
        };
        InputEventHandler::addCallback("onUserInputFn_Game", onUserInputFn);
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
                    if(map[nTestY * nMapWidth + nTestX] == '#'){
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
            Color shade = distanceToRGB(fDistanceToWall, 0, fDepth);
            // draw ceiling, wall and floor
            for (int y=0; y < mWindowHeight; y++){
                if (y < nCeiling){
                    drawPoint(x, y, {0, 0,0 });
                } else if(y > nCeiling && y < nFloor){
                    drawPoint(x, y, shade);
                } else{
                    // lower the value of y, higher the distance
                    float fFloorDistance = 1.0f - (((float)y- mWindowHeight/2.0f)/((float)mWindowHeight/2.0f));
                    Color floorShade = distanceToRGB(fFloorDistance, 0, 1);
                    drawPoint(x, y, floorShade);
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
