#include <stdlib.h>
#include <stdio.h>
#include <graphics.h>
#include <conio.h>
//graphics.h头文件来自外部库EasyX

#define GRAPH_WIDE 720//窗口宽度
#define GRAPH_HIGH 720//窗口高度
#define SPOT_NUM 100//小球总数
#define ENEMY_NUM 2//对手数量

//存储玩家和对手的数据的结构体
typedef struct palyer
{
	unsigned int x;//横坐标
	unsigned int y;//纵坐标
	unsigned int r;//半径
	unsigned int score;//分数
	unsigned int direction;//移动方向变量
	char name[20];//姓名
	int flag;//flag值为1为，表示存活
}PLAYER;
/*移动方向变量说明：
0 原地不动
1 上
2 右上
3 右
4 右下
5 下
6 左下
7 左
8 左上
*/

//存储小球数据的结构体
typedef struct apple
{
	unsigned int x;
	unsigned int y;
	const int r = 5;
	int flag;
}APPLE;

PLAYER me;//玩家
PLAYER enemy[ENEMY_NUM];//对手
PLAYER final[ENEMY_NUM + 1];//最终排序时存储信息的数组
APPLE spot[SPOT_NUM];//小球
int changeDirection;//改变方向的参数

int playgame();
void initgame();
void makePlayer();
void makeEnemy(PLAYER a);
void makeSpot(APPLE a);
void initXY(unsigned int *x, unsigned int *y, int m);
void cMove();
void move(PLAYER *p);
void edge(PLAYER *p);
void initEnemy();
void eat(PLAYER *p, APPLE *a);
void initDirection(PLAYER *p, long m);
void fight(PLAYER *p, PLAYER *a);
void DelLineBreak(char a[]);
void sort();

int main()
{
	int sign;//sign为0时玩家失败，sign为1时玩家胜利
	int i;
	printf("球球大作战单机版V1.0\n");
	printf("使用wasd改变小球移动方向\n");
	printf("你有两个对手，击败他们\n");
	printf("青色的小球是你，黄色的小球是你的对手\n");
	printf("请输入你的名字：");
	fgets(me.name, 20, stdin);
	DelLineBreak(me.name);
	for (i = 0; i < ENEMY_NUM; i++)
	{
		printf("输入你第%d个对手的名字：", i + 1);
		fgets(enemy[i].name, 20, stdin);
		DelLineBreak(enemy[i].name);
	}
	system("cls");
	printf("游戏即将开始\n");
	system("pause");
	initgraph(GRAPH_WIDE, GRAPH_HIGH, SHOWCONSOLE);//初始化图形界面且保留控制台
	initgame();
	sign = playgame();
	system("cls");
	if (sign == 0)
		printf("你已被过程淘汰！\n");
	else
		printf("恭喜你赢了\n");
	sort();
	system("pause");
	system("cls");
	for (i = 0; i < ENEMY_NUM + 1; i++)
		printf("第%d名：姓名 %s\t分数%d\n", i + 1, final[i].name, final[i].score);
	system("pause");
	system("cls");
	printf("游戏即将结束\n");
	system("pause");
	system("cls");
	return 0;
}

//游戏运行过程
int playgame()
{
	long i, j, k;
	for (i = 0;; i++)//通过不断地清屏、重新输出实现动画效果，并通过循环变量i生成随机数种子
	{
		Sleep(6);
		if (_kbhit())//如果有输入的话，改变方向，没有就方向不变
			changeDirection = _getch();//读取玩家输入的字符的ASCII，赋值给changeDirection
		cMove();
		edge(&me);
		move(&me);
		for (j = 0; j < ENEMY_NUM; j++)
		{
			if (i % 512 == 0 || enemy[j].direction == 0)//对手每移动200次或不动时
			{
				initDirection(&enemy[j], i + j);//用随机数改变方向
			}
			edge(&enemy[j]);
			move(&enemy[j]);
		}
		for (j = 0; j < SPOT_NUM; j++)
		{
			if (spot[j].flag == 1 && (spot[j].x - me.x)*(spot[j].x - me.x) + (spot[j].y - me.y)*(spot[j].y - me.y) <= (spot[j].r + me.r)*(spot[j].r + me.r))
			{
				eat(&me, &spot[j]);
				initXY(&spot[j].x, &spot[j].y, i);
				spot[j].flag = 1;
			}
		}
		for (j = 0; j < ENEMY_NUM; j++)
		{
			for (k = 0; k < SPOT_NUM; k++)
			{
				if (spot[k].flag == 1 && (spot[k].x - enemy[j].x)*(spot[k].x - enemy[j].x) + (spot[k].y - enemy[j].y)*(spot[k].y - enemy[j].y) <= (spot[k].r + enemy[j].r)*(spot[k].r + enemy[j].r))
					eat(&enemy[j], &spot[k]);
			}
			if (enemy[j].flag == 1 && (enemy[j].x - me.x)*(enemy[j].x - me.x) + (enemy[j].y - me.y)*(enemy[j].y - me.y) <= (enemy[j].r + me.r)*(enemy[j].r + me.r))
				fight(&me, &enemy[j]);
		}
		cleardevice();//清屏
		if (me.flag == 0 || (enemy[0].flag == 0 && enemy[1].flag == 0))
		{
			closegraph();
			if (me.flag == 0)
				return 0;
			else
				return 1;
		}
		makePlayer();
		for (j = 0; j < SPOT_NUM; j++)
		{
			if (spot[j].flag == 1)
				makeSpot(spot[j]);
		}
		for (j = 0; j < ENEMY_NUM; j++)
		{
			if (enemy[j].flag == 1)
				makeEnemy(enemy[j]);
		}
	}
}

//初始化游戏
void initgame()
{
	int i;
	setbkcolor(LIGHTGRAY);//设置背景色
	me.r = 10;//初始化玩家半径
	me.x = GRAPH_WIDE / 2;//初始化玩家横坐标
	me.y = GRAPH_HIGH / 2;//初始化玩家纵坐标
	me.flag = 1;//初始化玩家存活状态
	makePlayer();
	for (i = 0; i < SPOT_NUM; i++)
	{
		initXY(&spot[i].x, &spot[i].y, i);//初始化小球坐标
		spot[i].flag = 1;
		makeSpot(spot[i]);//画出所有小球
	}
	for (i = 0; i < ENEMY_NUM; i++)
	{
		initEnemy();
		makeEnemy(enemy[i]);
	}
}

//画出玩家
void makePlayer()
{
	setfillcolor(CYAN);//设置填充色为青色
	setlinecolor(CYAN);//设置线条色为青色
	fillcircle(me.x, me.y, me.r);//画一个填充圆
}

//画出对手
void makeEnemy(PLAYER a)
{
	setfillcolor(YELLOW); //设置填充色为黄色
	setlinecolor(YELLOW);
	fillcircle(a.x, a.y, a.r);
}

//画出小球
void makeSpot(APPLE a)
{
	setfillcolor(LIGHTRED);//设置填充色为浅红色
	setlinecolor(LIGHTRED);
	fillcircle(a.x, a.y, a.r);
}

//随机生成x和y，m用于改变随机数种子
void initXY(unsigned int *x, unsigned int *y, int m)
{
	srand(10086 + m * (m - 1));
	*x = rand() % GRAPH_WIDE;//保证x在宽度范围内
	srand(110 + m * (m + 2));
	*y = rand() % GRAPH_HIGH;//保证y在高度范围内
}

//改变移动参数
void cMove()
{
	if (me.direction == 0)
	{
		if (changeDirection == 119)//w
			me.direction = 1;
		if (changeDirection == 100)//d
			me.direction = 3;
		if (changeDirection == 115)//s
			me.direction = 5;
		if (changeDirection == 97)//a
			me.direction = 7;
	}
	else if (me.direction == 1)
	{
		if (changeDirection == 119)
			me.direction = 1;
		if (changeDirection == 100)
			me.direction = 2;
		if (changeDirection == 115)
			me.direction = 0;
		if (changeDirection == 97)
			me.direction = 8;
	}
	else if (me.direction == 2)
	{
		if (changeDirection == 119)
			me.direction = 1;
		if (changeDirection == 100)
			me.direction = 3;
		if (changeDirection == 115)
			me.direction = 3;
		if (changeDirection == 97)
			me.direction = 1;
	}
	else if (me.direction == 3)
	{
		if (changeDirection == 119)
			me.direction = 2;
		if (changeDirection == 100)
			me.direction = 3;
		if (changeDirection == 115)
			me.direction = 4;
		if (changeDirection == 97)
			me.direction = 0;
	}
	else if (me.direction == 4)
	{
		if (changeDirection == 119)
			me.direction = 3;
		if (changeDirection == 100)
			me.direction = 3;
		if (changeDirection == 115)
			me.direction = 5;
		if (changeDirection == 97)
			me.direction = 5;
	}
	else if (me.direction == 5)
	{
		if (changeDirection == 119)
			me.direction = 0;
		if (changeDirection == 100)
			me.direction = 4;
		if (changeDirection == 115)
			me.direction = 5;
		if (changeDirection == 97)
			me.direction = 6;
	}
	else if (me.direction == 6)
	{
		if (changeDirection == 119)
			me.direction = 7;
		if (changeDirection == 100)
			me.direction = 5;
		if (changeDirection == 115)
			me.direction = 5;
		if (changeDirection == 97)
			me.direction = 7;
	}
	else if (me.direction == 7)
	{
		if (changeDirection == 119)
			me.direction = 8;
		if (changeDirection == 100)
			me.direction = 0;
		if (changeDirection == 115)
			me.direction = 6;
		if (changeDirection == 97)
			me.direction = 7;
	}
	else if (me.direction == 8)
	{
		if (changeDirection == 119)
			me.direction = 1;
		if (changeDirection == 100)
			me.direction = 1;
		if (changeDirection == 115)
			me.direction = 7;
		if (changeDirection == 97)
			me.direction = 7;
	}
	changeDirection = 0;
}

//移动玩家及对手
void move(PLAYER *p)
{
	switch (p->direction)
	{
	case 0:
		break;
	case 1:
		p->y = p->y - 1;
		break;
	case 2:
		p->y = p->y - 1;
		p->x = p->x + 1;
		break;
	case 3:
		p->x = p->x + 1;
		break;
	case 4:
		p->x = p->x + 1;
		p->y = p->y + 1;
		break;
	case 5:
		p->y = p->y + 1;
		break;
	case 6:
		p->y = p->y + 1;
		p->x = p->x - 1;
		break;
	case 7:
		p->x = p->x - 1;
		break;
	case 8:
		p->x = p->x - 1;
		p->y = p->y - 1;
		break;
	}
}

//当玩家或对手移动到界面边缘时，使其向与边缘相反的方向移动
void edge(PLAYER *p)
{
	if (p->x == 0 && p->y == 0)
		p->direction = 4;
	else if (p->x == 0 && p->y == GRAPH_HIGH)
		p->direction = 2;
	else if (p->x == GRAPH_WIDE && p->y == 0)
		p->direction = 6;
	else if (p->x == GRAPH_WIDE && p->y == GRAPH_HIGH)
		p->direction = 8;
	else if (p->x == 0)
		p->direction = 3;
	else if (p->x == GRAPH_WIDE)
		p->direction = 7;
	else if (p->y == 0)
		p->direction = 5;
	else if (p->y == GRAPH_HIGH)
		p->direction = 1;
}

//随机生成一个移动方向参数
void initDirection(PLAYER *p, long m)
{
	srand(m);
	p->direction = rand() % 9;
}

//初始化对手信息
void initEnemy()
{
	int i;
	for (i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i].x = (2 * i + 1)*(GRAPH_HIGH / 4);
		enemy[i].y = enemy[i].x;
		enemy[i].r = 18;
		enemy[i].flag = 1;
	}
}

//玩家或对手吃小球
void eat(PLAYER *p, APPLE *a)
{
	p->score = p->score + 1;
	p->r = p->r + 2;
	a->flag = 0;
}

//玩家与对手相遇时的判定
void fight(PLAYER *p, PLAYER *a)
{
	if (p->r >= a->r)
	{
		p->score = p->score + a->score;
		p->r = p->r + a->r / 3;
		a->flag = 0;
	}
	else
	{
		a->score = a->score + p->score;
		a->r = a->r + p->r / 3;
		p->flag = 0;
	}
}

//去除fgets读入的换行符
void DelLineBreak(char a[])
{
	a[strlen(a) - 1] = '\0';
}

//分数排序
void sort()
{
	int i, j;
	PLAYER temp;
	for (i = 0; i < ENEMY_NUM; i++)
		final[i] = enemy[i];
	final[ENEMY_NUM] = me;
	for (i = 0; i < ENEMY_NUM + 1; i++)
	{
		for (j = 0; j < ENEMY_NUM - i; j++)
		{
			if (final[j].score < final[j + 1].score)
			{
				temp = final[j];
				final[j] = final[j + 1];
				final[j + 1] = temp;
			}
		}
	}
}