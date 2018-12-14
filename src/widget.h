#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "boardlabel.h"
#include <QTimer>
#include "tree.h"

typedef std::pair<int, Node *> paired;

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
    QVector<std::pair<int, int> > virtualSnake;
    QVector<std::pair<int, int> > availPlaces;
    QTimer *timer;
    int snakeMoveDirection;
    Node *root;
    QVector<QVector<std::pair<int, int> > > res;

    void setBoardLayout();
    void createSnake();
    void showSnakeAndFood();
    void generateFood();
    void gameOver();
    bool hasLost();
    void getAvailPlaces();
    void moveSnake(int);
    void moveSnakeHead(int, QVector<std::pair<int, int> > &);
    bool canFindFood();
    bool canFindFood2();
    bool canFindTail();
    bool canFindTail3();
    bool canFindTail2();
    void resetVisited(int);
    QVector<std::pair<int, int> > returnNbrPlaces(int, int);
    void BFS(int, int, int, QVector<std::pair<int, int> >);
    void dijkstra(int, int, int, QVector<std::pair<int, int> >);
    void DFS(int, int, int);
    QVector<std::pair<int, int> > returnPath(int);
    QVector<std::pair<int, int> > returnPath2(int);
    bool canFindObject(int, int, QVector<std::pair<int, int> >);
    void moveVirtualSnake(int);
    void moveVirtualSnake(QVector<std::pair<int, int> >);
    QVector<int> returnMoveablePlaces();
    int returnFarthestDirectionToFood();
    QVector<int> returnPreferredDirections();
    bool isPlaceAvaiable(int, int);
    bool isFoodAround();
    int returnSnakeMoveDirection(QVector<std::pair<int, int> >);

protected:
    void keyPressEvent(QKeyEvent *);

private slots:
    void whenTimeOut();
};

#endif // WIDGET_H
