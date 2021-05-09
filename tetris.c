#include "tetris.h"

static struct sigaction act, oact;

int main()
{
    int exit = 0;

    initscr();
    noecho();
    keypad(stdscr, TRUE);

    srand((unsigned int)time(NULL));

    createRankList();

    while (!exit)
    {
        clear();
        switch (menu())
        {
        case MENU_PLAY:
            play();
            break;
        case MENU_RANK:
            rank();
            break;
        case MENU_EXIT:
            exit = 1;
            break;
        default:
            break;
        }
    }

    writeRankFile();

    endwin();
    system("clear");
    return 0;
}

void InitTetris()
{
    int i, j;

    for (j = 0; j < HEIGHT; j++)
        for (i = 0; i < WIDTH; i++)
            field[j][i] = 0;

    nextBlock[0] = rand() % 7;
    nextBlock[1] = rand() % 7;
    nextBlock[2] = rand() % 7;
    blockRotate = 0;
    blockY = -1;
    blockX = WIDTH / 2 - 2;

    shadow_x = blockX;
    shadow_y = 12;

    score = 0;
    gameOver = 0;
    timed_out = 0;

    DrawOutline();
    DrawField();
    DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
    DrawNextBlock(nextBlock);
    PrintScore(score);
}

void DrawOutline()
{
    int i, j;
    /* 블럭이 떨어지는 공간의 테두리를 그린다.*/
    DrawBox(0, 0, HEIGHT, WIDTH);

    /* next block을 보여주는 공간의 테두리를 그린다.*/
    move(2, WIDTH + 10);
    printw("NEXT BLOCK");
    DrawBox(3, WIDTH + 10, 4, 8);

    /* 2번째 next block 용 테두리 그리기 */
    move(8, WIDTH + 10);
    DrawBox(9, WIDTH + 10, 4, 8);

    /* score를 보여주는 공간의 테두리를 그린다.*/
    move(15, WIDTH + 10);
    printw("SCORE");
    DrawBox(16, WIDTH + 10, 1, 8);
}

int GetCommand()
{
    int command;
    command = wgetch(stdscr);
    switch (command)
    {
    case KEY_UP:
        break;
    case KEY_DOWN:
        break;
    case KEY_LEFT:
        break;
    case KEY_RIGHT:
        break;
    case ' ': /* space key*/
        /*fall block*/
        break;
    case 'q':
    case 'Q':
        command = QUIT;
        break;
    default:
        command = NOTHING;
        break;
    }
    return command;
}

int ProcessCommand(int command)
{
    int ret = 1;
    int drawFlag = 0;
    switch (command)
    {
    case QUIT:
        ret = QUIT;
        break;
    case KEY_UP:
        if ((drawFlag = CheckToMove(field, nextBlock[0], (blockRotate + 1) % 4, blockY, blockX)))
            blockRotate = (blockRotate + 1) % 4;
        break;
    case KEY_DOWN:
        if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)))
            blockY++;
        break;
    case KEY_RIGHT:
        if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX + 1)))
            blockX++;
        break;
    case KEY_LEFT:
        if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX - 1)))
            blockX--;
        break;
    default:
        break;
    }
    if (drawFlag)
        DrawChange(field, command, nextBlock[0], blockRotate, blockY, blockX);
    return ret;
}

void DrawField()
{
    int i, j;
    for (j = 0; j < HEIGHT; j++)
    {
        move(j + 1, 1);
        for (i = 0; i < WIDTH; i++)
        {
            if (field[j][i] == 1)
            {
                attron(A_REVERSE);
                printw(" ");
                attroff(A_REVERSE);
            }
            else
                printw(".");
        }
    }
}

void PrintScore(int score)
{
    move(17, WIDTH + 11);
    printw("%8d", score);
}

void DrawNextBlock(int *nextBlock)
{
    int i, j;
    for (i = 0; i < 4; i++)
    {
        move(4 + i, WIDTH + 13);
        for (j = 0; j < 4; j++)
        {
            if (block[nextBlock[1]][0][i][j] == 1)
            {
                attron(A_REVERSE);
                printw(" ");
                attroff(A_REVERSE);
            }
            else
                printw(" ");
        }
    }

    for (i = 0; i < 4; i++)
    {
        move(10 + i, WIDTH + 13);
        for (j = 0; j < 4; j++)
        {
            if (block[nextBlock[2]][0][i][j] == 1)
            {
                attron(A_REVERSE);
                printw(" ");
                attroff(A_REVERSE);
            }
            else
                printw(" ");
        }
    }
}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile)
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0)
            {
                move(i + y + 1, j + x + 1);
                attron(A_REVERSE);
                printw("%c", tile);
                attroff(A_REVERSE);
            }
        }

    move(HEIGHT, WIDTH + 10);
}

void Draw_bck(int y, int x, int blockID, int blockRotate, char tile)
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0)
            {
                move(i + y + 1, j + x + 1);
                printw("%c", tile);
            }
        }

    move(HEIGHT, WIDTH + 10);
}

void DrawBox(int y, int x, int height, int width)
{
    int i, j;
    move(y, x);
    addch(ACS_ULCORNER);
    for (i = 0; i < width; i++)
        addch(ACS_HLINE);
    addch(ACS_URCORNER);
    for (j = 0; j < height; j++)
    {
        move(y + j + 1, x);
        addch(ACS_VLINE);
        move(y + j + 1, x + width + 1);
        addch(ACS_VLINE);
    }
    move(y + j + 1, x);
    addch(ACS_LLCORNER);
    for (i = 0; i < width; i++)
        addch(ACS_HLINE);
    addch(ACS_LRCORNER);
}

void play()
{
    int command;
    clear();
    act.sa_handler = BlockDown;
    sigaction(SIGALRM, &act, &oact);
    InitTetris();
    do
    {
        if (timed_out == 0)
        {
            alarm(1);
            timed_out = 1;
        }

        command = GetCommand();
        if (ProcessCommand(command) == QUIT)
        {
            alarm(0);
            DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
            move(HEIGHT / 2, WIDTH / 2 - 4);
            printw("Good-bye!!");
            refresh();
            getch();

            return;
        }
    } while (!gameOver);

    alarm(0);
    getch();
    DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
    move(HEIGHT / 2, WIDTH / 2 - 4);
    printw("GameOver!!");
    refresh();
    getch();
    newRank(score);
}

char menu()
{
    printw("1. play\n");
    printw("2. rank\n");
    printw("3. recommended play\n");
    printw("4. exit\n");
    return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX)
{
    int i, j;
    int chk_flag = 1;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (block[currentBlock][blockRotate][j][i] == 1)
            {
                if (f[blockY + j][blockX + i] == 1)
                {
                    chk_flag = 0;
                    break;
                }
                else if (blockY + j >= HEIGHT)
                {
                    chk_flag = 0;
                    break;
                }
                else if (blockX + i >= WIDTH || blockX + i < 0)
                {
                    chk_flag = 0;
                    break;
                }
            }
        }
    }

    if (chk_flag == 0)
        return 0;
    else
        return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX)
{
    int bck_block = currentBlock;
    int bck_rotate = blockRotate;
    int bck_Y = blockY;
    int bck_X = blockX;
    int chg_flag = 1;
    switch (command)
    {
    case KEY_UP:
        bck_rotate = (blockRotate + 3) % 4;
        break;
    case KEY_DOWN:
        bck_Y = blockY - 1;
        chg_flag = 0;
        break;
    case KEY_LEFT:
        bck_X = blockX + 1;
        break;
    case KEY_RIGHT:
        bck_X = blockX - 1;
        break;
    default:
        break;
    }

    Draw_bck(bck_Y, bck_X, bck_block, bck_rotate, '.'); // 기존 블록 .으로 초기화
    if (chg_flag == 1)
        Draw_bck(shadow_y, shadow_x, bck_block, bck_rotate, '.'); // 기존 shadow .으로 초기화
    DrawBlockWithFeature(blockY, blockX, currentBlock, blockRotate);

    //1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
    //2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
    //3. 새로운 블록 정보를 그린다.
}

void BlockDown(int sig)
{
    timed_out = 0;
    if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX) == 1)
    {
        blockY += 1;
        DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
        // 필드 정보 및 블록 정보 갱신 후 다시 그리기
    }
    else if (blockY == -1)
    {
        gameOver = 1;
        AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
        DrawField();
        // 필드 갱신 후 다시 그리기
    }
    else if (blockY != -1)
    {
        int temp_score;
        temp_score = AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
        score += DeleteLine(field); // 점수 계산 & 다시 그리기
        score += temp_score;

        nextBlock[0] = nextBlock[1];
        nextBlock[1] = nextBlock[2];
        nextBlock[2] = rand() % 7;

        blockRotate = 0;
        blockY = -1;
        blockX = WIDTH / 2 - 2;

        DrawNextBlock(nextBlock);
        PrintScore(score);

        DrawField();
    }

    return;
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX)
{
    int i, j;
    int touched = 0;

    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 4; i++)
        {
            if (block[currentBlock][blockRotate][j][i] == 1)
            {
                if (field[blockY + j + 1][blockX + i] == 1 || blockY + j == HEIGHT - 1)
                    touched++;
                field[blockY + j][blockX + i] = 1;
            }
        }
    }
    // Block이 추가된 영역의 필드값을 바꾼다.

    return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH])
{
    int i, j, del_line = 0;
    for (j = 0; j < HEIGHT; j++)
    {
        int del_flag = 0;
        for (i = 0; i < WIDTH; i++)
        {
            if (f[j][i] == 0)
            {
                del_flag = 1;
                break;
            }
        } // 필드 탐색

        if (del_flag == 0)
        {
            del_line++;
            int tempi, tempj;
            for (tempj = j - 1; tempj >= 0; tempj--)
            {
                for (tempi = 0; tempi < WIDTH; tempi++)
                {
                    f[tempj + 1][tempi] = f[tempj][tempi];
                }
            }
        }
    }
    //1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
    //2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.

    DrawField();

    return (del_line) * (del_line)*100;
}

///////////////////////////////////////////////////////////////////////////
void DrawShadow(int y, int x, int blockID, int blockRotate)
{ // 넘길때 blockX, blockY 넘겨야함
    int temp = y;
    while (1)
    {
        if (CheckToMove(field, nextBlock[0], blockRotate, temp + 1, x) == 1)
            temp++;
        else
            break;
    }

    DrawBlock(temp, x, nextBlock[0], blockRotate, '/');
    shadow_x = x;
    shadow_y = temp;
}

void DrawBlockWithFeature(int y, int x, int blockID, int blockRotate) // 넘길때 blockX, blockY 넘겨야함
{
    DrawBlock(y, x, nextBlock[0], blockRotate, ' ');
    DrawShadow(y, x, nextBlock[0], blockRotate);
}

/////////////////////// 2주차 구현 함수 ////////////////////////

void createRankList()
{
    FILE *rank_file = fopen("rank.txt", "r");

    if (rank_file == NULL)
    {
        rank_file = fopen("rank.txt", "w");
        if (rank_file == NULL)
            exit(-1);

        fprintf(rank_file, "0");
    }

    head = NULL;

    int main_i;
    fscanf(rank_file, "%d", &user_num);

    rank_pointer temp_node;

    for (main_i = 0; main_i < user_num; main_i++)
    {
        int user_score;
        char temp_name[NAMELEN];

        fscanf(rank_file, "%s %d", temp_name, &user_score);

        rank_pointer new_node = (rank_pointer)malloc(sizeof(struct rank_node));

        new_node->score = user_score;
        strcpy(new_node->name, temp_name);
        new_node->link = NULL;

        if (main_i == 0)
        {
            head = new_node;
            temp_node = head;
        }
        else
        {
            temp_node->link = new_node;
            temp_node = temp_node->link;
        }
    }

    fclose(rank_file);
}

void rank()
{
    int command, search_i, delete_rank;
    int rank_X = -1, rank_Y = -1;
    char search_name[NAMELEN];
    rank_pointer rank_search = head;

    clear();
    printw("1. list ranks from X to Y\n");
    printw("2. list ranks by a specific name\n");
    printw("3. delete a specific rank\n");

    while (1)
    {
        command = wgetch(stdscr);
        if (command >= '1' && command <= '3')
            break;
    }

    echo();

    switch (command)
    {
    case '1':
        printw("X: ");
        scanw("%d", &rank_X);
        printw("Y: ");
        scanw("%d", &rank_Y);

        printw("       name       |   score   \n");
        printw("------------------------------\n");

        if (rank_X == -1)
            rank_X = 0;
        else
            rank_X -= 1;

        if (rank_Y == -1)
            rank_Y = user_num;

        if (rank_X >= rank_Y)
        {
            printw("\nsearch failure: no rank in the list\n");
            break;
        }

        for (search_i = 0; search_i < rank_X; search_i++)
            rank_search = rank_search->link;

        for (search_i = rank_X; search_i < rank_Y; search_i++)
        {
            printw(" %-17s| %-10d\n", rank_search->name, rank_search->score);
            rank_search = rank_search->link;
        }

        break;

    case '2':
        printw("Input the name: ");
        scanw("%s", search_name);

        int name_search_flag = FALSE;

        printw("       name       |   score   \n");
        printw("------------------------------\n");

        while (rank_search)
        {

            if (strcmp(rank_search->name, search_name) == 0)
            {
                name_search_flag = TRUE;
                printw(" %-17s| %-10d\n", rank_search->name, rank_search->score);
            }

            rank_search = rank_search->link;
        }

        if (name_search_flag == FALSE)
            printw("\nsearch failure: no name in the list\n");

        break;

    case '3':
        printw("Input the rank: ");
        scanw("%d", &delete_rank);

        if (delete_rank <= 0 || delete_rank > user_num)
        {
            printw("\nsearch failure: the rank not in the list\n");
            break;
        }

        if (delete_rank == 1)
        {
            rank_pointer temp = rank_search;
            head = head->link;
            free(temp);
        }
        else
        {
            for (search_i = 0; search_i < delete_rank - 2; search_i++)
                rank_search = rank_search->link;

            rank_pointer temp = rank_search->link;
            rank_search->link = temp->link;
            free(temp);
        }

        printw("\nresult: the rank deleted\n");
        user_num--;

        break;

    default:
        break;
    }

    noecho();
    getch();

    return;
}

void writeRankFile()
{
    FILE *fp = fopen("rank.txt", "w+");
    int old_num, write_flag = FALSE;

    if (fp == NULL)
        exit(-1);

    rank_pointer temp = head;

    fscanf(fp, "%d", &old_num);

    if (old_num != user_num)
        write_flag = TRUE;
    else
    {
        int old_score;
        char old_name[NAMELEN];

        while (temp)
        {
            fscanf(fp, "%s %d", old_name, &old_score);

            if (strcmp(temp->name, old_name) != 0 || (temp->score != old_score))
            {
                write_flag = TRUE;
                break;
            }

            temp = temp->link;
        }
    }

    if (write_flag == FALSE)
    {
        fclose(fp);
        return;
    }

    fprintf(fp, "%d\n", user_num);
    temp = head;

    while (temp)
    {
        fprintf(fp, "%s %d\n", temp->name, temp->score);
        temp = temp->link;
    }

    fclose(fp);
    return;
}

void newRank(int score)
{
    char newRank_name[NAMELEN];
    int search_i;
    rank_pointer rank_search = head;
    int insert_flag = FALSE;

    clear();
    printw("your name: ");
    echo();
    scanw("%s", newRank_name);

    rank_pointer new_node = (rank_pointer)malloc(sizeof(struct rank_node));

    strcpy(new_node->name, newRank_name);
    new_node->score = score;
    new_node->link = NULL;

    if (rank_search == NULL)
    {
        head = new_node;
        user_num++;
        noecho();
        return;
    }

    if (rank_search->score < score)
    {
        rank_pointer temp = head;
        head = new_node;
        head->link = temp;
    }
    else
    {
        for (search_i = 0; search_i < user_num - 1; search_i++)
        {
            if (rank_search->score > score && rank_search->link->score < score)
            {
                new_node->link = rank_search->link;
                rank_search->link = new_node;
                insert_flag = TRUE;
                break;
            }

            rank_search = rank_search->link;
        }

        if (insert_flag == FALSE)
            rank_search->link = new_node;
    }

    user_num++;
    noecho();

    return;
}