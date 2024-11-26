#include <Arduino.h>

const int BOARD_SIZE = 3;
const char PLAYER_X = 'X';
const char PLAYER_O = 'O';
const int MODE_MAN_VS_MAN = 1;
const int MODE_MAN_VS_AI = 2;
const int MODE_AI_VS_AI = 3;

char board[BOARD_SIZE][BOARD_SIZE] = { {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'} };
bool isGameStarted = false;
int gameMode = 0; // 1 - Man vs Man, 2 - Man vs AI, 3 - AI vs AI
int lastServerMove = -1; // Last move of the AI
int playerCount = 0;

char globalCurrentPlayer = PLAYER_X;

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');

        if (command == "StartGame") {
            startGame();
        } else if (command.startsWith("SetMode ")) {
            setGameMode(command);
        }

        if (gameMode == MODE_MAN_VS_MAN) {
            handleManvsMan(command);
        } 
        if (gameMode == MODE_MAN_VS_AI) {
            handleManvsAI(command);
        }
        if (gameMode == MODE_AI_VS_AI) {
            handleAIvsAI(command);
        }  
    }
}

void startGame() {
    resetBoard();
    isGameStarted = true;
    Serial.println("GameStarted");
    printBoardGraphically();
}

void setGameMode(const String& command) {
    String mode = command.substring(8);
    gameMode = mode.toInt();
    Serial.println("Mode set to " + mode);    
}

void handleManvsMan(String command) {
    if (command.startsWith("Move") && isGameStarted) {
        int position = command.substring(5).toInt();

        char player = (playerCount % 2 == 0) ? PLAYER_X : PLAYER_O;

        if (makePlayerMove(position, player)) {
            printBoardGraphically();
            checkGameStatus();
            playerCount++;
        } else {
            Serial.println("InvalidMove");
        }
    }
}

void handleManvsAI(String command) {
    if (command.startsWith("Move") && isGameStarted) {
        int position = command.substring(5).toInt();
        
        if (makePlayerMove(position, PLAYER_X)) {
            if (!checkGameStatus()) {
                int aiMove[2];
                bestMove(PLAYER_O, aiMove);
                makeAIMove(aiMove, PLAYER_O);
                printBoardGraphically();
                checkGameStatus();
            }
        } else {
            Serial.println("InvalidMove");
        }
    }
}

void handleAIvsAI(String command) {
    if (isGameStarted) {
        globalCurrentPlayer = PLAYER_X;  // Почнемо з гравця X

        while (!checkGameStatus()) {  // Цикл поки гра не закінчиться
            int aiMove[2];
            bestMove(globalCurrentPlayer, aiMove);
            makeAIMove(aiMove, globalCurrentPlayer);  // Виконуємо хід поточного гравця
            printBoardGraphically();

            delay(500); 
            // Перевіряємо статус гри після кожного ходу
            if (checkGameStatus()) break;

            // Змінюємо поточного гравця
            globalCurrentPlayer = opponent(globalCurrentPlayer);
        }
    }
}

bool makePlayerMove(int position, char player) {
    if (isPositionValid(position)) {
        int row = (position - 1) / BOARD_SIZE;
        int col = (position - 1) % BOARD_SIZE;
        board[row][col] = player;
        return true;
    }
    return false;
}

void makeAIMove(int aiMove[2], char player) {
    board[aiMove[0]][aiMove[1]] = player;
    lastServerMove = aiMove[0] * BOARD_SIZE + aiMove[1] + 1;
    Serial.println("ServerMove: " + String(lastServerMove));
}

bool isPositionValid(int position) {
    int row = (position - 1) / BOARD_SIZE;
    int col = (position - 1) % BOARD_SIZE;
    return (position >= 1 && position <= 9 && board[row][col] != PLAYER_X && board[row][col] != PLAYER_O);
}

bool checkGameStatus() {
    if (checkWin(PLAYER_X)) {
        Serial.println("X Wins");
        return true;
    } else if (checkWin(PLAYER_O)) {
        Serial.println("O Wins");
        return true;
    } else if (isBoardFull()) {
        Serial.println("Draw");
        return true;
    }
    return false;
}

void resetBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = '1' + (i * BOARD_SIZE + j);
        }
    }
    lastServerMove = -1;
}

bool checkWin(char player) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }
    return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
           (board[0][2] == player && board[1][1] == player && board[2][0] == player);
}

bool isBoardFull() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] != PLAYER_X && board[i][j] != PLAYER_O) {
                return false;
            }
        }
    }
    return true;
}

void printBoardGraphically() {
    Serial.println("-------------");
    for (int i = 0; i < BOARD_SIZE; i++) {
        Serial.print("| ");
        for (int j = 0; j < BOARD_SIZE; j++) {
            Serial.print(board[i][j]);
            Serial.print(" | ");
        }
        Serial.println();
        Serial.println("-------------");
    }
    Serial.println(); // Blank line after board output
}

char opponent(char player) {
    return (player == PLAYER_X) ? PLAYER_O : PLAYER_X;
}

int minimax(char currentPlayer, char aiPlayer, int depth) {
    if (checkWin(aiPlayer)) {
        return 10 - depth; // AI wins
    } else if (checkWin(opponent(aiPlayer))) {
        return depth - 10; // Opponent wins
    } else if (isBoardFull()) {
        return 0; // Draw
    }
    int bestScore = (currentPlayer == aiPlayer) ? -1000 : 1000;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] != PLAYER_X && board[i][j] != PLAYER_O) {
                board[i][j] = currentPlayer; // Make the move
                int score = minimax(opponent(currentPlayer), aiPlayer, depth + 1);
                board[i][j] = '1' + (i * BOARD_SIZE + j); // Reset the move
                if (currentPlayer == aiPlayer) {
                    if (score > bestScore) {
                        bestScore = score;
                    }
                } else {
                    if (score < bestScore)
                    {
                        bestScore = score;
                    }
                }
            }
        }
    }
    return bestScore;
}

void bestMove(char aiPlayer, int move[2]) {
    int bestScore = -1000;
    move[0] = -1;
    move[1] = -1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] != PLAYER_X && board[i][j] != PLAYER_O) {
                board[i][j] = aiPlayer;
                int score = minimax(opponent(aiPlayer), aiPlayer, 0);
                board[i][j] = '1' + (i * BOARD_SIZE + j);;
                if (score > bestScore) {
                    bestScore = score;
                    move[0] = i;
                    move[1] = j;
                }
            }
        }
    }
}