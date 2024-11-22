#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define HEIGHT 20
#define WIDTH 60

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    STOP
};

int score;
int fruit_x, fruit_y;
int head_x, head_y;
int tail_x[WIDTH * HEIGHT], tail_y[WIDTH * HEIGHT];

enum Direction dir;

void setup();
void clear_screen();
void draw();
void input();
void logic();

struct termios old_props;
void set_terminal_attributes();
void reset_terminal_attributes();

int input_available();

int main()
{
    srand(time(NULL));
    set_terminal_attributes();
    setup();
    while (1)
    {
        draw();
        input();
        logic();
        int sleep_time = 300000 - score * 1000;
        usleep(sleep_time);
    }
}

void logic()
{
    for (int i = score - 1; i > 0; i--)
    {
        tail_x[i] = tail_x[i - 1];
        tail_y[i] = tail_y[i - 1];
    }
    tail_x[0] = head_x;
    tail_y[0] = head_y;

    switch (dir)
    {
    case UP:
        head_y--;
        break;
    case DOWN:
        head_y++;
        break;
    case LEFT:
        head_x--;
        break;
    case RIGHT:
        head_x++;
        break;
    default:
        break;
    }

    if (head_x < 0)
    {
        head_x = WIDTH - 1;
    }
    else if (head_x >= WIDTH)
    {
        head_x = 0;
    }

    if (head_y < 0)
    {
        head_y = HEIGHT - 1;
    }
    else if (head_y >= HEIGHT)
    {
        head_y = 0;
    }

    for (int i = 0; i < score; i++)
    {
        if (head_x == tail_x[i] && head_y == tail_y[i])
        {
            printf("Game Over\n");
            exit(0);
        }
    }

    if (head_x == fruit_x && head_y == fruit_y)
    {
        score++;
        fruit_x = rand() % WIDTH;
        fruit_y = rand() % HEIGHT;
    }
}

void clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void setup()
{
    score = 0;
    head_x = WIDTH / 2;
    head_y = HEIGHT / 2;
    fruit_x = rand() % WIDTH;
    fruit_y = rand() % HEIGHT;
    dir = STOP;
}

void draw()
{
    clear_screen();
    printf("\t\tWelcome to the Snake Game\n");
    for (int i = 0; i < WIDTH + 2; i++)
    {
        printf("#");
    }
    for (int i = 0; i < HEIGHT; i++)
    {
        printf("\n#");
        for (int j = 0; j < WIDTH; j++)
        {
            if (i == head_y && j == head_x)
            {
                printf("O");
            }
            else if (i == fruit_y && j == fruit_x)
            {
                printf("F");
            }
            else
            {
                int flag = 0;
                for (int k = 0; k < score; k++)
                {
                    if (i == tail_y[k] && j == tail_x[k])
                    {
                        printf("o");
                        flag = 1;
                        break;
                    }
                }
                if (!flag)
                {
                    printf(" ");
                }
            }
        }
        printf("#");
    }
    printf("\n");
    for (int i = 0; i < WIDTH + 2; i++)
    {
        printf("#");
    }
    printf("\nScore: %d\n", score);
}

void set_terminal_attributes()
{
    tcgetattr(STDIN_FILENO, &old_props);
    atexit(reset_terminal_attributes);
    struct termios new_props = old_props;
    new_props.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_props);
}

void reset_terminal_attributes()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &old_props);
}

int input_available()
{
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

void input()
{
    if (!input_available())
    {
        return;
    }
    char c = getchar();
    switch (c)
    {
    case 'w':
        dir = UP;
        break;
    case 's':
        dir = DOWN;
        break;
    case 'a':
        dir = LEFT;
        break;
    case 'd':
        dir = RIGHT;
        break;
    case 'x':
        printf("Game Exited Successfully\n");
        exit(0);
        break;
    default:
        break;
    }
}