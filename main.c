#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

struct ball_s
{
    float x;
    float y;
    float px;
    float py;
    float vx;
    float vy;
    int r;
    float bc;
    SDL_Color color;
    SDL_Texture *texture;
};

#define WINDOWX 1920
#define WINDOWY 1080
#define GRAVITY 982
#define MAX_BALLS 75


SDL_Texture* load_texture(const char *file, SDL_Renderer *renderer); 
void apply_velocity_change(struct ball_s *balls, int ball_n, double delta);
void update_postion(struct ball_s *balls, int ball_n, double delta);
void collision_check(struct ball_s *balls, int ball_n);
bool single_ball_check(struct ball_s *balls, int ball_n, struct ball_s ball);

float get_distance(float x1, float y1, float x2, float y2);
void clear_balls(struct ball_s *balls, int ball_n);


struct ball_s create_ball(SDL_Renderer* renderer);



int main(void) {
    srand(time(NULL));

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Cool Balls", 
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED, 
                                          WINDOWX, WINDOWY, 
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // Main loop flag
    int quit = 0;
    SDL_Event event;

    // Main event loop
    struct ball_s balls[MAX_BALLS];
    int ball_n = 1;
    for(int i = 0; i < ball_n; i++)
    {
        struct ball_s ball = create_ball(renderer);
        balls[i] = ball;
    }

    
    Uint64 last_time = SDL_GetPerformanceCounter();
    double delta_time = 0.0;
    double run_time = 0;

    SDL_Texture *spriteTexture = loadTexture("circle.png", renderer);
    while (!quit) {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        delta_time = (double)(currentTime - last_time) / SDL_GetPerformanceFrequency();
        run_time += delta_time;
        last_time = currentTime;
        printf("FPS: %.0f MS: %.05f\t Ball: %d\tTotaltime: %.1f\n", 1 / delta_time, delta_time, ball_n, run_time);
        fflush(stdout);


        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
        // (Optional) Clear the window with a color
        SDL_SetRenderDrawColor(SDL_GetRenderer(window), 17, 11, 36, 255); // Set black color
        SDL_RenderClear(SDL_GetRenderer(window));

        apply_velocity_change(balls, ball_n, delta_time);
        update_postion(balls, ball_n, delta_time);
        collision_check(balls, ball_n);



        for(int i = 0; i < ball_n; i++)
        {
            SDL_Rect destRect = {balls[i].x, balls[i].y, balls[i].r * 2,balls[i].r * 2 };  // Position and size
            SDL_RenderCopy(renderer, balls[i].texture, NULL, &destRect);
        }
    
        if(ball_n < MAX_BALLS && run_time > 0.5)
        {
            struct ball_s ball = create_ball(renderer);
            if(!single_ball_check(balls, ball_n, ball))
            {
                balls[ball_n] = ball;
                ball_n++;
                run_time = 0;
            }
            
        }

        if(ball_n == MAX_BALLS && run_time > 15.0)
        {
            ball_n = 1;
            run_time = 0;
            for(int i = 0; i < ball_n; i++)
            {
                struct ball_s ball = create_ball(renderer);
                balls[i] = ball;
            }
        }
 

        SDL_RenderPresent(renderer); // Update the window
        //SDL_Delay(16);
    }

    // Destroy the window and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bool single_ball_check(struct ball_s *balls, int ball_n, struct ball_s ball)
{

    for(int i = 0; i < ball_n; i++)
    {
        float distance = get_distance(balls[i].x + balls[i].r / 2, balls[i].y + balls[i].r / 2, ball.x + ball.r / 2, ball.y + ball.r / 2);
        if(distance <= balls[i].r + ball.r)
        {
            return true;
        }
    }

    return false;
}

SDL_Texture* load_texture(const char *file, SDL_Renderer *renderer) {
    SDL_Surface *surface = IMG_Load(file);
    if (!surface) {
        printf("Image load error: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);  // Free the surface after creating the texture

    return texture;
}

struct ball_s create_ball(SDL_Renderer* renderer)
{
    struct ball_s ball;
    int speed = 1000;
    ball.r = rand() % 45 + 5;
    ball.x = rand() % abs(WINDOWX - ball.r);
    ball.y = rand() % abs(WINDOWY - ball.r);
    ball.vx = rand() % speed - speed/2;
    ball.vy = rand() % speed - speed/2;
    ball.bc = 1;
    ball.color.r = rand() % 256;
    ball.color.g = rand() % 256;
    ball.color.b = rand() % 256;
    ball.texture = loadTexture("circle.png", renderer);
    SDL_SetTextureColorMod(ball.texture, ball.color.r, ball.color.g, ball.color.b);

    return ball;
}

void apply_velocity_change(struct ball_s *balls, int ball_n, double delta)
{
    for(int i = 0; i < ball_n; i++)
    {
        balls[i].vx += 0 * delta;
        balls[i].vy += GRAVITY * delta;
    }

    return;
}

void update_postion(struct ball_s *balls, int ball_n, double delta)
{
    for(int i = 0; i < ball_n; i++)
    {
        balls[i].px = balls[i].x;
        balls[i].py = balls[i].y;
        balls[i].x += balls[i].vx * delta;
        balls[i].y += balls[i].vy * delta;
    }

    return;
}

void collision_check(struct ball_s *balls, int ball_n)
{
    for(int i = 0; i < ball_n; i++)
    {
        if(balls[i].x+balls[i].r * 2 > WINDOWX || balls[i].x < 0)
        {
            balls[i].vx = -balls[i].vx * balls[i].bc;
            balls[i].x = balls[i].px;
        }
        if(balls[i].y+balls[i].r * 2 > WINDOWY || balls[i].y < 0)
        {
            balls[i].vy = -balls[i].vy * balls[i].bc;
            balls[i].y = balls[i].py;
        }
        for(int j = i + 1; j < ball_n; j++)
        {
            float distance = get_distance(balls[i].x + balls[i].r / 2, balls[i].y + balls[i].r / 2, balls[j].x + balls[j].r / 2, balls[j].y + balls[j].r / 2);
            //If balls are closer than collective radius
            if(distance <= balls[i].r + balls[j].r)
            {
                float ivx = balls[i].vx;
                float ivy = balls[i].vy;

                balls[i].vx = balls[j].vx;
                balls[i].vy = balls[j].vy;
                balls[j].vx = ivx;
                balls[j].vy = ivy;

                balls[i].x = balls[i].px;
                balls[i].y = balls[i].py;
                balls[j].x = balls[j].px;
                balls[j].y = balls[j].py;
            }
        }
    }

    return;
}


float get_distance(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}