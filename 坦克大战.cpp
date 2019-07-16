#include<windows.h>
#include <time.h>
#pragma once
#include "mmsystem.h"
#pragma comment(lib,"winmm.lib")

#define LENGTH 40					//游戏场地的宽
#define WIDTH 10					//组成蛇的的正方形的边长
#define RANGE 50					//游戏场地与客户去之间间隔
#define SNAKE_COLOR RGB(235,110,220)	  //蛇的颜色
#define BK_COLOR	RGB(104,234,207)  //窗体背景色
#define EMEY_COLOR RGB(10,20,30)//EMEY COLOR
#define NO_SNAKE 0
#define HAS_SNAKE 1
#define YSHOT 3
#define NSHOT 4
#define STEP 2
#define MAKECOOR(x) (x)*WIDTH		//把flags数组的下标映射为坐标
#define EMEY 5
typedef struct
{
	int x;
	int y;
	int flag;
} GRID;//结构
int FFLAG = 0;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void MakeParttank(int, int, int, GRID *);	//为蛇身增加或设置一个方块
void Movetank(HWND);
void Moveshot(HWND);
void MakeFood(GRID *shot);			//制造随机方块
void initializer();				//初始化游戏
void makeemey();//敌人
void Manager();
void exchange(GRID *, GRID *);
TCHAR szAppName[] = TEXT("Gluttony Snake");
int flags[LENGTH][LENGTH];	//游戏区域所有方块的状态标记
RECT playground;		//游戏场地
GRID tank[4] ;	//所在位置数组
GRID emey[4];
void Moveemey(HWND);
GRID *shot = NULL;	//对象
//GRID*emey = NULL;
int direct = 0;
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{

	MSG msg;
	HWND hwnd;
	WNDCLASS wndclass;
	while (TRUE)
	{
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hbrBackground = CreateSolidBrush(RGB(203, 202, 201));
		wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hInstance = hInstance;
		wndclass.lpfnWndProc = WndProc;
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = szAppName;
		wndclass.style = CS_VREDRAW | CS_HREDRAW;

		if (!RegisterClass(&wndclass))
		{
			MessageBox(NULL, TEXT("Register class failed!! Retry??"), szAppName, 0);
			return 0;
		}
		break;
	}
	hwnd = CreateWindow(szAppName,
		TEXT("Gluttony Snake Preview V1.0.1"),
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MINIMIZEBOX ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL, NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, SW_NORMAL);
	UpdateWindow(hwnd);
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Movetank(hwnd);
			Moveemey(hwnd);
		}
	}
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CREATE:
		if(MessageBox(NULL, TEXT("START"), TEXT("测试"), MB_OK | MB_OKCANCEL)== IDCANCEL)
			return 0;
		
		PlaySoundA("ss.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

		initializer();
		MoveWindow(hwnd, RANGE * 2, RANGE * 2, WIDTH * LENGTH + RANGE * 3, WIDTH * LENGTH + RANGE * 3, TRUE);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			tank[1].x = tank[0].x + 1;
			tank[1].y = tank[0].y;
			tank[2].x = tank[0].x + 1;
			tank[2].y = tank[0].y + 1;
			tank[3].x = tank[0].x + 1;
			tank[3].y = tank[0].y - 1;
			
				direct = VK_LEFT;
			break;
		case VK_RIGHT:
			tank[1].x = tank[0].x - 1;
			tank[1].y = tank[0].y;
			tank[2].x = tank[0].x - 1;
			tank[2].y = tank[0].y + 1;
			tank[3].x = tank[0].x - 1;
			tank[3].y = tank[0].y - 1;
				direct = VK_RIGHT;
			break;
		case VK_UP:
			tank[1].x = tank[0].x ;
			tank[1].y = tank[0].y+1;
			tank[2].x = tank[0].x + 1;
			tank[2].y = tank[0].y + 1;
			tank[3].x = tank[0].x -1;
			tank[3].y = tank[0].y + 1;
				direct = VK_UP;
			break;
		case VK_DOWN:
			tank[1].x = tank[0].x ;
			tank[1].y = tank[0].y-1;
			tank[2].x = tank[0].x - 1;
			tank[2].y = tank[0].y - 1;
			tank[3].x = tank[0].x + 1;
			tank[3].y = tank[0].y -1;
			FFLAG = 0;
				direct = VK_DOWN;
				break;
		case'S':
			MakeFood(shot);
			FFLAG = 1;
		
		default:
			break;
		}
		return 0;

	case WM_PAINT:
		for (int i = 0; i != 4; ++i)
		{
			flags[(tank + i)->x][(tank + i)->y] = (tank+ i)->flag;
		}
		for (int i = 0; i != 4; ++i)
		{
			flags[(emey + i)->x][(emey + i)->y] = (emey + i)->flag;
		}
		hdc = BeginPaint(hwnd, &ps);
		SetViewportOrgEx(hdc, RANGE / 2, RANGE, NULL);
		hBrush = CreateSolidBrush(BK_COLOR);
		SelectObject(hdc, hBrush);
		Rectangle(hdc, playground.left, playground.top, playground.right, playground.bottom);
		DeleteObject(hBrush);
		hBrush = CreateSolidBrush(SNAKE_COLOR);
		SelectObject(hdc, hBrush);
		for (int i = 0; i != LENGTH; ++i)
		{
			for (int j = 0; j != LENGTH; ++j)
			{
				if (flags[i][j] == HAS_SNAKE)
				{
					Rectangle(hdc, MAKECOOR(i), MAKECOOR(j), MAKECOOR(i + 1), MAKECOOR(j + 1));//判断区域有FLAG画方块
				}
			}
		}
		
					
					if (FFLAG==1)
					{
						
						Moveshot(hwnd);
						Rectangle(hdc, MAKECOOR(shot->x), MAKECOOR(shot->y), MAKECOOR(shot->x + 1), MAKECOOR(shot->y + 1));//判断区域有FLAG画方块
					
					}//print shot
					if (shot->x>LENGTH||shot->y>LENGTH)
					{
						free(shot);
					}
		DeleteObject(hBrush);
		hBrush = CreateSolidBrush(EMEY_COLOR);
		SelectObject(hdc, hBrush);
		for (int i = 0; i != LENGTH; ++i)
		{
			for (int j = 0; j != LENGTH; ++j)
			{
				if (flags[i][j] == EMEY)
				{
					Rectangle(hdc, MAKECOOR(i), MAKECOOR(j), MAKECOOR(i + 1), MAKECOOR(j + 1));//判断区域有FLAG画方块
				}
			}
		}
		DeleteObject(hBrush);
		EndPaint(hwnd, &ps);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
//////////////////////////初始化游戏各个参数////////////////////////////////////////////////////////////
void initializer()
{
	

	direct = VK_RIGHT;	//蛇的初始方向为向右
	
	shot = (GRID *)calloc(1, sizeof(GRID));//分配储存内存
	
	/*************初始化游戏场地*******************/
	playground.left = 0;
	playground.top = 0;
	playground.right = WIDTH * LENGTH;
	playground.bottom = WIDTH * LENGTH;
	/**************初始化游戏场地********************/

	for (int i = 0; i < LENGTH; ++i)
	{
		for (int j = 0; j < LENGTH; ++j)
		{
			flags[i][j] = NO_SNAKE;
		}
	}
	
	MakeParttank(LENGTH / 2 + 2 , LENGTH / 2, HAS_SNAKE, (tank ));
	MakeParttank(LENGTH / 2 + 1, (LENGTH / 2), HAS_SNAKE, (tank+1));
	MakeParttank(LENGTH / 2 + 1, (LENGTH / 2)-1, HAS_SNAKE, (tank+2));
	MakeParttank(LENGTH / 2 + 1, (LENGTH / 2)+1, HAS_SNAKE, (tank+3));
	makeemey();
	
}
void makeemey()
{
	int i, j;
	srand((unsigned)time(NULL));
	i= rand() % LENGTH;
	j = rand() % LENGTH;
	MakeParttank(i / 2 + 2, j / 2, EMEY, (emey));
	MakeParttank(i / 2 + 1, (j / 2), EMEY, (emey+ 1));
	MakeParttank(i/ 2 + 1, (j / 2) - 1, EMEY, (emey + 2));
	MakeParttank(i / 2 + 1, (j / 2) + 1, EMEY, (emey + 3));
	
	
}
void MakeParttank(int x, int y, int flag, GRID * snake)
{
	snake->x = x;
	snake->y = y;
	snake->flag = flag;
	flags[x][y] = flag;
}
void exchange(GRID *snake1, GRID *snake2)
{
	int temp;
	temp = snake1->x;
	snake1->x = snake2->x;
	snake2->x = temp;
	temp = snake1->y;
	snake1->y = snake2->y;
	snake2->y = temp;
}
////////////////////////////////////////////////////////////////////////////////



//////////////////控制移动///////////////////////
void Moveshot(HWND hwnd)
{
	
	Manager();
	flags[shot->x][shot->y] = NSHOT;
	if (shot->flag==VK_LEFT)
	{
		shot->x=shot->x-2;
	}
	if (shot->flag == VK_RIGHT)
	{
		shot->x=shot->x+2;
	}
	if (shot->flag == VK_UP)
	{
		shot->y=shot->y-2;
	}
	if (shot->flag == VK_DOWN)
	{
		shot->y=shot->y+2;
	}
	InvalidateRect(hwnd, NULL, FALSE);
	Sleep(100);

}
void Moveemey(HWND hwnd)
{

	Manager();
	int s;
	srand((unsigned)time(NULL));
	s = rand() % 4;
	for (int i = 0; i != 4; ++i)
	{
		flags[(emey + i)->x][(emey + i)->y] = NO_SNAKE;
	}


	if (s == 0)
	{

		for (int i = 0; i < 4; i++)
		{
			emey[i].x--;

		}


	}
	if (s == 1)
	{

		for (int i = 0; i < 4; i++)
		{
			emey[i].y--;

		}


	}
	if (s == 2)
	{

		for (int i = 0; i < 4; i++)
		{
			emey[i].x++;

		}


	}
	if (s == 3)
	{

		for (int i = 0; i < 4; i++)
		{
			emey[i].y++;

		}


	}
	for (int i = 0; i != 4; ++i)
	{
		flags[(emey + i)->x][(emey + i)->y] = (emey + i)->flag;
	}
	InvalidateRect(hwnd, NULL, FALSE);
	Sleep(100);

}
void Movetank(HWND hwnd)
{
	Manager();
	for (int i = 0; i != 4; ++i)
	{
		flags[(tank + i)->x][(tank + i)->y] = NO_SNAKE;
	}

	
	if (direct == VK_LEFT)
	{
		
		for (int i = 0; i < 4; i++)
		{
			tank[i].x--;
			
		}
		
		
	}
	if (direct == VK_RIGHT)
	{
		for (int i = 0; i < 4; i++)
		{
			tank[i].x++;

		}
	}
	if (direct == VK_UP)
	{
		for (int i = 0; i < 4; i++)
		{
			tank[i].y--;

		}
	}
	if (direct == VK_DOWN)
	{
		
		for (int i = 0; i < 4; i++)
		{
			tank[i].y++;

		}
	}
	InvalidateRect(hwnd, NULL, FALSE);
	Sleep(200);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////通过随机生成food
void MakeFood(GRID *food)
{
	srand((unsigned)time(NULL));
	food->x = tank->x;
	food->y = tank->y;
	food->flag = direct;
	
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////游戏规则的映射及内存的处理///////////////////////////////////////////////////
void Manager()
{
	
	int iif = 0;
	for (int i = 0; i < LENGTH; i++)
	{
		for (int j = 0; j < LENGTH; j++)
		{
			if (flags[i][j]==EMEY)
			{
				iif = 1;
			}
		}

	}
	if (iif!=1)
	{
		makeemey();
	}
	if (shot->x-2 < 0 || shot->x+2 >= LENGTH || shot->y-2 < 0 || shot->y+2 >= LENGTH)
	{
		//MessageBox(NULL, TEXT("Game Over!!"), szAppName, 0);
		//initializer();
		FFLAG = 0;
		return;
	}
	if (flags[shot->x][shot->y]==EMEY)
	{
		FFLAG = 0;
		MessageBox(NULL, TEXT("you have shot it"), szAppName, MB_OK);
		initializer();
		

	}


	
}
