#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>
#include <conio.h>
#include <windows.h>

using namespace std;

// 显示区域的大小
const size_t sWidth = 60, sHeight = 24;

// 迷宫的实际大小（必须是奇数！）
const size_t width = 33, height = 15;
const char wall = 1;
const char path = 0;
const char target = 2;
// 地图
char map[height][width];
bool fog[height][width];
bool walk[height][width];
// 带凿墙列表
vector<pair<pair<int, int>, int> > walls;

/*
随机prim算法的核心是：
1、让迷宫全是墙
2、选一个格作为迷宫的通路，然后把它的邻墙放入列表
3、当列表里还有墙时
——1、从列表里随机选一个墙，如果它对面的格子不是迷宫的通路
————1、把墙打通，让对面的格子成为迷宫的通路
————2、把那个格子的邻墙加入列表
——2、如果对面的格子已经是通路了，那就从列表里移除这面墙
*/

enum Dir { Up, Down, Left, Right };

// 四周一步
int xx[] = { 0, 0, -1, 1 };
int yy[] = { -1, 1, 0, 0 };
// 四周两步
int x2[] = { 0, 0, -2, 2 };
int y2[] = { -2, 2, 0, 0 };

// 玩家坐标
int playerx;
int playery;
int SPX;
int SPY;
const int margin = 3;

// 移动光标到xy位置
void gotoxy(int x, int y)
{
    COORD coord = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 设置全地图为墙
void setAllWall()
{
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            map[y][x] = wall;
}

// 是否越界
bool isOutBounds(int x, int y)
{
    return (x<0 || x >= width
        || y<0 || y >= height);
}

// 向四周探索
void explore(int x, int y)
{
    // 遍历四个方向
    for (int i = 0; i < 4; ++i)
    {
        // 顺着一条路走
        for (int x1 = x, y1 = y;
            !isOutBounds(x1, y1)
            && map[y1][x1] == path;
            x1 += xx[i], y1 += yy[i])
        {
            // 每走一个格子检查四周是否为通路
            for (int fy = y1 - 1; fy <= y1 + 1; ++fy)
                for (int fx = x1 - 1; fx <= x1 + 1; ++fx)
                    if (!isOutBounds(fx, fy)
                        && fog[fy][fx])
                        fog[fy][fx] = false;
        }
    }
}

// 将坐标处的墙添加到待通墙中
void pushAroundWall(int x, int y)
{
    // 遍历四个方向
    for (int i = 0; i < 4; ++i)
    {
        int tx = x + xx[i];
        int ty = y + yy[i];
        if (isOutBounds(tx, ty))
            continue;
        int wx = x + x2[i];
        int wy = y + y2[i];
        if (isOutBounds(wx, wy))
            continue;
        // 只要是墙就加入列表
        if (map[ty][tx] == wall)
            walls.push_back(pair<pair<int, int>, int>(pair<int, int>( tx,ty ),i ));
    }
}

// 展示地图
void showMap()
{
    int dx = sWidth / 2 - playerx;
    int dy = sHeight / 2 - playery;
    // 移动光标到最开始的位置
    gotoxy(0, 0);

    // 遍历整个地图
    for (int sy = 0; sy < sHeight; ++sy)
    {
        for (int sx = 0; sx < sWidth; ++sx)
        {
            //putchar(map[y][x]?'@':' ');
            int x = sx - dx;
            int y = sy - dy;

            const char *Symbol[5] = { "□", "▓", "▽", "☆", "◎" };

            if (isOutBounds(x, y))
            {
                printf("□");
                continue;
            }
            if (x == playerx && y == playery)
            {
                printf("◎");
                continue;
            }
            if (fog[y][x])
            {
                printf("▓");
                continue;
            }
            if (!walk[y][x])
            {
                printf("▽");
                continue;
            }
            if (map[y][x] == path)
            {
                printf("□");
                continue;
            }
            if (map[y][x] == target)
            {
                printf("☆");
                continue;
            }

            printf("▓");
            continue;
            // 到此截断，后面为单字符地图时用

            // int d = 0;
            // for (int i = 0; i < 4; ++i)
            // {
            //     if (!isOutBounds(x + xx[i], y + yy[i]) && map[y + yy[i]][x + xx[i]] == wall)
            //         d |= 1 << i;

            // }

            // const int up = 1;
            // const int down = 2;
            // const int left = 4;
            // const int right = 8;
            // char ch = 0;
            // switch (d)
            // {
            // case 0:
            //     ch = '&';
            //     break;
            //     //case up:
            //     //case down:
            // case up | down:
            //     ch = '|';
            //     break;
            //     //case left:
            //     //case right:
            // case left | right:
            //     ch = '-';
            //     break;
            // default:
            //     ch = '+';
            // }
            // putchar(ch);
        }
        cout << endl;
    }
    //gotoxy(sWidth/2+1, sHeight/2+1);
    //putchar('A');
}

// 生成地图算法，参数为起始点坐标
void generate(int cx, int cy)
{
    // 首先将地图全部设置为墙
    setAllWall();

    // 填满迷雾，清空已经走过的路径
    for (int i = 0; i < width*height; ++i)
        *((bool*)fog + i) = *((bool*)walk + i) = true;

    //设置玩家的坐标
    playerx = cx;
    playery = cy;

    // 从起点开始，将四周的墙壁添加到待凿列表中
    pushAroundWall(cx, cy);
    // 把这个点变成路
    map[cy][cx] = path;

    // 只要还有待凿的墙，就一直循环
    while (!walls.empty())
    {
        // 随机找个待凿的墙
        int index = rand() % walls.size();
        int wx = walls[index].first.first;
        int wy = walls[index].first.second;
        int d = walls[index].second;
        // 如果是墙，打穿，将下一个点四周的墙壁加入待凿列表
        if (map[wy + yy[d]][wx + xx[d]] == wall)
        {
            map[wy][wx] = path;
            map[wy + yy[d]][wx + xx[d]] = path;
            pushAroundWall(wx + xx[d], wy + yy[d]);
        }
        // 打穿后，将该墙移除待凿列表
        walls.erase(walls.begin() + index);
    }
    
    map[height-2][width-2] = target;

    // 展开迷雾
    explore(playerx, playery);
}


// 玩家移动，参数是移动的方向
bool playerMove(Dir dir)
{
    if (0 > dir || dir >= 4)
        return false;
    int tx = playerx + xx[dir];
    int ty = playery + yy[dir];
    if (isOutBounds(tx, ty))
        return false;
    if (map[ty][tx] == wall)
        return false;
    if (map[ty][tx] == target)
    {
        system("cls");
        cout << "游戏胜利！" << endl;
        system("pause");
        return true;
    }
    // 只有是路的时候才可以移动
    if (map[ty][tx] == path) {
        playerx = tx, playery = ty;
        walk[ty][tx] = false;
    }
    // 移动完后再次探索迷雾
    explore(playerx, playery);
    return false;
}

// 每一帧执行更新
bool updata()
{
    char ch = getch();
    bool win = false;
    switch (ch)
    {
    case 'w':
        win = playerMove(Dir::Up);
        break;
    case 's':
        win = playerMove(Dir::Down);
        break;
    case 'a':
        win = playerMove(Dir::Left);
        break;
    case 'd':
        win = playerMove(Dir::Right);
        break;
    case 'q':
        exit(0);
        break;
    case 'r':
        return false;
        break;
    }
    return !win;
}

int main()
{
    // 设置种子
    srand((unsigned int)time(0));

    printf("WSAD移动，Q退出，R重开");
    system("pause");

    while (true) {
        // 生成地图
        generate(1, 1);
        
        // 游戏循环
        do {
            // 输出地图
            showMap();
        } while (updata());    
    }
    
    return 0;
}