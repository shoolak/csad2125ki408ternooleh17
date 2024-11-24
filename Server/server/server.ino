#include <Arduino.h>

/** Constant representing the size of the tic-tac-toe board. */
const int BOARD_SIZE = 3;
/** Symbol representing Player X. */
const char PLAYER_X = 'X';
/** Symbol representing Player O. */
const char PLAYER_O = 'O';
/** Mode for Man vs Man gameplay. */
const int MODE_MAN_VS_MAN = 1;
/** Mode for Man vs AI gameplay. */
const int MODE_MAN_VS_AI = 2;
/** Mode for AI vs AI gameplay. */
const int MODE_AI_VS_AI = 3;

/** 3x3 tic-tac-toe board initialized with cell numbers. */
char board[BOARD_SIZE][BOARD_SIZE] = { {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'} };

/** Indicates whether a game has started. */
bool isGameStarted = false;

/** Variable representing the selected game mode. */
int gameMode = 0;

/** The last move made by the server AI, for tracking purposes. */
int lastServerMove = -1; 

/** Count of human players in the current game. */
int playerCount = 0;

/** Character representing the current player ('X' or 'O'). */
char globalCurrentPlayer = PLAYER_X;

/**
 * @brief Sets up the game environment and initializes serial communication.
 */
void setup() {
    Serial.begin(9600);
}

/**
 * @brief Main loop of game logic and serial interactions.
 * 
 * Listens to commands and executing them
 */
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

/**
 * @brief Starts a new tic-tac-toe game by resetting the board and setting the game state to active.
 * 
 * Resets the board, sets `isGameStarted` to true, and outputs a message via Serial.
 */
void startGame() {
    resetBoard();
    isGameStarted = true;
    Serial.println("GameStarted");
    printBoardGraphically();
}

/**
 * @brief Sets the game mode based on the received command.
 * 
 * @param command A string command indicating the desired game mode.
 */
void setGameMode(const String& command) {
    String mode = command.substring(8);
    gameMode = mode.toInt();
    Serial.println("Mode set to " + mode);    
}

/**
 * @brief Handles moves in the Man vs Man game mode.
 * 
 * @param command The command received indicating a player's move.
 */
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

/**
 * @brief Handles moves in the Man vs AI game mode.
 * 
 * @param command The command received indicating a player's move.
 */
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

/**
 * @brief Simulates moves in the AI vs AI game mode.
 * 
 * @param command The command received for AI interaction.
 */
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

/**
 * @brief Places a player's move on the board if the position is valid.
 * 
 * @param position The position on the board (1-9).
 * @param player The player symbol (X or O).
 * @return True if the move was valid, false otherwise.
 */
bool makePlayerMove(int position, char player) {
    if (isPositionValid(position)) {
        int row = (position - 1) / BOARD_SIZE;
        int col = (position - 1) % BOARD_SIZE;
        board[row][col] = player;
        return true;
    }
    return false;
}


/**
 * @brief Executes the AI's move and prints the move to the Serial Monitor.
 * 
 * @param aiMove An array containing the row and column of the AI move.
 * @param player The player symbol (usually O for AI).
 */
void makeAIMove(int aiMove[2], char player) {
    board[aiMove[0]][aiMove[1]] = player;
    lastServerMove = aiMove[0] * BOARD_SIZE + aiMove[1] + 1;
    Serial.println("ServerMove: " + String(lastServerMove));
}

/**
 * @brief Checks if the specified position is available on the board.
 * 
 * @param position The board position (1-9).
 * @return True if the position is valid and empty, false otherwise.
 */
bool isPositionValid(int position) {
    int row = (position - 1) / BOARD_SIZE;
    int col = (position - 1) % BOARD_SIZE;
    return (position >= 1 && position <= 9 && board[row][col] != PLAYER_X && board[row][col] != PLAYER_O);
}

/**
 * @brief Checks the game status to determine if there's a win or a draw.
 * 
 * @return True if the game has ended, false otherwise.
 */
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

/**
 * @brief Resets the tic-tac-toe board to its initial state.
 */
void resetBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = '1' + (i * BOARD_SIZE + j);
        }
    }
    lastServerMove = -1;
}

/**
 * @brief Checks if a specific player has won the game.
 * 
 * @param player The player symbol (X or O).
 * @return True if the player has won, false otherwise.
 */
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

/**
 * @brief Checks if the board is completely filled.
 * 
 * @return True if there are no empty spaces, false otherwise.
 */
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


/**
 * @brief Outputs the current board state to the Serial Monitor in a graphical format.
 */
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

/**
 * @brief Returns the opposing player symbol.
 * 
 * @param player The current player symbol.
 * @return The opposing player symbol.
 */
char opponent(char player) {
    return (player == PLAYER_X) ? PLAYER_O : PLAYER_X;
}


/**
 * @brief Calculates the best move for the AI player using minimax algorithm.
 * 
 * @param currentPlayer The player symbol (X or O) currently making the move.
 * @param aiPlayer The AI player symbol.
 * @param depth The current depth in the minimax recursion.
 * @return An integer score representing the value of the game state.
 */
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

/**
 * @brief Finds the best move for the AI player using the minimax algorithm.
 * 
 * @param aiPlayer The AI player symbol.
 * @param move An array to store the row and column of the best move.
 */
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