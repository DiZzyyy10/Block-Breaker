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

//new implementation of the ball
const int maxNumOfBall = 100;

struct Ball
{
	int x, y;
	int vx, vy;
	bool active;
};

Ball balls[maxNumOfBall];

int activeBallCount = 0;


bool isStoneBlockThere(int by, int bx);
bool isDeleteBlock(int by, int bx);
int getBlockNumY(int by);
int getBlockNumX(int bx);
void levelTracker(int scoreTmpData);
void SpawnNewBall(const Ball& parentBall);

void resetGame();
bool gameWinCheck();
bool playAgainPrompt();


int currentLevel = 1;
int score = 0;
int levelTwoThreshold = 500;
int levelThreeThreshold = 1000;
int levelFourThreshold = 4000;

int stoneBlockX1 = -1; 
int stoneBlockX2 = -1;
int stoneBlockX3 = -1;

//sound 
int bgmHandler = -1;
int blockHitSoundHandler = -1;
int barHitSoundHandler = -1;
int specialHitSoundHandler = -1;
int stoneHitSoundHandler = -1;

//bar size
int currentBarSizeX;

//end here for my declaration




//描画関数
void Draw()
{
	static int GrHandle = LoadGraph( "gamebg.bmp" );//背景画像登録 640x480
	static int colorBlock = LoadGraph("block.bmp");
	static int stoneBlockHandle = LoadGraph("stone.bmp");
	static int specialBlockHandle = LoadGraph("specialBlock.bmp");

	DrawGraph( 0 , 0 , GrHandle , FALSE );//背景を描く
	DrawBox(bar_x, bar_y, bar_x + currentBarSizeX, bar_y + BAR_SIZE_Y, GetColor(255, 255, 255), TRUE);;//BARを描く

	//drawing the ball(s)?? cough* cough*
	for (int i = 0; i < maxNumOfBall; ++i) {
		if (balls[i].active) {
			DrawCircle(balls[i].x, balls[i].y, BALL_SIZE, GetColor(255, 255, 0), TRUE);
		}
	}


	DrawFormatString(10, 10, GetColor(255, 255, 255), "Score: %d  Level: %d", score, currentLevel);
	for ( int y = 0; y < BLOCK_NUM_Y; y++ )
	{
		for (int x = 0; x < BLOCK_NUM_X; x++)
		{
			if (block[y][x] == 1)
			{
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
			else if (block[y][x] == 2)
			{
				if (y == 0)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 0, 0, 40, 20, specialBlockHandle, FALSE, FALSE);
				}
				else if (y == 1)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 40, 0, 40, 20, specialBlockHandle, FALSE, FALSE);
				}
				else if (y == 2)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 80, 0, 40, 20, specialBlockHandle, FALSE, FALSE);
				}
				else if (y == 3)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 120, 0, 40, 20, specialBlockHandle, FALSE, FALSE);
				}
				else if (y == 4)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 160, 0, 40, 20, specialBlockHandle, FALSE, FALSE);
				}
				else if (y == 5)
				{
					DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 200, 0, 40, 20, specialBlockHandle, FALSE, FALSE);
				}

			}
		}
	}

	//drawing stones for level 2
	if (currentLevel >= 2)
	{
		int stoneY = BLOCK_TOP_Y + BLOCK_NUM_Y * BLOCK_SIZE_Y;
		if (stoneBlockX1 >= 0 && stoneBlockX1 < BLOCK_NUM_X)
		{
			DrawRectGraph(stoneBlockX1 * BLOCK_SIZE_X, stoneY, 0, 0, 40,20, stoneBlockHandle, FALSE, FALSE);
		}
		if (stoneBlockX2 >= 0 && stoneBlockX2 < BLOCK_NUM_X)
		{
			DrawRectGraph(stoneBlockX2 * BLOCK_SIZE_X, stoneY, 0, 0, 40, 20, stoneBlockHandle, FALSE, FALSE);
		}
		if (stoneBlockX3 >= 0 && stoneBlockX3 < BLOCK_NUM_X)
		{
			DrawRectGraph(stoneBlockX3 * BLOCK_SIZE_X, stoneY, 0, 0, 40, 20, stoneBlockHandle, FALSE, FALSE);
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

void MoveBall()
{
	for (int i = 0; i < maxNumOfBall; ++i)
	{
		if (!balls[i].active) {
			continue;
		}

		int ballx1 = balls[i].x - BALL_SIZE;//left edge
		int ballx2 = balls[i].x + BALL_SIZE;//right edge
		int bally1 = balls[i].y - BALL_SIZE;//top edge
		int bally2 = balls[i].y + BALL_SIZE; //bottom edge

		bool bounced = false;

		// Right wall
		if (ballx2 > WINDOW_SIZE_X && balls[i].vx > 0) {
			balls[i].vx = -balls[i].vx;
			balls[i].x = WINDOW_SIZE_X - BALL_SIZE;
			bounced = true;
		}
		// Left wall
		else if (ballx1 < 0 && balls[i].vx < 0) {
			balls[i].vx = -balls[i].vx;
			balls[i].x = BALL_SIZE;
			bounced = true;
		}

		// Top wall
		if (bally1 < 0 && balls[i].vy < 0) {
			balls[i].vy = -balls[i].vy;
			balls[i].y = BALL_SIZE;
			bounced = true;
		}
		// collision with Bar
		else if (bally2 > bar_y && bally1 < (bar_y + BAR_SIZE_Y) && balls[i].vy > 0)
		{
			if ((ballx2 > bar_x) && (ballx1 < bar_x + currentBarSizeX))
			{
				balls[i].vy = -balls[i].vy;
				balls[i].y = bar_y - BALL_SIZE;
				PlaySoundMem(barHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
				bounced = true;
			}
		}

		
		else if (currentLevel >= 2 && !bounced) 
		{
			bool stoneCollisionProcessed = false;
			if (isStoneBlockThere(bally1, balls[i].x) && balls[i].vy < 0) {
				balls[i].vy = -balls[i].vy; // Bounce vertically
				PlaySoundMem(stoneHitSoundHandler, DX_PLAYTYPE_BACK, TRUE); 
				stoneCollisionProcessed = true;
				bounced = true; // Mark that a bounce occurred this frame
			}
			// Check bottom edge collision with stone (less likely to hit bottom first)
			else if (isStoneBlockThere(bally2, balls[i].x) && balls[i].vy > 0) {
				balls[i].vy = -balls[i].vy; // Bounce vertically
				PlaySoundMem(stoneHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
				stoneCollisionProcessed = true;
				bounced = true;
			}
			// Check left edge collision with stone
			else if (isStoneBlockThere(balls[i].y, ballx1) && balls[i].vx < 0) {
				balls[i].vx = -balls[i].vx; // Bounce horizontally
				PlaySoundMem(stoneHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
				stoneCollisionProcessed = true;
				bounced = true;
			}
			// Check right edge collision with stone
			else if (isStoneBlockThere(balls[i].y, ballx2) && balls[i].vx > 0) {
				balls[i].vx = -balls[i].vx; // Bounce horizontally
				PlaySoundMem(stoneHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
				stoneCollisionProcessed = true;
				bounced = true;
			}
		}
		

		if (!bounced)
		{
			bool blockCollisionProcessed = false;

			int checkPoints[4][2] = {
				{balls[i].x, bally1}, // Top Center
				{balls[i].x, bally2}, // Bottom Center
				{ballx1, balls[i].y}, // Left Center
				{ballx2, balls[i].y}  // Right Center
			};

			int bounceAxis[4] = { 1, 1, 0, 0 }; // 1=vertical, 0=horizontal

			for (int p = 0; p < 4 && !blockCollisionProcessed; ++p) {
				int checkX = checkPoints[p][0];
				int checkY = checkPoints[p][1];

				int hitBlockY = getBlockNumY(checkY);
				int hitBlockX = getBlockNumX(checkX);

				if (hitBlockY >= 0 && hitBlockY < BLOCK_NUM_Y && hitBlockX >= 0 && hitBlockX < BLOCK_NUM_X)
				{
					int blockType = block[hitBlockY][hitBlockX];

					if (blockType > 0)
					{
						//Bounce
						if (bounceAxis[p] == 1) { //Bounce vertically
							if ((p == 0 && balls[i].vy < 0) || (p == 1 && balls[i].vy > 0)) {
								balls[i].vy = -balls[i].vy;
							}
						}
						else { //Bounce horizontally
							if ((p == 2 && balls[i].vx < 0) || (p == 3 && balls[i].vx > 0)) {
								balls[i].vx = -balls[i].vx;
							}
						}

						//Handle actions after the bounce
						if (blockType == 1) // Normal Block Hit
						{
							block[hitBlockY][hitBlockX] = 0;
							score += (BLOCK_NUM_Y - hitBlockY) * 100;
							levelTracker(score);
							PlaySoundMem(blockHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
						}
						else if (blockType == 2) // Special Block Hit
						{
							block[hitBlockY][hitBlockX] = 0;
							score += 250;
							levelTracker(score);
							PlaySoundMem(specialHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);

							SpawnNewBall(balls[i]);
						}

						blockCollisionProcessed = true;
						bounced = true; // Also set bounced here to prevent further checks this frame
					}
				}
			}
		}

		if (balls[i].active) {
			balls[i].x += balls[i].vx;
			balls[i].y += balls[i].vy;
		}
	} 
}


void SpawnNewBall(const Ball& parentBall) {
	if (activeBallCount >= maxNumOfBall) {
		return; // Cannot spawn more balls
	}

	for (int i = 0; i < maxNumOfBall; ++i) {
		if (!balls[i].active) { // Found an inactive slot
			balls[i].active = true;
			balls[i].x = parentBall.x; // Start near the parent ball
			balls[i].y = parentBall.y;

			balls[i].vx = -parentBall.vx;
			balls[i].vy = parentBall.vy;

			activeBallCount++;
			return; // Spawned one ball, exit function
		}
	}
}



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
		int previousLevel = currentLevel;
		currentLevel = 3;
		//currentBarSizeX = BAR_SIZE_X / 2;

		if (previousLevel < 3) {//tryna fill the gaps with special blocks
			int gapX[BLOCK_NUM_Y * BLOCK_NUM_X];
			int gapY[BLOCK_NUM_Y * BLOCK_NUM_X];
			int gapCount = 0;
			for (int y = 0; y < BLOCK_NUM_Y; ++y) {
				for (int x = 0; x < BLOCK_NUM_X; ++x) {
					if (block[y][x] == 0) { // check for empty spot
						gapX[gapCount] = x;
						gapY[gapCount] = y;
						gapCount++;
					}
				}
			}

			int numToSpawn = 3;

			for (int i = 0; i < numToSpawn; ++i) {
				if (gapCount <= 0) break;

				int randomIndex = rand() % gapCount;

				// Place the special block
				block[gapY[randomIndex]][gapX[randomIndex]] = 2;

				gapX[randomIndex] = gapX[gapCount - 1];
				gapY[randomIndex] = gapY[gapCount - 1];
				gapCount--;
			}
		}
		else if (scoreTmpData > levelFourThreshold)
		{
			currentLevel = 4;
		}
	}
}



//check if a stone is there
bool isStoneBlockThere(int by, int bx)
{
	if (stoneBlock[getBlockNumX(bx)] == 1 && (by>160) && (by < 180))
	{
		return true;
	}
	else
	{
		return false;
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
	bool anyBallActive = false;
	for (int i = 0; i < maxNumOfBall; ++i) {
		if (balls[i].active) {
			if (balls[i].y - BALL_SIZE > WINDOW_SIZE_Y) {
				balls[i].active = false; 
				activeBallCount--;
			}
			else {
				anyBallActive = true; 
			}
		}
	}

	if (!anyBallActive && activeBallCount <= 0) {
		DrawFormatString(WINDOW_SIZE_X / 2 - 50, WINDOW_SIZE_Y / 2, GetColor(255, 0, 0), "GAME OVER!");
		ScreenFlip(); 
		return true;
	}
	else {
		return false;
	}
}


void resetGame()
{
	score = 0;
	currentLevel = 1;
	currentBarSizeX = BAR_SIZE_X; 

	// Reset bar position
	bar_x = WINDOW_SIZE_X / 2 - BAR_SIZE_X / 2;

	// Initialize Balls
	activeBallCount = 1; 
	balls[0].active = true;
	balls[0].x = bar_x + currentBarSizeX / 2; 
	balls[0].y = bar_y - BALL_SIZE;

	// Set initial velocity for the first ball
	int possibleValueForVx[4] = {-1, 1};
	int tmp = rand() % 2;
	balls[0].vx = possibleValueForVx[tmp];
	balls[0].vy = -1; 

	// Deactivate all other balls
	for (int i = 1; i < maxNumOfBall; ++i) {
		balls[i].active = false;
	}

	// Reset all blocks back
	for (int y = 0; y < BLOCK_NUM_Y; ++y) {
		for (int x = 0; x < BLOCK_NUM_X; ++x) {
			block[y][x] = 1;
		}
	}


	//code for the stone blocks
	for (int i = 0; i < BLOCK_NUM_X; ++i) {
		stoneBlock[i] = 0; 
	}

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
	bool blocksRemaining = false;
	for (int y = 0; y < BLOCK_NUM_Y; y++)
	{
		for (int x = 0; x < BLOCK_NUM_X; x++)
		{
			if (block[y][x] == 1 || block[y][x] == 2)
			{
				blocksRemaining = true;
				break;
			}
		}
		if (blocksRemaining)
		{
			break;
		}
	}

	if (!blocksRemaining) 
	{
		DrawFormatString(WINDOW_SIZE_X / 2 - 50, WINDOW_SIZE_Y / 2, GetColor(0, 255, 0), "YOU WIN!");
		ScreenFlip();
		return true;
	}
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
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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
	specialHitSoundHandler = LoadSoundMem("block_hit.wav");
	stoneHitSoundHandler = LoadSoundMem("block_hit.wav");

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