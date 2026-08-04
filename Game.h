#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include "Target.h"

enum GameState {
    MENU,
    PLAYING
};

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    
    int screenWidth;
    int screenHeight;
    bool isRunning;
    
    GameState currentState;
    Target target;
    
    int score;
    int totalClicks;
    float accuracy;
    int timeLeft;
    bool gameOver;
    
    Uint32 starttime;
    int mouseX, mouseY;
    int crosshairSize;

public:
    Game() : target(800, 600) { // Конструктор за замовчуванням
        window = nullptr;
        renderer = nullptr;
        font = nullptr;
        screenWidth = 800;
        screenHeight = 600;
        isRunning = true;
        currentState = MENU;
        score = 0;
        totalClicks = 0;
        accuracy = 100.0f;
        timeLeft = 30;
        gameOver = false;
        starttime = 0;
        mouseX = 0;
        mouseY = 0;
        crosshairSize = 10;
    }

    // Ініціалізація вікна, рендерера та шрифту
    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "Помилка ініціалізації: " << SDL_GetError() << std::endl;
            return false;
        }
        if (TTF_Init() == -1) {
            std::cout << "Помилка TTF: " << TTF_GetError() << std::endl;
            return false;
        }

        font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 24);
        if (font == nullptr) {
            std::cout << "Помилка завантаження шрифту: " << TTF_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("Aim Trainer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
        if (window == nullptr) {
            std::cout << "Помилка створення вікна: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_GetWindowSize(window, &screenWidth, &screenHeight);
        
        // Оновлюємо мішень під реальний розмір екрану
        target.respawn(screenWidth, screenHeight);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cout << "Помилка створення рендерера: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_ShowCursor(SDL_DISABLE);
        return true;
    }

    // Головний ігровий цикл
    void run() {
        SDL_Event event;
        while (isRunning) {
            handleEvents(event);
            update();
            render();
        }
    }

    // Деструктор для очищення пам'яті
    ~Game() {
        if (font) TTF_CloseFont(font);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }

private:
    void handleEvents(SDL_Event& event) {
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }

            // Обробка кліків миші у грі
            if (event.type == SDL_MOUSEBUTTONDOWN && currentState == PLAYING && !gameOver) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    totalClicks++;
                    int clickX = event.button.x;
                    int clickY = event.button.y;

                    if (target.checkHit(clickX, clickY)) {
                        score++;
                        std::cout << "Влучання!" << std::endl;
                        target.respawn(screenWidth, screenHeight);
                    }
                    accuracy = ((float)score / totalClicks) * 100.0f;
                }
            }

            // Обробка клавіатури
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    isRunning = false;
                }
                if (currentState == MENU && event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    currentState = PLAYING;
                    score = 0;
                    totalClicks = 0;
                    accuracy = 100.0f;
                    timeLeft = 30;
                    gameOver = false;
                    starttime = SDL_GetTicks();
                }
                if (currentState == PLAYING && event.key.keysym.scancode == SDL_SCANCODE_R && gameOver) {
                    score = 0;
                    totalClicks = 0;
                    accuracy = 100.0f;
                    starttime = SDL_GetTicks();
                    timeLeft = 30;
                    gameOver = false;
                }
            }
        }
    }

    void update() {
        if (currentState == PLAYING && !gameOver) {
            Uint32 currentTime = SDL_GetTicks();
            int secondsPassed = (currentTime - starttime) / 1000;
            timeLeft = 30 - secondsPassed;

            if (timeLeft <= 0) {
                timeLeft = 0;
                gameOver = true;
            }
        }
    }

    void render() {
        // Очищення екрана
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        if (currentState == MENU) {
            renderMenu();
        } else if (currentState == PLAYING) {
            renderGame();
        }

        // Малювання прицілу поверх усього
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, mouseX - crosshairSize, mouseY, mouseX + crosshairSize, mouseY);
        SDL_RenderDrawLine(renderer, mouseX, mouseY - crosshairSize, mouseX, mouseY + crosshairSize);

        SDL_RenderPresent(renderer);
    }

    void renderMenu() {
        std::string titleText = "AIM TRAINER";
        std::string startText = "Press ENTER to Start";
        
        SDL_Color titleColor = {0, 255, 255, 255};
        SDL_Color startColor = {255, 255, 255, 255};
        
        SDL_Surface* titleSurface = TTF_RenderText_Solid(font, titleText.c_str(), titleColor);
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        SDL_Rect titleRect = { screenWidth / 2 - titleSurface->w / 2, screenHeight / 2 - 100, titleSurface->w, titleSurface->h };
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
        
        SDL_Surface* startSurface = TTF_RenderText_Solid(font, startText.c_str(), startColor);
        SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
        SDL_Rect startRect = { screenWidth / 2 - startSurface->w / 2, screenHeight / 2 + 50, startSurface->w, startSurface->h };
        SDL_RenderCopy(renderer, startTexture, nullptr, &startRect);
        
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
        SDL_FreeSurface(startSurface);
        SDL_DestroyTexture(startTexture);
    }

    void renderGame() {
        // Сітка
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        for (int x = 0; x < screenWidth; x += 50) {
            SDL_RenderDrawLine(renderer, x, 0, x, screenHeight);
        }
        for (int y = 0; y < screenHeight; y += 50) {
            SDL_RenderDrawLine(renderer, 0, y, screenWidth, y);
        }

        // Мішень
        target.draw(renderer);

        // Статистика (Score, Time, Accuracy)
        renderText("Score: " + std::to_string(score), 10, 10);
        renderText("Time: " + std::to_string(timeLeft), 10, 40);
        renderText("Accuracy: " + std::to_string(static_cast<int>(accuracy)) + "%", 10, 70);

        if (gameOver) {
            std::string restartText = "Press R to restart";
            SDL_Color restartColor = {255, 255, 0, 255};
            SDL_Surface* restartSurface = TTF_RenderText_Solid(font, restartText.c_str(), restartColor);
            SDL_Texture* restartTexture = SDL_CreateTextureFromSurface(renderer, restartSurface);
            SDL_Rect restartRect = { screenWidth / 2 - restartSurface->w / 2, screenHeight / 2 - restartSurface->h / 2, restartSurface->w, restartSurface->h };
            SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);
            SDL_FreeSurface(restartSurface);
            SDL_DestroyTexture(restartTexture);
        }
    }

    void renderText(const std::string& text, int x, int y) {
        SDL_Color color = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = { x, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
};