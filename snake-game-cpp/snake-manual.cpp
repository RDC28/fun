// Simple terminal Snake game (step-per-key). Uses a singly linked list for the snake.
#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

// Board dimensions (rows x cols)
const int BOARD_HEIGHT = 12;
const int BOARD_WIDTH = 20;

// Frame buffer storing characters to print each frame
char board[BOARD_HEIGHT][BOARD_WIDTH];

// Apple position (x = col, y = row)
int apple_pos_w;
int apple_pos_h;

// Game state flags
bool apple_eaten = false;
bool game_over = false;

// Snake node (linked-list). Head is the first node in the list.
typedef struct node
{
    int x_pos;
    int y_pos;
    node *next;
} node;

// Pointer to the snake's head node
node *snake = NULL;

// Rendering and setup
void ClearBoard();
void PrintBoard();
void RenderBoard();
void StartGame();

// Game mechanics
int RandPosGenerator(int axis);
void AppleGenerator();
void MoveSnake(char key);

int main()
{
    srand(time(0));
    ClearBoard();

    StartGame();

    RenderBoard();
    PrintBoard();

    // Step-per-key loop: wait for input, move once if WASD, redraw
    while (!game_over)
    {
        char key = _getch();
        if (key == 'q' || key == 'Q')
        {
            game_over = true;
            break;
        }

        if (key == 'w' || key == 'W' || key == 'a' || key == 'A' ||
            key == 's' || key == 'S' || key == 'd' || key == 'D')
        {
            MoveSnake(key);
            RenderBoard();
            system("cls");
            PrintBoard();
        }
    }
    cout << "Game Over!" << endl;
    return 0;
}
void RenderBoard()
{
    // Rebuild the frame buffer from current game state
    ClearBoard();
    if (apple_pos_w >= 0 && apple_pos_w < BOARD_WIDTH && apple_pos_h >= 0 && apple_pos_h < BOARD_HEIGHT)
    {
        board[apple_pos_h][apple_pos_w] = 'O';
    }
    // Mark head distinctly for readability
    bool is_head = true;
    for (node *curr = snake; curr != NULL; curr = curr->next)
    {
        if (curr->x_pos >= 0 && curr->x_pos < BOARD_WIDTH && curr->y_pos >= 0 && curr->y_pos < BOARD_HEIGHT)
        {
            board[curr->y_pos][curr->x_pos] = is_head ? 'H' : 'S';
        }
        is_head = false;
    }
}

void ClearBoard()
{
    // Fill each row with '.' characters
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        memset(board[i], '.', BOARD_WIDTH);
    }
}

void PrintBoard()
{
    // Print the current frame buffer
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}

void StartGame()
{
    // Wait for a key to start and initialize a 1-cell snake at (0,0)
    cout << "Press any key to start the game" << endl;
    _getch();
    node *head = new node;
    head->next = NULL;
    head->x_pos = 0;
    head->y_pos = 0;
    snake = head;
    AppleGenerator();
    system("cls");
}

int RandPosGenerator(int axis)
{
    return rand() % axis;
}

void AppleGenerator()
{
    // Choose a random cell not currently occupied by the snake
    apple_eaten = false;
    for (int attempts = 0; attempts < 1000; ++attempts)
    {
        int x = RandPosGenerator(BOARD_WIDTH);
        int y = RandPosGenerator(BOARD_HEIGHT);
        bool on_snake = false;
        for (node *curr = snake; curr != NULL; curr = curr->next)
        {
            if (curr->x_pos == x && curr->y_pos == y)
            {
                on_snake = true;
                break;
            }
        }
        if (!on_snake)
        {
            apple_pos_w = x;
            apple_pos_h = y;
            break;
        }
    }
}

void MoveSnake(char key)
{
    // Translate key to delta
    if (game_over || snake == NULL) return;

    int dx = 0;
    int dy = 0;
    if (key == 'w' || key == 'W') { dy = -1; dx = 0; }
    else if (key == 's' || key == 'S') { dy = 1; dx = 0; }
    else if (key == 'a' || key == 'A') { dx = -1; dy = 0; }
    else if (key == 'd' || key == 'D') { dx = 1; dy = 0; }
    else { return; }

    // Next head position
    int new_x = snake->x_pos + dx;
    int new_y = snake->y_pos + dy;

    // End the game when hitting a wall
    if (new_x < 0 || new_x >= BOARD_WIDTH || new_y < 0 || new_y >= BOARD_HEIGHT)
    {
        game_over = true;
        return;
    }

    // End the game when colliding with own body
    for (node *curr = snake; curr != NULL; curr = curr->next)
    {
        if (curr->x_pos == new_x && curr->y_pos == new_y)
        {
            game_over = true;
            return;
        }
    }

    // Grow if we move into the apple; otherwise reuse the tail as the new head
    bool ate_apple = (new_x == apple_pos_w && new_y == apple_pos_h);

    if (!ate_apple)
    {
        // Detach tail
        node *curr = snake;
        node *prev = NULL;
        while (curr->next != NULL)
        {
            prev = curr;
            curr = curr->next;
        }
        // Reuse tail node as new head
        if (prev != NULL)
        {
            prev->next = NULL;
            curr->x_pos = new_x;
            curr->y_pos = new_y;
            curr->next = snake;
            snake = curr;
        }
        else
        {
            // Snake of length 1: just move the single node
            snake->x_pos = new_x;
            snake->y_pos = new_y;
        }
    }
    else
    {
        // Allocate a new head to grow the snake
        node *new_head = new node;
        new_head->x_pos = new_x;
        new_head->y_pos = new_y;
        new_head->next = snake;
        snake = new_head;
        apple_eaten = true;
        AppleGenerator();
    }
}