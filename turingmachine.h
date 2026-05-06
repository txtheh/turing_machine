#ifndef TURINGMACHINE_H
#define TURINGMACHINE_H

#include <QMainWindow>
#include <QTimer>
#include <QPropertyAnimation>
#include <QTableWidgetItem>

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
    void on_btnBackToAlpha_clicked();
    void on_btnAddState_clicked();
    void on_btnRemoveState_clicked();
    void on_btnSetString_clicked();
    void on_btnPlay_clicked();
    void on_btnStep_clicked();
    void on_btnPause_2_clicked();
    void on_btnStop_clicked();
    void on_btnSpeedUp_clicked();
    void on_btnSpeedDown_clicked();
    void makeStep();
    void onProgramCellChanged(QTableWidgetItem *cellItem); //проверка

private:
    Ui::TuringMachine *ui;

    QTimer *timer;
    int headPosition;
    int currentState;
    int tickSpeed;
    int lastRuleRow = -1; //подсветка правила
    int lastRuleCol = -1;
    QString mainAlphabet;
    void updateCarriagePosition(bool animate);
    void scrollTapeToHead();
    void setRunningState(bool running); //блок экрана
    bool hasStopRule() const;
    void highlightCurrentState();
};

#endif // TURINGMACHINE_H