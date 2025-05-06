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
// Block array: 0=empty, 1=normal, 2=special, 3=indestructible stone
int block[BLOCK_NUM_Y][BLOCK_NUM_X];
// Removed: int stoneBlock[BLOCK_NUM_X] = { 0, ... };
int BlockHandle[BLOCK_NUM_Y];//ブロックのビットマップ画像ハンドル

const int maxNumOfBall = 100;

struct Ball
{
	int x, y;
	int vx, vy;
	bool active;
};

Ball balls[maxNumOfBall];
int activeBallCount = 0;

// Removed: bool isStoneBlockThere(int by, int bx);
bool isDeleteBlock(int by, int bx); // Keep if used elsewhere, MoveBall handles types now
int getBlockNumY(int by);
int getBlockNumX(int bx);
void levelTracker(int scoreTmpData);
void SpawnNewBall(const Ball& parentBall);
void specialBlockSpawner(int spawnNum);
void resetGame();
bool gameWinCheck();
bool playAgainPrompt();

int currentLevel = 1;
int score = 0;
int levelTwoThreshold = 500;
int levelThreeThreshold = 1000;
int levelFourThreshold = 2500; // Adjusted threshold for stone blocks
int levelFiveThreshold = 4000;

// Removed: stoneBlockX1, stoneBlockX2, stoneBlockX3

int bgmHandler = -1;
int blockHitSoundHandler = -1;
int barHitSoundHandler = -1;
int specialHitSoundHandler = -1;
int stoneHitSoundHandler = -1;

int currentBarSizeX;

void Draw()
{
	static int GrHandle = LoadGraph("gamebg.bmp");
	static int colorBlock = LoadGraph("block.bmp");
	static int stoneBlockHandle = LoadGraph("stone.bmp"); // Handle for type 3
	static int specialBlockHandle = LoadGraph("specialBlock.bmp"); // Handle for type 2

	if (GrHandle == -1) { DrawFormatString(10, 30, GetColor(255, 0, 0), "Error: gamebg.bmp failed!"); }
	if (colorBlock == -1) { DrawFormatString(10, 40, GetColor(255, 0, 0), "Error: block.bmp failed!"); }
	if (stoneBlockHandle == -1) { DrawFormatString(10, 50, GetColor(255, 0, 0), "Error: stone.bmp failed!"); }
	if (specialBlockHandle == -1) { DrawFormatString(10, 60, GetColor(255, 0, 0), "Error: specialBlock.bmp failed!"); }

	DrawGraph(0, 0, GrHandle, FALSE);
	DrawBox(bar_x, bar_y, bar_x + currentBarSizeX, bar_y + BAR_SIZE_Y, GetColor(255, 255, 255), TRUE);

	for (int i = 0; i < maxNumOfBall; ++i) {
		if (balls[i].active) {
			DrawCircle(balls[i].x, balls[i].y, BALL_SIZE, GetColor(255, 255, 0), TRUE);
		}
	}

	DrawFormatString(10, 10, GetColor(255, 255, 255), "Score: %d  Level: %d", score, currentLevel);

	for (int y = 0; y < BLOCK_NUM_Y; y++)
	{
		for (int x = 0; x < BLOCK_NUM_X; x++)
		{
			int blockType = block[y][x];

			if (blockType == 1)
			{
				int srcX = y * BLOCK_SIZE_X;
				if (srcX >= 240) srcX = 200;
				DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y,
					srcX, 0, BLOCK_SIZE_X, BLOCK_SIZE_Y,
					colorBlock, FALSE, FALSE);
			}
			else if (blockType == 2)
			{
				DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y,
					0, 0, BLOCK_SIZE_X, BLOCK_SIZE_Y,
					specialBlockHandle, FALSE, FALSE);
			}
			else if (blockType == 3) // Draw indestructible stone block
			{
				DrawRectGraph(x * BLOCK_SIZE_X, BLOCK_TOP_Y + y * BLOCK_SIZE_Y,
					0, 0, BLOCK_SIZE_X, BLOCK_SIZE_Y,
					stoneBlockHandle, FALSE, FALSE);
			}
		}
	}
	// Removed decorative stone drawing section
}

void MoveBar()
{
	int moveSpeed = 2; // Keep original speed logic
	if (currentLevel >= 4) {
		moveSpeed = 3;
	}

	if (currentLevel == 1)
	{
		if (CheckHitKey(KEY_INPUT_RIGHT) && (bar_x + currentBarSizeX) < WINDOW_SIZE_X)
		{
			bar_x = bar_x + moveSpeed;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) && (bar_x) > 0)
		{
			bar_x -= moveSpeed;
		}
	}
	else if (currentLevel == 2)
	{
		if (CheckHitKey(KEY_INPUT_RIGHT) && (bar_x + currentBarSizeX) < WINDOW_SIZE_X)
		{
			bar_x = bar_x + moveSpeed;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) && (bar_x) > 0)
		{
			bar_x -= moveSpeed;
		}
	}
	else if (currentLevel == 3)
	{
		if (CheckHitKey(KEY_INPUT_RIGHT) && (bar_x + currentBarSizeX) < WINDOW_SIZE_X)
		{
			bar_x = bar_x + moveSpeed;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) && (bar_x) > 0)
		{
			bar_x -= moveSpeed;
		}
	}
	else if (currentLevel >= 4)
	{
		if (CheckHitKey(KEY_INPUT_RIGHT) && (bar_x + currentBarSizeX) < WINDOW_SIZE_X)
		{
			bar_x = bar_x + moveSpeed;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) && (bar_x) > 0)
		{
			bar_x -= moveSpeed;
		}
	}
	// Ensure bar doesn't go out of bounds after move
	if ((bar_x + currentBarSizeX) > WINDOW_SIZE_X) bar_x = WINDOW_SIZE_X - currentBarSizeX;
	if (bar_x < 0) bar_x = 0;
}

void MoveBall()
{
	for (int i = 0; i < maxNumOfBall; ++i)
	{
		if (!balls[i].active) {
			continue;
		}

		int ballx1 = balls[i].x - BALL_SIZE;
		int ballx2 = balls[i].x + BALL_SIZE;
		int bally1 = balls[i].y - BALL_SIZE;
		int bally2 = balls[i].y + BALL_SIZE;

		bool bounced = false;

		if (ballx2 > WINDOW_SIZE_X && balls[i].vx > 0) {
			balls[i].vx = -balls[i].vx;
			balls[i].x = WINDOW_SIZE_X - BALL_SIZE;
			bounced = true;
		}
		else if (ballx1 < 0 && balls[i].vx < 0) {
			balls[i].vx = -balls[i].vx;
			balls[i].x = BALL_SIZE;
			bounced = true;
		}

		if (bally1 < 0 && balls[i].vy < 0) {
			balls[i].vy = -balls[i].vy;
			balls[i].y = BALL_SIZE;
			bounced = true;
		}
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

		// Removed decorative stone collision check (else if currentLevel >= 2...)


		if (!bounced)
		{
			bool blockCollisionProcessed = false;

			int checkPoints[4][2] = {
				{balls[i].x, bally1},
				{balls[i].x, bally2},
				{ballx1, balls[i].y},
				{ballx2, balls[i].y}
			};

			int bounceAxis[4] = { 1, 1, 0, 0 };

			for (int p = 0; p < 4 && !blockCollisionProcessed; ++p) {
				int checkX = checkPoints[p][0];
				int checkY = checkPoints[p][1];

				int hitBlockY = getBlockNumY(checkY);
				int hitBlockX = getBlockNumX(checkX);

				if (hitBlockY >= 0 && hitBlockY < BLOCK_NUM_Y && hitBlockX >= 0 && hitBlockX < BLOCK_NUM_X)
				{
					int blockType = block[hitBlockY][hitBlockX];

					if (blockType > 0) // Hit any active block (1, 2, or 3)
					{
						if (bounceAxis[p] == 1) {
							if ((p == 0 && balls[i].vy < 0) || (p == 1 && balls[i].vy > 0)) {
								balls[i].vy = -balls[i].vy;
							}
						}
						else {
							if ((p == 2 && balls[i].vx < 0) || (p == 3 && balls[i].vx > 0)) {
								balls[i].vx = -balls[i].vx;
							}
						}

						if (blockType == 1)
						{
							block[hitBlockY][hitBlockX] = 0;
							score += (BLOCK_NUM_Y - hitBlockY) * 100;
							levelTracker(score);
							PlaySoundMem(blockHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
						}
						else if (blockType == 2)
						{
							block[hitBlockY][hitBlockX] = 0;
							score += 250;
							levelTracker(score);
							if (specialHitSoundHandler != -1) PlaySoundMem(specialHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
							else PlaySoundMem(blockHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);

							SpawnNewBall(balls[i]);
						}
						else if (blockType == 3) // Indestructible Stone Hit
						{
							if (stoneHitSoundHandler != -1) PlaySoundMem(stoneHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
							else PlaySoundMem(barHitSoundHandler, DX_PLAYTYPE_BACK, TRUE);
							// No score, no destruction
						}

						blockCollisionProcessed = true;
						bounced = true;
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
		return;
	}

	for (int i = 0; i < maxNumOfBall; ++i) {
		if (!balls[i].active) {
			balls[i].active = true;
			balls[i].x = parentBall.x;
			balls[i].y = parentBall.y;

			balls[i].vx = -parentBall.vx;
			balls[i].vy = -parentBall.vy;

			if (balls[i].vx == 0 && balls[i].vy == 0) {
				balls[i].vx = (rand() % 2) * 2 - 1;
				balls[i].vy = -1;
			}
			else if (balls[i].vx == 0) {
				balls[i].vx = (parentBall.vx != 0) ? -parentBall.vx : ((rand() % 2) * 2 - 1);
			}
			else if (balls[i].vy == 0) {
				balls[i].vy = (parentBall.vy != 0) ? -parentBall.vy : -1;
			}

			activeBallCount++;
			return;
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
	else currentLevel = 5;


	if (currentLevel > previousLevel)
	{
		if (currentLevel == 2)
		{
			// Removed activation of decorative stone blocks
		}
		else if (currentLevel == 3)
		{
			//currentBarSizeX = BAR_SIZE_X / 2; // Optionally make bar smaller earlier
			specialBlockSpawner(3);
		}
		else if (currentLevel == 4)
		{
			currentBarSizeX = BAR_SIZE_X / 2; // Make bar smaller for Level 4+
			specialBlockSpawner(4);

			// Spawn Indestructible Blocks (Type 3) by turning existing blocks
			// --- MODIFIED PATTERN ---
			// Example: Turn specific blocks in row 2 (index 1) into stone
			int stoneY = 1; // Row index 1 (second row from top)
			int stoneCols[] = { 3, 7, 11 }; // Columns to turn into stone
			int numStoneCols = sizeof(stoneCols) / sizeof(stoneCols[0]);

			for (int i = 0; i < numStoneCols; ++i) {
				int x = stoneCols[i];
				if (x >= 0 && x < BLOCK_NUM_X) { // Check if column index is valid
					if (block[stoneY][x] == 1) { // Only replace normal blocks
						block[stoneY][x] = 3;
					}
				}
			}
			// --- END MODIFIED PATTERN ---
		}
		else if (currentLevel == 5)
		{
			currentBarSizeX = BAR_SIZE_X / 2; // Keep bar small
			specialBlockSpawner(5);
			// Optionally add more indestructible blocks or change pattern for level 5
			// Example: Add more stones in row 4 (index 3)
			int stoneY = 3;
			int stoneCols[] = { 2, 6, 10, 14 };
			int numStoneCols = sizeof(stoneCols) / sizeof(stoneCols[0]);
			for (int i = 0; i < numStoneCols; ++i) {
				int x = stoneCols[i];
				if (x >= 0 && x < BLOCK_NUM_X) {
					if (block[stoneY][x] == 1) {
						block[stoneY][x] = 3;
					}
				}
			}
		}
	}
}

void specialBlockSpawner(int numToSpawn)
{
	int gapX[BLOCK_NUM_Y * BLOCK_NUM_X];
	int gapY[BLOCK_NUM_Y * BLOCK_NUM_X];
	int gapCount = 0;
	for (int y = 0; y < BLOCK_NUM_Y; ++y) {
		for (int x = 0; x < BLOCK_NUM_X; ++x) {
			if (block[y][x] == 0) {
				gapX[gapCount] = x;
				gapY[gapCount] = y;
				gapCount++;
			}
		}
	}

	if (gapCount < numToSpawn) {
		numToSpawn = gapCount;
	}

	for (int i = 0; i < numToSpawn; ++i) {
		if (gapCount <= 0) break;

		int randomIndex = rand() % gapCount;

		block[gapY[randomIndex]][gapX[randomIndex]] = 2;

		gapX[randomIndex] = gapX[gapCount - 1];
		gapY[randomIndex] = gapY[gapCount - 1];
		gapCount--;
	}
}

// Removed: isStoneBlockThere function

bool isDeleteBlock(int by, int bx)
{
	int blockY = getBlockNumY(by);
	int blockX = getBlockNumX(bx);
	if (blockY != -1 && blockX != -1 && block[blockY][blockX] == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

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
	if (by >= BLOCK_TOP_Y && by < (BLOCK_TOP_Y + BLOCK_NUM_Y * BLOCK_SIZE_Y)) {
		return (by - BLOCK_TOP_Y) / BLOCK_SIZE_Y;
	}
	else {
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

	bar_x = WINDOW_SIZE_X / 2 - BAR_SIZE_X / 2;

	activeBallCount = 1;
	balls[0].active = true;
	balls[0].x = bar_x + currentBarSizeX / 2;
	balls[0].y = bar_y - BALL_SIZE;

	int possibleValueForVx[2] = { -1, 1 };
	int tmp = rand() % 2;
	balls[0].vx = possibleValueForVx[tmp];
	balls[0].vy = -1;

	for (int i = 1; i < maxNumOfBall; ++i) {
		balls[i].active = false;
	}

	for (int y = 0; y < BLOCK_NUM_Y; ++y) {
		for (int x = 0; x < BLOCK_NUM_X; ++x) {
			block[y][x] = 1; // Reset all to normal breakable
		}
	}

	// Removed code for decorative stone blocks
}

bool gameWinCheck()
{
	bool blocksRemaining = false;
	for (int y = 0; y < BLOCK_NUM_Y; y++)
	{
		for (int x = 0; x < BLOCK_NUM_X; x++)
		{
			// Win if only empty (0) or indestructible (3) blocks remain
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
	if (playAgainBackground != -1) {
		DrawGraph(0, 0, playAgainBackground, FALSE);
	}
	else {
		DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, GetColor(0, 0, 50), TRUE);
	}

	DrawFormatString(WINDOW_SIZE_X / 2 - 80, WINDOW_SIZE_Y / 2 - 20, GetColor(255, 255, 255), "Play Again? (Y/N)");
	ScreenFlip();
	WaitTimer(50);
	while (1) {
		if (ProcessMessage() == -1) {
			return false;
		}
		if (CheckHitKey(KEY_INPUT_Y)) {
			WaitTimer(200);
			return true;
		}
		if (CheckHitKey(KEY_INPUT_N)) {
			WaitTimer(200);
			return false;
		}
		WaitTimer(10);
	}
}

// WinMain関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetMainWindowText("ブロック崩し見本 by 橋本");
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1)
		return -1;

	SetDrawScreen(DX_SCREEN_BACK);
	SetMouseDispFlag(TRUE);
	srand(static_cast<unsigned int>(time(NULL)));

	bgmHandler = LoadSoundMem("bgm.mp3");
	blockHitSoundHandler = LoadSoundMem("block_hit.wav");
	barHitSoundHandler = LoadSoundMem("bar_hit.wav");
	specialHitSoundHandler = LoadSoundMem("special_hit.wav"); // Use distinct sound if available
	stoneHitSoundHandler = LoadSoundMem("bar_hit.wav"); // Reuse bar sound for stone

	if (bgmHandler == -1 || blockHitSoundHandler == -1 || barHitSoundHandler == -1)
	{
		DxLib_End();
		return -1;
	}
	if (specialHitSoundHandler == -1) specialHitSoundHandler = blockHitSoundHandler; // Fallback
	if (stoneHitSoundHandler == -1) stoneHitSoundHandler = barHitSoundHandler; // Fallback

	PlaySoundMem(bgmHandler, DX_PLAYTYPE_LOOP, TRUE);

	while (1)
	{
		resetGame();

		// Alpha Blending for Start Screen (Optional, for consistency)
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
		DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		Draw();
		DrawFormatString(WINDOW_SIZE_X / 2 - 100, WINDOW_SIZE_Y / 2 + 40, GetColor(255, 255, 0), "Press SPACE to Start");
		ScreenFlip();

		while (!CheckHitKey(KEY_INPUT_SPACE))
		{
			if (ProcessMessage() == -1)
			{
				DxLib_End();
				return 0;
			}
			WaitTimer(10);
		}

		while (1)
		{
			if (ProcessMessage() == -1)
			{
				DxLib_End();
				return 0;
			}

			MoveBar();
			MoveBall();

			// Alpha Blending Fade Effect
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
			DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, GetColor(0, 0, 0), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			Draw();

			ScreenFlip();

			if (gameOverCheck() == true)
			{
				break;
			}
			if (gameWinCheck() == true)
			{
				break;
			}

			WaitTimer(4);
		}

		WaitTimer(1000);
		if (playAgainPrompt() == false)
		{
			break;
		}
	}

	DxLib_End();
	return 0;
}
