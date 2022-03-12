#define _USE_MATH_DEFINES

#define	SCRW		640		// ウィンドウ幅（Width
#define	SCRH		360		// ウィンドウ高さ（Height

#include <windows.h>
#include <stdio.h>
#include <d3dx9.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
//#include <unistd.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")


	////関数プロトタイプ宣言
void LoadTexture(LPDIRECT3DTEXTURE9 *lpTex, const char fname[], int W, int H, D3DCOLOR Color);
void Update(void);
void CreateConsole();
void Title_Init(void);
void Title_Update(void);
void Title_Render(void);
void Game_Init(void);
void Game_Update(void);
void Game_Render(void);
void Result_Init(void);
void Result_Update(void);
void Result_Render(void);





////列挙型

enum SceneName
{
	//default starting as 0 and +1
	TITLE_PROC, GAME_PROC, RESULT_PROC
};





////構造体宣言

// 頂点フォーマットの定義
struct VERTEX
{
	D3DXVECTOR3 Pos;
	D3DCOLOR Color;
	D3DXVECTOR2 Tex;
};

// 頂点用の構造体を宣言
typedef struct VERTEX2D
{
	float x;			// x座標
	float y;			// y座標
	float z;			// 常に0
	float rhw;			// 常に1
	D3DCOLOR color;		// 色
}Vertex2D;

typedef struct MOSQUITO
{
	D3DXVECTOR2 mPosition;
	float mPosTime;
	bool mFlg;
}Mos;






//// グローバル変数宣言, default as 0 when not specified

LPDIRECT3D9				lpD3D;		// Direct3Dインターフェイス

LPDIRECT3DDEVICE9		lpD3DDevice;	// Direct3DDeviceインターフェイス

D3DPRESENT_PARAMETERS d3dpp;

LPD3DXSPRITE lpSprite;	// スプライト
LPD3DXFONT lpFont;		// フォント

bool gameFullScreen;	// フルスクリーン（true,false)

int frame;   //variable to count the number of frames

//場面管理
int Scene;    //0 = Title    1 = Game

/*  LPDIRECT3DTEXTURE9 playerTex;  //画像を入れる入れ物
	D3DXMATRIX playerMat;  //行列(座標)  */

int timeLimit;
time_t Time1;
time_t Time2;
struct tm *ltime1;
struct tm *ltime2;
int timeLeft;

int score;

//For mouse
POINT Mouse;
HWND hwnd;   //cut this from the function WinMain, cause the priority is to "local variables" rather than "global variable" 
bool clickFlg;

const int enemyNum = 50;
Mos enemy[enemyNum];
Mos enemy2;





////  定数宣言









#define	FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)


////角度算出, Degree
float GetDegreeAngle(float x, float y, float tx, float ty)
{
	float fx = tx - x;
	float fy = ty - y;

	float rad = atan2f(fy, fx)/* arc tangent with 2 points giving a float answer */;

	float deg = D3DXToDegree(rad);

	if (deg < 0)
	{
		deg += 360.0f;
	}

	return deg;
}

////角度算出, Radian
float GetRadianAngle(float x, float y, float tx, float ty)
{
	float fx = tx - x;
	float fy = ty - y;

	float rad = atan2f(fy, fx)/* arc tangent with 2 points giving a float answer */;

	return rad;
}

////直線距離算出
float GetDistance(float x, float y, float tx, float ty)
{
	float fx = tx - x;
	float fy = ty - y;

	//斜辺
	float dist = sqrt((fx * fx) + (fy * fy));

	return dist;
}

//// 点を描く関数
void DrawPixel(float x, float y, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x, y, 0, 1, colour } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_POINTLIST /* primitive type, this draws a point */, 1 /* the amount of points */, vertex, sizeof(Vertex2D));
	//lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST /* primitive type, this draws a line between points */, 1 /* the amount of lines */, vertex, sizeof(Vertex2D));
	//lpD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP /* primitive type, this draws lines */, 3 /* the amount of lines */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

//// 2点間の線を描く関数
void DrawLine(float x1, float y1, float x2, float y2, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x1, y1, 0, 1, colour }, { x2, y2, 0, 1, colour } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST /* primitive type, this draws a line between points */, 1 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

void DrawLineColGrad(float x1, float y1, D3DCOLOR colour1, float x2, float y2, D3DCOLOR colour2)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x1, y1, 0, 1, colour1 }, { x2, y2, 0, 1, colour2 } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST /* primitive type, this draws a line between points */, 1 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

//// 三角形を線で描く関数
void DrawLineTriangle(float x1, float y1, float x2, float y2, float x3, float y3, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x1, y1, 0, 1, colour }, { x2, y2, 0, 1, colour }, { x2, y2, 0, 1, colour }, { x3, y3, 0, 1, colour }, { x3, y3, 0, 1, colour }, { x1, y1, 0, 1, colour } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST /* primitive type, this draws a line between points */, 3 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

void DrawLineTriColGrad(float x1, float y1, D3DCOLOR colour1, float x2, float y2, D3DCOLOR colour2, float x3, float y3, D3DCOLOR colour3)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x1, y1, 0, 1, colour1 }, { x2, y2, 0, 1, colour2 }, { x2, y2, 0, 1, colour2 }, { x3, y3, 0, 1, colour3 }, { x3, y3, 0, 1, colour3 }, { x1, y1, 0, 1, colour1 } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST /* primitive type, this draws a line between points */, 3 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

//// 三角形を描く関数
void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x1, y1, 0, 1, colour }, { x2, y2, 0, 1, colour }, { x3, y3, 0, 1, colour } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST /* primitive type, this draws a line between points */, 1 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

void DrawTriColGrad(float x1, float y1, D3DCOLOR colour1, float x2, float y2, D3DCOLOR colour2, float x3, float y3, D3DCOLOR colour3)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x1, y1, 0, 1, colour1 }, { x2, y2, 0, 1, colour2 }, { x3, y3, 0, 1, colour3 } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST /* primitive type, this draws a line between points */, 1 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

//// 四角形を線で描く関数
void DrawLineSquare(float x, float y, int width, int height, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x, y, 0, 1, colour }, { x + width, y, 0, 1, colour }, { x + width, y, 0, 1, colour }, { x + width, y + height, 0, 1, colour }, { x + width, y + height, 0, 1, colour }, { x, y + height, 0, 1, colour }, { x, y + height, 0, 1, colour }, { x, y, 0, 1, colour } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST /* primitive type, this draws a line between points */, 4 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

void DrawLineSquColGrad(float x, float y, int width, int height, D3DCOLOR colour1, D3DCOLOR colour2, D3DCOLOR colour3, D3DCOLOR colour4)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x, y, 0, 1, colour1 }, { x + width, y, 0, 1, colour2 }, { x + width, y, 0, 1, colour2 }, { x + width, y + height, 0, 1, colour3 }, { x + width, y + height, 0, 1, colour3 }, { x, y + height, 0, 1, colour4 }, { x, y + height, 0, 1, colour4 }, { x, y, 0, 1, colour1 } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST /* primitive type, this draws a line between points */, 4 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

//// 四角形を描く関数
void DrawSquare(float x, float y, int width, int height, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x, y, 0, 1, colour }, { x + width, y, 0, 1, colour }, { x, y + height, 0, 1, colour }, { x + width, y + height, 0, 1, colour } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP /* primitive type, this draws a line between points */, 2 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

void DrawSquColGrad(float x, float y, int width, int height, D3DCOLOR colour1, D3DCOLOR colour2, D3DCOLOR colour3, D3DCOLOR colour4)
{
	// 頂点用変数の宣言
	Vertex2D vertex[] = { { x, y, 0, 1, colour1 }, { x + width, y, 0, 1, colour2 }, { x, y + height, 0, 1, colour3 }, { x + width, y + height, 0, 1, colour4 } };

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP /* primitive type, this draws a line between points */, 2 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

//// 円を点で描く関数
void DrawDotCircle(float x, float y, float rx, float ry, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[360 + 1];

	float deg;   //Degree(0∼360)
	float rad;   //Radian(0∼2π)

	for (int i = 0; i < 360; i++)
	{
		deg = i;
		rad = D3DXToRadian(deg);
		vertex[i].x = x + cos(rad) * rx;
		vertex[i].y = y + sin(rad) * ry;
		vertex[i].z = 0.0f;
		vertex[i].rhw = 1.0f;
		vertex[i].color = colour;
	}

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_POINTLIST /* primitive type, this draws a point */, 360 /* the amount of points */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);

}

//// 円を線で描く関数
void DrawLineCircle(float x, float y, float rx, float ry, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[60 + 1];

	float deg;   //Degree(0∼360)
	float rad;   //Radian(0∼2π)

	for (int i = 0; i < 60 + 1; i++)
	{
		deg = i * 6;
		rad = D3DXToRadian(deg);
		vertex[i].x = x + cos(rad) * rx;
		vertex[i].y = y + sin(rad) * ry;
		vertex[i].z = 0.0f;
		vertex[i].rhw = 1.0f;
		vertex[i].color = colour;
	}

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP /* primitive type, this draws a line between points */, 60 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);

}

//// 円を描く関数
void DrawCircle(float x, float y, float rx, float ry, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[60 + 1];

	float deg;   //Degree(0∼360)
	float rad;   //Radian(0∼2π)

	for (int i = 0; i < 60 + 1; i++)
	{
		deg = i * 6;
		rad = D3DXToRadian(deg);
		vertex[i].x = x + cos(rad) * rx;
		vertex[i].y = y + sin(rad) * ry;
		vertex[i].z = 0.0f;
		vertex[i].rhw = 1.0f;
		vertex[i].color = colour;
	}

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN /* primitive type, this draws a line between points */, 60 - 1 /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}

//// 多角形を点で描く関数
void DrawDotPoly(float x, float y, float r, float poly, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[360 + 1];

	float deg;   //Degree(0∼360)
	float rad;   //Radian(0∼2π)

	for (int i = 0; i < poly + 1; i++)
	{
		deg = i * (360 / poly);
		rad = D3DXToRadian(deg);
		vertex[i].x = x + cos(rad) * r;
		vertex[i].y = y + sin(rad) * r;
		vertex[i].z = 0.0f;
		vertex[i].rhw = 1.0f;
		vertex[i].color = colour;
	}

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_POINTLIST /* primitive type, this draws a point */, poly /* the amount of points */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);

}

//// 多角形を線で描く関数
void DrawLinePoly(float x, float y, float r, float poly, float dir, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[60 + 1];

	float deg;   //Degree(0∼360)
	float rad;   //Radian(0∼2π)

	for (int i = 0; i < poly + 1; i++)
	{
		deg = i * (360 / poly) + dir;
		rad = D3DXToRadian(deg);
		vertex[i].x = x + cos(rad) * r;
		vertex[i].y = y + sin(rad) * r;
		vertex[i].z = 0.0f;
		vertex[i].rhw = 1.0f;
		vertex[i].color = colour;
	}

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP /* primitive type, this draws a line between points */, poly /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);

}

//// 多角形を描く関数
void DrawPolygon(float x, float y, float r, float poly, float dir, D3DCOLOR colour)
{
	// 頂点用変数の宣言
	Vertex2D vertex[60 + 1];

	float deg;   //Degree(0∼360)
	float rad;   //Radian(0∼2π)

	for (int i = 0; i < poly + 1; i++)
	{
		deg = i * (360 / poly) + dir;
		rad = D3DXToRadian(deg);
		vertex[i].x = x + cos(rad) * r;
		vertex[i].y = y + sin(rad) * r;
		vertex[i].z = 0.0f;
		vertex[i].rhw = 1.0f;
		vertex[i].color = colour;
	}

	// FVF設定（頂点構造体の情報を伝える）
	lpD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// 描画
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN /* primitive type, this draws a line between points */, (poly - 1) /* the amount of primitives */, vertex, sizeof(Vertex2D));

	// 元々のFVF設定に戻す
	lpD3DDevice->SetFVF(FVF_VERTEX);
}





//初期化関数　Function to format the game
void Initialise(void)
{
	/*//画像読み込み
	LoadTexture(&playerTex,    //何処に読み込みますか
		"player.png",   //何を読み込みますか
		256,   //画像の幅
		64,   //画像の高さ
		D3DCOLOR_XRGB(0, 0, 0));   //読み込みたくない色, change the rgb here to not show "a" specific colour from the sprite */

		////コンソールの作成
		//CreateConsole();

		//frame = 0;	//start counting the frames from 0





}

//終了関数　Function after ending the program
void Terminate(void)
{
	//////画像解放
	//playerTex->Release(); 




	lpSprite->Release();	// スプライト
	lpFont->Release();		// フォント

	////コンソールの解放
	//FreeConsole();
}





// タイトルの初期化
void Title_Init(void)
{
	Scene = TITLE_PROC;

	frame = 0;
}
void Title_Update(void)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		Game_Init();
	}
}
void Title_Render(void)
{
	int flash = 255;


	DrawSquare(0, 0, SCRW, SCRH, D3DCOLOR_ARGB(255, 0, 0, 0));



	// 描画開始
	lpSprite->Begin(D3DXSPRITE_ALPHABLEND);
	// 描画終了
	lpSprite->End();

	//文字表示はここから

	RECT rcStart = { SCRW /100, SCRH / 5, 0, 0 };
	char startText[256];
	sprintf_s(startText, sizeof(startText), "蚊、うぜー\n\n   絶滅してくれねーかな");
	if (frame < 255)
	{
		while (frame != 255)
		{
			lpFont->DrawText(NULL, startText, -1, &rcStart, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(frame, 255, 255, 255));
			frame++;
		}
	}
	else
	{
		lpFont->DrawText(NULL, startText, -1, &rcStart, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	//start instruction
	RECT rcStartInst = { SCRW / 5.5, SCRH / 1.5, 0, 0 };
	char startInstText[256];
	sprintf_s(startInstText, sizeof(startInstText), "Click to start");
	if (flash > 0)
	{
		lpFont->DrawText(NULL, startInstText, -1, &rcStartInst, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(flash, 255, 255, 255));
		flash--;
	}
	else
	{
		lpFont->DrawText(NULL, startInstText, -1, &rcStartInst, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(flash, 255, 255, 255));
		flash++;
	}

	
}


// ゲームの初期化
void Game_Init(void)
{
	Scene = GAME_PROC;

	timeLimit = 500;

	score = 0;

	//initialise random
	srand(timeGetTime());
	rand();

	time(&Time1);
	ltime1 = localtime(&Time1);


	for (int i = 0; i < enemyNum; i++)
	{
		enemy[i].mPosition.x = -120;
		enemy[i].mPosition.y = -120;

		enemy[i].mFlg = false;
		enemy[i].mPosTime = 10;
	}

	enemy2.mPosition.x = 200;
	enemy2.mPosition.y = 100;

}
void Game_Update(void)
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		Title_Init();
	}


	/*time(&Time2);
	ltime2 = localtime(&Time2);
	timeLeft = timeLimit - (ltime1->tm_sec - ltime2->tm_sec);*/

	timeLimit--;


	if (timeLimit <= 0)
	{
		Result_Init();
	}
	else
	{

		if (rand() % 100 == 1)
		{
			for (int i = 0; i < enemyNum; i++)
			{
				if (enemy[i].mFlg == false)
				{
					enemy[i].mFlg = true;
					enemy[i].mPosition.x = rand() % SCRW;
					enemy[i].mPosition.y = rand() % SCRH;
				}
				break;
			}
		}

		for (int i = 0; i < enemyNum; i++)
		{
			if (enemy[i].mFlg == true)
			{
				enemy[i].mPosTime--;
				if (enemy[i].mPosTime <= 0)
				{
					enemy[i].mPosTime = 30;
					enemy[i].mPosition.x = rand() % SCRW;
					enemy[i].mPosition.y = rand() % SCRH;
				}
			}
		}


		clickFlg = false;
		if (clickFlg == false)
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				clickFlg = true;

				for (int i = 0; i < enemyNum; i++)
				{
					if (enemy[i].mFlg == true)
					{
						//// 円の当たり判定
						float dist = GetDistance(Mouse.x, Mouse.y, enemy[i].mPosition.x, enemy[i].mPosition.y);
						//distance between player and enemy < player's radius + enemy's radius
						if (dist < 3 + 15)
						{
							enemy[i].mFlg = false;
							score++;
						}
					}
				}


			}

			GetCursorPos(&Mouse);
			ScreenToClient(hwnd, &Mouse);   //set (0, 0) to the (0, 0) of the window instead of the whole screen
		}


	}

}
void Game_Render(void)
{
	for (int i = 0; i < enemyNum; i++)
	{
		if (enemy[i].mFlg == true)
		{
			DrawCircle(enemy[i].mPosition.x, enemy[i].mPosition.y, 5, 5, D3DCOLOR_ARGB(255, 127, 127, 127));
			DrawCircle(enemy[i].mPosition.x + 2, enemy[i].mPosition.y + 12, 5, 10, D3DCOLOR_ARGB(255, 127, 127, 127));
			DrawCircle(enemy[i].mPosition.x - 5, enemy[i].mPosition.y, 3, 1, D3DCOLOR_ARGB(255, 127, 127, 127));
			DrawCircle(enemy[i].mPosition.x + 7, enemy[i].mPosition.y + 9, 10, 3, D3DCOLOR_ARGB(255, 127, 127, 127));
			DrawCircle(enemy[i].mPosition.x + 7, enemy[i].mPosition.y + 13, 7, 3, D3DCOLOR_ARGB(255, 127, 127, 127));
		}
	}

	if (clickFlg == true)
	{
		DrawCircle(Mouse.x, Mouse.y, 3, 3, D3DCOLOR_ARGB(255, 255, 255, 255));
	}


	// 描画開始
	lpSprite->Begin(D3DXSPRITE_ALPHABLEND);
	// 描画終了
	lpSprite->End();



	//文字表示はここから

	//text for time limit
	RECT rcTimeLeft = { 0, 0, 0, 0 };
	char TimeLeft[50];
	sprintf_s(TimeLeft, sizeof(TimeLeft), "Time Left: %d", timeLimit);
	lpFont->DrawText(NULL, TimeLeft, -1, &rcTimeLeft, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 255, 255));

	RECT rcScore = { SCRW / 1.9, SCRH - 40, 0, 0 };
	char scoreText[256];
	sprintf_s(scoreText, sizeof(scoreText), "Score: %d", score);
	lpFont->DrawText(NULL, scoreText, -1, &rcScore, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void Result_Init(void)
{
	Scene = RESULT_PROC;
}
void Result_Update(void)
{

}
void Result_Render(void)
{
	// 描画開始
	lpSprite->Begin(D3DXSPRITE_ALPHABLEND);
	// 描画終了
	lpSprite->End();

	//文字表示はここから

	RECT rcResult = { SCRW / 4, SCRH / 2, 0, 0 };
	char resultText[256];
	sprintf_s(resultText, sizeof(resultText), "Result: %d", score);
	lpFont->DrawText(NULL, resultText, -1, &rcResult, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 255, 255));
}



// 更新処理
void Update(void)
{
	////最終的な行列　＝　拡大　＊　移動     the multiplication has to be in this order or else there will be a bug
	//playerMat = playerScaleMat * playerTransMat;
	////拡大のMatrix
	//D3DXMatrixScaling(&playerScaleMat, -1.0f /* Xの拡大 */, 1.0f /* Yの拡大 */, 0.0f /* Zの拡大 */);

	switch (Scene)
	{
	case TITLE_PROC:
		Title_Update();
		break;
	case GAME_PROC:
		Game_Update();
		break;
	case RESULT_PROC:
		Result_Update();
		break;
	}


}

// 3D描画
void Render3D(void)
{

}

// 2D描画
void Render2D(void)
{
	//////////////////////////////////////////////////
	///	スプライトの描画処理
	//////////////////////////////////////////////////
	// 描画開始
	lpSprite->Begin(D3DXSPRITE_ALPHABLEND);

	switch (Scene)
	{
	case TITLE_PROC:
		Title_Render();
		break;
	case GAME_PROC:
		Game_Render();
		break;
	case RESULT_PROC:
		Result_Render();
		break;
	}

	/*//表示範囲
	RECT rcPlayer = { 0, 0, 64, 64 };  
	//移動のMatrix, 座標を行列にセット
	D3DXMatrixTranslation(&playerMat,   //どの行列にセットしますか
							playerX,   // X座標の移動
							playerY,   // Y座標の移動
							0.0f);   // Z座標の移動(2Dゲームは0固定)
	//行列をセット
	lpSprite->SetTransform(&playerMat);
	//表示
	lpSprite->Draw(playerTex,   //何を表示しますか
					&rcPlayer,   //表示範囲を指定
					&D3DXVECTOR3(32.0f, 32.0f, 0.0f),   //中心点
					NULL,   //注視点
					D3DCOLOR_ARGB(255, 255, 255, 255));   //表示させる色  */

	






	// 描画終了
	lpSprite->End();



	//文字表示はここから



}

void GameFrame(void)
{
	// バックバッファと Z バッファをクリア
	//change the rgb here to change the background color
	lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);


	// 更新処理
	Update();


	// 描画開始
	lpD3DDevice->BeginScene();

	D3DXMATRIX mView, mProj;

	// 視点行列の設定
	D3DXMatrixLookAtLH(&mView,
		&D3DXVECTOR3(0.0f, 0.0f, -10.0f),	// カメラの位置
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// カメラの視点
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f)	// カメラの頭の方向
	);

	// 投影行列の設定
	D3DXMatrixPerspectiveFovLH(&mProj, D3DXToRadian(60), (float)SCRW / (float)SCRH, 1.0f, 1000.0f);

	//行列設定
	lpD3DDevice->SetTransform(D3DTS_VIEW, &mView);
	lpD3DDevice->SetTransform(D3DTS_PROJECTION, &mProj);



	// 3D描画
	Render3D();

	// 2D描画
	Render2D();



	// 描画終了
	lpD3DDevice->EndScene();

	// バックバッファをプライマリバッファにコピー
	lpD3DDevice->Present(NULL, NULL, NULL, NULL);

	//frame++;	//+1 every frame
}

LRESULT APIENTRY WndFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}

void LoadTexture(LPDIRECT3DTEXTURE9 *lpTex, const char fname[], int W, int H, D3DCOLOR Color)
{
	if (W == 0)W = D3DX_DEFAULT;
	if (H == 0)H = D3DX_DEFAULT;
	D3DXCreateTextureFromFileEx(lpD3DDevice, fname, W, H, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, Color, NULL, NULL, lpTex);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev,
	LPSTR lpszCmdParam, int nCmdshow)
{
	//---------------------Window作成関連-----------------------

	MSG msg;

	WNDCLASS wc;
	char szAppName[] = "Generic Game SDK Window";

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WndFunc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		szAppName,
		"蚊、うぜー",		//this is the title shown on the window bar
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCRW, SCRH,
		NULL, NULL, hInst,
		NULL);

	if (!hwnd)return FALSE;

	ShowWindow(hwnd, nCmdshow);
	UpdateWindow(hwnd);
	SetFocus(hwnd);

	gameFullScreen = false;	// ウィンドウモード

	if (gameFullScreen) {
		ShowCursor(FALSE);
	}
	else {
		RECT rc = { 0,0,SCRW,SCRH };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowPos(hwnd, NULL, 50, 50, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW | SWP_NOZORDER);
	}

	//---------------------DirectX Graphics関連-----------------------

	// Direct3D オブジェクトを作成
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!lpD3D)
	{
		// オブジェクト作成失敗
		MessageBox(NULL, "Direct3D の作成に失敗しました。", "ERROR", MB_OK | MB_ICONSTOP);
		// 終了する
		return 0;
	}
	int adapter;

	// 使用するアダプタ番号
	adapter = D3DADAPTER_DEFAULT;

	// ウインドウの作成が完了したので、Direct3D を初期化する
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	// Direct3D 初期化パラメータの設定
	if (gameFullScreen)
	{
		// フルスクリーン : ほとんどのアダプタでサポートされているフォーマットを使用
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		// ウインドウ : 現在の画面モードを使用
		D3DDISPLAYMODE disp;
		// 現在の画面モードを取得
		lpD3D->GetAdapterDisplayMode(adapter, &disp);
		d3dpp.BackBufferFormat = disp.Format;
	}
	// 表示領域サイズの設定
	d3dpp.BackBufferWidth = SCRW;
	d3dpp.BackBufferHeight = SCRH;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

	if (!gameFullScreen)
	{
		// ウインドウモード
		d3dpp.Windowed = 1;
	}

	// Z バッファの自動作成
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	//ﾊﾞｯｸﾊﾞｯﾌｧをﾛｯｸ可能にする(GetDCも可能になる)
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	// デバイスの作成 - T&L HAL
	if (FAILED(lpD3D->CreateDevice(adapter, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &lpD3DDevice)))
	{
		// 失敗したので HAL で試行
		if (FAILED(lpD3D->CreateDevice(adapter, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &lpD3DDevice)))
		{
			// 失敗したので REF で試行
			if (FAILED(lpD3D->CreateDevice(adapter, D3DDEVTYPE_REF, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &lpD3DDevice)))
			{
				// 結局失敗した
				MessageBox(NULL, "DirectX9が初期化できません。\n未対応のパソコンと思われます。", "ERROR", MB_OK | MB_ICONSTOP);
				lpD3D->Release();
				// 終了する
				return 0;
			}
		}
	}

	// レンダリング・ステートを設定
	// Z バッファ有効化->前後関係の計算を正確にしてくれる
	lpD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// アルファブレンディング有効化
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// アルファブレンディング方法を設定
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// レンダリング時のアルファ値の計算方法の設定
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	// テクスチャの色を使用
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	// 頂点の色を使用
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	// レンダリング時の色の計算方法の設定
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	//裏面カリング
	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// フィルタ設定
	lpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// ライト
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	lpD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	// 頂点フォーマットの設定
	lpD3DDevice->SetFVF(FVF_VERTEX);

	timeBeginPeriod(1);

	// ゲームに関する初期化処理 ---------------------------


	//Init();   is the short form of initialise
	Initialise();





	// スプライト作成
	D3DXCreateSprite(lpD3DDevice, &lpSprite);
	lpSprite->OnResetDevice();

	// フォント作成
	D3DXCreateFont(lpD3DDevice, 30/* 高さ */, 30/* 幅 */, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "ＭＳ ゴシック", &lpFont);

	lpFont->OnResetDevice();

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			GameFrame();
		}
	}

	// ゲームに関する終了処理 ---------------------------

	//End();  is a easier word for terminate
	Terminate();


	timeEndPeriod(1);

	// Direct3D オブジェクトを解放
	lpD3DDevice->Release();
	lpD3D->Release();

	return (int)msg.wParam;
}

//コンソール作成
void CreateConsole()
{
	//コンソールを作成する
	AllocConsole();

	FILE *fp_c = NULL;

	freopen_s(&fp_c, "CONOUT$", "w", stdout);
	freopen_s(&fp_c, "CONIN$", "r", stdin);
}