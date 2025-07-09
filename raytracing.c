#include <stdio.h>
#include <SDL3/SDL.h>
#include <math.h>

#define WIDTH 1800
#define HEIGHT 1200
#define COLOUR_BLUE 0x2b79f8
#define COLOUR_ORANGE 0xf6ac18
#define COLOUR_BLACK 0x00000000
#define RAYS_COUNT 225
#define PI 3.14159265358979323846
#define COLOUR_RAY 0xf6C518

struct Circle
{
    double x;
    double y;
    double radius;
};

struct Ray
{
    double start_x, start_y;
    double angle;
};

void FillCircle(SDL_Surface *surface, struct Circle circle, Uint32 color)
{
    double radius_squared = pow(circle.radius, 2);
    for (double x = circle.x - circle.radius; x <= circle.x + circle.radius; x++)
    {
        for (double y = circle.y - circle.radius; y <= circle.y + circle.radius; y++)
        {
            double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);
            if (distance_squared < radius_squared)
            {
                SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
                SDL_FillSurfaceRect(surface, &pixel, color);
            }
        }
    }
}

void create_rays(struct Circle circle, struct Ray rays[RAYS_COUNT])
{
    for (int i = 0; i < RAYS_COUNT; i++)
    {
        double angle = ((double)i / RAYS_COUNT) * 2 * PI;
        struct Ray ray = {circle.x, circle.y, angle};
        rays[i] = ray;
    }
}

void FillRays(SDL_Surface *surface, struct Ray rays[RAYS_COUNT], Uint32 colour, struct Circle obstacle_obj)
{
    double radius_sqr = pow(obstacle_obj.radius, 2);
    for (int i = 0; i < RAYS_COUNT; i++)
    {
        struct Ray ray = rays[i];

        int end_screen = 0;
        int obstacle = 0;

        double step = 1;
        double x_drw = ray.start_x;
        double y_drw = ray.start_y;

        while (!end_screen && !obstacle)
        {
            x_drw += step * cos(ray.angle);
            y_drw += step * sin(ray.angle);

            SDL_Rect pixel = (SDL_Rect){x_drw, y_drw, 2, 2};
            SDL_FillSurfaceRect(surface, &pixel, colour);

            if (x_drw < 0 || x_drw > WIDTH)
                end_screen = 1;
            if (y_drw < 0 || y_drw > HEIGHT)
                end_screen = 1;

            double distance_sqr = pow(x_drw - obstacle_obj.x, 2) + pow(y_drw - obstacle_obj.y, 2);
            if (distance_sqr < radius_sqr)
            {
                break;
            }
        }
    }
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
        "Ray tracing with C", // window title
        WIDTH,                // width, in pixels
        HEIGHT,               // height, in pixels
        0                     // flags - see below
    );

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    struct Circle circle = {400, 400, 160};
    struct Circle obstacle_circle = {1250, 600, 280};
    SDL_Rect erase_rect = {0, 0, WIDTH, HEIGHT};

    struct Ray rays[RAYS_COUNT];
    create_rays(circle, rays);

    double obstacle_animation_y = 6;
    int animation = 1;
    SDL_Event event; // events that occur e.g key press
    while (animation)
    {
        while (SDL_PollEvent(&event)) // reading all the events in the application
        {
            if (event.type == SDL_EVENT_QUIT) // cross icon on a window
            {
                animation = 0;
            }

            /*
            gets the position of the mouse and fills to be circles x,y hence
            I am able to drag the circle.
            */
            if (event.type == SDL_EVENT_MOUSE_MOTION && event.motion.state != 0)
            {
                circle.x = event.motion.x;
                circle.y = event.motion.y;
                create_rays(circle, rays);
            }
        }

        SDL_FillSurfaceRect(surface, &erase_rect, COLOUR_BLACK);
        FillCircle(surface, circle, COLOUR_ORANGE);
        FillCircle(surface, obstacle_circle, COLOUR_BLUE);
        FillRays(surface, rays, COLOUR_RAY, obstacle_circle);

        obstacle_circle.y += obstacle_animation_y;
        if (obstacle_circle.y - obstacle_circle.radius < 0 || obstacle_circle.y + obstacle_circle.radius > HEIGHT)
        {
            obstacle_animation_y = -obstacle_animation_y;
        }

        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }

    return 0;
}