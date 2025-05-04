/*******************************************************************

	ブロック崩し プログラム ヒント　　　block0.cpp
	
	松江工業高等専門学校　情報工学科　教授　橋本　剛

*******************************************************************/
#include "DxLib.h"
#include <cstdlib>
#include <ctime>

// 定数 const を付けると定数 
const int WINDOW_SIZE_X = 640;
const int WINDOW_SIZE_Y = 480;
const int BAR_SIZE_X = 200;
const int BAR_SIZE_Y = 5;
const int BALL_SIZE = 4;
const int BLOCK_SIZE_X = 40;
const int BLOCK_SIZE_Y = 20;
const int BLOCK_NUM_X = WINDOW_SIZE_X / BLOCK_SIZE_X;
const int BLOCK_NUM_Y = 6;
const int BLOCK_TOP_Y = 40;

// グローバル変数
int bar_y = WINDOW_SIZE_Y * 9 / 10;//bar上端のy座標
int bar_x = WINDOW_SIZE_X / 2 - BAR_SIZE_X / 2;//bar左端のx座標
int ball_x = bar_x + BAR_SIZE_X / 2;//ball中心のx座標
int ball_y = bar_y - BALL_SIZE;//ball中心のy座標
int vx;
int vy;
int block[BLOCK_NUM_Y][BLOCK_NUM_X] = 
{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
};

int stoneBlock[BLOCK_NUM_X] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
int BlockHandle[BLOCK_NUM_Y];//ブロックのビットマップ画像ハンドル

//Kim's declaration
bool isDeleteBlock(int by, int bx);
int getBlockNumY(int by);
int getBlockNumX(int bx);
void levelTracker(int scoreTmpData);

void resetGame();
bool gameWinCheck();
bool playAgainPrompt();


int currentLevel = 1;
int score = 0;
int levelTwoThreshold = 100;
int levelThreeThreshold = 200;
int levelFourThreshold = 5000;

int stoneBlockX1 = -1; 
int stoneBlockX2 = -1;
int stoneBlockX3 = -1;

//sound 
int bgmHandler = -1;
int blockHitSoundHandler = -1;
int barHitSoundHandler = -1;

//bar size
int currentBarSizeX;

//描画関数
void Draw()
{
	static int GrHandle = LoadGraph( "gamebg.bmp" );//背景画像登録 640x480
	static int colorBlock = LoadGraph("block.bmp");//背景画像登録 640x480
	DrawGraph( 0 , 0 , GrHandle , FALSE );//背景を描く
	// === MODIFIED: Use current_bar_size_x for drawing ===
	DrawBox(bar_x, bar_y, bar_x + currentBarSizeX, bar_y + BAR_SIZE_Y, GetColor(255, 255, 255), TRUE);;//BARを描く
	DrawCircle(ball_x, ball_y, BALL_SIZE, GetColor( 255, 255, 0 ), TRUE);//BALLを描く
	DrawFormatString(10, 10, GetColor(255, 255, 255), "Score: %d  Level: %d", score, currentLevel);
	for ( int y = 0; y < BLOCK_NUM_Y; y++ )
	{
		for (int x = 0; x < BLOCK_NUM_X; x++)
		{
			if (block[y][x])
				//DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 0, 0, 40, 20, colorBlock, FALSE, FALSE);
				if (y == 0)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 0, 0, 40, 20, colorBlock, FALSE, FALSE);
				}
				else if (y == 1)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 40, 0, 40, 20, colorBlock, FALSE, FALSE);
				}
				else if (y == 2)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 80, 0, 40, 20, colorBlock, FALSE, FALSE);
				}
				else if (y == 3)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 120, 0, 40, 20, colorBlock, FALSE, FALSE);
				}
				else if (y == 4)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 160, 0, 40, 20, colorBlock, FALSE, FALSE);
				}
				else if (y == 5)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 200, 0, 40, 20, colorBlock, FALSE, FALSE);
				}
		}
	}

	//drawing stones for level 2
	if (currentLevel >= 2)
	{
		int stoneY = BLOCK_TOP_Y + BLOCK_NUM_Y * BLOCK_SIZE_Y;
		if (stoneBlockX1 >= 0 && stoneBlockX1 < BLOCK_NUM_X)
		{
			DrawBox(stoneBlockX1 * BLOCK_SIZE_X, stoneY, stoneBlockX1 * BLOCK_SIZE_X + BLOCK_SIZE_X, stoneY + BLOCK_SIZE_Y, GetColor(255, 0, 0), TRUE);
		}
		if (stoneBlockX2 >= 0 && stoneBlockX2 < BLOCK_NUM_X)
		{
			DrawBox(stoneBlockX2 * BLOCK_SIZE_X, stoneY, stoneBlockX2 * BLOCK_SIZE_X + BLOCK_SIZE_X, stoneY + BLOCK_SIZE_Y, GetColor(255, 0, 0), TRUE);
		}
		if (stoneBlockX3 >= 0 && stoneBlockX3 < BLOCK_NUM_X)
		{
			DrawBox(stoneBlockX3 * BLOCK_SIZE_X, stoneY, stoneBlockX3 * BLOCK_SIZE_X + BLOCK_SIZE_X, stoneY + BLOCK_SIZE_Y, GetColor(255, 0, 0), TRUE);
		}
	}

}

//Barの座標を変える関数
void MoveBar()
{
	if (currentLevel == 1)
	{
		if (CheckHitKey(KEY_INPUT_RIGHT) && (bar_x+currentBarSizeX)<WINDOW_SIZE_X) // → キー を押したか
		{
			bar_x = bar_x + 2;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) && (bar_x) > 0)
		{
			bar_x -= 2;
		}
	}
	else if (currentLevel == 2)
	{
		if (CheckHitKey(KEY_INPUT_RIGHT) && (bar_x + currentBarSizeX) < WINDOW_SIZE_X) // → キー を押したか
		{
			bar_x = bar_x + 2;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) && (bar_x) > 0)
		{
			bar_x -= 2;
		}
	}
	else if (currentLevel == 3)
	{
		if (CheckHitKey(KEY_INPUT_RIGHT) && (bar_x + currentBarSizeX) < WINDOW_SIZE_X) // → キー を押したか
		{
			bar_x = bar_x + 2;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) && (bar_x) > 0)
		{
			bar_x -= 2;
		}
	}
	else if (currentLevel == 4)
	{
		if (CheckHitKey(KEY_INPUT_RIGHT) && (bar_x + currentBarSizeX) < WINDOW_SIZE_X) // → キー を押したか
		{
			bar_x = bar_x + 2;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) && (bar_x) > 0)
		{
			bar_x -= 2;
		}
	}
}

//ボールの座標を変える関数 反射を考える
void MoveBall()
{
	int ballx1 = ball_x - BALL_SIZE;//ballの左端
	int ballx2 = ball_x + BALL_SIZE;//ballの右端
	int bally1 = ball_y - BALL_SIZE;//ballの上端
	int bally2 = ball_y + BALL_SIZE;//ballの下端


	//反射を考える

	if (ballx2 > WINDOW_SIZE_X)
	{
		vx = -vx;
	}
	else if (bally1 < 0)
	{
		vy = -vy;
	}
	else if (ballx1 < 0)
	{
		vx = -vx;
	}
	else if ((bally2 > bar_y) && (bally2 < bar_y + BAR_SIZE_Y))
	{
		if ((ball_x > bar_x) && (ball_x < bar_x + currentBarSizeX))
		{
			vy = -vy;
			PlaySoundMem(barHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
		}
	}
	//test part is below. Note!!!! -> gotta check all sides. Bottom, top, left, right!
	//add points when collision is detected here
	else if (isDeleteBlock(bally1, ball_x) == true && getBlockNumY(bally1) >= 0 && getBlockNumX(ball_x)>= 0)
	{
		vy = -vy;
		block[getBlockNumY(bally1)][getBlockNumX(ball_x)] = 0;
		score += (BLOCK_NUM_Y - getBlockNumY(bally1)) * 100;
		levelTracker(score);

		//sound effect
		PlaySoundMem(blockHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);

	}
	else if (isDeleteBlock(ball_y, ballx2) == true && getBlockNumY(ball_y) >= 0 && getBlockNumX(ballx2) >= 0)
	{
		vx = -vx;
		block[getBlockNumY(ball_y)][getBlockNumX(ballx2)] = 0;
		score += (BLOCK_NUM_Y - getBlockNumY(ball_y)) * 100;
		levelTracker(score);

		//sound effect
		PlaySoundMem(blockHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
	}
	else if (isDeleteBlock(ball_y, ballx1) == true && getBlockNumY(ball_y) >= 0 && getBlockNumX(ballx1) >= 0)
	{
		vx = -vx;
		block[getBlockNumY(ball_y)][getBlockNumX(ballx1)] = 0;
		score += (BLOCK_NUM_Y - getBlockNumY(ball_y)) * 100;
		levelTracker(score);

		//sound effect
		PlaySoundMem(blockHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
	}
	else if (isDeleteBlock(bally2, ball_x) == true && getBlockNumY(bally2) >= 0 && getBlockNumX(ball_x) >= 0)
	{
		vy = -vy;
		block[getBlockNumY(bally2)][getBlockNumX(ball_x)] = 0;
		score += (BLOCK_NUM_Y - getBlockNumY(bally2)) * 100;
		levelTracker(score);

		//sound effect
		PlaySoundMem(blockHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
	}

		//until here
	ball_x += vx;
	ball_y += vy;
}

//keep tract of level
void levelTracker(int scoreTmpData)
{
	if (scoreTmpData >= levelTwoThreshold && scoreTmpData <= levelThreeThreshold) //level 2 here
	{
		currentLevel = 2;
		//setting the block to ACTIVE
		stoneBlock[stoneBlockX1] = 1;
		stoneBlock[stoneBlockX2] = 1;
		stoneBlock[stoneBlockX3] = 1;
		
	}
	else if (scoreTmpData > levelThreeThreshold && scoreTmpData <= levelFourThreshold)
	{
		currentLevel = 3;
		currentBarSizeX = BAR_SIZE_X/2;
	}
	else if (scoreTmpData > levelFourThreshold) 
	{
		currentLevel = 4;
	}
}







//check if block is there
bool isDeleteBlock(int by, int bx)
{
	if (block[getBlockNumY(by)][getBlockNumX(bx)] == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//get index of the ball
int getBlockNumX(int bx)
{
	if (bx >= 0 && bx < WINDOW_SIZE_X)
	{
		return (bx / BLOCK_SIZE_X);
	}
	else
	{
		return -1;
	}
}

int getBlockNumY(int by)
{
	if ((by >= 40) && (by <= 60))
	{
		return 0;
	}
	else if ((by > 60) && (by <= 80))
	{
		return 1;
	}
	else if ((by > 80) && (by <= 100))
	{
		return 2;
	}
	else if ((by > 100) && (by <= 120))
	{
		return 3;
	}
	else if ((by > 120) && (by <= 140))
	{
		return 4;
	}
	else if ((by > 140) && (by <= 160))
	{
		return 5;
	}
	else
	{
		return -1;
	}
}







bool gameOverCheck()
{
	if (ball_y - BALL_SIZE > WINDOW_SIZE_Y) {
		// Game Over!
		DrawFormatString(WINDOW_SIZE_X / 2 - 50, WINDOW_SIZE_Y / 2, GetColor(255, 0, 0), "GAME OVER!");
		ScreenFlip(); // Show the game over message
		return true;
	}
	else
	{
		return false;
	}
}


void resetGame()
{
	score = 0;
	currentLevel = 1;
	currentBarSizeX = BAR_SIZE_X;
	
	//set the bar position to its original pos n the ball pos too
	bar_x = WINDOW_SIZE_X / 2 - BAR_SIZE_X / 2;
	ball_x = bar_x + BAR_SIZE_X / 2;
	ball_y = bar_y - BALL_SIZE;


	int possibleValueForVx[4] = { -2, -1, 1, 2 };
	int tmp = rand() % 4;
	//set the speed back
	vx = possibleValueForVx[tmp];
	vy = -1;
	
	//set all blocks backkkk 
	for (int y = 0; y < BLOCK_NUM_Y; ++y) {
		for (int x = 0; x < BLOCK_NUM_X; ++x) {
			block[y][x] = 1; // Set block back to visible
		}
	}

	//generate the stone's pos hereeeee 
	stoneBlockX1 = rand() % BLOCK_NUM_X;
	do
	{
		stoneBlockX2 = rand() % BLOCK_NUM_X;
	} while (stoneBlockX1 == stoneBlockX2);
	do
	{
		stoneBlockX3 = rand() % BLOCK_NUM_X;
	} while (stoneBlockX3 == stoneBlockX1 || stoneBlockX3 == stoneBlockX2);
}

bool gameWinCheck()
{
	bool allBlocksDestroyed = true;
	for (int y = 0; y < BLOCK_NUM_Y; y++)
	{
		for (int x = 0; x < BLOCK_NUM_X; x++)
		{
			if (block[y][x] == 1)
			{
				allBlocksDestroyed = false;
				break;
			}
		}
		if (allBlocksDestroyed == false)
		{
			break;
		}
	}
	if (allBlocksDestroyed ==  true)
	{
		DrawFormatString(WINDOW_SIZE_X / 2 - 50, WINDOW_SIZE_Y / 2, GetColor(0, 255, 0), "YOU WIN!");
		ScreenFlip();
		return true;
	}
	//if not we gonna return false
	return false;
}

bool playAgainPrompt()
{
	ClearDrawScreen();
	static int playAgainBackground = LoadGraph("playagain.bmp");
	DrawGraph(0, 0, playAgainBackground, FALSE);

	DrawFormatString(WINDOW_SIZE_X / 2 - 80, WINDOW_SIZE_Y / 2 - 20, GetColor(255, 255, 255), "Play Again? (Y/N)");
	ScreenFlip();
	WaitTimer(50);
	while (1) {
		if (ProcessMessage() == -1) {
			return false; // User closed the window
		}
		if (CheckHitKey(KEY_INPUT_Y)) {
			WaitTimer(200); 
			return true; //initiate a new gameeee
		}
		if (CheckHitKey(KEY_INPUT_N)) {
			WaitTimer(200);
			return false; //exit
		}
		WaitTimer(10);
	}
}

// WinMain関数
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// タイトルを 変更
	SetMainWindowText( "ブロック崩し見本 by 橋本" ) ;
	// ウインドウモードに変更
	ChangeWindowMode( TRUE ) ;
	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 )		// ＤＸライブラリ初期化処理
		return -1 ;			// エラーが起きたら直ちに終了
	// マウスを表示状態にする
	SetMouseDispFlag( TRUE ) ;

	srand(static_cast<unsigned int>(time(NULL)));

	bgmHandler = LoadSoundMem("bgm.mp3");
	blockHitSoundHandler = LoadSoundMem("block_hit.wav");
	barHitSoundHandler = LoadSoundMem("bar_hit.wav");

	if (bgmHandler == -1 || blockHitSoundHandler == -1 || barHitSoundHandler == -1)
	{
		DxLib_End();
		return -1;
		//if the sound fail to load
	}

	//playing bgm
	PlaySoundMem(bgmHandler, DX_PLAYTYPE_LOOP, TRUE);

	currentBarSizeX = BAR_SIZE_X;
	
	Draw();
	DrawFormatString(10, 10, GetColor(255, 255, 255), "Score: %d  Level: %d", score, currentLevel);

	while(1)//ゲームを何度もプレイするためのループ
	{
		resetGame();
		DrawFormatString(WINDOW_SIZE_X / 2 - 100, WINDOW_SIZE_Y / 2 + 40, GetColor(255, 255, 0), "Press SPACE to Start"); // Optional: Add a start message
		ScreenFlip(); // Show the start message
		// スペースボタンが押されるまで待機
		while( !CheckHitKey( KEY_INPUT_SPACE ) )
		{
			// メッセージループに代わる処理をする
			if( ProcessMessage() == -1 )
			{
				DxLib_End() ;				// ＤＸライブラリ使用の終了処理
				return 0 ;				// ソフトの終了 
			}
		}

		// ゲームメインループ
		while ( 1 )
		{
			// メッセージループに代わる処理をする
			if( ProcessMessage() == -1 )
			{
				DxLib_End() ;				// ＤＸライブラリ使用の終了処理
				return 0 ;				// ソフトの終了 
			}
			MoveBar();
			MoveBall();
			Draw();
			if (gameOverCheck() == true)
			{
				break;
			}
			if (gameWinCheck() == true)
			{
				break;
			}

			//check if game is over
			
			WaitTimer(4);
		}
		WaitTimer(1000);
		if (playAgainPrompt() == false)
		{
			DxLib_End();
			return 0;
		}
	}
	DxLib_End() ;				// ＤＸライブラリ使用の終了処理
	return 0 ;				// ソフトの終了 
}