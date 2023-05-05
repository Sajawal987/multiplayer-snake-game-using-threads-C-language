#include <termios.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

sem_t lock1;
int select_1[4];
int players;

int a[4];
int m[4], n[4];
int x_axis[4] = {3, 81, 3, 81}, y_axis[4] = {2, 2, 22, 22};
int snake_x[4] = {35, 113, 35, 113};
int snake_y[4] = {2, 2, 22, 22};
int current_Score_x[4] = {3, 81, 3, 81};
int current_Score_y[4] = {19, 19, 39, 39};
int x_1[4] = {3, 81, 3, 81}, x_2[4] = {75, 153, 75, 153}, y_1[4] = {4, 4, 24, 24}, y_2[4] = {17, 17, 37, 37};
int Game_Over_x[4] = {34, 112, 34, 112};
int Game_Over_y[4] = {10, 10, 34, 34};
int Final_Score_x[4] = {32, 110, 32, 110};
int Final_Score_y[4] = {12, 12, 32, 32};
int cleaning_x[4] = {2, 80, 2, 80};
int cleaning_y[4] = {1, 1, 21, 21};
int menu_x1[4] = {20, 98, 20, 98};
int menu_y1[4] = {10, 10, 30, 30};
int menu_x2[4] = {20, 98, 20, 98};
int menu_y2[4] = {12, 12, 32, 32};
int Food_x[4] = {73, 73, 73, 73};
int Food_y[4] = {14, 14, 14, 14};
int Add_Food_x[4] = {3, 81, 3, 81};
int Add_Food_y[4] = {4, 4, 24, 23};
char control_snake_up[4] = "wtix";
char control_snake_down[4] = "sgkv";
char control_snake_left[4] = "afjz";
char control_snake_right[4] = "dhlc";
int input_x[4] = {37, 115, 37, 115};
int input_y[4] = {17, 17, 37, 37};
// volatile char key = 'a';
char key = 'a';
char key_arr[4] = "afjz";
int flags[4] = {0, 0, 0, 0};
int counter = 0;
int score_x[4] = {19, 97, 19, 97};
int score_y[4] = {19, 19, 39, 39};

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"
#define RESET "\x1b[0m"

// macros (constant) -for Bold & Regular text
#define BOLD_FONT "\033[1m"
#define REGULAR_FONT "\033[22m"

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int getch()
{
    int ch;
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt); // save old terminal attributes
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // set new terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply new terminal attributes

    ch = getchar(); // read character

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore old terminal attributes

    return ch;
}

void boundary(int);
void gotoxy(int x, int y)
{
    printf("\033[%d;%df", y, x);
}

void cleaning(int j)
{
    int y;
    for (y = 0; y <= 20; y++)
    {
        gotoxy(cleaning_x[j], cleaning_y[j] + y);
        printf("                                                                              ");
    }
}

int snake(int x[1500], int y[1500], char d[1500], int j)
{
    int i;
    int score = (a[j] - 5) * 100;
    if (x[0] == m[j] && y[0] == n[j])
    {
    take:
        gotoxy(m[j], n[j]);
        printf(" ");
        m[j] = rand() % Food_x[j] + Add_Food_x[j];
        n[j] = rand() % Food_y[j] + Add_Food_y[j];
        for (i = 0; i < a[j]; i++)
            if (x[i] == m[j] && y[i] == n[j])
                goto take;
        a[j]++;
    }
    //-----------------Make Changes-------------------------------
    if (a[j] >= 1406) // 1046 is max score to win
    {
        gotoxy(30, 12);
        printf(BLUE "Congratulation You Won The Game");
        gotoxy(32, 12);
        printf(BLUE "Your Score : %d", score);
        getch();
        return 1;
    }
    //--------------------------------------------------------------
    sem_wait(&lock1);
    gotoxy(m[j], n[j]);
    printf(RED "\u25CB"); // round circle food
    for (i = 0; i < a[j]; i++)
    {
        gotoxy(x[i], y[i]);
        printf("%c", d[i]);
    }
    gotoxy(x[i], y[i]);
    printf(" ");
    gotoxy(score_x[j], score_y[j]);
    printf(YELLOW "%d", score);
    sem_post(&lock1);
    for (i = 1; i < a[j]; i++) // check if head collides with its body than game over
        if (x[0] == x[i] && y[0] == y[i])
            goto out;
    if (x[0] < x_1[j] || x[0] > x_2[j] || y[0] < y_1[j] || y[0] > y_2[j]) // if head touches the boundry
    {
    out:
        gotoxy(Game_Over_x[j], Game_Over_y[j]);
        printf(BLUE "GAME OVER");
        gotoxy(Final_Score_x[j], Final_Score_y[j]);
        printf(BLUE "Your Score : %d", score);
        usleep(1000 * 1000);
        return 1;
    }
    return 0;
}
// https://www.ssec.wisc.edu/~tomw/java/unicode.html#x2500
//  used this site for unicodes
void boundary(int x)
{
    int i, ab = x_axis[x], de = x_axis[x] + 73, cd = y_axis[x], dc = y_axis[x] - 1;
    gotoxy(snake_x[x], snake_y[x]);
    printf(YELLOW "SNAKE GAME %d", x + 1);
    gotoxy(x_axis[x] - 1, dc);
    printf(GREEN "\u2554");
    gotoxy(de, dc);
    printf(GREEN "\u2557");
    gotoxy(x_axis[x] - 1, y_axis[x] + 18);
    printf(GREEN "\u255A");
    gotoxy(de, y_axis[x] + 18);
    printf(GREEN "\u255D");
    for (i = 0; i <= 72; i++, ab++)
    {
        gotoxy(ab, dc);
        printf(GREEN "\u2550");
        gotoxy(ab, dc + 2);
        printf(GREEN "\u2550");
        gotoxy(ab, dc + 17);
        printf(GREEN "\u2550");
        gotoxy(ab, dc + 19);
        printf(GREEN "\u2550");
    }
    de = x_axis[x] + 73, ab = x_axis[x] - 1;
    gotoxy(current_Score_x[x], current_Score_y[x]);
    printf(YELLOW "  Your Score : ");
    for (i = 0; i < 18; i++, cd++)
    {
        gotoxy(ab, cd);
        printf(GREEN "\u2551");
        gotoxy(de, cd);
        printf(GREEN "\u2551");
    }
    gotoxy(ab, y_axis[x] + 1);
    printf(GREEN "\u2560");
    gotoxy(ab, y_axis[x] + 16);
    printf(GREEN "\u2560");
    gotoxy(de, y_axis[x] + 1);
    printf(GREEN "\u2563");
    gotoxy(de, y_axis[x] + 16);
    printf(GREEN "\u2563");
}

void *snake_game(void *k)
{

    int j = *(int *)k;
    j = j - 1;
    // a is snake length, x is the x-axis and y is the y-axis
    int i, c, set, max = 1406, x[1500], y[1500], speed = 150;
    char d[1500];
    m[j] = rand() % Food_x[j] + Add_Food_x[j];
    n[j] = rand() % Food_y[j] + Add_Food_y[j];

    switch (select_1[j])
    {
    case 1:
        if (j == 0)
        { // snake game #1
            for (i = 0; i < max; i++)
                d[i] = '*', x[i] = i + 40, y[i] = 10;
        }
        else if (j == 1)
        { // snake game #2
            for (i = 0; i < max; i++)
                d[i] = '*', x[i] = i + 118, y[i] = 10;
        }
        else if (j == 2)
        { // snake game #3
            for (i = 0; i < max; i++)
                d[i] = '*', x[i] = i + 40, y[i] = 30;
        }
        else
        { // snake game #4
            for (i = 0; i < max; i++)
                d[i] = '*', x[i] = i + 118, y[i] = 30;
        }
        d[0] = '#', a[j] = 5;
        // char key_arr[j] = 'a';
        while (1)
        {
            if ((key == 'a' || key == 's' || key == 'd' || key == 'w') && j == 0) // snake_1
            {
                key_arr[0] = key;
            }
            else if ((key == 't' || key == 'f' || key == 'g' || key == 'h') && j == 1) // snake 2
            {
                key_arr[1] = key;
            }
            else if ((key == 'i' || key == 'j' || key == 'k' || key == 'l') && j == 2) // snake 3
            {
                key_arr[2] = key;
            }
            else if ((key == 'z' || key == 'x' || key == 'c' || key == 'v') && j == 3) // snake 4
            {
                key_arr[3] = key;
            }

            if (key_arr[j] == control_snake_right[j]) // right
            {
                if (x[0] + 1 == x[1])
                    goto same;
                for (i = a[j]; i > 0; i--)
                    y[i] = y[i - 1], x[i] = x[i - 1];
                x[0]++, set = 1;
                if (snake(x, y, d, j))
                {
                    break;
                }

                // goto phir;
            }
            else if (key_arr[j] == control_snake_left[j]) // left
            {

                if (x[0] - 1 == x[1])
                    goto same;
                for (i = a[j]; i > 0; i--)
                    y[i] = y[i - 1], x[i] = x[i - 1];
                x[0] -= 1, set = 2;
                if (snake(x, y, d, j))
                {
                    break;
                }
                // goto phir;
            }
            else if (key_arr[j] == control_snake_up[j]) // top
            {
                if (y[0] - 1 == y[1])
                    goto same;
                for (i = a[j]; i > 0; i--)
                    y[i] = y[i - 1], x[i] = x[i - 1];
                y[0]--, set = 3;
                if (snake(x, y, d, j))
                {
                    break;
                }
                // goto phir;
            }
            else if (key_arr[j] == control_snake_down[j]) // bottom
            {
                if (y[0] + 1 == y[1])
                    goto same;
                for (i = a[j]; i > 0; i--)
                    y[i] = y[i - 1], x[i] = x[i - 1];
                y[0]++, set = 4;
                if (snake(x, y, d, j))
                {
                    break;
                }
                // goto phir;
            }
            else
            {
            same:
                if (set == 1)
                    key_arr[j] = control_snake_right[j];
                else if (set == 2)
                    key_arr[j] = control_snake_left[j];
                else if (set == 3)
                    key_arr[j] = control_snake_up[j];
                else
                    key_arr[j] = control_snake_down[j];
            }
            if (key_arr[j] == control_snake_down[j] || key_arr[j] == control_snake_up[j])
                usleep(speed * 1000);
            else
            {
                usleep(speed / 2 * 1000);
            }
        }
    case 2:
        flags[j] = 1;
        pthread_exit(NULL);
    }
    flags[j] = 1;
    pthread_exit(NULL);
}

void *get_input()
{
    while (flags[0] == 0 || flags[1] == 0 || flags[2] == 0 || flags[3] == 0)
    {

        if (kbhit())
        {
            key = getch();
            if (!key)
                key = getch();
        }
    }
    pthread_exit(NULL);
}

void welcomeArt(void)
{
    printf("\033[32m\n");
    printf("\t\t\t\t\t\t\t\t    _________         _________ 			\n");
    printf("\t\t\t\t\t\t\t\t   /         \\       /         \\ 			\n");
    printf("\t\t\t\t\t\t\t\t  /  /~~~~~\\  \\     /  /~~~~~\\  \\ 			\n");
    printf("\t\t\t\t\t\t\t\t  |  |     |  |     |  |     |  | 			\n");
    printf("\t\t\t\t\t\t\t\t  |  |     |  |     |  |     |  | 			\n");
    printf("\t\t\t\t\t\t\t\t  |  |     |  |     |  |     |  |         /	\n");
    printf("\t\t\t\t\t\t\t\t  |  |     |  |     |  |     |  |       //	\n");
    printf("\t\t\t\t\t\t\t\t (o  o)    \\  \\_____/  /     \\  \\_____/ / 	\n");
    printf("\t\t\t\t\t\t\t\t  \\__/      \\         /       \\        / 	\n");
    printf("\t\t\t\t\t\t\t\t    |        ~~~~~~~~~         ~~~~~~~~ 		\n");
    printf("\t\t\t\t\t\t\t\t    ^											\n");
    printf("\t\t\t\t\t\t\t		Welcome To The Snake Game!			\n\n");
    printf(BOLD_FONT RED "\t\t\t\t\t\t\t		Made By :			\n");
    printf(BOLD_FONT RED "\t\t\t\t\t\t\t		  Sajawal ( 2021-CS-602 )			\n");
    printf(BOLD_FONT RED "\t\t\t\t\t\t\t     	Press Any Key To Continue......	\n");
    printf(BOLD_FONT RED "\n\033[0m");
    getch();
    return;
}
void main()
{

    sem_init(&lock1, 0, 1); // binary semaphore  mutex lock
    system("clear");
    welcomeArt();
    do
    {
        printf(MAGENTA "Enter The Number of Player (Max 4):");
        scanf("%d", &players);
        if (players > 4 || players < 0)
        {
            printf(BOLD_FONT RED "\aWrong Input\n");
            // system("clear";)
        }
    } while (players > 4 || players < 0);
    system("clear");
    int j;
    for (j = 0; j < players; j++)
    {
        gotoxy(snake_x[j], snake_y[j]);
        printf(RED "SNAKE GAME %d ", j + 1);
        gotoxy(menu_x1[j], menu_y1[j]);
        printf(YELLOW "1.) START\t\t\tpress 1");
        gotoxy(menu_x2[j], menu_y2[j]);
        printf("2.) Exit \t\t\tpress 2");
    }
    for (j = 0; j < players; j++)
    {
        do
        {
            gotoxy(input_x[j], input_y[j]);
            scanf("%d", &select_1[j]);
            if ((select_1[j] > 2 || select_1[j] < 0))
            {
                gotoxy(input_x[j], input_y[j]);
                printf("     ");
                printf("\a");
            }
        } while (select_1[j] > 2 || select_1[j] < 0);
    }
    for (int j = 0; j < players; j++)
    {
        if (select_1[j] == 1)
        {
            cleaning(j);
            boundary(j);
        }
    }
    players += 1;
    pthread_t *snake = (pthread_t *)malloc(sizeof(pthread_t) * (players));
    int i;
    int *index[4];
    for (i = 0; i < players - 1; i++)
    {
        index[i] = &i;
    }
    for (i = 0; i < players; i++)
    {
        if (i == players - 1)
        {
            pthread_create(&snake[i], NULL, &get_input, (void *)index[i]);
        }
        else
        {
            pthread_create(&snake[i], NULL, &snake_game, (void *)index[i]);
        }
    }
    for (i = 0; i < players; i++)
    {
        pthread_join(snake[i], NULL);
    }
    system("clear");
}
