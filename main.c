#include <stdio.h>
#include <math.h>
#include <SDL.h>

typedef struct { int x; int y; int z; } place;

float Q_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*)&y;                       // evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);               // what the fuck? 
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration
    //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}

int main(int argv, char** args)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Rottating Cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_bool isRunning = SDL_TRUE;
    SDL_Event event;


    place p = { .x = 0, .y = 400, .z = 0 };
    float velocity = .1;


    int z = 100 / sqrt(2);
    place points[8] = {
        {.x = 100, .y = 100, .z = -z },
        {.x = 100, .y = 200, .z = -z },
        {.x = 200, .y = 200, .z = -z },
        {.x = 200, .y = 100, .z = -z },
        {.x = 100, .y = 100, .z = z},
        {.x = 100, .y = 200, .z = z},
        {.x = 200, .y = 200, .z = z},
        {.x = 200, .y = 100, .z = z},
    };
    SDL_Point pointsInWin[8];
    int pointsNumber = 8;
    int connections[12][2] = {
        { 0, 1 },
        { 0, 3},
        { 0, 4},
        { 1, 2},
        { 1, 5},
        { 2, 3},
        { 2, 6},
        { 3, 7},
        { 4, 7},
        { 4, 5},
        { 5, 6},
        { 6, 7},
    };
    int connectionsNumber = 12;
    float a = 0;


    float windowDistance = 100;

    SDL_bool moveX = SDL_FALSE;
    SDL_bool moveY = SDL_FALSE;
    SDL_bool moveZ = SDL_FALSE;

    float toMoveX = 0;
    float toMoveY = 0;
    float toMoveZ = 0;

    float inMoveX = 0;
    float inMoveY = 0;
    float inMoveZ = 0;

    while (isRunning)
    {
        a += 0.0001f;
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        for (int i = 0; i < pointsNumber; i++)
        {
            float toRotateX = i % 4 * M_PI / 2;
            float angleX = a + toRotateX;

            float radius = 100;

            points[i].x = cos(angleX) * radius;
            points[i].y = sin(angleX) * radius;

            float distanceY = (points[i].y - p.y);
            float distanceX = (points[i].x - p.x);
            float distanceZ = (points[i].z - p.z);

            float inVisionX; 
            float inVisionY;

            if (sqrt(distanceY * distanceY + distanceX * distanceX) > 0 && sqrt(distanceY * distanceY + distanceZ * distanceZ) > 0) {
                inVisionX = (points[i].x - p.x) * windowDistance * Q_rsqrt(distanceY * distanceY + distanceX * distanceX) * 10 + 400;
                inVisionY = (points[i].z - p.z) * windowDistance * Q_rsqrt(distanceY * distanceY + distanceZ * distanceZ) * 10 + 300;

                pointsInWin[i].x = inVisionX;
                pointsInWin[i].y = inVisionY;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < connectionsNumber; i++)
        {
            SDL_Point begin = pointsInWin[connections[i][0]];
            SDL_Point end = pointsInWin[connections[i][1]];
            if (begin.x && end.x)
            {
                SDL_RenderDrawLine(renderer, begin.x, begin.y, end.x, end.y);
            }
        }

        SDL_RenderPresent(renderer);

        if (moveX) inMoveX += toMoveX;
        if (moveY) inMoveY += toMoveY;
        if (moveZ) inMoveZ += toMoveZ;

        if (abs(inMoveX) >= 1)
        {
            p.x += inMoveX;
            inMoveX = 0;
        }
        if (abs(inMoveY) >= 1)
        {
            p.y += inMoveY;
            inMoveY = 0;
        }
        if (abs(inMoveZ) >= 1)
        {
            p.z += inMoveZ;
            inMoveZ = 0;
        }

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                isRunning = SDL_FALSE;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    moveY = SDL_TRUE;
                    toMoveY = -velocity;
                    break;
                case SDLK_DOWN:
                    moveY = SDL_TRUE;
                    toMoveY = velocity;
                    break;
                case SDLK_LEFT:
                    moveX = SDL_TRUE;
                    toMoveX = -velocity;
                    break;
                case SDLK_RIGHT:
                    moveX = SDL_TRUE;
                    toMoveX = velocity;
                    break;
                case SDLK_SPACE:
                    moveZ = SDL_TRUE;
                    toMoveZ = -velocity;
                    break;
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    moveZ = SDL_TRUE;
                    toMoveZ = velocity;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                case SDLK_DOWN:
                    moveY = SDL_FALSE;
                    toMoveY = 0;
                    break;
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    moveX = SDL_FALSE;
                    toMoveX = 0;
                    break;
                case SDLK_SPACE:
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    moveZ = SDL_FALSE;
                    toMoveZ = 0;
                    break;
                }
                break;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}