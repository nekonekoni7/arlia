#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <stdlib.h>

//����֡����Χ�ƹ̶�ʱ���������޵�ʱ������д
//����֡��+�ɱ�ʱ����Χ����ʵ/ʵ��ʱ��� ����д

#define MAP_WIDTH 250
#define MAP_HEIGTH 15
#define ENTITYS_NUM 11

//��ά����/��������
struct Vec2 {
	float x;
	float y;
};

//������Һ͵��˵�ö������
enum EntityTpye {
	Player = 1, Enemy = 2
};

//ʵ������
struct Entity {
	Vec2 position;  //λ��
	Vec2 velocity;  //�ٶ�
	EntityTpye tpye; //���or����
	char texture;    //����Ҫ��ʾ��ͼ�Σ�
	bool grounded;   //�Ƿ��ڵ����ϣ������ж���Ծ��
	bool active;     //�Ƿ���
};

//��������
struct Scene {
	Entity eneities[ENTITYS_NUM];    //�����������ʵ��
	bool barrier[MAP_WIDTH][MAP_HEIGTH];   //�ϰ������ǹ涨����ֵΪfalse����û���ϰ���
										   //����ֵΪtrue�������ϰ���
	Entity* player;    //�ṩ���ʵ���ָ�룬����������
	float gravity;     //���� -1119.8f
};

//��ʼ����������
void initScene(Scene* scene) {
	//-----------------------------�ϰ���ʼ��
	bool(*barr)[15] = scene->barrier;
	//���еط���ʼ��Ϊ���ϰ�
	for (int i = 0; i < MAP_WIDTH; ++i)
		for (int j = 0; j < MAP_HEIGTH; ++j)
			barr[i][j] = false;
	//����Ҳ��һ���ϰ����߶�Ϊ0
	for (int i = 0; i < MAP_WIDTH; ++i)
		barr[i][0] = true;
	//�Զ����ϰ�
	barr[4][1] = barr[4][2] = barr[4][3] = barr[5][1] = barr[5][2] = barr[6][1]
		= barr[51][3] = barr[52][3] = barr[53][3] = barr[54][3] = barr[55][3] = barr[56][3] = barr[57][3]
		= true;
	//-----------------------------ʵ���ʼ��
	//���˳�ʼ��
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
	//��ҳ�ʼ��
	scene->player = &scene->eneities[ENTITYS_NUM - 1];
	scene->player->position.x = 0;
	scene->player->position.y = 15;
	scene->player->velocity.x = 0;
	scene->player->velocity.y = 0;
	scene->player->texture = '@';
	scene->player->tpye = Player;
	scene->player->active = true;
	scene->player->grounded = false;

	//---------------��������
	scene->gravity = -29.8f;
}


#define BUFFER_WIDTH 50
#define BUFFER_HEIGTH 15

//��ʾ�õĸ�������
struct ViewBuffer {
	char buffer[BUFFER_WIDTH][BUFFER_HEIGTH];  //�Լ�������ַ�������
	HANDLE hOutBuf[2];   //2������̨��Ļ������
};

//��ʼ����ʾ
void initViewBuffer(ViewBuffer * vb) {
	//��ʼ���ַ�������
	for (int i = 0; i < BUFFER_WIDTH; ++i)
		for (int j = 0; j < BUFFER_HEIGTH; ++j)
			vb->buffer[i][j] = ' ';

	//��ʼ��2������̨��Ļ������
	vb->hOutBuf[0] = CreateConsoleScreenBuffer(
		GENERIC_WRITE,//������̿�����������д����
		FILE_SHARE_WRITE,//���建�����ɹ���дȨ��
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	vb->hOutBuf[1] = CreateConsoleScreenBuffer(
		GENERIC_WRITE,//������̿�����������д����
		FILE_SHARE_WRITE,//���建�����ɹ���дȨ��
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	//����2������̨��Ļ�������Ĺ��
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 1;
	SetConsoleCursorInfo(vb->hOutBuf[0], &cci);
	SetConsoleCursorInfo(vb->hOutBuf[1], &cci);
}

//ÿ֡  ���ݳ������� ���� ��ʾ������
void updateViewBuffer(Scene* scene, ViewBuffer * vb) {
	//����BUFFER�еĵ���+�ϰ���
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
	//����BUFFER�е�ʵ��
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

//ÿ֡  ������ʾ������ ��ʾ����
void drawViewBuffer(ViewBuffer * vb) {
	//�ٽ��ַ�������������д������һ����Ļ������
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
	//��ʾ д����ɵĻ�����
	SetConsoleActiveScreenBuffer(vb->hOutBuf[buffer_index]);

	//��һ�ν�ʹ����һ��������
	buffer_index = !buffer_index;
	//!1 = 0    !0 = 1
}

//��������
void handleInput(Scene* scene) {
	//���������������ܲ���
	if (scene->player->active != true)return;
	//������Ծ
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		if (scene->player->grounded)
			scene->player->velocity.y = 15.0f;
	}
	//���������ƶ�
	bool haveMoved = false;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		scene->player->velocity.x = -5.0f;
		haveMoved = true;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		scene->player->velocity.x = 5.0f;
		haveMoved = true;
	}
	//��û���ƶ������ٶ�ͣ������
	if (haveMoved != true) {
		scene->player->velocity.x = max(0, scene->player->velocity.x * 0.5f);//ʹ�������ٶȵĽ�������
	}
}

//���¹���AI
void updateAI(Scene* scene, float dt) {
	//�򵥼�ʱ��
	static float timeCounter = 0.0f;
	timeCounter += dt;
	//ÿ2�����һ�η���������򣬿��ܷ��򲻱䣩
	if (timeCounter >= 2.0f) {
		timeCounter = 0.0f;
		//�ı䷽��Ĵ���
		for (int i = 0; i < ENTITYS_NUM; ++i) {
			//����ŵĹ�����ܱ�AI�ٿ����ƶ�
			if (scene->eneities[i].active == true && scene->eneities[i].tpye == Enemy) {
				scene->eneities[i].velocity.x = 3.0f * (1 - 2 * (rand() % 2));//(1-2*(rand()%1)Ҫ���� -1Ҫ����1
			}
		}
	}
}

//��������ƽ��
float distanceSq(Vec2 a1, Vec2 a2) {
	float dx = a1.x - a2.x;
	float dy = a1.y - a2.y;
	return dx * dx + dy * dy;
}


//ĳ��ʵ������
void entityDie(Scene* scene, int entityIndex) {
	scene->eneities[entityIndex].active = false;
	scene->eneities[entityIndex].velocity.x = 0;
	scene->eneities[entityIndex].velocity.y = 0;
}

//������ײ�¼�
void handleCollision(Scene* scene, int i, int j, float disSq) {
	//�������������
	if (scene->eneities[i].tpye == Player && scene->eneities[j].tpye == Enemy) {
		//����Ҹ߶ȸ��ڹ���0.3,��֤����Ҳ��ڹ���ͷ�ϣ�����������
		if (scene->eneities[i].position.y - 0.3f > scene->eneities[j].position.y) { entityDie(scene, j); }
		//�����������
		else { entityDie(scene, i); }
	}
	//�������������
	if (scene->eneities[i].tpye == Enemy && scene->eneities[j].tpye == Player) {
		//����Ҹ߶ȸ��ڹ���0.3,��֤����Ҳ��ڹ���ͷ�ϣ�����������
		if (scene->eneities[j].position.y - 0.3f > scene->eneities[i].position.y) { entityDie(scene, i); }
		//�����������
		else { entityDie(scene, j); }
	}
}

//��������&��ײ
void updatePhysics(Scene* scene, float dt, int stepNum) {
	dt /= stepNum;
	for (int i = 0; i < stepNum; ++i) {
		//����ʵ��
		for (int i = 0; i < ENTITYS_NUM; ++i) {
			//��ʵ�����������������
			if (scene->eneities[i].active != true)continue;
			//��¼ԭʵ��λ��
			float x0f = scene->eneities[i].position.x;
			float y0f = scene->eneities[i].position.y;
			int x0 = x0f + 0.5f;
			int y0 = y0f + 0.5f;
			//��¼ģ����ʵ��λ��
					//��λ�� + ʱ����ٶ� = ��λ��
			float x1f = min(max(scene->eneities[i].position.x + dt * scene->eneities[i].velocity.x, 0.0f), MAP_WIDTH - 1);
			float y1f = min(max(scene->eneities[i].position.y + dt * scene->eneities[i].velocity.y, 1.0f), MAP_HEIGTH - 1);
			int x1 = x1f + 0.5f;
			int y1 = y1f + 0.5f;
			//�ж��ϰ���ײ
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
			//�ж��Ƿ�ȵ����棨λ�õ���һ�񣩣����ڴ�����Ծ
			if (scene->barrier[x1][max(y1 - 1, 0)] == true) {
				scene->eneities[i].grounded = true;
			}
			else {
				//     ���ӵ��ٶȴ�С = ʱ��*������/������
				scene->eneities[i].velocity.y += dt * (scene->gravity / 1.0f);
				scene->eneities[i].grounded = false;
			}

			//�ж�ʵ����ײ
			for (int j = i + 1; j < ENTITYS_NUM; ++j) {
				//��ʵ���������������ж�
				if (scene->eneities[j].active != true)continue;

				float disSq = distanceSq(scene->eneities[i].position, scene->eneities[j].position);

				if (disSq < 1 * 1) {
					//��������ײ���������ײ�¼�
					handleCollision(scene, i, j, disSq);
				}
			}
			//����ʵ��λ�ã������Ǿ�λ��Ҳ��������λ�ã�
			scene->eneities[i].position.x = x1f;
			scene->eneities[i].position.y = y1f;
		}
	}
}

//���³�������
void updateScene(Scene* scene, float dt) {
	//��Сʱ��߶�Ϊ�뵥λ��1000ms = 1s
	dt /= 1000.0f;
	//���¹���AI
	updateAI(scene, dt);
	//�����������ײ
	//���10��ģ��
	updatePhysics(scene, dt, 10);
}

int main() {
	//����֡����ѭ��  <60fps
	double TimePerFrame = 1000.0f / 60;//ÿ֡�̶���ʱ���,�˴�����fpsΪ60֡ÿ��
	  //��¼��һ֡��ʱ���
	DWORD lastTime = GetTickCount();

	//��ʾ������
	ViewBuffer vb;
	initViewBuffer(&vb);

	//����
	Scene sc;
	initScene(&sc);

	while (1) {
		DWORD nowTime = GetTickCount();     //��õ�ǰ֡��ʱ���
		DWORD deltaTime = nowTime - lastTime;  //������һ֡����һ֡��ʱ���
		lastTime = nowTime;                 //������һ֡��ʱ���

		handleInput(&sc);//��������
		updateScene(&sc, deltaTime);//���³�������
		updateViewBuffer(&sc, &vb);//������ʾ��
		drawViewBuffer(&vb);//��Ⱦ����ʾ��

		//�� ʵ��ʱ��� ���� ÿ֡�̶�ʱ�����û������� ���ڵĲ���ʱ�䡣
		if (deltaTime <= TimePerFrame)
			Sleep(TimePerFrame - deltaTime);
	}

	return 0;
}