#ifndef TURINGMACHINE_H
#define TURINGMACHINE_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class TuringMachine; }
QT_END_NAMESPACE

class TuringMachine : public QMainWindow
{
    Q_OBJECT

public:
    TuringMachine(QWidget *parent = nullptr);
    ~TuringMachine();

private slots:
    void on_btnSetAlphabets_clicked();
    void on_btnAddState_clicked();
    void on_btnRemoveState_clicked();
    void on_btnSetString_clicked();
    void on_btnBackToAlpha_clicked();
    void on_btnSpeedUp_clicked();
    void on_btnSpeedDown_clicked();
    void on_btnPlay_clicked();
    void on_btnStep_clicked();
    void on_btnPause_clicked();
    void on_btnStop_clicked();
    void makeStep();

private:
    Ui::TuringMachine *ui;
    int headPosition;
    int currentState;
    int tickSpeed;
    QTimer *timer;
    void updateCarriagePosition();
};

#endif