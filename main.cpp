#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QButtonGroup>
#include <vector>
#include <algorithm>
#include <limits>

class TicTacToe : public QMainWindow {
    Q_OBJECT

public:
    TicTacToe(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        resetGame();
    }

private slots:
    void handleButtonClick(int index) {
        if (board[index] != EMPTY || (mode == 2 && currentPlayer == PLAYER1)) return;

        makeMove(index);
    }

    void setPlayerVsPlayer() {
        mode = 1;
        resetGame();
    }

    void setPlayerVsAI() {
        mode = 2;
        resetGame();
    }

private:
    const char PLAYER1 = 'X';
    const char PLAYER2 = 'O';
    const char EMPTY = ' ';
    
    int mode = 1; // 1: PvP, 2: PvAI
    char currentPlayer = PLAYER1;
    std::vector<char> board;
    QButtonGroup *buttonGroup;
    QLabel *statusLabel;

    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // Game mode selection
        QHBoxLayout *modeLayout = new QHBoxLayout();
        QPushButton *pvpButton = new QPushButton("Player vs Player", this);
        QPushButton *pvaiButton = new QPushButton("Player vs AI", this);
        
        connect(pvpButton, &QPushButton::clicked, this, &TicTacToe::setPlayerVsPlayer);
        connect(pvaiButton, &QPushButton::clicked, this, &TicTacToe::setPlayerVsAI);
        
        modeLayout->addWidget(pvpButton);
        modeLayout->addWidget(pvaiButton);
        mainLayout->addLayout(modeLayout);

        // Game board
        QGridLayout *gridLayout = new QGridLayout();
        buttonGroup = new QButtonGroup(this);
        
        for (int i = 0; i < 9; i++) {
            QPushButton *button = new QPushButton("", this);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            button->setMinimumSize(80, 80);
            button->setFont(QFont("Arial", 24));
            gridLayout->addWidget(button, i / 3, i % 3);
            buttonGroup->addButton(button, i);
        }
        
        connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), 
                this, &TicTacToe::handleButtonClick);
        
        mainLayout->addLayout(gridLayout);

        // Status label
        statusLabel = new QLabel("Player X's turn", this);
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setFont(QFont("Arial", 14));
        mainLayout->addWidget(statusLabel);

        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);
        setWindowTitle("Tic Tac Toe");
    }

    void resetGame() {
        board = std::vector<char>(9, EMPTY);
        currentPlayer = PLAYER1;
        
        for (int i = 0; i < 9; i++) {
            QPushButton *button = buttonGroup->button(i);
            button->setText("");
            button->setEnabled(true);
        }
        
        updateStatus();
        
        if (mode == 2 && currentPlayer == PLAYER1) {
            QTimer::singleShot(500, this, &TicTacToe::makeAIMove);
        }
    }

    void updateBoard() {
        for (int i = 0; i < 9; i++) {
            QPushButton *button = buttonGroup->button(i);
            button->setText(board[i] == EMPTY ? "" : QString(board[i]));
        }
    }

    void updateStatus() {
        statusLabel->setText(QString("Player %1's turn").arg(currentPlayer));
    }

    void makeMove(int index) {
        board[index] = currentPlayer;
        updateBoard();
        
        if (checkWin(currentPlayer)) {
            gameOver(QString("Player %1 wins!").arg(currentPlayer));
            return;
        }
        
        if (checkTie()) {
            gameOver("It's a tie!");
            return;
        }
        
        currentPlayer = (currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1;
        updateStatus();
        
        if (mode == 2 && currentPlayer == PLAYER1) {
            QTimer::singleShot(500, this, &TicTacToe::makeAIMove);
        }
    }

    void makeAIMove() {
        int move = bestMove();
        if (move != -1) {
            makeMove(move);
        }
    }

    void gameOver(const QString &message) {
        for (int i = 0; i < 9; i++) {
            buttonGroup->button(i)->setEnabled(false);
        }
        
        QMessageBox::information(this, "Game Over", message);
        resetGame();
    }

    // Game logic functions (same as your original code)
    bool checkWin(char player) const {
        return (board[0] == player && board[1] == player && board[2] == player) ||
               (board[3] == player && board[4] == player && board[5] == player) ||
               (board[6] == player && board[7] == player && board[8] == player) ||
               (board[0] == player && board[3] == player && board[6] == player) ||
               (board[1] == player && board[4] == player && board[7] == player) ||
               (board[2] == player && board[5] == player && board[8] == player) ||
               (board[0] == player && board[4] == player && board[8] == player) ||
               (board[2] == player && board[4] == player && board[6] == player);
    }

    bool checkTie() const {
        return std::all_of(board.begin(), board.end(), [](char c) { return c != EMPTY; });
    }

    int minimax(std::vector<char>& board, bool isMaximizing) {
        if (checkWin(PLAYER1)) return 1;
        if (checkWin(PLAYER2)) return -1;
        if (checkTie()) return 0;

        int bestScore = isMaximizing ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

        for (int i = 0; i < 9; i++) {
            if (board[i] == EMPTY) {
                board[i] = isMaximizing ? PLAYER1 : PLAYER2;
                int score = minimax(board, !isMaximizing);
                board[i] = EMPTY;
                bestScore = isMaximizing ? std::max(score, bestScore) : std::min(score, bestScore);
            }
        }
        return bestScore;
    }

    int bestMove() {
        int move = -1;
        int bestScore = std::numeric_limits<int>::min();
        for (int i = 0; i < 9; i++) {
            if (board[i] == EMPTY) {
                board[i] = PLAYER1;
                int score = minimax(board, false);
                board[i] = EMPTY;
                if (score > bestScore) {
                    bestScore = score;
                    move = i;
                }
            }
        }
        return move;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    TicTacToe game;
    game.resize(400, 500);
    game.show();
    
    return app.exec();
}

#include "main.moc"
