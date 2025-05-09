# Block Breaker Game

A classic block-breaking arcade game developed in C++ using the DxLib library. Test your reflexes and see how many levels you can clear!

## Features

* Classic block-breaking gameplay
* Multiple levels with increasing difficulty
* Score tracking
* Player lives system
* Variety of block types:
    * Standard colored blocks (now with cute designs!)
    * Special blocks that can trigger events (e.g., multiple balls)
    * Indestructible stone blocks
* Sound effects and background music
* Adjustable paddle size and ball speed at higher levels

## Downloads

You can download the latest playable version for Windows from the **[Releases Page](https://github.com/DiZzyyy10/Block-Breaker/releases)**.

**How to Run:**

1.  Go to the [Releases Page](https://github.com/DiZzyyy10/Block-Breaker/releases).
2.  Download the `.zip` file for the latest release (e.g., `BlockBreaker_vX.X.zip`).
3.  Extract all the contents of the `.zip` file into a new folder on your computer.
4.  Run `Block.exe` (or the similarly named executable file) from that folder.
    * **Important:** Ensure all asset files (`.bmp`, `.wav`, `.mp3`) and any included DLL files (like those for DxLib) remain in the same folder as the executable for the game to work correctly.

## How to Play

* **Move Paddle:** Use the `LEFT` and `RIGHT` arrow keys.
* **Launch Ball:** Press `SPACE` at the start of the game or after losing a life to launch the ball.
* **Objective:** Clear all breakable blocks (colored and special) on the screen to advance to the next level or win the game. Avoid letting the ball fall below your paddle.

## Development

This game was developed using:

* **Language:** C++
* **Library:** [DxLib](https://dxlib.xsrv.jp/) (A Japanese multimedia library for game development)
* **IDE:** Primarily developed with Visual Studio 2022

To build from source, you would need a C++ compiler (like the one in Visual Studio) and the DxLib library correctly set up in your development environment. The main game logic is in `block0.cpp`.

## Credits & Acknowledgements

* This project was developed based on a hint program ("ブロック崩し プログラム ヒント") provided by Professor Tsuyoshi Hashimoto, Department of Information Engineering, Matsue National College of Technology.
* Uses the [DxLib](https://dxlib.xsrv.jp/) library for graphics, sound, and input.
* Custom block designs conceptualized and generated with assistance.

---

*Feel free to contribute, report issues, or suggest features!*
