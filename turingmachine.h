#ifndef TURINGMACHINE_H
#define TURINGMACHINE_H

#include <QMainWindow>
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
    void on_btnAddState_clicked();
    void on_btnRemoveState_clicked();
    void on_btnSetString_clicked();
    void on_btnBackToAlpha_clicked();
    void on_btnPause_2_clicked();

private:
    Ui::TuringMachine *ui;
};

#endif // TURINGMACHINE_H#endif // TURINGMACHINE_H#endif // TURINGMACHINE_H#endif // TURINGMACHINE_H#endif // TURINGMACHINE_H