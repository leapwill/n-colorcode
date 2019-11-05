#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include "n-colorcode.h"

int main(__attribute__((unused)) int argc, __attribute__((unused)) char const *argv[])
{
    srand(time(NULL));
    int ans[4] = {rand() % 6 + 1, rand() % 6 + 1, rand() % 6 + 1, rand() % 6 + 1};

    // TODO: command-line options

    // region window setup
    initscr();
    if (has_colors() == FALSE)
    {
        endwin();
        puts("Your terminal does not support color.");
        exit(1);
    }

    start_color();
    init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_WHITE);
    init_color(COLOR_GRAY, 200, 200, 200);
    init_pair(COLOR_GRAY, COLOR_GRAY, COLOR_BLACK);
    char *COLORS[] = {"", "Red", "Green", "Yellow", "Blue", "Magenta", "Cyan"};

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    // endregion

    // region game setup
    int *game[11];
    game[0] = calloc(6, sizeof(int));
    game[0][0] = COLOR_RED, game[0][1] = COLOR_GREEN, game[0][2] = COLOR_YELLOW;
    game[0][3] = COLOR_BLUE, game[0][4] = COLOR_MAGENTA, game[0][5] = COLOR_CYAN;

    for (int i = 1; i < 11; i++)
    {
        game[i] = calloc(4, sizeof(int));
    }

    int selector1[2] = {4, 4};
    for (int i = 1; i <= 6; i++)
    {
        printColorBlock(selector1[0], selector1[1], i);
        selector1[0] += 3;
    }
    selector1[0] = 4;
    selector1[1] = 7;
    mvaddch(selector1[0], selector1[1], '*');

    for (int i = 0; i < 23; i++)
    {
        mvaddch(i, 10, ACS_VLINE);
    }

    // setup board
    int selector2[2] = {2, 14};
    for (int i = selector2[0]; i < selector2[0] + 20; i += 2)
    {
        for (int j = selector2[1]; j <= selector2[1] + 15; j++)
        {
            if ((j - selector2[1]) % 4 != 0)
            {
                mvaddch(i, j, '_');
            }
        }
    }
    selector2[0] = 3;
    selector2[1] = 16;
    mvaddch(selector2[0], selector2[1], '*');

    move(23, 79);
    refresh();
    // endregion

    // region game loop
    // TODO debug mode to print answer
    // printMsg("ans is %d %d %d %d", ans[0], ans[1], ans[2], ans[3]);
    int attempt = 1;
    while (true)
    {
        char inp = getch();
        // clear message row
        move(23, 0);
        clrtoeol();

        // handle input
        if (inp == 'q')
        {
            break;
        }
        switch (inp)
        {
        case 3: // arrow up
            mvaddch(selector1[0], selector1[1], ' ');
            selector1[0] -= 3;
            if (selector1[0] < 4)
            {
                selector1[0] = 19;
            }
            mvaddch(selector1[0], selector1[1], '*');
            break;
        case 2: // arrow down
            mvaddch(selector1[0], selector1[1], ' ');
            selector1[0] += 3;
            if (selector1[0] > 19)
            {
                selector1[0] = 4;
            }
            mvaddch(selector1[0], selector1[1], '*');
            break;
        case 32:; // space (select)
            // translate selector1 to color
            int idx1 = (selector1[0] - 4) / 3;
            // TODO debug mode
            // printMsg("selected color idx %d", idx1);
            int color = game[0][idx1];

            // translate selector2 to board slot
            int idx2 = (selector2[1] - 16) / 4;
            game[attempt][idx2] = color;

            // put color on board
            printColorBlock(selector2[0] - 1, selector2[1] - 1, color);
            __attribute__((fallthrough));
        case 5: // arrow right
            mvaddch(selector2[0], selector2[1], ' ');
            selector2[1] += 4;
            if (selector2[1] > 28)
            {
                selector2[1] = 16;
            }
            mvaddch(selector2[0], selector2[1], '*');
            break;
        case 4: // arrow left
            mvaddch(selector2[0], selector2[1], ' ');
            selector2[1] -= 4;
            if (selector2[1] < 16)
            {
                selector2[1] = 28;
            }
            mvaddch(selector2[0], selector2[1], '*');
            break;
        case 10:; // enter (submit)
            if (game[attempt][0] == 0 || game[attempt][1] == 0 || game[attempt][2] == 0 || game[attempt][3] == 0)
            {
                printMsg("incomplete attempt");
                break;
            }
            int result[4] = {0, 0, 0, 0};
            int usedAns[4] = {FALSE, FALSE, FALSE, FALSE};
            // calculate result
            // first set usedAns based on exact match
            for (int i = 0; i < 4; i++)
            {
                int color = game[attempt][i];
                if (color == ans[i])
                {
                    usedAns[i] = TRUE;
                    result[i] = 2;
                }
            }
            // then set wrong-location match
            for (int i = 0; i < 4; i++)
            {
                int color = game[attempt][i];
                for (int a = 0; a < 4; a++)
                {
                    if (usedAns[a] == FALSE && color == ans[a] && result[i] == 0) // don't reuse ans[x] or overwrite result[x]
                    {
                        usedAns[a] = TRUE;
                        result[i] = 1;
                        break;
                    }
                }
            }

            // bubble sort result for obfuscation
            int isUnsorted = TRUE;
            do
            {
                isUnsorted = FALSE;
                for (int i = 0; i < 3; i++)
                {
                    if (result[i] < result[i+1])
                    {
                        isUnsorted = TRUE;
                        int swap = result[i];
                        result[i] = result[i+1];
                        result[i+1] = swap;
                    }
                }
            } while (isUnsorted == TRUE);
            

            // show result
            int resultPos[2] = {selector2[0] - 1, 33};
            move(resultPos[0], resultPos[1]);
            for (int i = 0; i < 4; i++)
            {
                switch (result[i])
                {
                case 1: // color right but not location
                    addch(ACS_CKBOARD | COLOR_PAIR(COLOR_GRAY));
                    break;
                case 2: // color and location right
                    addch(ACS_CKBOARD | COLOR_PAIR(COLOR_WHITE));
                default:
                    break;
                }
            }
            if (result[0] == 2 && result[1] == 2 && result[2] == 2 && result[3] == 2)
            {
                printMsg("win");
                getch();
                quit(0);
            }
            else if (attempt >= 10)
            {
                // lose, out of attempts
                printMsg("lose, answer was %s %s %s %s", COLORS[ans[0]], COLORS[ans[1]], COLORS[ans[2]], COLORS[ans[3]]);
                getch();
                quit(0);
            }
            else
            {
                attempt++;
                mvaddch(selector2[0], selector2[1], ' ');
                selector2[0] += 2;
                mvaddch(selector2[0], selector2[1], '*');
            }
            break;
        default:
            break;
        }

        move(23, 79);
        refresh();
    }

    // endregion
    endwin();
    exit(0);
    return 0;
}

int quit(int status)
{
    endwin();
    exit(status);
    return status;
}

void printMsg(const char *str, ...)
{
    va_list argp;
    va_start(argp, str);
    move(23, 0);
    vw_printw(stdscr, str, argp);
}

void printColorBlock(int y, int x, int colorPair)
{
    move(y, x);
    addch(ACS_CKBOARD | COLOR_PAIR(colorPair));
    addch(ACS_CKBOARD | COLOR_PAIR(colorPair));
    addch(ACS_CKBOARD | COLOR_PAIR(colorPair));
}
