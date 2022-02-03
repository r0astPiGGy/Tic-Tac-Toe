#include <iostream>
#include <string>
#include <conio.h>
#include <ctime>
using namespace std;

#define iterateThroughTable() for (int j = 0; j < 9; j++)

enum Player {
   Cross,
   Zero
};

class Cell {
public:
                // diag   horiz  vert
    bool FLAGS[3]{ false, false, false };

    Player* owner = nullptr;

    void setOwner(Player* owner) {
        if (isEmpty()) {
            this->owner = owner;
        }
    }

    bool isEmpty() {
        return owner == nullptr ? true : false;
    }

    Player* getOwner() {
        return owner;
    }

    void resetFlags() {
        for (auto& flag : FLAGS)
            flag = false;
    }
    void setFlag(int ind, bool val) {
        FLAGS[ind] = val;
    }
};

class Game {
private:
    Player* currentTurn;
    int winner = -1;
    Cell* table;

    Player playerContainer[2];

    int turnsCount = 0;

    bool inGame = false;
    bool isRunning = true;

    // Validation constant
    const pair<int, int> modifiers[4]{ {-1,-1},{1,1},{1,-1},{-1,1} };

    void start() {
        srand(time(0));
        turnsCount = 0;
        winner = -1;
        refillPlayerContainer();
        refillTable();
        currentTurn = &playerContainer[(rand() % 2 + 1) - 1];

        inGame = true;

        gameLogic();
    }
    void gameLogic() {
        while (inGame) {
            drawTable();
            printCurrentTurn();
            int index = getIndexInput();
            if (performAction(index)) { // if action valid
                incTurns();
                getNextTurn();
                checkForMatches();
                resetFlagsInCells();
                if (inGame) checkTurnsCount();
            }
        }
    }
    void stop() {
        drawTable();
        printWinner();
        inGame = false;
    }
    void printWinner() {
        cout << "Игра окончена. ";
        if (winner == -1) {
            cout << "Ничья." << endl;
        }
        else {
            cout << "Победил " << (winner > 0 ? "Крестик" : "Нолик") << endl;
        }
    }
    void refillTable() {
        delete[] table;

        table = new Cell[9];

        iterateThroughTable() {
            table[j] = Cell();
        }
    }
    void refillPlayerContainer() {
        playerContainer[0] = Zero;
        playerContainer[1] = Cross;
    }
    void getNextTurn() {
        // В массиве Zero под индексом 0, Cross - 1
        // Если getType() != Zero, то index будет 0 и из массива возьмётся нолик
        int index = *currentTurn == Zero;
        currentTurn = &playerContainer[index];
    }
    void printHelp() {
        cout << "\nTic-Tac-Toe\nЧтобы начать, нажми 's'. Для выхода - 'e' \n" << endl;
    }
    void printCurrentTurn() {
        cout << "Сейчас ходит - " << (*currentTurn == Cross ? "Крестик" : "Нолик") << endl;
        cout << "Введите цифру на экране, куда хотите поставить знак\n" << endl;
    }
    void drawTable() {
        cout << "\n";
        for (int j = 0; j < 3; j++) {
            cout << " " << getDrawable(0 + (j * 3)) << " | " << getDrawable(1 + (j * 3)) << " | " << getDrawable(2 + (j * 3)) << " " << endl;
            if (j != 2)
                cout << "---|---|---" << endl;
        }
    }
    char getDrawable(int index) {
        Cell* cell = &table[index];

        if (cell->isEmpty())
            return (index + 1) + 48;
        if (*cell->getOwner() == Zero)
            return '0';

        return 'X';
    }
    void printError(string str) {
        cout << str << endl;
    }
    bool performAction(int pos) {
        if (pos < 1 || pos > 9) {
            printError("Недопустимый индекс!");
            return false;
        }
        if (table[pos-1].isEmpty()) {
            table[pos-1].setOwner(currentTurn);
            return true;
        }
        printError("На этом индексе уже стоит знак!");

        return false;
    }
    void incTurns() {
        turnsCount++;
    }
    void checkTurnsCount() {
        if (turnsCount >= 9) {
            stop();
        }
    }
    void checkForMatches() {
        Player* winner = validate();
        if (winner != nullptr) {
            this->winner = *winner == Zero ? 0 : 1;
            stop();
        }
    }
    Player* validate() {
        iterateThroughTable() {
            Cell* cell = &table[j];
            const int
                x = j % 3,
                y = j / 3;

            if (cell->getOwner() == nullptr) {
                continue;
            }

            Player* cellOwner = cell->getOwner();

            for (int FLAG = 0; FLAG < 3; FLAG++) {
                if (cell->FLAGS[FLAG])
                    continue;

                int count = 0;
                for (int modif = 0; modif < 4; modif++) {
                    int iterIndex = 1;

                    int Xmodifier = modifiers[modif].first;
                    int Ymodifier = modifiers[modif].second;

                    if (FLAG == 1)     Ymodifier = 0;
                    else if(FLAG == 2) Xmodifier = 0;

                    while (true) {
                        Cell* target = getCell(x + (Xmodifier * iterIndex), y + (Ymodifier * iterIndex));
                        if (target == nullptr) {
                            break;
                        }
                        else {
                            if (target->getOwner() != nullptr) {
                                if (*target->getOwner() == *cellOwner)
                                    count++;
                            }
                            if (count == 2) {
                                return cellOwner;
                            }
                            target->setFlag(FLAG, true);
                        }
                        iterIndex++;
                    }
                    if (FLAG > 0 && modif == 1) 
                        break;
                }
            }
        }
        return nullptr;
    }
    void resetFlagsInCells() {
        iterateThroughTable() {
            table[j].resetFlags();
        }
    }
    Cell* getCell(const int& x, const int& y) {
        if (x < 0 || x > 2 || y < 0 || y > 2) {
            return nullptr;
        }
        return &table[x + 3 * y];
    }
    int getIndexInput() {
        return _getch() - 48;
    }
    Player* getCurrentTurn() {
        return currentTurn;
    }
public:

    ~Game() {
        delete[] table;
    }
    void run() {
        while (isRunning) {
            printHelp();

            char ans = _getch();

            switch (ans) {
            case 's':
                start();
                break;
            case 'e':
                isRunning = false;
                break;
            }
        }
    }
};

int main()
{
    setlocale(LC_ALL, "Russian");

    Game* game = new Game();
    game->run();
    
    delete game;
}
