/*******************************************************************

	ブロック崩し プログラム ヒント　　　block0.cpp

	松江工業高等専門学校　情報工学科　教授　橋本　剛

*******************************************************************/
#include "DxLib.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

//Game constants
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

//Global var
int bar_y = WINDOW_SIZE_Y * 9 / 10;
float bar_x = WINDOW_SIZE_X / 2.0f - BAR_SIZE_X / 2.0f;
int block[BLOCK_NUM_Y][BLOCK_NUM_X];
int BlockHandle[BLOCK_NUM_Y]; 
int blockDesign[BLOCK_NUM_Y][BLOCK_NUM_X]; 

const int maxNumOfBall = 100;

struct Ball
{
	float x, y;
	int vx, vy;
	bool active;
};

Ball balls[maxNumOfBall];
int activeBallCount = 0;

//Speed factors for delta time adjustment, these guys are tunable!!!!!! <---
const float BALL_SPEED_FACTOR = 200.0f;
const float BAR_SPEED_FACTOR = 220.0f;


bool isDeleteBlock(int by, int bx);
int getBlockNumY(int by);
int getBlockNumX(int bx);
void levelTracker(int scoreTmpData);
void SpawnNewBall(const Ball& parentBall);
void specialBlockSpawner(int spawnNum);
void resetGame();
bool gameWinCheck();
bool playAgainPrompt();
bool gameOverCheck();
void ResetBallAfterLifeLost();

int currentLevel = 1;
int score = 0;
int playerLives = 3;
int levelTwoThreshold = 300;
int levelThreeThreshold = 700;
int levelFourThreshold = 2000;
int levelFiveThreshold = 4000;
int levelSixThreshold = 8000;
int levelSevenThreshold = 12000;

int bgmHandler = -1;
int blockHitSoundHandler = -1;
int barHitSoundHandler = -1;
int specialHitSoundHandler = -1;
int stoneHitSoundHandler = -1;

int currentBarSizeX; 

void Draw()
{
	static int GrHandle = LoadGraph("gamebackground.png");
	if (GrHandle == -1) {
		DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, GetColor(10, 20, 80), TRUE); 
	}
	else 
	{
		DrawGraph(0, 0, GrHandle, FALSE);
	}

	static int colorBlock = LoadGraph("block.bmp");
	static int stoneBlockHandle = LoadGraph("stone.bmp");
	static int specialBlockHandle = LoadGraph("specialBlock.bmp");

	DrawBox((int)bar_x, bar_y, (int)bar_x + currentBarSizeX, bar_y + BAR_SIZE_Y, GetColor(255, 255, 255), TRUE);

	for (int i = 0; i < maxNumOfBall; ++i) {
		if (balls[i].active) {
			DrawCircle((int)balls[i].x, (int)balls[i].y, BALL_SIZE, GetColor(255, 255, 0), TRUE);
		}
	}

	DrawFormatString(10, 10, GetColor(255, 255, 255), "Score: %d  Level: %d", score, currentLevel);
	DrawFormatString(WINDOW_SIZE_X - 100, 10, GetColor(255, 255, 255), "Lives: %d", playerLives);

	for (int y = 0; y < BLOCK_NUM_Y; y++)
	{
		for (int x = 0; x < BLOCK_NUM_X; x++)
		{
			int blockType = block[y][x];
			if (blockType == 1)
			{
				int designIndex = blockDesign[y][x];
				int posToGetExactBlockDesign = designIndex * BLOCK_SIZE_X;
				DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, posToGetExactBlockDesign, 0, BLOCK_SIZE_X, BLOCK_SIZE_Y, colorBlock, FALSE, FALSE);
			}
			else if (blockType == 2)
			{
				DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 0, 0, BLOCK_SIZE_X, BLOCK_SIZE_Y, specialBlockHandle, FALSE, FALSE);
			}
			else if (blockType == 3)
			{
				DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y, 0, 0, BLOCK_SIZE_X, BLOCK_SIZE_Y, stoneBlockHandle, FALSE, FALSE);
			}
		}
	}
}

//need deltaTime
void MoveBar(float deltaTime)
{
	int moveSpeedBase = 2;
	if (currentLevel >= 4) {
		moveSpeedBase = 3;
	}
	float actualMoveDistance = moveSpeedBase * BAR_SPEED_FACTOR * deltaTime;

	if (CheckHitKey(KEY_INPUT_RIGHT) && (bar_x + currentBarSizeX) < WINDOW_SIZE_X)
	{
		bar_x += actualMoveDistance;
	}
	else if (CheckHitKey(KEY_INPUT_LEFT) && (bar_x) > 0)
	{
		bar_x -= actualMoveDistance;
	}

	if ((bar_x + currentBarSizeX) > WINDOW_SIZE_X) bar_x = (float)WINDOW_SIZE_X - currentBarSizeX;
	if (bar_x < 0) bar_x = 0.0f;
}

//deltaTime's also needed here for consistency
void MoveBall(float deltaTime)
{
	for (int i = 0; i < maxNumOfBall; ++i)
	{
		if (!balls[i].active) continue;

		// Checkin if the ball is out of window area
		if (balls[i].y - BALL_SIZE > WINDOW_SIZE_Y) {
			balls[i].active = false;
			activeBallCount--;
			continue;
		}

		float moveX_thisFrame = balls[i].vx * BALL_SPEED_FACTOR * deltaTime;
		float moveY_thisFrame = balls[i].vy * BALL_SPEED_FACTOR * deltaTime;
		float nextX = balls[i].x + moveX_thisFrame;
		float nextY = balls[i].y + moveY_thisFrame;
		bool bounced = false;

		// Wall collisions
		if (nextX + BALL_SIZE > WINDOW_SIZE_X && balls[i].vx > 0) {
			balls[i].vx *= -1;
			bounced = true;
		}
		else if (nextX - BALL_SIZE < 0 && balls[i].vx < 0) {
			balls[i].vx *= -1; 
			bounced = true;
		}

		if (nextY - BALL_SIZE < 0 && balls[i].vy < 0) {
			balls[i].vy *= -1; 
			bounced = true;
		}

		// Bar collision
		else if (balls[i].vy > 0 && nextY + BALL_SIZE > bar_y && balls[i].y - BALL_SIZE < bar_y + BAR_SIZE_Y)
		{
			if ((nextX + BALL_SIZE > bar_x) && (nextX - BALL_SIZE < bar_x + currentBarSizeX))
			{
				balls[i].vy *= -1; 
				balls[i].y = (float)bar_y - BALL_SIZE;

				PlaySoundMem(barHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
				bounced = true;
			}
		}

		//block collision
		if (!bounced)
		{
			bool blockCollisionProcessed = false;
			int ballXcoordi = (int)balls[i].x;
			int ballYcoordi = (int)balls[i].y;
			int checkPoints[4][2] = { 
				{ballXcoordi, (int)(balls[i].y - BALL_SIZE)}, 
				{ballXcoordi, (int)(balls[i].y + BALL_SIZE)},
				{(int)(balls[i].x - BALL_SIZE), ballYcoordi}, 
				{(int)(balls[i].x + BALL_SIZE), ballYcoordi}
			};
			int bounceAxis[4] = { 1, 1, 0, 0 }; //1 for Y-axis     0 for X-axis

			for (int p = 0; p < 4 && !blockCollisionProcessed; ++p) {
				int hitBlockY = getBlockNumY(checkPoints[p][1]);
				int hitBlockX = getBlockNumX(checkPoints[p][0]);

				if (hitBlockY >= 0 && hitBlockY < BLOCK_NUM_Y && hitBlockX >= 0 && hitBlockX < BLOCK_NUM_X) {
					int blockType = block[hitBlockY][hitBlockX];
					if (blockType > 0) {
						if (bounceAxis[p] == 1) {
							if ((p == 0 && balls[i].vy < 0) || (p == 1 && balls[i].vy > 0)) balls[i].vy *= -1;
						}
						else {
							if ((p == 2 && balls[i].vx < 0) || (p == 3 && balls[i].vx > 0)) balls[i].vx *= -1;
						}
						if (blockType == 1) {
							block[hitBlockY][hitBlockX] = 0; 
							score += (BLOCK_NUM_Y - hitBlockY) * 100; 
							
							levelTracker(score);
							PlaySoundMem(blockHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
						}
						else if (blockType == 2) {
							block[hitBlockY][hitBlockX] = 0; 
							score += 250; 
							
							levelTracker(score);
							PlaySoundMem(specialHitSoundHandler != -1 ? specialHitSoundHandler : blockHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
							SpawnNewBall(balls[i]);
						}
						else if (blockType == 3) {
							PlaySoundMem(stoneHitSoundHandler != -1 ? stoneHitSoundHandler : barHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
						}
						blockCollisionProcessed = true;
					}
				}
			}
		}
		if (balls[i].active) {
			balls[i].x += balls[i].vx * BALL_SPEED_FACTOR * deltaTime;
			balls[i].y += balls[i].vy * BALL_SPEED_FACTOR * deltaTime;
		}
	}
}

// Spawns a new ball
void SpawnNewBall(const Ball& parentBall) 
{
	if (activeBallCount >= maxNumOfBall) return;

	for (int i = 0; i < maxNumOfBall; ++i) 
	{
		if (!balls[i].active) 
		{
			balls[i].active = true; 
			balls[i].x = parentBall.x; 
			balls[i].y = parentBall.y;

			balls[i].vx = -parentBall.vx; 
			balls[i].vy = -parentBall.vy;

			if (balls[i].vx == 0 && balls[i].vy == 0) 
			{ 
				balls[i].vx = (rand() % 2) * 2 - 1; 
				balls[i].vy = -1; 
			}
			else if (balls[i].vx == 0) 
			{ 
				balls[i].vx = (parentBall.vx != 0) ? -parentBall.vx : ((rand() % 2) * 2 - 1); 
			}
			else if (balls[i].vy == 0) 
			{ 
				balls[i].vy = (parentBall.vy != 0) ? -parentBall.vy : -1; 
			}
			activeBallCount++; return;
		}
	}
}

void levelTracker(int scoreTmpData)
{
	int previousLevel = currentLevel;
	if (scoreTmpData < levelTwoThreshold) currentLevel = 1;
	else if (scoreTmpData < levelThreeThreshold) currentLevel = 2;
	else if (scoreTmpData < levelFourThreshold) currentLevel = 3;
	else if (scoreTmpData < levelFiveThreshold) currentLevel = 4;
	else if (scoreTmpData < levelSixThreshold) currentLevel = 5;
	else if (scoreTmpData < levelSevenThreshold) currentLevel = 6;
	else currentLevel = 7;

	if (currentLevel > previousLevel) { // Level up actions
		if (currentLevel == 2) specialBlockSpawner(2);
		else if (currentLevel == 3) specialBlockSpawner(3);
		else if (currentLevel == 4) 
		{
			specialBlockSpawner(4);

			int stoneY = 1; 
			int stoneCols[] = { 3, 7, 11 };

			for (int i = 0; i < sizeof(stoneCols) / sizeof(stoneCols[0]); ++i)
			{
				if (stoneCols[i] >= 0 && stoneCols[i] < BLOCK_NUM_X && block[stoneY][stoneCols[i]] == 1) 
					{ 
						block[stoneY][stoneCols[i]] = 3;
					}
			}
				
					
		}
		else if (currentLevel == 5) 
		{
			currentBarSizeX = BAR_SIZE_X / 2; 
			specialBlockSpawner(5);

			int stoneY = 3; 
			int stoneCols[] = { 2, 6, 10, 14 };

			for (int i = 0; i < sizeof(stoneCols) / sizeof(stoneCols[0]); ++i)
				if (stoneCols[i] >= 0 && stoneCols[i] < BLOCK_NUM_X && block[stoneY][stoneCols[i]] == 1) block[stoneY][stoneCols[i]] = 3;
		}
		else if (currentLevel == 6) 
		{
			currentBarSizeX = BAR_SIZE_X / 2; 
			specialBlockSpawner(6);

			int stoneY = 0; 
			int stoneCols[] = { 1, 5, 9, 13 };

			for (int i = 0; i < sizeof(stoneCols) / sizeof(stoneCols[0]); ++i)
				if (stoneCols[i] >= 0 && stoneCols[i] < BLOCK_NUM_X && block[stoneY][stoneCols[i]] == 1) block[stoneY][stoneCols[i]] = 3;
		}
		else if (currentLevel == 7) 
		{
			currentBarSizeX = BAR_SIZE_X / 2; 
			specialBlockSpawner(7);


			int stoneY = 5; 
			int stoneCols[] = { 0, 4, 8, 12 };

			for (int i = 0; i < sizeof(stoneCols) / sizeof(stoneCols[0]); ++i)
				if (stoneCols[i] >= 0 && stoneCols[i] < BLOCK_NUM_X && block[stoneY][stoneCols[i]] == 1) block[stoneY][stoneCols[i]] = 3;
		}
	}
}

//spawn special blocks (type 2) in empty spaces
void specialBlockSpawner(int numToSpawn)
{
	int gapX[BLOCK_NUM_Y * BLOCK_NUM_X], gapY[BLOCK_NUM_Y * BLOCK_NUM_X], gapCount = 0;

	for (int y = 0; y < BLOCK_NUM_Y; ++y) 
	{ 
		for (int x = 0; x < BLOCK_NUM_X; ++x) 
		{ 
			if (block[y][x] == 0) 
			{ 
				gapX[gapCount] = x; 
				gapY[gapCount++] = y; 
			} 
		}
	}
	if (gapCount < numToSpawn) numToSpawn = gapCount;

	for (int i = 0; i < numToSpawn; ++i) 
	{
		if (gapCount <= 0) break;

		int randomIndex = rand() % gapCount; 
		block[gapY[randomIndex]][gapX[randomIndex]] = 2;

		gapX[randomIndex] = gapX[--gapCount]; 
		gapY[randomIndex] = gapY[gapCount]; // Replace chosen with last and shrink
	}
}


bool isDeleteBlock(int by, int bx) {
	int blockY = getBlockNumY(by); 
	int blockX = getBlockNumX(bx);
	return (blockY != -1 && blockX != -1 && block[blockY][blockX] == 1);
}
int getBlockNumX(int bx) { return (bx >= 0 && bx < WINDOW_SIZE_X) ? (bx / BLOCK_SIZE_X) : -1; }
int getBlockNumY(int by) { return (by >= BLOCK_TOP_Y && by < (BLOCK_TOP_Y + BLOCK_NUM_Y * BLOCK_SIZE_Y)) ? ((by - BLOCK_TOP_Y) / BLOCK_SIZE_Y) : -1; }



void ResetBallAfterLifeLost()
{
	WaitTimer(500);
	activeBallCount = 1; 
	balls[0].active = true;

	balls[0].x = bar_x + currentBarSizeX / 2.0f; 
	balls[0].y = (float)bar_y - BALL_SIZE;

	balls[0].vx = (rand() % 2) * 2 - 1; 
	balls[0].vy = -1;

	for (int i = 1; i < maxNumOfBall; ++i) balls[i].active = false;
}


bool gameOverCheck()
{
	if (activeBallCount <= 0) {
		if (playerLives > 1) 
		{ 
			playerLives--; 
			ResetBallAfterLifeLost(); 
			return false; 
		}
		else { 
			DrawFormatString(WINDOW_SIZE_X / 2 - 50, WINDOW_SIZE_Y / 2, GetColor(255, 0, 0), "GAME OVER!"); 
			ScreenFlip(); 
			return true; 
		}
	}
	return false;
}


void resetGame()
{
	//reset game state
	score = 0; 
	currentLevel = 1; 
	playerLives = 3; 
	currentBarSizeX = BAR_SIZE_X;
	bar_x = WINDOW_SIZE_X / 2.0f - currentBarSizeX / 2.0f;

	//r ball state
	activeBallCount = 1; 
	balls[0].active = true;
	//r ball position
	balls[0].x = bar_x + currentBarSizeX / 2.0f; 
	balls[0].y = (float)bar_y - BALL_SIZE;
	//r ball velocity
	balls[0].vx = (rand() % 2) * 2 - 1; 
	balls[0].vy = -1;

	//r block state
	for (int i = 1; i < maxNumOfBall; ++i) 
	{
		balls[i].active = false;
	}

	for (int y = 0; y < BLOCK_NUM_Y; ++y) 
	{ 
		for (int x = 0; x < BLOCK_NUM_X; ++x)
		{
			block[y][x] = 1;
		}
	}
	
	//set up block desgin. Basically we gonna rand() between 6, and put that number in the blockDesign[y][x] then gon pull that up when Draw() the block
	const int NUM_OF_BLOCK_DESIGNS = 6; 
	for (int y = 0; y < BLOCK_NUM_Y; ++y) 
	{
		for (int x = 0; x < BLOCK_NUM_X; ++x)
			blockDesign[y][x] = (block[y][x] == 1) ? (rand() % NUM_OF_BLOCK_DESIGNS) : 0;
	}
}


bool gameWinCheck()
{
	bool blocksRemaining = false;
	for (int y = 0; y < BLOCK_NUM_Y && !blocksRemaining; y++)
		for (int x = 0; x < BLOCK_NUM_X && !blocksRemaining; x++)
			if (block[y][x] == 1 || block[y][x] == 2) blocksRemaining = true;
	if (!blocksRemaining) {
		for (int i = 0; i < maxNumOfBall; ++i) balls[i].active = false; activeBallCount = 0;
		DrawFormatString(WINDOW_SIZE_X / 2 - 50, WINDOW_SIZE_Y / 2, GetColor(0, 255, 0), "YOU WIN!"); ScreenFlip(); return true;
	}
	return false;
}


bool playAgainPrompt()
{
	ClearDrawScreen();
	static int playAgainBackground = LoadGraph("playagain.bmp");
	if (playAgainBackground != -1) 
	{ 
		DrawGraph(0, 0, playAgainBackground, FALSE); 
	}
	else
	{
		DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, GetColor(0, 0, 50), TRUE);
	}

	DrawFormatString(WINDOW_SIZE_X / 2 - 80, WINDOW_SIZE_Y / 2 - 20, GetColor(255, 255, 255), "Play Again? (Y/N)");
	ScreenFlip(); WaitTimer(50);

	while (1) {
		if (ProcessMessage() == -1) return false;
		if (CheckHitKey(KEY_INPUT_Y)) { WaitTimer(200); return true; }
		if (CheckHitKey(KEY_INPUT_N)) { WaitTimer(200); return false; }
		WaitTimer(10);
	}
}

// Main Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetMainWindowText("ブロック崩しゲーム");
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;

	SetDrawScreen(DX_SCREEN_BACK);
	SetMouseDispFlag(TRUE);
	srand(static_cast<unsigned int>(time(NULL)));

	//sounds
	bgmHandler = LoadSoundMem("bgm.mp3");
	blockHitSoundHandler = LoadSoundMem("block_hit.wav");
	barHitSoundHandler = LoadSoundMem("bar_hit.wav");
	specialHitSoundHandler = LoadSoundMem("special_hit.mp3");
	stoneHitSoundHandler = LoadSoundMem("stone_hit.mp3");

	if (bgmHandler == -1 || blockHitSoundHandler == -1 || barHitSoundHandler == -1) { DxLib_End(); return -1; }
	if (specialHitSoundHandler == -1) specialHitSoundHandler = blockHitSoundHandler; 
	if (stoneHitSoundHandler == -1) stoneHitSoundHandler = barHitSoundHandler;     

	PlaySoundMem(bgmHandler, DX_PLAYTYPE_LOOP, TRUE);

	long long lastFrameTime = GetNowCount();
	long long currentTime = 0;
	float deltaTime = 0.0f;

	while (1)
	{
		resetGame();

		Draw();
		DrawFormatString(WINDOW_SIZE_X / 2 - 100, WINDOW_SIZE_Y / 2 + 40, GetColor(255, 255, 0), "Press SPACE to Start");
		ScreenFlip();

		while (!CheckHitKey(KEY_INPUT_SPACE)) { 
			if (ProcessMessage() == -1) { DxLib_End(); return 0; }
			lastFrameTime = GetNowCount(); 
			WaitTimer(10);
		}

		lastFrameTime = GetNowCount(); //reset timer just before gameplay starts


		//main
		while (1)
		{
			currentTime = GetNowCount();
			deltaTime = (currentTime - lastFrameTime) / 1000.0f;
			lastFrameTime = currentTime;

			//incase dt's broken lmaooo
			if (deltaTime > 0.1f) deltaTime = 0.1f;           
			if (deltaTime <= 0.0f) deltaTime = 1.0f / 60.0f; 

			if (ProcessMessage() == -1) { DxLib_End(); return 0; } 

			MoveBar(deltaTime);
			MoveBall(deltaTime);

			
			//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160); // Alpha value for trail (e.g., 64, 128, 192)
			//DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, GetColor(0, 0, 0), TRUE);
			//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			Draw();
			ScreenFlip();

			if (gameOverCheck() || gameWinCheck()) break;
		}

		WaitTimer(1000);
		if (!playAgainPrompt()) break;
	}

	DxLib_End();
	return 0;
}
