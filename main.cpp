#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cstdlib> // Для генерації випадкових чисел (rand)
#include <ctime>   // Для ініціалізації генератора (time)
#include <string> //для роботи з рядками

 void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    //проходимо по квадрату в який вписане коло
    for (int w = 0; w < radius * 2; w++){
        for(int h = 0; h < radius * 2; h++){
            int dx = radius - w;//відстань по X від центру
            int dy = radius -h; //відстань по Y від центру
            //якщо точка в межах радіуса (x^2 + y^2 <= r^2)
            if ((dx * dx + dy * dy) <= (radius * radius)){
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
 }
 
 int main(int argc, char* args[]) {
 if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Помилка ініціалізації: " << SDL_GetError() << std::endl;
    return -1;
 }
 if (TTF_Init() == -1){
    std::cout << "Помилка TTF: " << TTF_GetError() << std::endl;
    return -1;
 }
//  шрифт з системної папки macOS
TTF_Font* font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 24);
    if(font == nullptr){
        std::cout<< "Помилка завантаження шрифту: " << TTF_GetError() << std::endl;
        return -1;
    }
 SDL_Window* window = SDL_CreateWindow("Aim Trainer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);

 if (window == nullptr){
    std::cout << "Помилка створення вікна: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return -1;
 }
 int screenWidth, screenHeight;
 SDL_GetWindowSize(window, &screenWidth, &screenHeight); //отримуємо розмір екрану
 //створюємо рендер, який використовує відеокарту для прискорення
 SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
 if (renderer == nullptr){
    std::cout << "Помилка створення рендерера: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
 }
 
 enum GameState {
    MENU,
    PLAYING
 };
 GameState currentState = MENU; // початковий стан гри - меню
 int targetX = 400; // поки що по центру
 int targetY = 300;
 int score = 0;
 int mouseX = 0;
 int mouseY = 0;
 int crosshairSize = 10; // розмір прицілу
 int targetRadius = 30;// радіус мішені
 bool isRunning = true;
 Uint32 starttime = SDL_GetTicks(); //запам'ятовуємо час початку гри
 int timeLeft = 30; // 30sec na gru
 int gameOver = false; // гра триває
 int totalClicks = 0;
 float accuracy = 100.0f;
 SDL_Event event;

 srand(time(nullptr));
 SDL_ShowCursor(SDL_DISABLE); //ховаємо курсор миші
 while (isRunning) {
    //якщо гра триває, рахуємо час
    if (currentState == PLAYING && !gameOver){
        Uint32 currentTime = SDL_GetTicks();
        //віднімаємо від поточного часу час старту і ділимо на 1000 щоб перевести мілісекунди в секунди
        int secondsPassed = (currentTime - starttime) / 1000;
        timeLeft = 30 - secondsPassed;
        
        //Якщо час вийшов
        if (timeLeft <= 0){
            timeLeft = 0;
            gameOver = true; //гра зупинена
        }
    }
    SDL_GetMouseState(&mouseX, &mouseY); //отримуємо координати миші
    // SDL_PollEvent зчитує чергу подій (рухи миші, кліки, клавіатуру)
    while (SDL_PollEvent(&event)) {
        
        // БЛОК 1: закриття вікна
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        // --- БЛОК 2: Миша (тільки якщо гра триває) ---
        if (event.type == SDL_MOUSEBUTTONDOWN && currentState == PLAYING && !gameOver) {
            // перевіряємо чи це ліва кнопка
            if (event.button.button == SDL_BUTTON_LEFT) {
                totalClicks++; // рахуємо кожен клік

                // отримуємо коорди кліку
                int clickX = event.button.x;
                int clickY = event.button.y;

                // ЛОГІКА ПЕРЕВІРКИ КЛІКУ 
                int dx = clickX - targetX;
                int dy = clickY - targetY;

                // якщо клік був всередині кола
                if ((dx * dx + dy * dy) <= (targetRadius * targetRadius)) {
                    score++;
                    std::cout << "Влучання!" << std::endl;

                    // ЗМІНА КООРДИНАТ МІШЕНІ (рандом спавну)
                    targetRadius = 15 + rand() % 35;
                    targetX = targetRadius + rand() % (screenWidth - targetRadius * 2);
                    targetY = targetRadius + rand() % (screenHeight - targetRadius * 2);
                }
                
                // оновлюємо точність після кожного кліку
                accuracy = ((float)score / totalClicks) * 100.0f; 
            }
        }

        // клавіатура (незалежний від інших подій)
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                isRunning = false; // вихід з гри
            }
            if (currentState == MENU && event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                currentState = PLAYING;
                score = 0;
                totalClicks = 0;
                accuracy = 100.0f;
                timeLeft = 30;
                gameOver = false;
                starttime = SDL_GetTicks(); //  секундомір
            }

            // використовуємо scancode, щоб працювало на будь-якій мові розкладки
            if (currentState == PLAYING && event.key.keysym.scancode == SDL_SCANCODE_R && gameOver) {
                // перезапуск гри
                score = 0;
                totalClicks = 0;
                accuracy = 100.0f;
                starttime = SDL_GetTicks(); //скидаємо секундомір
                timeLeft = 30;
                gameOver = false; //гра ресетнулась
            }
        }
    }
    
    
    // СЕКЦІЯ РЕНДЕРИНГУ (МАЛЮВАННЯ КАДРУ)    
    // 1.беремо темно сірий колір i замальовуємо весь екран (робимо це завжди)
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    //ЯКЩО МИ В МЕНЮ 
    if (currentState == MENU) {
        std::string titleText = "AIM TRAINER";
        std::string startText = "Press ENTER to Start";
        
        SDL_Color titleColor = {0, 255, 255, 255}; 
        SDL_Color startColor = {255, 255, 255, 255}; 
        
        //малюємо заголовок
        SDL_Surface* titleSurface = TTF_RenderText_Solid(font, titleText.c_str(), titleColor);
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        SDL_Rect titleRect;
        titleRect.w = titleSurface->w;
        titleRect.h = titleSurface->h;
        titleRect.x = (screenWidth / 2) - (titleRect.w / 2);
        titleRect.y = (screenHeight / 2) - 100;
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
        
        //малюємо підказку "Press ENTER"
        SDL_Surface* startSurface = TTF_RenderText_Solid(font, startText.c_str(), startColor);
        SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
        SDL_Rect startRect;
        startRect.w = startSurface->w;
        startRect.h = startSurface->h;
        startRect.x = (screenWidth / 2) - (startRect.w / 2);
        startRect.y = (screenHeight / 2) + 50;
        SDL_RenderCopy(renderer, startTexture, nullptr, &startRect);
        
        // Очищаємо пам'ять від текстів меню
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
        SDL_FreeSurface(startSurface);
        SDL_DestroyTexture(startTexture);
    } 
    //ЯКЩО МИ В ГРІ
    else if (currentState == PLAYING) {
        
        // встановлюємо колір для сітки
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        // малюємо вертикальні лінії
        for (int x=0; x < screenWidth; x += 50){
            SDL_RenderDrawLine(renderer, x, 0, x, screenHeight);
        }
        // малюємо горизонтальні лінії
        for (int y=0; y < screenHeight; y += 50){
            SDL_RenderDrawLine(renderer, 0, y, screenWidth, y);
        }
        
        // задаємо колі мішені - блакитний
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        DrawCircle(renderer, targetX, targetY, targetRadius);
        
        // формуємо рядок (з'єднуємо слово і число)
        std::string scoreText = "Score: " + std::to_string(score);
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect;
        textRect.x = 10;
        textRect.y = 10;
        textRect.w = textSurface->w; 
        textRect.h = textSurface->h;

        // формуємо рядок таймеру 
        std::string timerText = "Time: " + std::to_string(timeLeft);
        SDL_Color timerColor = {255, 255, 255, 255};
        SDL_Surface* timerSurface = TTF_RenderText_Solid(font, timerText.c_str(), timerColor);
        SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
        SDL_Rect timerRect;
        timerRect.x = 10; 
        timerRect.y = 40;
        timerRect.w = timerSurface->w;
        timerRect.h = timerSurface->h; 
        
        // формуємо рядок точності 
        std::string accuracyText = "Accuracy: " + std::to_string(static_cast<int>(accuracy)) + "%";
        SDL_Color accuracyColor = {255, 255, 255, 255};
        SDL_Surface* accuracySurface = TTF_RenderText_Solid(font, accuracyText.c_str(), accuracyColor);
        SDL_Texture* accuracyTexture = SDL_CreateTextureFromSurface(renderer, accuracySurface);
        SDL_Rect accuracyRect;
        accuracyRect.x = 10;
        accuracyRect.y = 70;
        accuracyRect.w = accuracySurface->w;
        accuracyRect.h = accuracySurface->h;

        // Копіюємо текстури статистики на екран
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);
        SDL_RenderCopy(renderer, accuracyTexture, nullptr, &accuracyRect);
        
        // видаляємо тимчасові об'єкти статистики з пам'яті
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        SDL_DestroyTexture(timerTexture);
        SDL_FreeSurface(timerSurface);
        SDL_DestroyTexture(accuracyTexture);
        SDL_FreeSurface(accuracySurface);

        // Якщо гра закінчилась, малюємо текст рестарту ПОВЕРХ гри
        if (gameOver) {
            std::string restartText = "Press R to restart";
            SDL_Color restartColor = {255, 255, 0, 255}; // жовтий колір
            
            SDL_Surface* restartSurface = TTF_RenderText_Solid(font, restartText.c_str(), restartColor);
            SDL_Texture* restartTexture = SDL_CreateTextureFromSurface(renderer, restartSurface);

            SDL_Rect restartRect;
            restartRect.w = restartSurface->w;
            restartRect.h = restartSurface->h;
            // математика для вирівнювання тексту по центру
            restartRect.x = (screenWidth / 2) - (restartRect.w / 2);
            restartRect.y = (screenHeight / 2) - (restartRect.h / 2);

            SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);

            SDL_FreeSurface(restartSurface);
            SDL_DestroyTexture(restartTexture);
        }
    } // Кінець блоку if (currentState == PLAYING)

    // МАЛЮЄМО ПРИЦІЛ ЗАВЖДИ (і в меню, і в грі)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // білий приціл
    SDL_RenderDrawLine(renderer, mouseX - crosshairSize, mouseY, mouseX + crosshairSize, mouseY);
    SDL_RenderDrawLine(renderer, mouseX, mouseY - crosshairSize, mouseX, mouseY + crosshairSize);
    
    // показуємо намальований кадр на екран
    SDL_RenderPresent(renderer);
 }
 TTF_CloseFont(font);
 TTF_Quit();
 SDL_DestroyRenderer(renderer);
 SDL_DestroyWindow(window);
 SDL_Quit();
    return 0;
}