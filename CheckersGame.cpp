#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <tuple>

// Размер клетки доски по X и Y
constexpr int cellSizeX = 83; 
constexpr int cellSizeY = 61;

// Координаты клетки А1 в пикселях для расстановки всех шашек на доске
constexpr int boardOffsetX = 68;
constexpr int boardOffsetY = 540;

// Перечисление состояний клеток (пустая, белая, черная, дамка белая, дамка черная)
enum class CellState { Empty, White, Black, WhiteKing, BlackKing };

// Размер доски 
const int BOARD_SIZE = 8;

// Объявление двумерного массива доски
using BoardType = std::array<std::array<CellState, BOARD_SIZE>, BOARD_SIZE>;

// Структура для хранения информации о позиции
struct TranspositionTableEntry {
    int depth;
    int score;
    bool isMaximizing;
};

// Функция используется для хранения результатов оценок определенных игровых позиций, чтобы избежать повторного вычисления этих оценок
uint64_t computeBoardHash(const BoardType& board) {
    uint64_t hash = 0;
    uint64_t prime = 31;

    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            hash = hash * prime + static_cast<int>(board[y][x]);
        }
    }
    return hash;
}


class CheckersGame {
public:
    std::unordered_map<uint64_t, TranspositionTableEntry> transpositionTable;
    CheckersGame() { reset(); }

    // Функция для подготовки игры
    void reset() {
        board.fill({ CellState::Empty });
        selectedX = -1;
        selectedY = -1;
        validMoves.clear();

        // Расстановка белых шашек на высоты 1-3 на черные клетки
        for (int y = 0; y < 3; y++)
            for (int x = (y % 2 == 0 ? 0 : 1); x < BOARD_SIZE; x += 2)
                board[y][x] = CellState::White;

        // Расстановка черных шашек на высоты 6-8 на черные клетки
        for (int y = 5; y < 8; y++)
            for (int x = (y % 2 == 0 ? 0 : 1); x < BOARD_SIZE; x += 2)
                board[y][x] = CellState::Black;

        // Первый ход делают белые
        currentPlayer = CellState::White;

        // Вызов функций для вывода данных в консоль на старте для обеих сторон
        countCheckers(board, CellState::White);
        countKings(board, CellState::White);
        distancesToEdge(board, CellState::White);
        countPossibleMoves(board, CellState::White);

        countCheckers(board, CellState::Black);
        countKings(board, CellState::Black);
        distancesToEdge(board, CellState::Black);
        countPossibleMoves(board, CellState::Black);
    }

    // Функция для отображения доски и шашек
    void draw(sf::RenderWindow& window) {
        // Создаем круглый объект для отрисовки шашек
        sf::CircleShape piece(25);
        piece.setOrigin(25, 25);  // Устанавливаем центр фигуры в середину

        // Создаем зеленый прямоугольник для подсветки возможных ходов
        sf::RectangleShape highlight(sf::Vector2f(cellSizeX, cellSizeY));
        highlight.setFillColor(sf::Color(0, 255, 0, 128)); 

        // Отображаем подсказки (возможные ходы) на доске
        for (auto& move : validMoves) {
            highlight.setPosition(
                boardOffsetX + move.first * cellSizeX,  // Позиция по X
                boardOffsetY - move.second * cellSizeY - cellSizeY // Позиция по Y (инверсия из-за координат SFML)
            );
            window.draw(highlight); 
        }

        // Проходим по всей доске и рисуем шашки
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                if (board[y][x] != CellState::Empty) { // Если клетка не пустая (т.е. там есть шашка)

                    // Устанавливаем цвет шашки
                    if (board[y][x] == CellState::White || board[y][x] == CellState::WhiteKing) {
                        piece.setFillColor(sf::Color::White); // Белая шашка
                    }
                    else if (board[y][x] == CellState::Black || board[y][x] == CellState::BlackKing) {
                        piece.setFillColor(sf::Color::Black); // Черная шашка
                    }

                    // Если шашка - дамка, добавляем зеленую обводку
                    if (board[y][x] == CellState::WhiteKing || board[y][x] == CellState::BlackKing) {
                        piece.setOutlineThickness(3); 
                        piece.setOutlineColor(sf::Color::Green); 
                    }
                    else {
                        piece.setOutlineThickness(0); 
                    }

                    // Устанавливаем позицию шашки на доске в центре клетки
                    piece.setPosition(
                        boardOffsetX + x * cellSizeX + cellSizeX / 2, 
                        boardOffsetY - y * cellSizeY - cellSizeY / 2  
                    );

                    window.draw(piece); 
                }
            }
        }
    }

    // Функция для обработки кликов мыши (выбор шашки и выполнение хода)
    void handleMouseClick(int mouseX, int mouseY) {
        // Преобразуем координаты мыши в координаты клеток на доске
        int x = (mouseX - boardOffsetX) / cellSizeX;
        int y = (boardOffsetY - mouseY) / cellSizeY;

        // Проверяем, что координаты находятся в пределах игрового поля
        if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
            // Если игрок кликнул по своей шашке, выбираем её
            if (board[y][x] == currentPlayer ||
                (board[y][x] == CellState::WhiteKing && currentPlayer == CellState::White) ||
                (board[y][x] == CellState::BlackKing && currentPlayer == CellState::Black)) {

                selectedX = x; // Запоминаем выбранную шашку
                selectedY = y;
                validMoves = getValidMoves(x, y); // Получаем доступные ходы для этой шашки
            }
            // Если игрок кликнул по клетке, куда можно пойти, выполняем ход
            else if (isMoveValid(x, y)) {
                movePiece(x, y);
            }
        }
    }

    // Функция проверяет, является ли данный ход допустимым
    bool isMoveValid(int x, int y) {
        // Перебираем список возможных ходов
        for (const auto& move : validMoves) {
            // Если координаты совпадают с возможным ходом, то ход разрешен
            if (move.first == x && move.second == y) return true;
        }
        return false; // В противном случае ход недопустим
    }

    // Фунция получения возможных правильных ходов  (РАССКОММЕНТИРОВАТЬ COUT)
    std::vector<std::pair<int, int>> getValidMoves(int x, int y) {
        // Список обычных возможных ходов
        std::vector<std::pair<int, int>> moves;     
        // Список ходов с взятием (рубкой)
        std::vector<std::pair<int, int>> captureMoves; 

        int directions[2] = { 1, -1 }; // Возможные направления движения по вертикали (вверх и вниз)

        // Логика для обычных шашек 
        if (board[y][x] == CellState::White || board[y][x] == CellState::Black) {
            int direction = (board[y][x] == CellState::White) ? 1 : -1; // Белые двигаются вниз, черные - вверх

            for (int dx : {-1, 1}) { // Движение влево и вправо по диагонали
                // Проверяем, можно ли просто пойти вперед
                if (isCellEmpty(x + dx, y + direction) && !hasCaptureMoves(currentPlayer)) {
                    moves.push_back({ x + dx, y + direction }); // Добавляем в список обычных ходов
                }

                // Проверяем возможность взятия 
                for (int dy : directions) { // Проверяем обе диагонали 
                    if (canCapturePiece(x, y, x + dx, y + dy, x + 2 * dx, y + 2 * dy)) {
                        captureMoves.push_back({ x + 2 * dx, y + 2 * dy }); // Добавляем ход с рубкой
                    }
                }
            }
        }

        // Логика для дамок
        else if (board[y][x] == CellState::WhiteKing || board[y][x] == CellState::BlackKing) {
            for (int dy : directions) { // Дамка может ходить вверх и вниз
                for (int dx : {-1, 1}) { // И влево, и вправо
                    int steps = 1;

                    // Дамка может двигаться по диагонали, пока не встретит препятствие
                    while (isCellEmpty(x + steps * dx, y + steps * dy)) {
                        moves.push_back({ x + steps * dx, y + steps * dy }); // Добавляем доступные ходы
                        steps++;
                    }

                    // Проверяем, есть ли перед дамкой шашка противника
                    int nx = x + steps * dx;
                    int ny = y + steps * dy;
                    if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && canCapturePiece(x, y, nx, ny, nx + dx, ny + dy)) {
                        int captureX = nx + dx;
                        int captureY = ny + dy;

                        // После рубки дамка может продолжать движение дальше, пока перед ней пусто
                        while (captureX >= 0 && captureX < BOARD_SIZE && captureY >= 0 && captureY < BOARD_SIZE && isCellEmpty(captureX, captureY)) {
                            captureMoves.push_back({ captureX, captureY });
                            captureX += dx;
                            captureY += dy;
                        }
                    }
                }
            }
        }

        // Вывод доступных ходов в консоль (для отладки)
        if (!captureMoves.empty()) {
            std::cout << "Доступные рубки:\n";
            for (const auto& captureMove : captureMoves) {
                std::cout << "(" << captureMove.first + 1 << ", " << captureMove.second + 1 << ")\n";
            }
        }
        else if (!moves.empty()) {
            //std::cout << "Доступные ходы:\n";
            for (const auto& move : moves) {
                //std::cout << "(" << move.first + 1 << ", " << move.second + 1 << ")\n";
            }
        }

        // Если есть возможность рубки, обычные ходы запрещены
        return captureMoves.empty() ? moves : captureMoves;
    }

    // Функция проверяет возможность конкретного удара
    bool canCapturePiece(int x, int y, int enemyX, int enemyY, int targetX, int targetY) {
        // Проверяем, не выходит ли целевая клетка за границы доски
        if (targetX < 0 || targetX >= BOARD_SIZE || targetY < 0 || targetY >= BOARD_SIZE) return false;
        if (board[enemyY][enemyX] == CellState::Empty) return false;

        CellState piece = board[y][x];
        bool isWhite = (piece == CellState::White || piece == CellState::WhiteKing);
        CellState opponent1 = isWhite ? CellState::Black : CellState::White;
        CellState opponent2 = isWhite ? CellState::BlackKing : CellState::WhiteKing;

        return board[targetY][targetX] == CellState::Empty &&
            (board[enemyY][enemyX] == opponent1 || board[enemyY][enemyX] == opponent2) &&
            (board[enemyY][enemyX] != piece); // нельзя прыгать через свои шашки
    }

    // Функция для хождения шашками и рубки
    void movePiece(int newX, int newY) {
        if (selectedX == -1 || selectedY == -1) return;

        bool mustCapture = hasCaptureMoves(currentPlayer);
        bool isKing = (board[selectedY][selectedX] == CellState::WhiteKing || board[selectedY][selectedX] == CellState::BlackKing);

        if (mustCapture && !canCapture(selectedX, selectedY) && !canKingCapture(selectedX, selectedY)) {
            std::cout << "Обязательная рубка! Ход невозможен." << std::endl;

            return; // Блокируем ход, если игрок пытается ходить не рубящей шашкой

        }
        //std::cout << "Передвижение с (" << selectedX << ", " << selectedY << ") на (" << newX << ", " << newY << ")\n";
        board[newY][newX] = board[selectedY][selectedX];
        board[selectedY][selectedX] = CellState::Empty;

        // Проверка и удаление побитых шашек
        int dx = (newX - selectedX) / std::abs(newX - selectedX);
        int dy = (newY - selectedY) / std::abs(newY - selectedY);

        int x = selectedX + dx;
        int y = selectedY + dy;
        bool captured = false;
        while (x != newX && y != newY) {
            if (board[y][x] != CellState::Empty) {
                board[y][x] = CellState::Empty;
                std::cout << "Удалена шашка на (" << x << ", " << y << ")\n";
                captured = true;
            }
            x += dx;
            y += dy;
        }

        // Проверка на множественную рубку только если была выполнена рубка
        if (captured) {
            selectedX = newX;
            selectedY = newY;
            auto newMoves = getValidMoves(newX, newY);
            if ((isKing && !newMoves.empty() && canKingCapture(newX, newY)) || (!isKing && !newMoves.empty() && canCapture(newX, newY))) {
                validMoves = newMoves;
                std::cout << "Возможна множественная рубка, игрок не сменяется.\n";
                return; // Останавливаем выполнение, чтобы продолжить рубку
            }
        }

        // Проверка на превращение в дамку
        if ((board[newY][newX] == CellState::White && newY == 7) ||
            (board[newY][newX] == CellState::Black && newY == 0)) {
            board[newY][newX] = (currentPlayer == CellState::White) ? CellState::WhiteKing : CellState::BlackKing;
            //std::cout << "Шашка на (" << newX << ", " << newY << ") стала дамкой.\n";
        }

        // Вызов оценочной функции
        int evaluationScore = Evaluation(board, currentPlayer);
        std::cout << "Оценка состояния: " << evaluationScore << std::endl;

        // Смена игрока
        selectedX = -1;
        selectedY = -1;
        validMoves.clear();
        switchPlayer();
        std::cout << "Игрок сменился.\n";
    }

    // Функция-флаг для обозначения обязательной рубки
    bool hasCaptureMoves(CellState player) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] == player || (player == CellState::White && board[y][x] == CellState::WhiteKing) ||
                    (player == CellState::Black && board[y][x] == CellState::BlackKing)) {
                    if (canCapture(x, y) || canKingCapture(x, y)) return true;
                }
            }
        }
        return false;
    }

    // Функция проверяет, может ли шашка вообще кого-то бить
    bool canCapture(int x, int y) {
        static const int dx[] = { -1, 1, -1, 1 };
        static const int dy[] = { -1, -1, 1, 1 };

        CellState piece = board[y][x];
        if (piece == CellState::Empty) return false;

        bool isWhite = (piece == CellState::White || piece == CellState::WhiteKing);
        CellState opponent1 = isWhite ? CellState::Black : CellState::White;
        CellState opponent2 = isWhite ? CellState::BlackKing : CellState::WhiteKing;

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            int nx2 = x + 2 * dx[i];
            int ny2 = y + 2 * dy[i];

            if (nx2 >= 0 && nx2 < BOARD_SIZE && ny2 >= 0 && ny2 < BOARD_SIZE &&
                board[ny][nx] != CellState::Empty &&
                (board[ny][nx] == opponent1 || board[ny][nx] == opponent2) &&
                board[ny2][nx2] == CellState::Empty) {
                return true;
            }
        }
        return false;
    }

    //Функция-аналог canCapture, но для дамки
    bool canKingCapture(int x, int y) {
        if (board[y][x] != CellState::WhiteKing && board[y][x] != CellState::BlackKing)
            return false; // Проверяем, что это дамка

        std::cout << "Проверяем дамку на (" << x + 1 << ", " << y + 1 << ")\n";

        int directions[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

        for (auto dir : directions) {
            int dx = dir[0], dy = dir[1];
            bool foundEnemy = false;
            int enemyX = -1, enemyY = -1;

            std::cout << "Проверяем направление (" << dx + 1 << ", " << dy + 1 << ")\n";

            for (int step = 1; step < BOARD_SIZE; ++step) { // Дамка движется по диагонали
                int nx = x + dx * step, ny = y + dy * step;
                if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) {
                    std::cout << "Вышли за границы доски.\n";
                    break;
                }

                if (board[ny][nx] != CellState::Empty) {
                    if (!foundEnemy) {
                        if ((board[y][x] == CellState::WhiteKing && (board[ny][nx] == CellState::White || board[ny][nx] == CellState::WhiteKing)) ||
                            (board[y][x] == CellState::BlackKing && (board[ny][nx] == CellState::Black || board[ny][nx] == CellState::BlackKing))) {
                            std::cout << "Нашли союзную фигуру на (" << nx + 1 << ", " << ny + 1 << "), не рубим.\n";
                            break;
                        }

                        foundEnemy = true;
                        enemyX = nx;
                        enemyY = ny;
                        std::cout << "Найдена шашка противника на (" << enemyX + 1 << ", " << enemyY + 1 << ")\n";
                    }
                    else {
                        std::cout << "Дальше по диагонали фигура, дамка не может прыгнуть.\n";
                        break;
                    }
                }

                else if (foundEnemy) {
                    // дамка может двигаться дальше после сруба 
                    for (int moveStep = 1; moveStep < BOARD_SIZE; ++moveStep) {
                        int mx = enemyX + dx * moveStep, my = enemyY + dy * moveStep;
                        if (mx < 0 || mx >= BOARD_SIZE || my < 0 || my >= BOARD_SIZE) {
                            std::cout << "Вышли за границы после рубки.\n";
                            break;
                        }

                        if (board[my][mx] == CellState::Empty) {
                            std::cout << "Дамка может встать на (" << mx << ", " << my << "), рубим.\n";
                            return true;
                        }

                    }
                }
            }
        }
        std::cout << "Нет возможных ходов для рубки дамкой.\n";
        return false;
    }

    // Функция смены хода между сторонами
    void switchPlayer() {
        currentPlayer = (currentPlayer == CellState::White || currentPlayer == CellState::WhiteKing) ? CellState::Black : CellState::White;
    }

    // Функция для отмечивания пустых клеток недоступных для хода
    bool isCellEmpty(int x, int y) {
        return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[y][x] == CellState::Empty;
    }

    // Оценочная функция (1 этап)
    int Evaluation(const BoardType& board, CellState player) {
        int whiteScore = 0;
        int blackScore = 0;

        // Контроль центра (бонус за нахождение в центральных 4х4)
        whiteScore += countCenterPieces(board, CellState::White) * 10;
        blackScore += countCenterPieces(board, CellState::Black) * 10;

        // Подсчет количества шашек
        whiteScore += countCheckers(board, CellState::White) * 20;
        blackScore += countCheckers(board, CellState::Black) * 20;

        // Подсчет количества дамок
        whiteScore += countKings(board, CellState::White) * 45;
        blackScore += countKings(board, CellState::Black) * 45;

        // Вычисление расстояний до противоположного края поля
        whiteScore -= distancesToEdge(board, CellState::White);
        blackScore -= distancesToEdge(board, CellState::Black);

        // Вычисление возможных ходов шашками
        whiteScore += countPossibleMoves(board, CellState::White) * 2;
        blackScore += countPossibleMoves(board, CellState::Black) * 2;

        // Возвращение разницы в количестве очков
        return whiteScore - blackScore;
    }

    int countCenterPieces(const BoardType& board, CellState player) {
        int count = 0;
        for (int y = 2; y < 6; ++y) {
            for (int x = 2; x < 6; ++x) {
                if (board[y][x] == player || board[y][x] == (player == CellState::White ? CellState::WhiteKing : CellState::BlackKing)) {
                    count++;
                }
            }
        }
        return count;
    }

    int countCheckers(const BoardType& board, CellState player) {
        int count = 0;
        for (const auto& row : board) {
            for (const auto& cell : row) {
                if (cell == player) {
                    count++;
                }
            }
        }
        //std::string color = (player == CellState::White) ? "Белые" : "Черные";
        //std::cout << color << ": Количество шашек: " << count << std::endl;
        return count;
    }

    int countKings(const BoardType& board, CellState player) {
        int count = 0;
        CellState queen = (player == CellState::White) ? CellState::WhiteKing : CellState::BlackKing;
        for (const auto& row : board) {
            for (const auto& cell : row) {
                if (cell == queen) {
                    count++;
                }
            }
        }
        //std::string color = (player == CellState::White) ? "Белые" : "Черные";
        //std::cout << color << ": Количество дамок: " << count << std::endl;
        return count;
    }

    int distancesToEdge(const BoardType& board, CellState player) {
        int totalDistance = 0;
        int direction = (player == CellState::White) ? 1 : -1;
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] == player || (board[y][x] == CellState::WhiteKing && player == CellState::White) ||
                    (board[y][x] == CellState::BlackKing && player == CellState::Black)) {
                    totalDistance += (player == CellState::White) ? (7 - y) : y;
                }
            }
        }
        //std::string color = (player == CellState::White) ? "Белые" : "Черные";
        //std::cout << color << ": Расстояние до края: " << totalDistance << std::endl;
        return totalDistance;
    }

    int countPossibleMoves(const BoardType& board, CellState player) {
        int moveCount = 0;

        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] == player ||
                    (player == CellState::White && board[y][x] == CellState::WhiteKing) ||
                    (player == CellState::Black && board[y][x] == CellState::BlackKing)) {

                    auto validMoves = getValidMoves(x, y);
                    moveCount += validMoves.size();
                }
            }
        }
        //std::string color = (player == CellState::White) ? "Белые" : "Черные";
        //std::cout << color << ": Количество возможных ходов: " << moveCount << std::endl << std::endl;
        return moveCount;
    }

    // Функция для загрузки шрифта, текстур и спрайтов
    void loadTextures(std::map<std::string, sf::Texture>& textures, std::map<std::string, sf::Sprite>& sprites, sf::Font& font, sf::Text& text, sf::Text& statusText, CellState currentPlayer) {
        // Контейнер для хранения спрайтов персонажа
        std::vector<std::string> names = { "end", "flip_over", "surprised", "hungry" };

        for (const auto& name : names) {
            if (!textures[name].loadFromFile("resources/" + name + ".png")) {
                std::cerr << "Ошибка загрузки: resources/" << name << ".png\n";
            }
            else {
                sprites[name].setTexture(textures[name]);
                sprites[name].setPosition(830, 270);  // Установка позиции
            }
        }

        // Загрузка картинки диалога
        if (!textures["dialog"].loadFromFile("resources/dialog.png")) {
            std::cerr << "Ошибка загрузки: resources/dialog.png\n";
        }
        else {
            sprites["dialog"].setTexture(textures["dialog"]);
            sprites["dialog"].setPosition(830, 100);  // Установка позиции
        }

        // Загрузка шрифта
        if (!font.loadFromFile("resources/CyrilicOld.ttf")) {
            std::cerr << "Ошибка загрузки: resources/CyrilicOld.ttf\n";
        }
        else {
            std::string playerText = (currentPlayer == CellState::White) ? "Ход белых" : "Ход черных";
            text.setFont(font);
            text.setString(playerText);
            text.setCharacterSize(24); // Размер шрифта
            text.setFillColor(sf::Color::Red); // Цвет текста
            text.setPosition(860,50); // Позиция текста внутри картинки диалога
        
            if (hasCaptureMoves(currentPlayer)) {
                statusText.setString("Обязательная рубка!");
                statusText.setFont(font);
                statusText.setCharacterSize(18);
                statusText.setFillColor(sf::Color::Red);
                statusText.setPosition(850, 150);
            }
            else if (isGameOver()) {
                statusText.setString("Игра окончена!");
                statusText.setFont(font);
                statusText.setCharacterSize(18);
                statusText.setFillColor(sf::Color::Red);
                statusText.setPosition(860, 150);
            }
            else {
                statusText.setString("");
                statusText.setPosition(850, 150);
            }
        }
        
    }

    bool isGameOver() {
        int whiteCount = countCheckers(board, CellState::White) + countKings(board, CellState::White);
        int blackCount = countCheckers(board, CellState::Black) + countKings(board, CellState::Black);

        return (whiteCount == 0 || blackCount == 0);
    }

    CellState getCurrentPlayer() const {  // switchPlayer!!!!
        return currentPlayer;
    }



    // Функция для получения всех возможных ходов для компьютера
    std::vector<std::pair<int, int>> getAllValidMoves(CellState player) {
        std::vector<std::pair<int, int>> allMoves;

        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                if ((player == CellState::White && (board[y][x] == CellState::White || board[y][x] == CellState::WhiteKing)) ||
                    (player == CellState::Black && (board[y][x] == CellState::Black || board[y][x] == CellState::BlackKing))) {

                    std::vector<std::pair<int, int>> moves = getValidMoves(x, y);

                    for (const auto& move : moves) {
                        allMoves.push_back({ x, y }); // Откуда ходим
                        allMoves.push_back(move);    // Куда ходим
                    }
                }
            }
        }

        return allMoves;
    }

    // Функция для перемещения шашек компьютером
    bool makeMove(int fromX, int fromY, int toX, int toY) {
        int midX = (fromX + toX) / 2;
        int midY = (fromY + toY) / 2;
        bool wasCapture = (abs(toX - fromX) == 2);

        if (wasCapture) {
            //std::cout << "Рубка: удаление шашки на (" << midX + 1 << ", " << midY + 1 << ")\n";
            board[midY][midX] = CellState::Empty; // Удаляем съеденную шашку
        }

        board[toY][toX] = board[fromY][fromX];
        board[fromY][fromX] = CellState::Empty;

        // Превращаем в дамку
        if (toY == 7 && board[toY][toX] == CellState::White) {
            board[toY][toX] = CellState::WhiteKing;
        }
        if (toY == 0 && board[toY][toX] == CellState::Black) {
            board[toY][toX] = CellState::BlackKing;
        }

        // Если был удар, проверяем возможность продолжить
        if (wasCapture) {
            std::vector<std::pair<int, int>> nextCaptures = getCapturingMoves(toX, toY);
            if (!nextCaptures.empty()) {
                //std::cout << "Продолжение множественной рубки для (" << toX + 1 << ", " << toY + 1 << ")\n";
                for (const auto& capture : nextCaptures) {
                    int captureX = capture.first;
                    int captureY = capture.second;
                    if (makeMove(toX, toY, captureX, captureY)) {
                        return false;
                    }
                }
                return false; // Не передаем ход другому игроку
            }
        }

        return true; // Передаем ход
    }

    /*   // Минимакс без а-б отсечения 
    int minimax(int depth, bool isMaximizing) {
        if (depth == 0) {
            int evaluation = Evaluation(board, currentPlayer);
            return evaluation;
        }

        std::vector<std::pair<int, int>> moves = getAllValidMoves(currentPlayer);
        std::vector<std::pair<int, int>> capturingMoves;

        for (size_t i = 0; i < moves.size(); i += 2) {
            int fromX = moves[i].first, fromY = moves[i].second;
            int toX = moves[i + 1].first, toY = moves[i + 1].second;
            if (abs(toX - fromX) == 2) {
                capturingMoves.push_back(moves[i]);   // Откуда идём
                capturingMoves.push_back(moves[i + 1]); // Куда идём
            }
        }

        if (!capturingMoves.empty()) {
            moves = capturingMoves;
        }

        if (moves.empty()) {
            return (isMaximizing) ? -1000 : 1000;
        }

        if (isMaximizing) {
            int bestScore = -10000;
            for (size_t i = 0; i < moves.size(); i += 2) {
                int fromX = moves[i].first, fromY = moves[i].second;
                int toX = moves[i + 1].first, toY = moves[i + 1].second;

                BoardType boardCopy = board;
                bool turnPassed = makeMove(fromX, fromY, toX, toY);

                int score;
                if (!turnPassed) { // Если ход не передан, продолжаем рубку
                    score = minimax(depth, isMaximizing);
                }
                else {
                    currentPlayer = (currentPlayer == CellState::White) ? CellState::Black : CellState::White;
                    score = minimax(depth - 1, false);
                    currentPlayer = (currentPlayer == CellState::White) ? CellState::Black : CellState::White;
                }

                board = boardCopy;
                bestScore = std::max(bestScore, score);

                std::cout << "Максимизирующий: Ход (" << fromX + 1 << ", " << fromY + 1 << ") -> (" << toX + 1 << ", " << toY + 1 << "), Оценка: " << score << std::endl;
            }
            return bestScore;
        }
        else {
            int bestScore = 10000;
            for (size_t i = 0; i < moves.size(); i += 2) {
                int fromX = moves[i].first, fromY = moves[i].second;
                int toX = moves[i + 1].first, toY = moves[i + 1].second;

                BoardType boardCopy = board;
                bool turnPassed = makeMove(fromX, fromY, toX, toY);

                int score;
                if (!turnPassed) { // Если ход не передан, продолжаем рубку
                    score = minimax(depth, isMaximizing);
                }
                else {
                    currentPlayer = (currentPlayer == CellState::White) ? CellState::Black : CellState::White;
                    score = minimax(depth - 1, true);
                    currentPlayer = (currentPlayer == CellState::White) ? CellState::Black : CellState::White;
                }

                board = boardCopy;
                bestScore = std::min(bestScore, score);

                std::cout << "Минимизирующий: Ход (" << fromX + 1 << ", " << fromY + 1 << ") -> (" << toX + 1 << ", " << toY + 1 << "), Оценка: " << score << std::endl;
            }
            return bestScore;
        }
    }

    std::pair<std::pair<int, int>, std::pair<int, int>> getBestMove(int depth, CellState player) {
        int bestScore = (player == CellState::White) ? -10000 : 10000;
        std::pair<std::pair<int, int>, std::pair<int, int>> bestMove;

        std::vector<std::pair<int, int>> moves = getAllValidMoves(player);

        for (size_t i = 0; i < moves.size(); i += 2) {
            int fromX = moves[i].first, fromY = moves[i].second;
            int toX = moves[i + 1].first, toY = moves[i + 1].second;

            BoardType boardCopy = board;
            makeMove(fromX, fromY, toX, toY);
            int score = minimax(depth - 1, player == CellState::Black);
            board = boardCopy;

            std::cout << "Рассматриваемый ход: (" << fromX + 1 << ", " << fromY + 1 << ") -> (" << toX + 1 << ", " << toY + 1 << "), Оценка: " << score << std::endl;

            if ((player == CellState::White && score > bestScore) || (player == CellState::Black && score < bestScore)) {
                bestScore = score;
                bestMove = { {fromX, fromY}, {toX, toY} };
            }
        }

        std::cout << "Выбранный ход: (" << bestMove.first.first + 1 << ", " << bestMove.first.second + 1 << ") -> (" << bestMove.second.first + 1 << ", " << bestMove.second.second + 1 << "), Оценка: " << bestScore << std::endl;

        return bestMove;
    }
    */

    // Минимакс с а-б отсечением
    int minimax(int depth, bool isMaximizing, int alpha, int beta) {
        uint64_t boardHash = computeBoardHash(board);

        // Проверка транспозиционной таблицы
        if (transpositionTable.find(boardHash) != transpositionTable.end()) {
            const auto& entry = transpositionTable[boardHash];
            if (entry.depth >= depth && entry.isMaximizing == isMaximizing) {
                return entry.score;  // Возвращение сохраненного значения
            }
        }

        // Базовый случай: достигнута максимальная глубина
        if (depth == 0) {
            int evaluation = Evaluation(board, currentPlayer);
            transpositionTable[boardHash] = { depth, evaluation, isMaximizing };
            return evaluation;
        }

        std::vector<std::pair<int, int>> moves = getAllValidMoves(currentPlayer);

        // Если ходов нет, фиксируем проигрыш
        if (moves.empty()) {
            int score = (isMaximizing) ? -1000 : 1000;
            transpositionTable[boardHash] = { depth, score, isMaximizing };
            return score;
        }

        int bestScore = (isMaximizing) ? -10000 : 10000; // Инициализация bestScore
        CellState originalPlayer = currentPlayer; // Сохранение текущего игрока

        for (size_t i = 0; i < moves.size(); i += 2) {
            int fromX = moves[i].first, fromY = moves[i].second;
            int toX = moves[i + 1].first, toY = moves[i + 1].second;

            BoardType boardCopy = board; // Сохранение состояния доски
            bool turnPassed = makeMove(fromX, fromY, toX, toY);

            int score;
            if (!turnPassed) { // Если ход не передан, продолжаем рубку
                score = minimax(depth, isMaximizing, alpha, beta); // Уменьшение глубины
            }
            else {
                currentPlayer = (currentPlayer == CellState::White) ? CellState::Black : CellState::White;
                score = minimax(depth - 1, !isMaximizing, alpha, beta); // Смена флага maximizer/minimizer
                currentPlayer = (currentPlayer == CellState::White) ? CellState::Black : CellState::White;
            }

            // Восстановление состояния доски и игрока
            board = boardCopy;
            currentPlayer = originalPlayer;

            if (isMaximizing) {
                bestScore = std::max(bestScore, score);
                alpha = std::max(alpha, bestScore);
            }
            else {
                bestScore = std::min(bestScore, score);
                beta = std::min(beta, bestScore);
            }

            // Глобальный массив оценок
            std::vector<int> chosenValues; 

            std::cout << (isMaximizing ? "Максимизирующий" : "Минимизирующий")
                << ": Ход (" << fromX + 1 << ", " << fromY + 1 << ") -> ("
                << toX + 1 << ", " << toY + 1 << "), Оценка: " << score << std::endl;

            if (beta <= alpha) {
                break; // Альфа-бета отсечение
            }
        }

        transpositionTable[boardHash] = { depth, bestScore, isMaximizing };
        return bestScore;
    }

    // Функция для выбора лучшего хода компьютером
    std::pair<std::pair<int, int>, std::pair<int, int>> getBestMove(int depth, CellState player) {
        int alpha = -10000;
        int beta = 10000;
        int bestScore = (player == CellState::White) ? -10000 : 10000;

        std::pair<std::pair<int, int>, std::pair<int, int>> bestMove;
        std::vector<std::pair<int, int>> moves = getAllValidMoves(player);
        // Вектор для ходов с рубкой (приоритетнее)
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> captureMoves;

        // Поиск всех рубящих ходов
        for (size_t i = 0; i < moves.size(); i += 2) {
            int fromX = moves[i].first, fromY = moves[i].second;
            int toX = moves[i + 1].first, toY = moves[i + 1].second;

            if (abs(toX - fromX) == 2) { // Рубка
                captureMoves.push_back({ {fromX, fromY}, {toX, toY} });
            }
        }

        // Если есть рубящие ходы — выбираем лучший
        if (!captureMoves.empty()) {
            int bestCaptureScore = (player == CellState::White) ? -10000 : 10000;
            std::pair<std::pair<int, int>, std::pair<int, int>> bestCaptureMove;

            for (auto& move : captureMoves) {
                BoardType boardCopy = board;
                makeMove(move.first.first, move.first.second, move.second.first, move.second.second);
                bool isMaximizing = (player == CellState::White);
                int score = minimax(depth - 1, isMaximizing, alpha, beta);

                // Копируем доску для симуляции ходов
                board = boardCopy;

                // Если находим ход лучше - записываем в соответствующие векторы
                if ((player == CellState::White && score > bestCaptureScore) ||
                    (player == CellState::Black && score < bestCaptureScore)) {
                    bestCaptureScore = score;
                    bestCaptureMove = move;
                }
            }

            //std::cout << "Есть рубка! Выбираем лучший из возможных рубящих ходов.\n";
            return bestCaptureMove;
        }

        // Поиск лучшего хода с использованием minimax (не рубка)
        for (size_t i = 0; i < moves.size(); i += 2) {
            int fromX = moves[i].first, fromY = moves[i].second;
            int toX = moves[i + 1].first, toY = moves[i + 1].second;

            BoardType boardCopy = board;
            makeMove(fromX, fromY, toX, toY);
            bool isMaximizing = (player == CellState::White);
            int score = minimax(depth - 1, isMaximizing, alpha, beta);

            board = boardCopy;

            // Запомниаес ход если нашли лучше
            if ((player == CellState::White && score > bestScore) || (player == CellState::Black && score < bestScore)) {
                bestScore = score;
                bestMove = { {fromX, fromY}, {toX, toY} };
            }

            if (player == CellState::White) {
                alpha = std::max(alpha, score);
            }
            else {
                beta = std::min(beta, score);
            }

            if (alpha >= beta) {
                break;  // Альфа-бета отсечение
            }
        }

        return bestMove;
    }

    // Функция для получения всех возможных ходов с рубкой (приоритет)
    std::vector<std::pair<int, int>> getCapturingMoves(int x, int y) {
        std::vector<std::pair<int, int>> capturingMoves;
        std::vector<std::pair<int, int>> directions = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

        CellState piece = board[y][x];

        for (auto dir : directions) {
            int midX = x + dir.first;
            int midY = y + dir.second;
            int newX = x + 2 * dir.first;
            int newY = y + 2 * dir.second;

            if (midX >= 0 && midX < BOARD_SIZE && midY >= 0 && midY < BOARD_SIZE &&
                newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE) {

                if ((board[midY][midX] != CellState::Empty && board[midY][midX] != piece) &&
                    board[newY][newX] == CellState::Empty) {
                    capturingMoves.push_back({ newX, newY });
                }
            }
        }
        return capturingMoves;
    }

private:
    BoardType board;
    CellState currentPlayer;
    int selectedX, selectedY;
    std::vector<std::pair<int, int>> validMoves;
};

int main() {
    setlocale(LC_ALL, "Russian");

    // Задание окна
    sf::RenderWindow window(sf::VideoMode(1024, 600), "Checkers SFML");

    // Загрузка картинки доски
    sf::Texture boardTexture;
    if (!boardTexture.loadFromFile("resources/board.png")) {
        throw std::runtime_error("Failed to load board texture");
    }
    sf::Sprite boardSprite(boardTexture);

    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Sprite> sprites;
    sf::Font font;
    sf::Text text;
    sf::Text statusText;

    CheckersGame game;
    game.loadTextures(textures, sprites, font, text, statusText, game.getCurrentPlayer());  // Сначала загружаем текстуры

    std::vector<std::string> textureNames = { "flip_over", "end", "surprised", "hungry" };
    int currentTextureIndex = 0;

    // Проверка, существует ли начальная текстура
    if (sprites.find(textureNames[currentTextureIndex]) == sprites.end()) {
        std::cerr << "Ошибка: текстура '" << textureNames[currentTextureIndex] << "' не загружена!\n";
        return 1;
    }

    sf::Sprite* currentSprite = &sprites[textureNames[currentTextureIndex]];

    bool gameOver = false;

    // Обработчик событий окна игры
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (!gameOver) {
                    game.handleMouseClick(event.mouseButton.x, event.mouseButton.y);

                    // Проверка нажатия на текущее изображение и переключение на следующее, если игра не окончена
                    sf::Vector2f spritePos = currentSprite->getPosition();
                    sf::FloatRect spriteBounds = currentSprite->getGlobalBounds();

                    if (spriteBounds.contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                        currentTextureIndex = (currentTextureIndex + 1) % textureNames.size();
                        currentSprite = &sprites[textureNames[currentTextureIndex]];
                    }

                    // Обновление текста в зависимости от текущего игрока
                    game.loadTextures(textures, sprites, font, text, statusText, game.getCurrentPlayer());

                    // AI делает ход, если текущий игрок - AI
                    if (game.getCurrentPlayer() == CellState::Black) { // Предполагаем, что AI играет за черных
                        auto bestMove = game.getBestMove(5, CellState::Black);
                        game.makeMove(bestMove.first.first, bestMove.first.second, bestMove.second.first, bestMove.second.second);
                        game.switchPlayer();

                        // Обновление текста после хода AI
                        game.loadTextures(textures, sprites, font, text, statusText, game.getCurrentPlayer());
                    }
                }
            }
        }

        // Проверка, закончилась ли игра
        if (game.isGameOver()) {
            gameOver = true;
            currentSprite = &sprites["end"];
        }

        window.clear();
        window.draw(boardSprite);
        window.draw(*currentSprite);  // Отрисовка текущей картинки или "end" при окончании игры
        window.draw(sprites["dialog"]);  // Отрисовка картинки диалога
        window.draw(text);  // Отрисовка текста внутри диалога
        window.draw(statusText);
        game.draw(window);
        window.display();
    }
    return 0;
}
