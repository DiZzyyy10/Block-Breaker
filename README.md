# Block

A classic Block Breaker (Arkanoid/Breakout style) game built for Windows using C and the DxLib library.

Feel free to download the code, set it up, and test it out! We've included a few levels to make the gameplay challenging and fun.

## Features

* Classic block-breaking gameplay.
* Multiple levels.
* Built with C and DxLib.

## Requirements

* Windows Operating System.
* A C compiler compatible with DxLib (like Visual Studio).
* **DxLib Library:** This project requires the DxLib library, which is *not* included in this repository.

## Setup Instructions

1.  **Get the Code:** You have two options to get the project files:

    * **Option A (Easier): Download ZIP**
        1.  On the main page of this repository on GitHub, click the green **`< > Code`** button.
        2.  Select **`Download ZIP`**.
        3.  Save the ZIP file to your computer and then **extract** it. This will create the `Block` project folder.

    * **Option B (Using Git): Clone Repository**
        1.  If you have Git installed, open a terminal or command prompt.
        2.  Run the following commands (replace `<your-repository-url>` with the actual URL found on the GitHub page under the 'Code' button):
            ```bash
            git clone <your-repository-url>
            cd Block
            ```
        3.  This will download the project files into a `Block` folder and navigate into it.

2.  **Download DxLib:**
    * Go to the official DxLib download page: [https://dxlib.xsrv.jp/dxdload.html](https://dxlib.xsrv.jp/dxdload.html)
    * Download the version appropriate for your compiler (e.g., the Visual Studio version).

3.  **Extract and Place DxLib:**
    * Extract the downloaded DxLib archive (it will likely create a folder named `DxLib` or similar).
    * Move the extracted `DxLib` folder so that it sits **next to** the `Block` folder you downloaded or cloned in Step 1. Your directory structure should look like this:

        ```
        Your_Projects_Folder/
        ├── Block/       <-- This repository's code (contains .sln, .vcxproj, src/, etc.)
        └── DxLib/       <-- The DxLib library folder you just downloaded and placed here
        ```

4.  **Configure Project (If Necessary):**
    * The Visual Studio project (`.vcxproj`) included in the `Block` folder should already be configured with relative paths (`..\DxLib\...`) to find the library.
    * If you encounter issues, double-check the project settings in Visual Studio:
        * **Configuration Properties -> VC++ Directories -> Include Directories:** Should point to the DxLib include path (e.g., `$(SolutionDir)..\DxLib\プロジェクトに追加すべきファイル_VC用`).
        * **Configuration Properties -> VC++ Directories -> Library Directories:** Should point to the DxLib library path (e.g., `$(SolutionDir)..\DxLib\プロジェクトに追加すべきファイル_VC用`).
        * **Configuration Properties -> Linker -> Input -> Additional Dependencies:** Should include `DxLib.lib`, `DxUseCLib.lib`, etc.

5.  **Build and Run:**
    * Open the Solution file (`.sln`) located inside the `Block` folder with Visual Studio.
    * Select the desired build configuration (e.g., Debug or Release).
    * Build the solution (F7 or Build -> Build Solution).
    * Run the game (F5 or Debug -> Start Debugging).
