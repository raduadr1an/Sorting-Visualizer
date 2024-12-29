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
    static constexpr int DELAY_TIME = 5;
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
                SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255); // red - comparing
            else if (i == minIndex)
                SDL_SetRenderDrawColor(renderer, 50, 255, 50, 255); // green - minimum
            else
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // white - normal

            SDL_RenderFillRect(renderer, &rect);
        }
        SDL_RenderPresent(renderer);
    }

    void insertionSort()
    {
        sorting = true;
        for (int i = 1; i < ARRAY_SIZE && !shouldStopSorting; i++)
        {
            int key = array[i];
            int j = i - 1;

            while (j >= 0 && array[j] > key && !shouldStopSorting)
            {
                checkEvents();
                array[j + 1] = array[j];
                visualize(j + 1, i, j);
                SDL_Delay(DELAY_TIME);
                j--;
            }
            array[j + 1] = key;
            visualize(j + 1, i);
            SDL_Delay(DELAY_TIME);
        }
        sorting = false;
        if (!shouldStopSorting)
            completionAnimation();
    }

    void merge(int left, int mid, int right)
    {
        std::vector<int> temp(right - left + 1);
        int i = left, j = mid + 1, k = 0;

        while (i <= mid && j <= right && !shouldStopSorting)
        {
            checkEvents();
            visualize(i, j);
            SDL_Delay(DELAY_TIME);

            if (array[i] <= array[j])
                temp[k++] = array[i++];
            else
                temp[k++] = array[j++];
        }

        while (i <= mid && !shouldStopSorting)
        {
            checkEvents();
            visualize(i, -1);
            temp[k++] = array[i++];
            SDL_Delay(DELAY_TIME);
        }

        while (j <= right && !shouldStopSorting)
        {
            checkEvents();
            visualize(j, -1);
            temp[k++] = array[j++];
            SDL_Delay(DELAY_TIME);
        }

        for (i = 0; i < k && !shouldStopSorting; i++)
        {
            array[left + i] = temp[i];
            visualize(left + i, -1);
            SDL_Delay(DELAY_TIME);
        }
    }

    void mergeSort(int left, int right)
    {
        if (left < right && !shouldStopSorting)
        {
            int mid = left + (right - left) / 2;
            mergeSort(left, mid);
            mergeSort(mid + 1, right);
            merge(left, mid, right);
        }
    }

    void mergeSortWrapper()
    {
        sorting = true;
        mergeSort(0, ARRAY_SIZE - 1);
        sorting = false;
        if (!shouldStopSorting)
            completionAnimation();
    }

    int partition(int low, int high)
    {
        int pivot = array[high];
        int i = low - 1;

        for (int j = low; j < high && !shouldStopSorting; j++)
        {
            checkEvents();
            visualize(j, high, i + 1);
            SDL_Delay(DELAY_TIME);

            if (array[j] <= pivot)
            {
                i++;
                std::swap(array[i], array[j]);
            }
        }
        std::swap(array[i + 1], array[high]);
        return i + 1;
    }

    void quickSort(int low, int high)
    {
        if (low < high && !shouldStopSorting)
        {
            int pi = partition(low, high);
            quickSort(low, pi - 1);
            quickSort(pi + 1, high);
        }
    }

    void quickSortWrapper()
    {
        sorting = true;
        quickSort(0, ARRAY_SIZE - 1);
        sorting = false;
        if (!shouldStopSorting)
            completionAnimation();
    }

    void heapify(int n, int i)
    {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && array[left] > array[largest])
            largest = left;

        if (right < n && array[right] > array[largest])
            largest = right;

        if (largest != i && !shouldStopSorting)
        {
            std::swap(array[i], array[largest]);
            visualize(i, largest);
            SDL_Delay(DELAY_TIME);
            heapify(n, largest);
        }
    }

    void heapSort()
    {
        sorting = true;

        for (int i = ARRAY_SIZE / 2 - 1; i >= 0 && !shouldStopSorting; i--)
            heapify(ARRAY_SIZE, i);

        for (int i = ARRAY_SIZE - 1; i > 0 && !shouldStopSorting; i--)
        {
            checkEvents();
            std::swap(array[0], array[i]);
            visualize(0, i);
            SDL_Delay(DELAY_TIME);
            heapify(i, 0);
        }

        sorting = false;
        if (!shouldStopSorting)
            completionAnimation();
    }

    void shellSort()
    {
        sorting = true;
        for (int gap = ARRAY_SIZE / 2; gap > 0 && !shouldStopSorting; gap /= 2)
        {
            for (int i = gap; i < ARRAY_SIZE && !shouldStopSorting; i++)
            {
                int temp = array[i];
                int j;
                for (j = i; j >= gap && array[j - gap] > temp && !shouldStopSorting; j -= gap)
                {
                    checkEvents();
                    array[j] = array[j - gap];
                    visualize(j, j - gap, i);
                    SDL_Delay(DELAY_TIME);
                }
                array[j] = temp;
            }
        }
        sorting = false;
        if (!shouldStopSorting)
            completionAnimation();
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
                SDL_Delay(DELAY_TIME);
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
                SDL_Delay(DELAY_TIME);
            }

            if (!shouldStopSorting && minIndex != i)
            {
                std::swap(array[minIndex], array[i]);
                visualize(i, minIndex, i);
                SDL_Delay(DELAY_TIME * 2);
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
                case SDLK_3:
                    if (!sorting)
                    {
                        shouldStopSorting = false;
                        insertionSort();
                    }
                    break;
                case SDLK_4:
                    if (!sorting)
                    {
                        shouldStopSorting = false;
                        mergeSortWrapper();
                    }
                    break;
                case SDLK_5:
                    if (!sorting)
                    {
                        shouldStopSorting = false;
                        quickSortWrapper();
                    }
                    break;
                case SDLK_6:
                    if (!sorting)
                    {
                        shouldStopSorting = false;
                        heapSort();
                    }
                    break;
                case SDLK_7:
                    if (!sorting)
                    {
                        shouldStopSorting = false;
                        shellSort();
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
    std::cout << "| 3 - Start Insertion Sort                                      |\n";
    std::cout << "| 4 - Start Merge Sort                                          |\n";
    std::cout << "| 5 - Start Quick Sort                                          |\n";
    std::cout << "| 6 - Start Heap Sort                                          |\n";
    std::cout << "| 7 - Start Shell Sort                                         |\n";
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
