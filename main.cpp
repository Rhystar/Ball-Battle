#include <stdlib.h>
#include <stdio.h>
#include <graphics.h>
#include <conio.h>
//graphics.hͷ�ļ������ⲿ��EasyX

#define GRAPH_WIDE 720//���ڿ��
#define GRAPH_HIGH 720//���ڸ߶�
#define SPOT_NUM 100//С������
#define ENEMY_NUM 2//��������

//�洢��ҺͶ��ֵ����ݵĽṹ��
typedef struct palyer
{
	unsigned int x;//������
	unsigned int y;//������
	unsigned int r;//�뾶
	unsigned int score;//����
	unsigned int direction;//�ƶ��������
	char name[20];//����
	int flag;//flagֵΪ1Ϊ����ʾ���
}PLAYER;
/*�ƶ��������˵����
0 ԭ�ز���
1 ��
2 ����
3 ��
4 ����
5 ��
6 ����
7 ��
8 ����
*/

//�洢С�����ݵĽṹ��
typedef struct apple
{
	unsigned int x;
	unsigned int y;
	const int r = 5;
	int flag;
}APPLE;

PLAYER me;//���
PLAYER enemy[ENEMY_NUM];//����
PLAYER final[ENEMY_NUM + 1];//��������ʱ�洢��Ϣ������
APPLE spot[SPOT_NUM];//С��
int changeDirection;//�ı䷽��Ĳ���

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
	int sign;//signΪ0ʱ���ʧ�ܣ�signΪ1ʱ���ʤ��
	int i;
	printf("�������ս������V1.0\n");
	printf("ʹ��wasd�ı�С���ƶ�����\n");
	printf("�����������֣���������\n");
	printf("��ɫ��С�����㣬��ɫ��С������Ķ���\n");
	printf("������������֣�");
	fgets(me.name, 20, stdin);
	DelLineBreak(me.name);
	for (i = 0; i < ENEMY_NUM; i++)
	{
		printf("�������%d�����ֵ����֣�", i + 1);
		fgets(enemy[i].name, 20, stdin);
		DelLineBreak(enemy[i].name);
	}
	system("cls");
	printf("��Ϸ������ʼ\n");
	system("pause");
	initgraph(GRAPH_WIDE, GRAPH_HIGH, SHOWCONSOLE);//��ʼ��ͼ�ν����ұ�������̨
	initgame();
	sign = playgame();
	system("cls");
	if (sign == 0)
		printf("���ѱ�������̭��\n");
	else
		printf("��ϲ��Ӯ��\n");
	sort();
	system("pause");
	system("cls");
	for (i = 0; i < ENEMY_NUM + 1; i++)
		printf("��%d�������� %s\t����%d\n", i + 1, final[i].name, final[i].score);
	system("pause");
	system("cls");
	printf("��Ϸ��������\n");
	system("pause");
	system("cls");
	return 0;
}

//��Ϸ���й���
int playgame()
{
	long i, j, k;
	for (i = 0;; i++)//ͨ�����ϵ��������������ʵ�ֶ���Ч������ͨ��ѭ������i�������������
	{
		Sleep(6);
		if (_kbhit())//���������Ļ����ı䷽��û�оͷ��򲻱�
			changeDirection = _getch();//��ȡ���������ַ���ASCII����ֵ��changeDirection
		cMove();
		edge(&me);
		move(&me);
		for (j = 0; j < ENEMY_NUM; j++)
		{
			if (i % 512 == 0 || enemy[j].direction == 0)//����ÿ�ƶ�200�λ򲻶�ʱ
			{
				initDirection(&enemy[j], i + j);//��������ı䷽��
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
		cleardevice();//����
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

//��ʼ����Ϸ
void initgame()
{
	int i;
	setbkcolor(LIGHTGRAY);//���ñ���ɫ
	me.r = 10;//��ʼ����Ұ뾶
	me.x = GRAPH_WIDE / 2;//��ʼ����Һ�����
	me.y = GRAPH_HIGH / 2;//��ʼ�����������
	me.flag = 1;//��ʼ����Ҵ��״̬
	makePlayer();
	for (i = 0; i < SPOT_NUM; i++)
	{
		initXY(&spot[i].x, &spot[i].y, i);//��ʼ��С������
		spot[i].flag = 1;
		makeSpot(spot[i]);//��������С��
	}
	for (i = 0; i < ENEMY_NUM; i++)
	{
		initEnemy();
		makeEnemy(enemy[i]);
	}
}

//�������
void makePlayer()
{
	setfillcolor(CYAN);//�������ɫΪ��ɫ
	setlinecolor(CYAN);//��������ɫΪ��ɫ
	fillcircle(me.x, me.y, me.r);//��һ�����Բ
}

//��������
void makeEnemy(PLAYER a)
{
	setfillcolor(YELLOW); //�������ɫΪ��ɫ
	setlinecolor(YELLOW);
	fillcircle(a.x, a.y, a.r);
}

//����С��
void makeSpot(APPLE a)
{
	setfillcolor(LIGHTRED);//�������ɫΪǳ��ɫ
	setlinecolor(LIGHTRED);
	fillcircle(a.x, a.y, a.r);
}

//�������x��y��m���ڸı����������
void initXY(unsigned int *x, unsigned int *y, int m)
{
	srand(10086 + m * (m - 1));
	*x = rand() % GRAPH_WIDE;//��֤x�ڿ�ȷ�Χ��
	srand(110 + m * (m + 2));
	*y = rand() % GRAPH_HIGH;//��֤y�ڸ߶ȷ�Χ��
}

//�ı��ƶ�����
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

//�ƶ���Ҽ�����
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

//����һ�����ƶ��������Եʱ��ʹ�������Ե�෴�ķ����ƶ�
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

//�������һ���ƶ��������
void initDirection(PLAYER *p, long m)
{
	srand(m);
	p->direction = rand() % 9;
}

//��ʼ��������Ϣ
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

//��һ���ֳ�С��
void eat(PLAYER *p, APPLE *a)
{
	p->score = p->score + 1;
	p->r = p->r + 2;
	a->flag = 0;
}

//������������ʱ���ж�
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

//ȥ��fgets����Ļ��з�
void DelLineBreak(char a[])
{
	a[strlen(a) - 1] = '\0';
}

//��������
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