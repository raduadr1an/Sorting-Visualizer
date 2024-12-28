#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <iostream>
#include <random>
#include <vector>

class SortingVisualizer
{
  private:
    static constexpr int WINDOW_WIDTH = 1280;
    static constexpr int WINDOW_HEIGHT = 720;
    static constexpr int ARRAY_SIZE = 160;
    static constexpr int RECTANGLE_WIDTH = 8;
    static constexpr const char *WINDOW_NAME = "Sorting Visualizer";

    SDL_Window *window;
    SDL_Renderer *renderer;
    std::vector<int> array;
    bool sorting;
    bool quit;
    bool shouldStopSorting;

    void completionAnimation()
    {
        for (int i = 0; i < ARRAY_SIZE && !shouldStopSorting; ++i)
        {
            checkEvents();
            visualizeWithCompletion(i);
            SDL_Delay(10);
        }

        for (int flash = 0; flash < 3 && !shouldStopSorting; ++flash)
        {
            visualizeAllGold();
            SDL_Delay(200);

            visualize();
            SDL_Delay(200);
        }
    }

    void visualizeWithCompletion(int completionIndex)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < ARRAY_SIZE; ++i)
        {
            SDL_Rect rect = {i * RECTANGLE_WIDTH, WINDOW_HEIGHT - array[i], RECTANGLE_WIDTH - 1, array[i]};

            if (i <= completionIndex)
                SDL_SetRenderDrawColor(renderer, 50, 255, 50, 255);
            else
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);

            SDL_RenderFillRect(renderer, &rect);
        }
        SDL_RenderPresent(renderer);
    }

    void visualizeAllGold()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < ARRAY_SIZE; ++i)
        {
            SDL_Rect rect = {i * RECTANGLE_WIDTH, WINDOW_HEIGHT - array[i], RECTANGLE_WIDTH - 1, array[i]};
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
        SDL_RenderPresent(renderer);
    }

    bool initSDL()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cerr << "SDL initialization failed: " << SDL_GetError();
            return false;
        }

        window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window)
        {
            std::cerr << "Window creation failed: " << SDL_GetError();
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            std::cerr << "Renderer creation failed: " << SDL_GetError();
            SDL_DestroyWindow(window);
            return false;
        }

        return true;
    }

    void randomizeArray()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(50, WINDOW_HEIGHT - 50);

        for (int &value : array)
        {
            value = dis(gen);
        }
    }

    void visualize(int compareIndex = -1, int minIndex = -1, int swapIndex = -1)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < ARRAY_SIZE; ++i)
        {
            SDL_Rect rect = {i * RECTANGLE_WIDTH, WINDOW_HEIGHT - array[i], RECTANGLE_WIDTH - 1, array[i]};

            if (i == compareIndex || i == swapIndex)
            {
                SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255); // red - comparing
            }
            else if (i == minIndex)
            {
                SDL_SetRenderDrawColor(renderer, 50, 255, 50, 255); // green - minimum
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // white - normal
            }

            SDL_RenderFillRect(renderer, &rect);
        }
        SDL_RenderPresent(renderer);
    }

    void bubbleSort()
    {
        sorting = true;
        for (int i = 0; i < ARRAY_SIZE - 1 && !shouldStopSorting; ++i)
        {
            for (int j = 0; j < ARRAY_SIZE - i - 1 && !shouldStopSorting; ++j)
            {
                checkEvents();

                if (array[j] > array[j + 1])
                {
                    std::swap(array[j], array[j + 1]);
                    visualize(j, j + 1);
                }
                else
                {
                    visualize(j, j + 1);
                }
                SDL_Delay(5);
            }
        }
        sorting = false;
        if (!shouldStopSorting)
            completionAnimation();
    }

    void selectionSort()
    {
        sorting = true;
        for (int i = 0; i < ARRAY_SIZE - 1 && !shouldStopSorting; ++i)
        {
            int minIndex = i;
            for (int j = i + 1; j < ARRAY_SIZE && !shouldStopSorting; ++j)
            {
                checkEvents();
                if (array[j] < array[minIndex])
                {
                    minIndex = j;
                }
                visualize(j, minIndex, i);
                SDL_Delay(5);
            }

            if (!shouldStopSorting && minIndex != i)
            {
                std::swap(array[minIndex], array[i]);
                visualize(i, minIndex, i);
                SDL_Delay(10);
            }
        }
        sorting = false;
        if (!shouldStopSorting)
            completionAnimation();
    }

    void checkEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                shouldStopSorting = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_q:
                    quit = true;
                    shouldStopSorting = true;
                    break;
                case SDLK_0:
                    if (!sorting)
                        randomizeArray();
                    break;
                case SDLK_1:
                    if (!sorting)
                    {
                        shouldStopSorting = false;
                        selectionSort();
                    }
                    break;
                case SDLK_2:
                    if (!sorting)
                    {
                        shouldStopSorting = false;
                        bubbleSort();
                    }
                    break;
                }
                break;
            }
        }
    }

  public:
    SortingVisualizer() : window(nullptr), renderer(nullptr), array(ARRAY_SIZE), sorting(false), quit(false), shouldStopSorting(false) {}

    ~SortingVisualizer()
    {
        if (renderer)
            SDL_DestroyRenderer(renderer);
        if (window)
            SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool initialize()
    {
        if (!initSDL())
            return false;
        randomizeArray();
        return true;
    }

    void run()
    {
        visualize();

        while (!quit)
        {
            checkEvents();
            if (!sorting)
                visualize();
            SDL_Delay(16);
        }
    }
};

void displayMenu()
{
    std::cout << "\033[2J\033[1;1H";
    std::cout << "+---------------------------------------------------------------+\n";
    std::cout << "|                   Sorting Visualizer Controls                  |\n";
    std::cout << "+---------------------------------------------------------------+\n";
    std::cout << "| 0 - Generate new array                                        |\n";
    std::cout << "| 1 - Start Selection Sort                                      |\n";
    std::cout << "| 2 - Start Bubble Sort                                         |\n";
    std::cout << "| q - Return to this menu                                       |\n";
    std::cout << "+---------------------------------------------------------------+\n";
    std::cout << "| Enter 'R' to run visualizer or 'Q' to quit program           |\n";
    std::cout << "+---------------------------------------------------------------+\n\n";
}

int main(int argc, char *argv[])
{
    char input;
    do
    {
        displayMenu();
        std::cin >> input;
        input = toupper(input);

        if (input == 'R')
        {
            SortingVisualizer visualizer;
            if (visualizer.initialize())
                visualizer.run();
        }
    } while (input != 'Q');

    return 0;
}
