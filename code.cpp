#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>
#include <conio.h>
#include <windows.h>

using namespace std;

// ��ʾ����Ĵ�С
const size_t sWidth = 60, sHeight = 24;

// �Թ���ʵ�ʴ�С����������������
const size_t width = 33, height = 15;
const char wall = 1;
const char path = 0;
const char target = 2;
// ��ͼ
char map[height][width];
bool fog[height][width];
bool walk[height][width];
// ����ǽ�б�
vector<pair<pair<int, int>, int> > walls;

/*
���prim�㷨�ĺ����ǣ�
1�����Թ�ȫ��ǽ
2��ѡһ������Ϊ�Թ���ͨ·��Ȼ���������ǽ�����б�
3�����б��ﻹ��ǽʱ
����1�����б������ѡһ��ǽ�����������ĸ��Ӳ����Թ���ͨ·
��������1����ǽ��ͨ���ö���ĸ��ӳ�Ϊ�Թ���ͨ·
��������2�����Ǹ����ӵ���ǽ�����б�
����2���������ĸ����Ѿ���ͨ·�ˣ��Ǿʹ��б����Ƴ�����ǽ
*/

enum Dir { Up, Down, Left, Right };

// ����һ��
int xx[] = { 0, 0, -1, 1 };
int yy[] = { -1, 1, 0, 0 };
// ��������
int x2[] = { 0, 0, -2, 2 };
int y2[] = { -2, 2, 0, 0 };

// �������
int playerx;
int playery;
int SPX;
int SPY;
const int margin = 3;

// �ƶ���굽xyλ��
void gotoxy(int x, int y)
{
    COORD coord = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// ����ȫ��ͼΪǽ
void setAllWall()
{
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            map[y][x] = wall;
}

// �Ƿ�Խ��
bool isOutBounds(int x, int y)
{
    return (x<0 || x >= width
        || y<0 || y >= height);
}

// ������̽��
void explore(int x, int y)
{
    // �����ĸ�����
    for (int i = 0; i < 4; ++i)
    {
        // ˳��һ��·��
        for (int x1 = x, y1 = y;
            !isOutBounds(x1, y1)
            && map[y1][x1] == path;
            x1 += xx[i], y1 += yy[i])
        {
            // ÿ��һ�����Ӽ�������Ƿ�Ϊͨ·
            for (int fy = y1 - 1; fy <= y1 + 1; ++fy)
                for (int fx = x1 - 1; fx <= x1 + 1; ++fx)
                    if (!isOutBounds(fx, fy)
                        && fog[fy][fx])
                        fog[fy][fx] = false;
        }
    }
}

// �����괦��ǽ��ӵ���ͨǽ��
void pushAroundWall(int x, int y)
{
    // �����ĸ�����
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
        // ֻҪ��ǽ�ͼ����б�
        if (map[ty][tx] == wall)
            walls.push_back(pair<pair<int, int>, int>(pair<int, int>( tx,ty ),i ));
    }
}

// չʾ��ͼ
void showMap()
{
    int dx = sWidth / 2 - playerx;
    int dy = sHeight / 2 - playery;
    // �ƶ���굽�ʼ��λ��
    gotoxy(0, 0);

    // ����������ͼ
    for (int sy = 0; sy < sHeight; ++sy)
    {
        for (int sx = 0; sx < sWidth; ++sx)
        {
            //putchar(map[y][x]?'@':' ');
            int x = sx - dx;
            int y = sy - dy;

            const char *Symbol[5] = { "��", "��", "��", "��", "��" };

            if (isOutBounds(x, y))
            {
                printf("��");
                continue;
            }
            if (x == playerx && y == playery)
            {
                printf("��");
                continue;
            }
            if (fog[y][x])
            {
                printf("��");
                continue;
            }
            if (!walk[y][x])
            {
                printf("��");
                continue;
            }
            if (map[y][x] == path)
            {
                printf("��");
                continue;
            }
            if (map[y][x] == target)
            {
                printf("��");
                continue;
            }

            printf("��");
            continue;
            // ���˽ضϣ�����Ϊ���ַ���ͼʱ��

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

// ���ɵ�ͼ�㷨������Ϊ��ʼ������
void generate(int cx, int cy)
{
    // ���Ƚ���ͼȫ������Ϊǽ
    setAllWall();

    // ������������Ѿ��߹���·��
    for (int i = 0; i < width*height; ++i)
        *((bool*)fog + i) = *((bool*)walk + i) = true;

    //������ҵ�����
    playerx = cx;
    playery = cy;

    // ����㿪ʼ�������ܵ�ǽ����ӵ������б���
    pushAroundWall(cx, cy);
    // ���������·
    map[cy][cx] = path;

    // ֻҪ���д����ǽ����һֱѭ��
    while (!walls.empty())
    {
        // ����Ҹ������ǽ
        int index = rand() % walls.size();
        int wx = walls[index].first.first;
        int wy = walls[index].first.second;
        int d = walls[index].second;
        // �����ǽ���򴩣�����һ�������ܵ�ǽ�ڼ�������б�
        if (map[wy + yy[d]][wx + xx[d]] == wall)
        {
            map[wy][wx] = path;
            map[wy + yy[d]][wx + xx[d]] = path;
            pushAroundWall(wx + xx[d], wy + yy[d]);
        }
        // �򴩺󣬽���ǽ�Ƴ������б�
        walls.erase(walls.begin() + index);
    }
    
    map[height-2][width-2] = target;

    // չ������
    explore(playerx, playery);
}


// ����ƶ����������ƶ��ķ���
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
        cout << "��Ϸʤ����" << endl;
        system("pause");
        return true;
    }
    // ֻ����·��ʱ��ſ����ƶ�
    if (map[ty][tx] == path) {
        playerx = tx, playery = ty;
        walk[ty][tx] = false;
    }
    // �ƶ�����ٴ�̽������
    explore(playerx, playery);
    return false;
}

// ÿһִ֡�и���
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
    // ��������
    srand((unsigned int)time(0));

    printf("WSAD�ƶ���Q�˳���R�ؿ�");
    system("pause");

    while (true) {
        // ���ɵ�ͼ
        generate(1, 1);
        
        // ��Ϸѭ��
        do {
            // �����ͼ
            showMap();
        } while (updata());    
    }
    
    return 0;
}