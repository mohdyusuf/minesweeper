#include <iostream>
#include <queue>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;
using namespace olc;

//----------------------------------------------
// Enum & Define sections
//----------------------------------------------
enum eGridAction { HIDDEN, REVEALED, FLAGGED, CHECK };
enum eGameStatus { GAME_RUNNING, GAME_END };

#define MINE    9   // the mine - the mine is nine.. Ooo that ryhmes

class Mines {
private:
    int width;
    int height;
    int totMines;

public:
    bool gameStatus;

    typedef struct grid {
        int** grid;
        int** status;
    } Grid;

    Grid field;

    Mines() {}

    Mines (int w, int h) {
        width = w;
        height = h;
        init(width, height);
    }

    void init(int w, int h) {
        width = w;
        height = h;
        int i = 0;
        
        // generate grid memory
        field.grid = new int* [height];
        field.status = new int* [height];
        for (i = 0; i < height; i++) {
            field.grid[i] = new int [width];
            field.status[i] = new int [width];
        }
        // reset the field
        reset();
    }

    void reset() {
        int y = 0;
        int x = 0;
        int k = 0;

        srand(time(NULL));

        gameStatus = GAME_RUNNING;

        // set difficulty.. boink boink
        totMines = 99;
        if (height == 10) {
            totMines = 10;
        }
        else if (width == 16) {
            totMines = 40;
        }

        // clear the field first
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                field.grid[y][x] = 0;
                field.status[y][x] = HIDDEN;
            }
        }

        // seed the mines.. oo oo.. dangerous
        for (k = 0; k < totMines; k++) {
            y = rand() % height;
            x = rand() % width;
            while(field.grid[y][x] == MINE) {
                y = rand() % height;
                x = rand() % width;
            }
            field.grid[y][x] = MINE;
        }
        
        // set adjacent tiles mine count
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                if (field.grid[y][x] != MINE) {
                    field.grid[y][x] = countNeighborMines(x, y);
                }
            }
        }
    }

    int countNeighborMines(int x, int y) {
        int m = 0;
        int n = 0;
        int count = 0;
        for (m = y-1; m <= y+1; m++) {
            for (n = x-1; n <= x+1; n++) {
                if (m == y && n == x) {
                    continue;
                }
                else if (m >= 0 && n >= 0 && m < height && n < width){
                    if (field.grid[m][n] == MINE) { count++; }
                }
            }
        }
        return count;
    }

    // check if all adjacent mines are marked correctly
    bool isAdjacentPerfect(int x, int y) {
        int m = 0;
        int n = 0;
        for (m = y-1; m <= y+1; m++) {
            for (n = x-1; n <= x+1; n++) {
                if (m == y && n == x) {
                    continue;
                }
                else if (m >= 0 && n >= 0 && m < height && n < width){
                    if (field.grid[m][n] == MINE && field.status[m][n] != FLAGGED) {
                        cout << "Perfect=false" << endl;
                        return false;
                    }
                }
            }
        }
        cout << "Perfect=true" << endl;
        return true;
    }

    int getGrid(int x, int y) {
        if (field.status[y][x] == REVEALED) {
            return field.grid[y][x];
        }
        else if (field.status[y][x] == FLAGGED) {
            return 10;
        }
        else {
            return 11;
        }
    }

    int getGridStatus(int x, int y) {
        return field.status[y][x];
    }

    void setGridAction(int x, int y, eGridAction action) {
        int i = 0;
        int j = 0;
        // TODO:
        if (field.status[y][x] != REVEALED) {
            field.status[y][x] = action;
            if (field.grid[y][x] == MINE && action == REVEALED) {
                setGameEnd();
                for (i = 0; i < height; i++) {
                    for (j = 0; j < width; j++) {
                        if (field.grid[i][j] == MINE) {
                            field.status[i][j] = REVEALED;
                        }
                    }
                }
            }
            if (field.grid[y][x] == 0 && action == REVEALED) {
                clearEmptySection(x, y);
            }
        }
        else {  // now we are doing the peek thing / check adjacent tiles
            if (isAdjacentPerfect(x, y)) {
                for (i = y-1; i <= y+1; i++) {
                    for (j = x-1; j <= x+1; j++) {
                        if (i == y && j == x) {
                            continue;
                        }
                        else if (i >= 0 && j >= 0 && i < height && j < width) {
                            // consider which tile that hasnt been turned
                            if (field.status[i][j] != REVEALED && field.grid[i][j] != MINE) {
                                field.status[i][j] = REVEALED;
                                if (field.grid[i][j] == 0) {
                                    clearEmptySection(j, i);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void clearEmptySection(int x, int y) {
        int i = 0;
        int j = 0;
        vi2d checkTile;
        queue<vi2d> tileQ;
        tileQ.push({x, y});
        
        // algorithm here is to queue the blank tile
        // search adjacent tiles for any more blank tiles - add to queue
        // then iterate - stop at edges when found non blank tiles
        while(!tileQ.empty()) {
            checkTile = tileQ.front();
            tileQ.pop();
            for (i = checkTile.y-1; i <= checkTile.y+1; i++) {
                for (j = checkTile.x-1; j <= checkTile.x+1; j++) {
                    if (i == checkTile.y && j == checkTile.x) {
                        continue;
                    }
                    else if (i >= 0 && j >= 0 && i < height && j < width){
                        if (field.grid[i][j] == 0 && field.status[i][j] == HIDDEN) {
                            field.status[i][j] = REVEALED;
                            tileQ.push({j, i});
                        }
                        else if (field.grid[i][j] < MINE) {
                            field.status[i][j] = REVEALED;
                        }
                    }
                }
            }
        }
    }

    void setGameEnd() {
        // TODO: stop timer
        gameStatus = GAME_END;
    }

    void printGrid() {
        int i = 0;
        int j = 0;
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                if (field.grid[i][j] != 0) {
                    cout << field.grid[i][j] << " ";
                }
                else {
                    cout << "_" << " ";
                }
            }
            cout << endl;
        }
    }

    ~Mines () {
        int i = 0;
        // delete grid memory
        for (i = 0; i < height; i++) {
            delete[] field.grid[i];
            delete[] field.status[i];
        }
        delete[] field.grid;
        delete[] field.status;
    }
};

class OlcFramework : public olc::PixelGameEngine {
    private:
        unique_ptr<Sprite> sprites[12];
        unique_ptr<Decal> decals[12];
        float decalRatio;
        Mines mineField;

    public:
        OlcFramework() {
            sAppName = "Minesweeper";
        }

    public:
        bool OnUserCreate() override {
            // Called once at the start, so create things here
            int i = 0;
            decalRatio = (float)(1.0 / 16.0);   // sprites are 16x16

            mineField.init(ScreenWidth(), ScreenHeight());
            mineField.printGrid();
            
            sprites[0] = make_unique<Sprite>("./sprites/empty.png");
            sprites[1] = make_unique<Sprite>("./sprites/one.png");
            sprites[2] = make_unique<Sprite>("./sprites/two.png");
            sprites[3] = make_unique<Sprite>("./sprites/three.png");
            sprites[4] = make_unique<Sprite>("./sprites/four.png");
            sprites[5] = make_unique<Sprite>("./sprites/five.png");
            sprites[6] = make_unique<Sprite>("./sprites/six.png");
            sprites[7] = make_unique<Sprite>("./sprites/seven.png");
            sprites[8] = make_unique<Sprite>("./sprites/eight.png");
            sprites[9] = make_unique<Sprite>("./sprites/mine.png");
            sprites[10] = make_unique<Sprite>("./sprites/flagged.png");
            sprites[11] = make_unique<Sprite>("./sprites/block.png");
            
            for (i = 0; i < 12; i++) {
                decals[i] = make_unique<Decal>(sprites[i].get());
            }

            return true;
        }

        bool OnUserUpdate(float fElapsedTime) override {

            // called once per frame

            // reset the field
            if (GetKey(Key::N).bReleased) {
                mineField.reset();
            }
            if (mineField.gameStatus == GAME_RUNNING) {
                if (mineField.getGridStatus(GetMouseX(), GetMouseY()) == FLAGGED && (GetMouse(0).bReleased || GetMouse(1).bReleased)) {
                    mineField.setGridAction(GetMouseX(), GetMouseY(), HIDDEN);
                }
                else {
                    if (GetMouse(0).bReleased) {
                        mineField.setGridAction(GetMouseX(), GetMouseY(), REVEALED);
                    }
                    if (GetMouse(1).bReleased) {
                        mineField.setGridAction(GetMouseX(), GetMouseY(), FLAGGED);
                    }
                }   
            }
		        
            // Draw this frame
            Clear(olc::BLACK);
            for (int x = 0; x < ScreenWidth(); x++) {
                for (int y = 0; y < ScreenHeight(); y++) {
                    if (mineField.getGrid(x, y) <= MINE) {
                        DrawDecal({(float)x, float(y)}, decals[mineField.getGrid(x, y)].get(), {decalRatio, decalRatio});
                    }
                    else if (mineField.getGrid(x, y) == 10) {
                        DrawDecal({(float)x, float(y)}, decals[10].get(), {decalRatio, decalRatio});
                    }
                    else {
                        DrawDecal({(float)x, float(y)}, decals[11].get(), {decalRatio, decalRatio});
                    }
                }
            }

            return true;
        }
};

int main(int argc, char const *argv[]) {
	
    OlcFramework game;
	if (game.Construct(16, 16, 25, 25, false, true)) {
        game.Start();
    }
	
    // Mines mines;
    // mines.init(10, 10);
    // mines.printGrid();

	return 0;
}
