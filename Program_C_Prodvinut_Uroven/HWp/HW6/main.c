/*
//------------------------------------------------------
//       Игра змейка 5
//------------------------------------------------------ 
1.  Поедание зерна змейкой
Такое событие возникает, когда координаты головы совпадают с координатой зерна. В
этом случае зерно помечается как enable=0.
Проверка того, является ли какое-то из зерен съеденным, происходит при помощи функции
логической функции _Bool haveEat(struct snake_t *head, struct food f[]): в этом
случае происходит увеличение хвоста на 1 элемент функцией void addTail(struct
snake_t *head).
В цикле for(size_t i=0; i<MAX_FOOD_SIZE; i++) происходит проверка наличия еды и
совпадения координат head->x == f[i].x и head->y == f[i].y.
В случае выполнения условий enable=0 и возвращается единица.
За увеличение хвоста — отвечает функция addTail(&snake).
В структуре head параметр длины tsize увеличивается на единицу.
head->tsize++;

2. Проверка корректности выставления зерна
Написать функцию, которая будет проверять корректность выставления зерна
на поле
void repairSeed(struct food f[], size_t nfood, struct snake_t *head).
Она должна исключать ситуацию, когда хвост змейки совпадает с зерном, то
есть
f[j].x == head->tail[i].x и f[j].y == head->tail[i].y в моменты, когда
f[i].enable.
А также не допускать размещения двух зерен в одной точке игрового поля:
i!=j && f[i].enable && f[j].enable && f[j].x == f[i].x && f[j].y ==
f[i].y && f[i].enable

3. Добавление второй змейки
Добавим в программу вторую змею.
Вместо одной змеи передаём массив данных. PLAYERS определяем
количество игроков (змей) в программе.

*/

#include <stdio.h>      // стандартный ввод-вывод (printf, etc.)
#include <stdlib.h>     // функции работы с памятью (malloc, free)
#include <time.h>       // работа со временем (clock)
#include <ncurses.h>    // библиотека для работы с «графикой» в терминале
#include <inttypes.h>   // типы с фиксированной разрядностью (int32_t)
#include <string.h>     // функции работы со строками (strlen)
#include <unistd.h>     // POSIX-функции (здесь напрямую не нужны, но можно оставить)
#include <ctype.h>      //Для tolower

//------------------------------------------------------
// Константы и перечисления
//------------------------------------------------------

// Минимальная координата Y, с которой может двигаться змейка.
// Строки 0 и 1 используются под текст/подсказки.
#define MIN_Y 1
#define CONTROLS 3
#define MAX_PLAYERS 3   // Максимальное количество змей
#define PERIOD_SH_ZM 150 //Период шага змейки в мс

// Перечисление направлений движения змейки и кода остановки игры.
// STOP_GAME соответствует нажатию клавиши F10 (макрос KEY_F(10) из ncurses).
enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10)};

// Разные игровые параметры
enum {
    MAX_TAIL_SIZE       = 100,  // максимальная длина хвоста
    START_TAIL_SIZE     = 3,    // начальная длина хвоста
    MAX_FOOD_SIZE       = 200,  // максимальное количество еды
    SEED_NUMBER         = 30,   // количество еды на поле
    FOOD_EXPIRE_SECONDS = 10    // время жизни еды
};

enum SnakeType 
{
    SNAKE_HUMAN = 0,
    SNAKE_BOT   = 1
};

//------------------------------------------------------
// Структуры
//------------------------------------------------------
typedef struct tail_t {
    int x;
    int y;
} tail_t;

// Структура, описывающая коды клавиш управления змейкой.
struct control_buttons {
    int down;
    int up;
    int left;
    int right;
} control_buttons;

// Набор кнопок по умолчанию — стрелки и WASD (верхний/нижний регистр).
struct control_buttons default_controls[CONTROLS] = {
    {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT}, // 0: стрелки
    {'S', 'W', 'A', 'D'},                    // 1: верхний регистр                
    {'s', 'w', 'a','d'}                      // 2: нижний регистр
};

typedef struct snake_t {
    int x;
    int y;
    int direction;
    size_t tsize;
    tail_t *tail;
    struct control_buttons controls;
    int color_id;      // номер цветовой пары для этой змейки
} snake_t;

typedef struct food {
    int x;
    int y;
    time_t put_time;
    char point;
    uint8_t enable;
} food_t;

// Настройки игры
typedef struct {
    int mode;                        // режим игры 1..4
    int num_snakes;                  // фактическое количество змей (<= MAX_PLAYERS)

    int snake_type[MAX_PLAYERS];     // SNAKE_HUMAN или SNAKE_BOT
    int control_index[MAX_PLAYERS];  // 0 - arrows, 1 - WSAD, 2 - wsad
    int color_id[MAX_PLAYERS];       // номер цветовой пары змей
    int food_color_id;               // номер цветовой пары для еды
} GameSettings;

// Глобальная конфигурация
GameSettings g_cfg;

// Прототипы
void setColor(int objectType);
void resetColor(void);
int menuChooseColor(const char *title, int default_color);
void startMenu(GameSettings *cfg);

//------------------------------------------------------
// Инициализация
//------------------------------------------------------
void initTail(tail_t t[], size_t size)
{
    tail_t init_t = {0, 0};
    for (size_t i = 0; i < size; i++)
        t[i] = init_t;
}

void initHead(snake_t *head, int x, int y)
{
    head->x = x;
    head->y = y;
    head->direction = RIGHT;
}

void initSnake(snake_t **head, size_t size, int x, int y,
               int ctrl_index, int color_id)
{
    *head = malloc(sizeof(snake_t));
    if (!*head)
        exit(EXIT_FAILURE);

    tail_t* tail = malloc(MAX_TAIL_SIZE * sizeof(tail_t));
    if (!tail)
        exit(EXIT_FAILURE);
    initTail(tail, MAX_TAIL_SIZE);

    initHead(*head, x, y);

    (*head)->tail     = tail;
    (*head)->tsize    = size;
    (*head)->controls = default_controls[ctrl_index % CONTROLS];
    (*head)->color_id = color_id;
}

// Размер окна с поправкой на индексы
void razmer_okna(int *y, int *x)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    cols -= 1;
    rows -= 1;

    *x = cols;
    *y = rows;
}

// Инициализация еды
void initFood(food_t f[], size_t size)
{
    food_t init = {0,0,0,0,0};

    int max_x, max_y;
    razmer_okna(&max_y, &max_x);

    for(size_t i=0; i<size; i++)
        f[i] = init;
}

//------------------------------------------------------
// Движение головы
//------------------------------------------------------
void go(snake_t *head)
{
    char ch = '@';

    int max_x, max_y;
    razmer_okna(&max_y, &max_x);

    switch (head->direction)
    {
        case LEFT:
            if (head->x <= 0)
                head->x = max_x;
            else
                head->x--;
            break;
        case RIGHT:
            if (head->x >= max_x)
                head->x = 0;
            else
                head->x++;
            break;
        case UP:
            if (head->y <= MIN_Y)
                head->y = max_y;
            else
                head->y--;
            break;
        case DOWN:
            if (head->y >= max_y)
                head->y = MIN_Y;
            else
                head->y++;
            break;
        default:
            break;
    }

    mvprintw(head->y, head->x, "%c", ch);
}

//------------------------------------------------------
// Движение хвоста
//------------------------------------------------------
void goTail(snake_t *head, int head_prev_x, int head_prev_y)
{
    char ch = '*';

    mvprintw(head->tail[head->tsize - 1].y,
             head->tail[head->tsize - 1].x,
             " ");

    for (size_t i = head->tsize - 1; i > 0; i--)
        head->tail[i] = head->tail[i - 1];

    head->tail[0].x = head_prev_x;
    head->tail[0].y = head_prev_y;

    for (size_t i = 0; i < head->tsize; ++i)
    {
        if (head->tail[i].x || head->tail[i].y)
            mvprintw(head->tail[i].y,
                     head->tail[i].x,
                     "%c", ch);
    }
}

//------------------------------------------------------
// Запрещаем разворот змейки на 180°
//------------------------------------------------------
int checkDirection(snake_t* snake, int32_t key)
{
    if ((snake->direction == LEFT  && key == RIGHT) ||
        (snake->direction == RIGHT && key == LEFT ) ||
        (snake->direction == UP    && key == DOWN ) ||
        (snake->direction == DOWN  && key == UP   ))
    {
        return 1;
    }
    return 0;
}

//------------------------------------------------------
// Изменение направления
//------------------------------------------------------
void changeDirection(snake_t *snake, const int32_t key)
{
    int new_dir = snake->direction;
    struct control_buttons *c = &snake->controls;
    int k = key;

    if (c->down == 's')  // схема wasd
        k = tolower(key);

    if (k == c->down)
        new_dir = DOWN;
    else if (k == c->up)
        new_dir = UP;
    else if (k == c->right)
        new_dir = RIGHT;
    else if (k == c->left)
        new_dir = LEFT;
    else
        return;

    if (checkDirection(snake, new_dir))
        return;

    snake->direction = new_dir;
}

//------------------------------------------------------
// Цвет
//------------------------------------------------------
void setColor(int objectType)
{
    // Сбрасываем все пары 1..5
    for (int i = 1; i <= 5; ++i)
        attroff(COLOR_PAIR(i));

    if (objectType >= 1 && objectType <= 5)
        attron(COLOR_PAIR(objectType));
}

void resetColor(void)
{
    for (int i = 1; i <= 5; ++i)
        attroff(COLOR_PAIR(i));
}

// Выбор цвета: 1 — красный, 2 — синий, 3 — зелёный, 4 — жёлтый, 5 — белый
int menuChooseColor(const char *title, int default_color)
{
    int ch;
    int color = default_color;

    while (1)
    {
        clear();
        resetColor();
        mvprintw(2, 2, "%s", title);
        mvprintw(4, 4, "1 - Red");
        mvprintw(5, 4, "2 - Blue");
        mvprintw(6, 4, "3 - Green");
        mvprintw(7, 4, "4 - Yellow");
        mvprintw(8, 4, "5 - White");
        mvprintw(10, 2, "Choose a color (1-5): ");

        refresh();

        ch = getch();
        if (ch == '1') { color = 1; break; }
        if (ch == '2') { color = 2; break; }
        if (ch == '3') { color = 3; break; }
        if (ch == '4') { color = 4; break; }
        if (ch == '5') { color = 5; break; }
        // остальное игнорируем
    }

    return color;
}
//------------------------------------------------
//          Еда
//------------------------------------------------ 
// Обновить/разместить текущее зерно на поле
void putFoodSeed(food_t *fp)
{
    char spoint[2] = {0};

    int max_x, max_y;
    razmer_okna(&max_y, &max_x);

    mvprintw(fp->y, fp->x, " ");

    fp->x = rand() % (max_x - 1);
    fp->y = rand() % (max_y - 2) + 1;
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;

    setColor(g_cfg.food_color_id);
    mvprintw(fp->y, fp->x, "%s", spoint);
    resetColor();
}

void putFood(food_t f[], size_t number_seeds)
{
    for(size_t i=0; i<number_seeds; i++)
        putFoodSeed(f+i);
}

void refreshFood(food_t f[], int nfood)
{
    int max_x, max_y;
    razmer_okna(&max_y, &max_x);
   
    for(int i=0; i<nfood; i++)
    {
        if( f[i].put_time )
        {
            if( !f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS )
                putFoodSeed(f+i);
        }
    }
}

_Bool haveEat(snake_t *head, food_t f[], int nfood)
{
    for(int i=0; i<nfood; i++)
        if( f[i].enable && head->x == f[i].x && head->y == f[i].y)
        {
            f[i].enable = 0;
            return 1;
        }
    return 0;
}

void addTail(snake_t *head)
{
    if (head->tsize < MAX_TAIL_SIZE)
        head->tsize++;
}

//------------------------------------------------------
// Столкновение с собой
//------------------------------------------------------
int isCrush(const snake_t *snakes)
{
    for (size_t i = 0; i < snakes->tsize; i++)
    {
        if (snakes->tail[i].x == snakes->x &&
            snakes->tail[i].y == snakes->y)
            return 0; // сейчас пересечение не считается смертью
    }
    return 0;
}

//------------------------------------------------------
// ИИ
//------------------------------------------------------
int distance(snake_t* snake, struct food* food) 
{
    return (abs(snake->x - food->x) + abs(snake->y - food->y));
}

void autoChangeDirection(snake_t *snake, struct food food[], int nfood)
{
    int pointer = 0;
    for (int i = 1; i < nfood; i++) 
        pointer = (distance(snake, food+i) < distance(snake, food+pointer)) ? i : pointer;

    if ((snake->direction == RIGHT || snake->direction == LEFT) &&
        (snake->y != food[pointer].y))
    { 
        snake->direction = (food[pointer].y > snake->y) ? DOWN : UP;
    } 
    else if ((snake->direction == DOWN || snake->direction == UP) &&
             (snake->x != food[pointer].x))
    { 
        snake->direction = (food[pointer].x > snake->x) ? RIGHT : LEFT;
    }
}

//------------------------------------------------------
// Обработка игры
//------------------------------------------------------
void update(snake_t *snake, food_t f[], const int32_t key_pressed,
            int nfood, const int TICK_MS, clock_t *last_tick,
            int is_bot)
{
    if (is_bot)
        autoChangeDirection(snake, f, nfood);
    else
        changeDirection(snake, key_pressed);

    clock_t now = clock();
    double elapsed_ms = (double)(now - *last_tick) * 1000.0 / CLOCKS_PER_SEC;

    if (elapsed_ms >= TICK_MS)
    {
        *last_tick = now;

        int old_x = snake->x;
        int old_y = snake->y;

        goTail(snake, old_x, old_y);
        go(snake);

        refreshFood(f, nfood);
        if (haveEat(snake, f, nfood))
            addTail(snake);

        refresh();
    }
}

//------------------------------------------------------
// Проверка корректности зёрен
//------------------------------------------------------
void repairSeed(food_t f[], size_t nfood, snake_t *snakes[], size_t n_snakes)
{
    // 1) зерно не должно быть на голове/хвосте любой змейки
    for (size_t j = 0; j < nfood; j++)
    {
        if (!f[j].enable)
            continue;

        int need_reseed = 0;

        for (size_t s = 0; s < n_snakes && !need_reseed; s++)
        {
            snake_t *sn = snakes[s];

            if (sn->x == f[j].x && sn->y == f[j].y)
            {
                need_reseed = 1;
                break;
            }

            for (size_t i = 0; i < sn->tsize; i++)
            {
                if (sn->tail[i].x == f[j].x &&
                    sn->tail[i].y == f[j].y)
                {
                    need_reseed = 1;
                    break;
                }
            }
        }

        if (need_reseed)
            putFoodSeed(&f[j]);
    }

    // 2) два активных зерна не должны стоять в одной точке
    for (size_t i = 0; i < nfood; i++)
    {
        if (!f[i].enable)
            continue;

        for (size_t j = i + 1; j < nfood; j++)
        {
            if (!f[j].enable)
                continue;

            if (f[i].x == f[j].x && f[i].y == f[j].y)
                putFoodSeed(&f[j]);
        }
    }
}

//------------------------------------------------------
// Ввод чисел из меню
//------------------------------------------------------
int menuGetNumber(int min, int max)
{
    int ch;
    while (1)
    {
        ch = getch();
        if (ch >= '0' + min && ch <= '0' + max)
            return ch - '0';
    }
}

//------------------------------------------------------
// Стартовое меню 
//------------------------------------------------------
void startMenu(GameSettings *cfg)
{
    clear();
    resetColor();

    // Сначала выбираем цвет еды
    cfg->food_color_id = menuChooseColor("Choosing a color for food", 4); // жёлтый по умолчанию

    // Меню режимов
    clear();
    mvprintw(2, 2, "WELCOME TO SNAKE GAME");
    mvprintw(4, 2, "Select game mode:");
    mvprintw(6, 4, "1 - Player vs Computer   (2-3 snakes)");
    mvprintw(7, 4, "2 - 2 Players + Computer (3 snakes)");
    mvprintw(8, 4, "3 - 2 Players            (2 snakes, no computer)");
    mvprintw(9, 4, "4 - Computer only        (1-3 snakes)");

    mvprintw(11, 2, "Enter mode (1-4): ");
    refresh();

    int mode = menuGetNumber(1, 4);
    cfg->mode = mode;

    // Выбор количества змей
    switch (mode)
    {
        case 1:
            clear();
            mvprintw(2, 2, "Mode 1: Player + Computer");
            mvprintw(4, 2, "Select number of snakes (2-3): ");
            refresh();
            cfg->num_snakes = menuGetNumber(2, 3);
            break;

        case 2:
            cfg->num_snakes = 3;
            break;

        case 3:
            cfg->num_snakes = 2;
            break;

        case 4:
            clear();
            mvprintw(2, 2, "Mode 4: Computer only");
            mvprintw(4, 2, "Select number of snakes (1-3): ");
            refresh();
            cfg->num_snakes = menuGetNumber(1, 3);
            break;
    }

    // По умолчанию
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        cfg->snake_type[i]    = SNAKE_BOT;
        cfg->control_index[i] = 0;
        cfg->color_id[i]      = 1;
    }

    // Настройка типов и управления
    if (mode == 1)
    {
        cfg->snake_type[0]    = SNAKE_HUMAN;
        cfg->control_index[0] = 0;  // arrows

        cfg->snake_type[1]    = SNAKE_BOT;
        cfg->control_index[1] = 2;  // wasd

        if (cfg->num_snakes == 3)
        {
            cfg->snake_type[2]    = SNAKE_BOT;
            cfg->control_index[2] = 2;
        }
    }
    else if (mode == 2)
    {
        cfg->snake_type[0]    = SNAKE_HUMAN;
        cfg->control_index[0] = 0;

        cfg->snake_type[1]    = SNAKE_HUMAN;
        cfg->control_index[1] = 2;

        cfg->snake_type[2]    = SNAKE_BOT;
        cfg->control_index[2] = 1;
    }
    else if (mode == 3)
    {
        cfg->snake_type[0]    = SNAKE_HUMAN;
        cfg->control_index[0] = 0;

        cfg->snake_type[1]    = SNAKE_HUMAN;
        cfg->control_index[1] = 2;
    }
    else if (mode == 4)
    {
        for (int i = 0; i < cfg->num_snakes; ++i)
        {
            cfg->snake_type[i]    = SNAKE_BOT;
            cfg->control_index[i] = 2;
        }
    }

    // Выбор цвета для каждой змейки
    for (int i = 0; i < cfg->num_snakes; ++i)
    {
        char title[64];

        if (cfg->snake_type[i] == SNAKE_HUMAN)
            snprintf(title, sizeof(title), "Color selection for the Player %d", i + 1);
        else
            snprintf(title, sizeof(title), "Choosing a color for your Computer %d", i + 1);

        cfg->color_id[i] = menuChooseColor(title, (i % 5) + 1);
    }

    clear();
    refresh();
}

//------------------------------------------------------
// main — точка входа в программу
//------------------------------------------------------
int main()
{
    food_t food[MAX_FOOD_SIZE]; 

    initscr();

    start_color();
    init_pair(1, COLOR_RED,    COLOR_BLACK); // красный
    init_pair(2, COLOR_BLUE,   COLOR_BLACK); // синий
    init_pair(3, COLOR_GREEN,  COLOR_BLACK); // зелёный
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // жёлтый
    init_pair(5, COLOR_WHITE,  COLOR_BLACK); // белый

    startMenu(&g_cfg);

    snake_t* snakes[MAX_PLAYERS];

    for (int i = 0; i < g_cfg.num_snakes; ++i)
    {
        int start_x = 5 + i * 5;
        int start_y = 5 + i * 3;

        initSnake(&snakes[i],
                  START_TAIL_SIZE,
                  start_x,
                  start_y,
                  g_cfg.control_index[i],
                  g_cfg.color_id[i]);
    }

    keypad(stdscr, TRUE);
    raw();
    noecho();
    curs_set(FALSE);                                      

    initFood(food, MAX_FOOD_SIZE);
    putFood(food, SEED_NUMBER);

    resetColor();
    mvprintw(0, 0, "Use arrows / WASD. Press F10 for EXIT"); 
    timeout(0);                                         

    int key_pressed = 0;
    int game_over   = 0;

    const int TICK_MS = PERIOD_SH_ZM;

    clock_t last_tick[MAX_PLAYERS];
    for (int i = 0; i < MAX_PLAYERS; i++)
        last_tick[i] = clock();

    for (int i = 0; i < g_cfg.num_snakes; i++)
    {
        setColor(snakes[i]->color_id);
        mvprintw(snakes[i]->y, snakes[i]->x, "@");
    }

    resetColor();
    refresh();                                            

    while (!game_over && key_pressed != STOP_GAME)
    {
        key_pressed = getch();

        for (int i = 0; i < g_cfg.num_snakes; ++i)
        {
            setColor(snakes[i]->color_id);

            int is_bot = (g_cfg.snake_type[i] == SNAKE_BOT);
            update(snakes[i], food, key_pressed, SEED_NUMBER,
                   TICK_MS, &last_tick[i], is_bot);
        }   

        resetColor();
        repairSeed(food, SEED_NUMBER, snakes, g_cfg.num_snakes);

        for (int i = 0; i < g_cfg.num_snakes; ++i)
            if (isCrush(snakes[i]))
                game_over = 1;
    }

    int max_x, max_y;
    razmer_okna(&max_y, &max_x);
    resetColor();

    if (game_over)
    {
        const char *msg = "THE END!";
        mvprintw(max_y / 2,
                 (max_x - (int)strlen(msg)) / 2,
                 "%s", msg);
    }
    else if (key_pressed == STOP_GAME)
    {        
        const char *msg = "STOP GAME!";
        mvprintw(max_y / 2,
                 (max_x - (int)strlen(msg)) / 2,
                 "%s", msg);
    }
    refresh();

    clock_t start = clock();
    while ((double)(clock() - start) / CLOCKS_PER_SEC < 2)
    {
        // ждём ~2 секунды
    }

    for (int i = 0; i < g_cfg.num_snakes; i++)
    {
        free(snakes[i]->tail);
        free(snakes[i]);
    }

    endwin();
    return 0;
}
