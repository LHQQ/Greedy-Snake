#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "boardlabel.h"
#include <QTimer>
#include "tree.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

    static const int ROWS;
    static const int COLS;
    static const int NO_DIRECTION;
    static const int UP;
    static const int DOWN;
    static const int LEFT;
    static const int RIGHT;
    static const int FIND_FOOD;
    static const int FIND_TAIL;
    int foodRow, foodCol;
    int tempFoodRow, tempFoodCol;
    QVector<QVector<BoardLabel *> > boardLblVec;
    QVector<std::pair<int, int> > snakeVec;
    QVector<std::pair<int, int> > tempSnakeVec;
    QVector<std::pair<int, int> > availPlaces;
    QTimer *timer;
    int snakeMoveDirection;
    Node *root;
    QVector<QVector<std::pair<int, int> > > res;

    void setBoardLayout();
    void createSnake();
    void showSnakeAndFood();
    void generateFood();
    void removeOldSnake();
    void gameOver();
    bool hasLost();
    bool hasFoodEaten();
    void getAvailPlaces();
    void moveSnake(int);
    void moveSnakeHead(int, QVector<std::pair<int, int> > &);
    bool canFindFood();
    bool canFindTail();
    void resetVisited();
    QVector<std::pair<int, int> > returnNbrPlaces(int, int);
    void BFS(int, int, int);
    QVector<std::pair<int, int> > returnFindFoodPath();
    bool canFindObject(int, int, QVector<std::pair<int, int> >);
    int getSnakeMoveDirection(int);

protected:
    void keyPressEvent(QKeyEvent *);

private slots:
    void whenTimeOut();
};

#endif // WIDGET_H
