#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <conio.h>

//prototypes of our functions
void gotoxy(int, int);
void control(char *, char *, int *, int *, int *, int *, int *);
void draw_frame();
void draw_ship(int, int);
void move_ship(int *, int *, char);
void show_health(int);
void draw_bullet(int, int);
void clear_bullet(int, int);
void move_bullet(int *, int *, int *, int *, int *, int *, int *, int *);
void setcursor(bool);
void setcolor(int, int);
char cursor(int, int);
void draw_star();
void star_fragment(int *, int *, int *);
void update(int *); //when star is destroyed
void explosion(int, int);
void show_score(int);
void HUD(int, int);
void ship_damage(int *, int *, int *);
void set_game_over(int);
void game_over();

int gameOverState = 0;

int main()
{
    char ch = '.', ship_direction;
    int x = 38, y = 20,        //first position of our ship
        bx, by, i, bullet = 0, //location of bullet and number of bullet
        score = 0,             // score and its pointer
        ship_hp = 5,           // ship's health point
        fragment[5] = {0, 0, 0, 0, 0}, fx[5], fy[5], f_index = 0;

    //place our stars accross the screen randomly
    draw_star();

    draw_ship(x, y); //place the ship in the screen first
    setcursor(0);
    setcolor(15, 0);

    show_score(score);

    draw_frame();
    HUD(bullet, ship_hp);

    //main game loop
    do
    {
        control(&ch, &ship_direction, &bullet, &x, &y, &bx, &by);
        move_bullet(&bullet, &bx, &by, &score, fragment, &f_index, fx, fy);
        move_ship(&x, &y, ship_direction);
        show_score(score);
        star_fragment(fragment, fx, fy);
        ship_damage(&x, &y, &ship_hp);
        HUD(bullet, ship_hp);
        set_game_over(ship_hp);

        Sleep(50);

    } while (ch != 'x' && gameOverState == 0);
    game_over();
    return 0;
}

void gotoxy(int x, int y)
{
    COORD c = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

char cursor(int x, int y)
{
    HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
    char buf[2];
    COORD c = {x, y};
    DWORD num_read;
    if (
        !ReadConsoleOutputCharacter(hStd, (LPTSTR)buf, 1, c, (LPDWORD)&num_read))

        return '\0';
    else
        return buf[0];
}

void draw_frame()
{
    setcolor(1, 0);
    for (int i = 0; i < 82; i++)
    {
        gotoxy(i, 21);
        printf("X");
        setcursor(0);
    }
    for (int i = 0; i < 21; i++)
    {
        gotoxy(81, i);
        printf("X");
        setcursor(0);
    }
}

void draw_ship(int x, int y)
{
    gotoxy(x, y);
    setcursor(0);
    setcolor(11, 0);
    printf(" <-0-> ");
}

void control(char *ch, char *direction, int *bullet, int *x, int *y, int *bx, int *by)
{
    if (_kbhit())
    {
        *ch = _getch();
        if (*ch == 'a' || *ch == 'A') // press a button
        {
            *direction = 'l'; //left
        }
        if (*ch == 'd' || *ch == 'D') //press d button
        {
            *direction = 'r'; //right
        }
        if (*ch == 's' || *ch == 'S') //press s button
        {
            *direction = 'h'; //halt
        }
        if (*bullet != 1 && (*ch == ' ' || *ch == 'w' || *ch == 'W')) //space bar or w
        {
            *bullet = 1;
            *bx = *x + 3;
            *by = *y - 1;
            Beep(700, 50);
        }
        fflush(stdin);
    }
}

void move_ship(int *x, int *y, char direction)
{
    if (direction == 'l')
    {
        if (*x == 0)
        {
            draw_ship(0, *y);
        }
        else
        {
            draw_ship(--*x, *y);
        }
    }
    else if (direction == 'r')
    {
        if (*x == 73)
        {
            draw_ship(73, *y);
        }
        else
        {
            draw_ship(++*x, *y);
        }
    }
    else if (direction == 'h')
    {
        draw_ship(*x, *y);
    }
}

void show_health(int hp)
{
    gotoxy(83, 23);
    setcolor(15, 0);
    printf("HP: ");
    if (hp == 5)
    {
        setcolor(11, 0);
        printf("##########");
    }
    else if (hp == 4)
    {
        setcolor(11, 0);
        printf("########  ");
    }
    else if (hp == 3)
    {
        setcolor(11, 0);
        printf("######    ");
    }
    else if (hp == 2)
    {   
        setcolor(12,0);
        printf("####      ");
    }
    else if (hp == 1)
    {   
        setcolor(12,0);
        printf("##        ");
    }
    else if (hp == 0)
    {
        printf("          ");
    }
}

void draw_bullet(int x, int y)
{
    setcolor(12, 0);
    gotoxy(x, y);
    printf("A");
    setcursor(0);
}

void clear_bullet(int x, int y)
{
    gotoxy(x, y);
    printf(" ");
    setcursor(0);
}

void move_bullet(int *bullet, int *bx, int *by, int *score, int *fragment, int *i, int *fx, int *fy)
{
    if (*bullet == 1)
    {
        clear_bullet(*bx, *by);
        if (*by == 2)
        {
            *bullet = 0;
        }
        else if (cursor(*bx, *by - 1) == '*')
        {
            clear_bullet(*bx, *by);
            *bullet = 0;
            explosion(*bx, *by - 1);
            update(score);
            *(fragment + *i) = 1;
            *(fx + *i) = *bx;
            *(fy + *i) = *by + 4;
            *i++;
        }
        else if (*bullet == 1)
        {
            draw_bullet(*bx, --*by);
        }
    }
}

void setcolor(int fg, int bg)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}

void setcursor(bool visible)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = 20;
    SetConsoleCursorInfo(console, &lpCursor);
}

void draw_star()
{
    srand(time(NULL));
    int temp_a, temp_b;
    for (int i = 0; i < 20; i++)
    {
        temp_a = 10 + rand() % 60;
        temp_b = 2 + rand() % 3;
        while (cursor(temp_a, temp_b) == '*')
        {
            temp_a = 10 + rand() % 60;
            temp_b = 2 + rand() % 3;
        }
        gotoxy(temp_a, temp_b);
        setcolor(13, 0);
        printf("*");
        setcursor(0);
    }
}

void explosion(int x, int y)
{
    gotoxy(x, y);
    setcolor(12, 0);
    printf("o");
    Beep(600, 100);
    gotoxy(x, y);
    setcolor(14, 0);
    printf("O");
    Beep(600, 100);
    gotoxy(x, y);
    setcolor(12, 0);
    printf("O");
    Beep(600, 100);
    gotoxy(x, y);
    setcolor(14, 0);
    printf("x");
    Beep(600, 100);
    gotoxy(x, y);
    printf(" ");
}

void update(int *score)
{
    int temp_a = 10 + rand() % 60, temp_b = 2 + rand() % 3;
    while (cursor(temp_a, temp_b) == '*' || cursor(temp_a, temp_b) == 'o' || cursor(temp_a, temp_b) == 'x' || cursor(temp_a, temp_b) == 'O')
    {
        temp_a = 10 + rand() % 60;
        temp_b = 2 + rand() % 3;
    }
    gotoxy(temp_a, temp_b);
    setcolor(13, 0);
    printf("*");
    setcursor(0);
    *score += 1;
}

void show_score(int score)
{
    gotoxy(83, 3);
    setcolor(15, 0);
    printf("Your score: %d", score);
    setcursor(0);
}

void HUD(int ammo, int health)
{
    gotoxy(83, 5);
    setcolor(15, 0);
    printf("plasma cannon:");
    if (ammo != 1)
    {
        setcolor(13, 0);
        printf("##########");
    }
    else
    {
        printf("          ");
    }
    
    setcolor(15,0);
    gotoxy(83,8);
    printf("Tantatorn Suksangwarn");
    gotoxy(83,9);
    printf("61010402 1D");

    show_health(health);
}

void star_fragment(int *fragment, int *fx, int *fy)
{
    for (int i = 0; i < 5; i++)
    {
        if (*(fragment + i) == 1 && *fy != 19)
        {
            if (*fy == 18)
            {
                gotoxy(*(fx + i), *(fy + i));
                printf("     ");
            }
            else
            {
                gotoxy(*(fx + i), *(fy + i));
                printf("     ");
                gotoxy(*(fx + i), ++*(fy + i));
                setcolor(13, 0);
                printf("V V V");
            }
        }
    }
}

void ship_damage(int *x, int *y, int *hp)
{

    for (int i = 1; i < 4; i++)
    {
        if (cursor(*x + i, *y - 2) == 'V')
        {
            *hp -= 1;
            Beep(500, 100);
        }
    }
}

void set_game_over(int hp)
{
    if (hp == 0)
    {
        system("CLS");
        gameOverState = 1;
    }
}
void game_over()
{
    gotoxy(35, 10);
    printf("GAME OVER!");
    Beep(330, 100);
    Sleep(100);
    Beep(330, 100);
    Sleep(300);
    Beep(330, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(100);
    Beep(330, 100);
    Sleep(300);
    Beep(392, 100);
    Sleep(700);
    Beep(196, 100);
    Sleep(700);
    Beep(262, 300);
    Sleep(300);
    Beep(196, 300);
    Sleep(300);
    Beep(164, 300);
    Sleep(300);
    Beep(220, 300);
    Sleep(100);
    Beep(246, 100);
    Sleep(300);
    Beep(233, 200);
    Beep(220, 100);
    Sleep(300);
    Beep(196, 100);
    Sleep(150);
    Beep(330, 100);
    Sleep(150);
    Beep(392, 100);
    Sleep(150);
    Beep(440, 100);
    Sleep(300);
    Beep(349, 100);
    Sleep(100);
    Beep(392, 100);
    Sleep(300);
    Beep(330, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(100);
    Beep(294, 100);
    Sleep(100);
    Beep(247, 100);
    Sleep(500);
    Beep(262, 300);
    Sleep(300);
    Beep(196, 300);
    Sleep(300);
    Beep(164, 300);
    Sleep(300);
    Beep(220, 300);
    Sleep(100);
    Beep(246, 100);
    Sleep(300);
    Beep(233, 200);
    Beep(220, 100);
    Sleep(300);
    Beep(196, 100);
    Sleep(150);
    Beep(330, 100);
    Sleep(150);
    Beep(392, 100);
    Sleep(150);
    Beep(440, 100);
    Sleep(300);
    Beep(349, 100);
    Sleep(100);
    Beep(392, 100);
    Sleep(300);
    Beep(330, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(100);
    Beep(294, 100);
    Sleep(100);
    Beep(247, 100);
    Sleep(900);
    Beep(392, 100);
    Sleep(100);
    Beep(370, 100);
    Sleep(100);
    Beep(349, 100);
    Sleep(100);
    Beep(311, 100);
    Sleep(300);
    Beep(330, 100);
    Sleep(300);
    Beep(207, 100);
    Sleep(100);
    Beep(220, 100);
    Sleep(100);
    Beep(262, 100);
    Sleep(300);
    Beep(220, 100);
    Sleep(100);
    Beep(262, 100);
    Sleep(100);
    Beep(294, 100);
    Sleep(500);
    Beep(392, 100);
    Sleep(100);
    Beep(370, 100);
    Sleep(100);
    Beep(349, 100);
    Sleep(100);
    Beep(311, 100);
    Sleep(300);
    Beep(330, 100);
    Sleep(300);
    Beep(523, 100);
    Sleep(300);
    Beep(523, 100);
    Sleep(100);
    Beep(523, 100);
    Sleep(1100);
    Beep(392, 100);
    Sleep(100);
    Beep(370, 100);
    Sleep(100);
    Beep(349, 100);
    Sleep(100);
    Beep(311, 100);
    Sleep(300);
    Beep(330, 100);
    Sleep(300);
    Beep(207, 100);
    Sleep(100);
    Beep(220, 100);
    Sleep(100);
    Beep(262, 100);
    Sleep(300);
    Beep(220, 100);
    Sleep(100);
    Beep(262, 100);
    Sleep(100);
    Beep(294, 100);
    Sleep(500);
    Beep(311, 300);
    Sleep(300);
    Beep(296, 300);
    Sleep(300);
    Beep(262, 300);
    Sleep(1300);
    Beep(262, 100);
    Sleep(100);
    Beep(262, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(100);
    Beep(294, 100);
    Sleep(300);
    Beep(330, 200);
    Sleep(50);
    Beep(262, 200);
    Sleep(50);
    Beep(220, 200);
    Sleep(50);
    Beep(196, 100);
    Sleep(700);
    Beep(262, 100);
    Sleep(100);
    Beep(262, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(100);
    Beep(294, 100);
    Sleep(100);
    Beep(330, 100);
    Sleep(700);
    Beep(440, 100);
    Sleep(300);
    Beep(392, 100);
    Sleep(500);
    Beep(262, 100);
    Sleep(100);
    Beep(262, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(100);
    Beep(294, 100);
    Sleep(300);
    Beep(330, 200);
    Sleep(50);
    Beep(262, 200);
    Sleep(50);
    Beep(220, 200);
    Sleep(50);
    Beep(196, 100);
    Sleep(700);
    /*Intro*/
    Beep(330, 100);
    Sleep(100);
    Beep(330, 100);
    Sleep(300);
    Beep(330, 100);
    Sleep(300);
    Beep(262, 100);
    Sleep(100);
    Beep(330, 100);
    Sleep(300);
    Beep(392, 100);
    Sleep(700);
    Beep(196, 100);
    Sleep(700);
    Beep(196, 100);
    Sleep(125);
    Beep(262, 100);
    Sleep(125);
    Beep(330, 100);
    Sleep(125);
    Beep(392, 100);
    Sleep(125);
    Beep(523, 100);
    Sleep(125);
    Beep(660, 100);
    Sleep(125);
    Beep(784, 100);
    Sleep(575);
    Beep(660, 100);
    Sleep(575);
    Beep(207, 100);
    Sleep(125);
    Beep(262, 100);
    Sleep(125);
    Beep(311, 100);
    Sleep(125);
    Beep(415, 100);
    Sleep(125);
    Beep(523, 100);
    Sleep(125);
    Beep(622, 100);
    Sleep(125);
    Beep(830, 100);
    Sleep(575);
    Beep(622, 100);
    Sleep(575);
    Beep(233, 100);
    Sleep(125);
    Beep(294, 100);
    Sleep(125);
    Beep(349, 100);
    Sleep(125);
    Beep(466, 100);
    Sleep(125);
    Beep(587, 100);
    Sleep(125);
    Beep(698, 100);
    Sleep(125);
    Beep(932, 100);
    Sleep(575);
    Beep(932, 100);
    Sleep(125);
    Beep(932, 100);
    Sleep(125);
    Beep(932, 100);
    Sleep(125);
    Beep(1046, 675);
}