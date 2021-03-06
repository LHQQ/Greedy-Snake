#include "widget.h"
#include "ui_widget.h"
#include <QKeyEvent>
#include <QDebug>
#include <QQueue>
#include <QStack>
#include <queue>

const int Widget::ROWS = 20; //20
const int Widget::COLS = 20;
const int Widget::NO_DIRECTION = -1;
const int Widget::UP = 0;
const int Widget::DOWN = 1;
const int Widget::LEFT = 2;
const int Widget::RIGHT = 3;
const int Widget::FIND_FOOD = 0;
const int Widget::FIND_TAIL = 1;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("Greedy Snake AI");

    setBoardLayout();
    createSnake();
    generateFood();
    showSnakeAndFood();

    timer = new QTimer(this);
    timer->start(20);

    connect(timer, &QTimer::timeout, this, &Widget::whenTimeOut);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setBoardLayout()
{
    ui->gridLayout->setSpacing(1);
    for(int i = 0; i < ROWS; ++i) {
        QVector<BoardLabel *> colVec;
        for(int j = 0; j < COLS; ++j) {
            BoardLabel *boardLabel = new BoardLabel(this);
            ui->gridLayout->addWidget(boardLabel, i, j);
            colVec.push_back(boardLabel);
        }
        boardLblVec.push_back(colVec);
    }
}

void Widget::createSnake()
{
    snakeVec.push_back(std::make_pair(ROWS / 2, COLS / 2));
    snakeVec.push_back(std::make_pair(ROWS / 2, COLS / 2 - 1));
    snakeVec.push_back(std::make_pair(ROWS / 2, COLS / 2 - 2));

    virtualSnake.clear();
    virtualSnake = snakeVec;

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (i == ROWS / 2 && j == COLS / 2)
                continue;
            if (i == ROWS / 2 && j == COLS / 2 - 1)
                continue;
            if (i == ROWS / 2 && j == COLS / 2 - 2)
                continue;
            else
                availPlaces.push_back(std::make_pair(i, j));
        }
    }

    snakeMoveDirection = RIGHT;
}

void Widget::showSnakeAndFood()
{
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            boardLblVec[i][j]->setStyleSheet("QLabel { border: 1px solid grey; }");
        }
    }

    for(int i = 0; i < snakeVec.size(); ++i) {
        if(i == 0) {
            boardLblVec[snakeVec[i].first][snakeVec[i].second]->setStyleSheet("QLabel { background: red; }");
        }
        else if (i == snakeVec.size() - 1) {
            boardLblVec[snakeVec[i].first][snakeVec[i].second]->setStyleSheet("QLabel { background: blue; }");
        }
        else {
            boardLblVec[snakeVec[i].first][snakeVec[i].second]->setStyleSheet("QLabel { background: black; }");
        }
    }

    boardLblVec[foodRow][foodCol]->setStyleSheet("QLabel { background: yellow; }");
}

void Widget::generateFood()
{
    getAvailPlaces();
    if (!availPlaces.empty()) {
        int i = rand() % availPlaces.size();
        foodRow = availPlaces[i].first;
        foodCol = availPlaces[i].second;
    }
}

void Widget::gameOver()
{
    timer->stop();
    qDebug() << "Game Over!";
}

bool Widget::hasLost()
{
    if (snakeVec.front().first < 0 || snakeVec.front().first >= ROWS || snakeVec.front().second < 0 || snakeVec.front().second >= COLS) {
        return true;
    }

    auto p = std::make_pair(snakeVec.front().first, snakeVec.front().second);

    if (std::find(snakeVec.begin() + 1, snakeVec.end(), p) != snakeVec.end()) {
        return true;
    }

    return false;
}

void Widget::getAvailPlaces()
{
    availPlaces.clear();

    QVector<std::pair<int, int> > mVec;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            mVec.push_back(std::make_pair(i, j));
        }
    }

    QVector<std::pair<int, int> > tempSnake(snakeVec);

    std::sort(mVec.begin(), mVec.end());
    std::sort(tempSnake.begin(), tempSnake.end());

    std::set_difference(mVec.begin(), mVec.end(),
                        tempSnake.begin(), tempSnake.end(),
                        std::back_inserter(availPlaces));
}

void Widget::moveSnake(int direction)
{
    //move snake head
    moveSnakeHead(direction, snakeVec);

    //move snake body
    if (snakeVec.front().first == foodRow && snakeVec.front().second == foodCol) {
        generateFood();
    }
    else {
        snakeVec.pop_back();
    }

    if (hasLost()) {
        snakeVec.pop_front();
        showSnakeAndFood();
        boardLblVec[snakeVec.front().first][snakeVec.front().second]->setStyleSheet("QLabel { background: black; }");

        gameOver();
        return;
    }

    showSnakeAndFood();
}

void Widget::moveSnakeHead(int direction, QVector<std::pair<int, int> > &snake)
{
    switch (direction) {
    case UP:
        snake.push_front(std::make_pair(snake.front().first - 1, snake.front().second));
        break;
    case DOWN:
        snake.push_front(std::make_pair(snake.front().first + 1, snake.front().second));
        break;
    case LEFT:
        snake.push_front(std::make_pair(snake.front().first, snake.front().second - 1));
        break;
    case RIGHT:
        snake.push_front(std::make_pair(snake.front().first, snake.front().second + 1));
        break;
    default:
        break;
    }
}

bool Widget::canFindFood()
{
    dijkstra(snakeVec.front().first, snakeVec.front().second, FIND_FOOD, snakeVec);
    QVector<std::pair<int, int> > path = returnPath(FIND_FOOD);

    if (path.empty() == false) {
        moveVirtualSnake(path);
        snakeMoveDirection = returnSnakeMoveDirection(path);
        return true;
    }

    return false;
}

bool Widget::canFindTail()
{
    dijkstra(virtualSnake.front().first, virtualSnake.front().second, FIND_TAIL, virtualSnake);
    QVector<std::pair<int, int> > path = returnPath(FIND_TAIL);

    if (!path.empty())
        return true;

    return false;
}

std::pair<bool, int> Widget::canGetSteps()
{
    dijkstra(virtualSnake.front().first, virtualSnake.front().second, FIND_TAIL, virtualSnake);
    QVector<std::pair<int, int> > path = returnPath(FIND_TAIL);

    if (!path.empty())
        return std::make_pair(true, path.size());

    return std::make_pair(false, 0);;
}

void Widget::resetVisited(int option)
{
    for (int i = 0; i < boardLblVec.size(); ++i) {
        for (int j = 0; j < boardLblVec[i].size(); ++j) {
            boardLblVec[i][j]->visited = false;
        }
    }

    if (option == FIND_FOOD) {
        for (int i = 0; i < snakeVec.size(); ++i) {
            boardLblVec[snakeVec[i].first][snakeVec[i].second]->visited = true;
        }
    }
    else if (option == FIND_TAIL) {
        for (int i = 0; i < virtualSnake.size() - 1; ++i) {
            boardLblVec[virtualSnake[i].first][virtualSnake[i].second]->visited = true;
        }
    }
}

QVector<std::pair<int, int> > Widget::returnNbrPlaces(int row, int col)
{
    QVector<std::pair<int, int> > res;

    if (row - 1 >= 0) {
        if (boardLblVec[row - 1][col]->visited == false) {
            res.push_back(std::make_pair(row - 1, col));
        }
    }
    if (row + 1 < ROWS) {
        if (boardLblVec[row + 1][col]->visited == false) {
            res.push_back(std::make_pair(row + 1, col));
        }
    }
    if (col - 1 >= 0) {
        if (boardLblVec[row][col - 1]->visited == false) {
            res.push_back(std::make_pair(row, col - 1));
        }
    }
    if (col + 1 < COLS) {
        if (boardLblVec[row][col + 1]->visited == false) {
            res.push_back(std::make_pair(row, col + 1));
        }
    }

    return res;
}

void Widget::dijkstra(int row, int col, int option, QVector<std::pair<int, int> > mVec)
{
    resetVisited(option);

    root = make_shared<Node>(row, col);
    root->cost = 0;

    std::priority_queue<int, vector<paired>, std::greater<paired> > queue;
    QVector<pair<int, int> > visited;

    queue.push(make_pair(root->cost, root));

    while (queue.empty() == false) {
        shared_ptr<Node> curr = queue.top().second;
        row = curr->row;
        col = curr->col;

        queue.pop();

        if (option == FIND_FOOD) {
            if (row == foodRow && col == foodCol)
                break;
        }
        else if (option == FIND_TAIL) {
            if (row == mVec.back().first && col == mVec.back().second)
                break;
        }

        auto it = make_pair(row, col);
        if (std::find(visited.begin(), visited.end(), it) != visited.end())
            continue;
        else {
            QVector<pair<int, int> > neighbors = returnNbrPlaces(row, col);
            for (int i = 0; i < neighbors.size(); ++i)
                queue.push(make_pair(curr->cost + 1, root->createChild(curr, neighbors[i].first, neighbors[i].second, curr->cost + 1)));

            visited.push_back(it);
        }
    }
}

QVector<std::pair<int, int> > Widget::returnPath(int option)
{
    QVector<std::pair<int, int> > path;
    res.clear();
    root->rootToLeaf(root, path, res);

    QVector<QVector<std::pair<int, int> > > avaiablePath;

    for (int i = 0; i < res.size(); ++i) {
        if (option == FIND_FOOD) {
            if (canFindObject(foodRow, foodCol, res[i])) {
                avaiablePath.push_back(res[i]);
            }
        }
        else if (option == FIND_TAIL) {
            if (canFindObject(virtualSnake.back().first, virtualSnake.back().second, res[i])) {
                avaiablePath.push_back(res[i]);
            }
        }
    }

    if (avaiablePath.empty()) {
        return path;
    }

    int index = 0;

    if (avaiablePath.size() > 1) {
        int n = avaiablePath.front().size();
        for (int i = 0; i < avaiablePath.size(); ++i) {
            if (avaiablePath[i].size() < n) {
                n = avaiablePath[i].size();
                index = i;
            }
        }
    }

    return avaiablePath[index];
}

bool Widget::canFindObject(int row, int col, QVector<std::pair<int, int> > mVec)
{
    auto it = std::make_pair(row, col);

    if (std::find(mVec.begin(), mVec.end(), it) != mVec.end())
        return true;

    return false;
}

void Widget::moveVirtualSnake(int direction)
{
    virtualSnake.clear();
    virtualSnake = snakeVec;
    moveSnakeHead(direction, virtualSnake);
    if (virtualSnake.front().first != foodRow && virtualSnake.front().second != foodCol)
        virtualSnake.pop_back(); //bug here
}

void Widget::moveVirtualSnake(QVector<std::pair<int, int> > path)
{
    virtualSnake.clear();
    virtualSnake = snakeVec;

    //path[0] is snake head, virtual snake already has snake head
    int popTimes = -1;
    for (int i = 1; i < path.size(); ++i) {
        virtualSnake.push_front(std::make_pair(path[i].first, path[i].second));
        ++popTimes;
    }

    //because virtual snake has already ate food, the length grow one
    for (int i = 0; i < popTimes; ++i) {
        virtualSnake.pop_back();
    }
}

int Widget::returnFarthestDirectionToFood()
{
    int steps = 0, dir = NO_DIRECTION;

    if (isPlaceAvaiable(snakeVec.front().first - 1, snakeVec.front().second)) {
        moveVirtualSnake(UP);
        std::pair<bool, int> res = canGetSteps();
        if (res.first == true) {
            if (steps < res.second) {
                steps = res.second;
                dir = UP;
            }
        }
    }
    if (isPlaceAvaiable(snakeVec.front().first + 1, snakeVec.front().second)) {
        moveVirtualSnake(DOWN);
        std::pair<bool, int> res = canGetSteps();
        if (res.first == true) {
            if (steps < res.second) {
                steps = res.second;
                dir = DOWN;
            }
        }
    }
    if (isPlaceAvaiable(snakeVec.front().first, snakeVec.front().second - 1)) {
        moveVirtualSnake(LEFT);
        std::pair<bool, int> res = canGetSteps();
        if (res.first == true) {
            if (steps < res.second) {
                steps = res.second;
                dir = LEFT;
            }
        }
    }
    if (isPlaceAvaiable(snakeVec.front().first, snakeVec.front().second + 1)) {
        moveVirtualSnake(RIGHT);
        std::pair<bool, int> res = canGetSteps();
        if (res.first == true) {
            if (steps < res.second) {
                steps = res.second;
                dir = RIGHT;
            }
        }
    }

    return dir;
}

bool Widget::isPlaceAvaiable(int row, int col)
{
    QVector<std::pair<int, int> > tempSnake(snakeVec);
    tempSnake.pop_back();

    if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
        auto it = std::make_pair(row, col);
        if (std::find(tempSnake.begin(), tempSnake.end(), it) != tempSnake.end())
            return false;
        else
            return true;
    }

    return false;
}

bool Widget::isFoodAround()
{
    if (snakeVec.front().first - 1 == foodRow && snakeVec.front().second == foodCol)
        return true;
    else if (snakeVec.front().first + 1 == foodRow && snakeVec.front().second == foodCol)
        return true;
    else if (snakeVec.front().first == foodRow && snakeVec.front().second - 1 == foodCol)
        return true;
    else if (snakeVec.front().first == foodRow && snakeVec.front().second + 1 == foodCol)
        return true;

    return false;
}

int Widget::returnSnakeMoveDirection(QVector<std::pair<int, int> > path)
{
    int row = path[1].first;
    int col = path[1].second;

    if (row > snakeVec.front().first)
        return DOWN;
    if (row < snakeVec.front().first)
        return UP;
    if (col > snakeVec.front().second)
        return RIGHT;
    if (col < snakeVec.front().second)
        return LEFT;

    return 0;
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down || event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
        if (event->key() == Qt::Key_Up) {
            snakeMoveDirection = UP;
        }
        else if (event->key() == Qt::Key_Down) {
            snakeMoveDirection = DOWN;
        }
        else if (event->key() == Qt::Key_Left) {
            snakeMoveDirection = LEFT;
        }
        else if (event->key() == Qt::Key_Right) {
            snakeMoveDirection = RIGHT;
        }

        moveSnake(snakeMoveDirection);
    }
}

void Widget::whenTimeOut()
{
    if (canFindFood()) {
        if (canFindTail()) {
            moveSnake(snakeMoveDirection);
        }

        virtualSnake.clear();
        virtualSnake = snakeVec;
    }
    if (canFindFood() == false || canFindTail() == false) {
        if (availPlaces.size() == 1 && isFoodAround() == true) {
            snakeVec.push_front(std::make_pair(foodRow, foodCol));
            showSnakeAndFood();
            boardLblVec[foodRow][foodCol]->setStyleSheet("QLabel { background: red; }");
            gameOver();
            return;
        }

        moveSnake(returnFarthestDirectionToFood());
    }
}
