#pragma once
#include <SDL2/SDL.h>
#include <cstdlib>

class Target {
    private:
        int x, y;//координати мішені
        int radius;//радіус мішені

    public:
        Target(int screenWidth, int screenHeight){
            radius = 15 + rand() % 35;
        }

        //метод  для зміни коорд і розміру мішені
        void respawn(int screenWidth, int screenHeight){
            radius = 15 + rand() % 35;
            x = radius + rand() % (screenWidth - radius *2);
            y = radius + rand() % (screenHeight - radius *2);
        }

        //перевірка влучання за піфагором
        bool checkHit(int mouseX, int mouseY){
            int dx = mouseX - x;
            int dy = mouseY - y;
            return ( dx * dx + dy * dy) <= (radius * radius);
        }

        //малювання мішені через алгоритм кола
        void draw(SDL_Renderer* renderer){
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);//блакитний колір
            for (int w = 0; w < radius * 2; w++){
                for (int h = 0; h < radius * 2; h++){
                    int dx = radius - w;//відстань по X від центру
                    int dy = radius - h;//відстань по Y від центру
                    if ((dx * dx + dy *dy) <= (radius * radius)){
                        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                    }
                }
            }
        }

        int getX() const { return x; }
        int getY() const { return y; }
        int getRadius() const { return radius;};

};