#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <stdlib.h>

//限制帧数：围绕固定时间差（限制上限的时间差）来编写
//限制帧数+可变时长：围绕现实/实际时间差 来编写

#define MAP_WIDTH 250
#define MAP_HEIGTH 15
#define ENTITYS_NUM 11

//二维坐标/向量类型
struct Vec2 {
	float x;
	float y;
};

//区分玩家和敌人的枚举类型
enum EntityTpye {
	Player = 1, Enemy = 2
};

//实体类型
struct Entity {
	Vec2 position;  //位置
	Vec2 velocity;  //速度
	EntityTpye tpye; //玩家or敌人
	char texture;    //纹理（要显示的图形）
	bool grounded;   //是否在地面上（用于判断跳跃）
	bool active;     //是否存活
};

//场景类型
struct Scene {
	Entity eneities[ENTITYS_NUM];    //场景里的所有实体
	bool barrier[MAP_WIDTH][MAP_HEIGTH];   //障碍：我们规定假如值为false，则没有障碍。
										   //假如值为true，则有障碍。
	Entity* player;    //提供玩家实体的指针，方便访问玩家
	float gravity;     //重力 -1119.8f
};

//初始化场景函数
void initScene(Scene* scene) {
	//-----------------------------障碍初始化
	bool(*barr)[15] = scene->barrier;
	//所有地方初始化为无障碍
	for (int i = 0; i < MAP_WIDTH; ++i)
		for (int j = 0; j < MAP_HEIGTH; ++j)
			barr[i][j] = false;
	//地面也是一种障碍，高度为0
	for (int i = 0; i < MAP_WIDTH; ++i)
		barr[i][0] = true;
	//自定义障碍
	barr[4][1] = barr[4][2] = barr[4][3] = barr[5][1] = barr[5][2] = barr[6][1]
		= barr[51][3] = barr[52][3] = barr[53][3] = barr[54][3] = barr[55][3] = barr[56][3] = barr[57][3]
		= true;
	//-----------------------------实体初始化
	//敌人初始化
	for (int i = 0; i < ENTITYS_NUM - 1; ++i) {
		scene->eneities[i].position.x = 5.0f + rand() % (MAP_WIDTH - 5);
		scene->eneities[i].position.y = 10;
		scene->eneities[i].velocity.x = 0;
		scene->eneities[i].velocity.y = 0;
		scene->eneities[i].texture = '#';
		scene->eneities[i].tpye = Enemy;
		scene->eneities[i].grounded = false;
		scene->eneities[i].active = true;
	}
	//玩家初始化
	scene->player = &scene->eneities[ENTITYS_NUM - 1];
	scene->player->position.x = 0;
	scene->player->position.y = 15;
	scene->player->velocity.x = 0;
	scene->player->velocity.y = 0;
	scene->player->texture = '@';
	scene->player->tpye = Player;
	scene->player->active = true;
	scene->player->grounded = false;

	//---------------设置重力
	scene->gravity = -29.8f;
}


#define BUFFER_WIDTH 50
#define BUFFER_HEIGTH 15

//显示用的辅助工具
struct ViewBuffer {
	char buffer[BUFFER_WIDTH][BUFFER_HEIGTH];  //自己定义的字符缓冲区
	HANDLE hOutBuf[2];   //2个控制台屏幕缓冲区
};

//初始化显示
void initViewBuffer(ViewBuffer * vb) {
	//初始化字符缓冲区
	for (int i = 0; i < BUFFER_WIDTH; ++i)
		for (int j = 0; j < BUFFER_HEIGTH; ++j)
			vb->buffer[i][j] = ' ';

	//初始化2个控制台屏幕缓冲区
	vb->hOutBuf[0] = CreateConsoleScreenBuffer(
		GENERIC_WRITE,//定义进程可以往缓冲区写数据
		FILE_SHARE_WRITE,//定义缓冲区可共享写权限
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	vb->hOutBuf[1] = CreateConsoleScreenBuffer(
		GENERIC_WRITE,//定义进程可以往缓冲区写数据
		FILE_SHARE_WRITE,//定义缓冲区可共享写权限
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	//隐藏2个控制台屏幕缓冲区的光标
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 1;
	SetConsoleCursorInfo(vb->hOutBuf[0], &cci);
	SetConsoleCursorInfo(vb->hOutBuf[1], &cci);
}

//每帧  根据场景数据 更新 显示缓冲区
void updateViewBuffer(Scene* scene, ViewBuffer * vb) {
	//更新BUFFER中的地面+障碍物
	int playerX = scene->player->position.x + 0.5f;
	int offsetX = min(max(0, playerX - BUFFER_WIDTH / 2), MAP_WIDTH - BUFFER_WIDTH - 1);
	for (int i = 0; i < BUFFER_WIDTH; ++i)
		for (int j = 0; j < BUFFER_HEIGTH; ++j)
		{
			if (scene->barrier[i + offsetX][j] == false)
				vb->buffer[i][j] = ' ';
			else
				vb->buffer[i][j] = '=';
		}
	//更新BUFFER中的实体
	for (int i = 0; i < ENTITYS_NUM; ++i) {
		int x = scene->eneities[i].position.x + 0.5f - offsetX;
		int y = scene->eneities[i].position.y + 0.5f;
		if (scene->eneities[i].active == true
			&& 0 <= x && x < BUFFER_WIDTH
			&& 0 <= y && y < BUFFER_HEIGTH
			) {
			vb->buffer[x][y] = scene->eneities[i].texture;
		}
	}
}

//每帧  根据显示缓冲区 显示画面
void drawViewBuffer(ViewBuffer * vb) {
	//再将字符缓冲区的内容写入其中一个屏幕缓冲区
	static int buffer_index = 0;

	COORD coord = { 0,0 };
	DWORD bytes = 0;
	for (int i = 0; i < BUFFER_WIDTH; ++i)
		for (int j = 0; j < BUFFER_HEIGTH; ++j)
		{
			coord.X = i;
			coord.Y = BUFFER_HEIGTH - 1 - j;
			WriteConsoleOutputCharacterA(vb->hOutBuf[buffer_index], &vb->buffer[i][j], 1, coord, &bytes);
		}
	//显示 写入完成的缓冲区
	SetConsoleActiveScreenBuffer(vb->hOutBuf[buffer_index]);

	//下一次将使用另一个缓冲区
	buffer_index = !buffer_index;
	//!1 = 0    !0 = 1
}

//处理输入
void handleInput(Scene* scene) {
	//如果玩家死亡，则不能操作
	if (scene->player->active != true)return;
	//控制跳跃
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		if (scene->player->grounded)
			scene->player->velocity.y = 15.0f;
	}
	//控制左右移动
	bool haveMoved = false;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		scene->player->velocity.x = -5.0f;
		haveMoved = true;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		scene->player->velocity.x = 5.0f;
		haveMoved = true;
	}
	//若没有移动，则速度停顿下来
	if (haveMoved != true) {
		scene->player->velocity.x = max(0, scene->player->velocity.x * 0.5f);//使用线性速度的渐进减速
	}
}

//更新怪物AI
void updateAI(Scene* scene, float dt) {
	//简单计时器
	static float timeCounter = 0.0f;
	timeCounter += dt;
	//每2秒更改一次方向（随机方向，可能方向不变）
	if (timeCounter >= 2.0f) {
		timeCounter = 0.0f;
		//改变方向的代码
		for (int i = 0; i < ENTITYS_NUM; ++i) {
			//存活着的怪物才能被AI操控着移动
			if (scene->eneities[i].active == true && scene->eneities[i].tpye == Enemy) {
				scene->eneities[i].velocity.x = 3.0f * (1 - 2 * (rand() % 2));//(1-2*(rand()%1)要不是 -1要不是1
			}
		}
	}
}

//计算距离的平方
float distanceSq(Vec2 a1, Vec2 a2) {
	float dx = a1.x - a2.x;
	float dy = a1.y - a2.y;
	return dx * dx + dy * dy;
}


//某个实体死亡
void entityDie(Scene* scene, int entityIndex) {
	scene->eneities[entityIndex].active = false;
	scene->eneities[entityIndex].velocity.x = 0;
	scene->eneities[entityIndex].velocity.y = 0;
}

//处理碰撞事件
void handleCollision(Scene* scene, int i, int j, float disSq) {
	//若玩家碰到怪物
	if (scene->eneities[i].tpye == Player && scene->eneities[j].tpye == Enemy) {
		//若玩家高度高于怪物0.3,则证明玩家踩在怪物头上，怪物死亡。
		if (scene->eneities[i].position.y - 0.3f > scene->eneities[j].position.y) { entityDie(scene, j); }
		//否则玩家死亡
		else { entityDie(scene, i); }
	}
	//若怪物碰到玩家
	if (scene->eneities[i].tpye == Enemy && scene->eneities[j].tpye == Player) {
		//若玩家高度高于怪物0.3,则证明玩家踩在怪物头上，怪物死亡。
		if (scene->eneities[j].position.y - 0.3f > scene->eneities[i].position.y) { entityDie(scene, i); }
		//否则玩家死亡
		else { entityDie(scene, j); }
	}
}

//更新物理&碰撞
void updatePhysics(Scene* scene, float dt, int stepNum) {
	dt /= stepNum;
	for (int i = 0; i < stepNum; ++i) {
		//更新实体
		for (int i = 0; i < ENTITYS_NUM; ++i) {
			//若实体死亡，则无需更新
			if (scene->eneities[i].active != true)continue;
			//记录原实体位置
			float x0f = scene->eneities[i].position.x;
			float y0f = scene->eneities[i].position.y;
			int x0 = x0f + 0.5f;
			int y0 = y0f + 0.5f;
			//记录模拟后的实体位置
					//旧位置 + 时间×速度 = 新位置
			float x1f = min(max(scene->eneities[i].position.x + dt * scene->eneities[i].velocity.x, 0.0f), MAP_WIDTH - 1);
			float y1f = min(max(scene->eneities[i].position.y + dt * scene->eneities[i].velocity.y, 1.0f), MAP_HEIGTH - 1);
			int x1 = x1f + 0.5f;
			int y1 = y1f + 0.5f;
			//判断障碍碰撞
			if (scene->barrier[x0][y1] == true) {
				scene->eneities[i].velocity.y = 0;
				y1 = y0;
				y1f = y0f;
			}
			if (scene->barrier[x1][y1] == true) {
				scene->eneities[i].velocity.x = 0;
				x1 = x0;
				x1f = x0f;
			}
			//判断是否踩到地面（位置的下一格），用于处理跳跃
			if (scene->barrier[x1][max(y1 - 1, 0)] == true) {
				scene->eneities[i].grounded = true;
			}
			else {
				//     增加的速度大小 = 时间*（重力/质量）
				scene->eneities[i].velocity.y += dt * (scene->gravity / 1.0f);
				scene->eneities[i].grounded = false;
			}

			//判断实体碰撞
			for (int j = i + 1; j < ENTITYS_NUM; ++j) {
				//若实体死亡，则无需判定
				if (scene->eneities[j].active != true)continue;

				float disSq = distanceSq(scene->eneities[i].position, scene->eneities[j].position);

				if (disSq < 1 * 1) {
					//若发生碰撞，则处理该碰撞事件
					handleCollision(scene, i, j, disSq);
				}
			}
			//更新实体位置（可能是旧位置也可能是新位置）
			scene->eneities[i].position.x = x1f;
			scene->eneities[i].position.y = y1f;
		}
	}
}

//更新场景数据
void updateScene(Scene* scene, float dt) {
	//缩小时间尺度为秒单位，1000ms = 1s
	dt /= 1000.0f;
	//更新怪物AI
	updateAI(scene, dt);
	//更新物理和碰撞
	//拆分10次模拟
	updatePhysics(scene, dt, 10);
}

int main() {
	//限制帧数的循环  <60fps
	double TimePerFrame = 1000.0f / 60;//每帧固定的时间差,此处限制fps为60帧每秒
	  //记录上一帧的时间点
	DWORD lastTime = GetTickCount();

	//显示缓冲区
	ViewBuffer vb;
	initViewBuffer(&vb);

	//场景
	Scene sc;
	initScene(&sc);

	while (1) {
		DWORD nowTime = GetTickCount();     //获得当前帧的时间点
		DWORD deltaTime = nowTime - lastTime;  //计算这一帧与上一帧的时间差
		lastTime = nowTime;                 //更新上一帧的时间点

		handleInput(&sc);//处理输入
		updateScene(&sc, deltaTime);//更新场景数据
		updateViewBuffer(&sc, &vb);//更新显示区
		drawViewBuffer(&vb);//渲染（显示）

		//若 实际时间差 少于 每帧固定时间差，则让机器休眠 少于的部分时间。
		if (deltaTime <= TimePerFrame)
			Sleep(TimePerFrame - deltaTime);
	}

	return 0;
}