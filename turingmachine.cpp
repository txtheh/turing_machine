#include "turingmachine.h"
#include "ui_turingmachine.h"
#include <QMessageBox>
#include <QTimer>

TuringMachine::TuringMachine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TuringMachine)
{
    ui->setupUi(this);
    currentState = 0;
    headPosition = 0;
    tickSpeed = 500;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TuringMachine::makeStep);

    ui->tableTape->horizontalHeader()->setDefaultSectionSize(45);
    ui->tableTape->verticalHeader()->setDefaultSectionSize(45);
    ui->tableTape->horizontalHeader()->setVisible(false);
    ui->tableTape->verticalHeader()->setVisible(false);
    ui->tableTape->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableTape->setSelectionMode(QAbstractItemView::NoSelection);

    if (ui->stackedWidget) ui->stackedWidget->setCurrentIndex(0);
}

TuringMachine::~TuringMachine() {
    delete ui;
}

void TuringMachine::on_btnSpeedUp_clicked() {
    if (tickSpeed > 100) {
        tickSpeed -= 100;
        if (timer->isActive()) timer->start(tickSpeed);
        ui->statusbar->showMessage("Speed: " + QString::number(tickSpeed) + " ms", 1000);
    }
}

void TuringMachine::on_btnSpeedDown_clicked() {
    if (tickSpeed < 2000) {
        tickSpeed += 100;
        if (timer->isActive()) timer->start(tickSpeed);
        ui->statusbar->showMessage("Speed: " + QString::number(tickSpeed) + " ms", 1000);
    }
}

void TuringMachine::on_btnPlay_clicked() {
    timer->start(tickSpeed);
    ui->statusbar->showMessage("Running...");
}

void TuringMachine::on_btnStep_clicked() {
    timer->stop();
    makeStep();
}

void TuringMachine::on_btnPause_clicked() {
    timer->stop();
    ui->statusbar->showMessage("Paused");
}

void TuringMachine::on_btnStop_clicked() {
    timer->stop();
    on_btnSetString_clicked();
    ui->statusbar->showMessage("Stopped");
}

void TuringMachine::makeStep() {
    if (headPosition < 0 || headPosition >= ui->tableTape->columnCount()) {
        timer->stop();
        return;
    }

    QString currentSymbol = ui->tableTape->item(0, headPosition)->text();
    int col = -1;
    for (int i = 0; i < ui->tableProgram->columnCount(); ++i) {
        if (ui->tableProgram->horizontalHeaderItem(i)->text() == currentSymbol) {
            col = i; break;
        }
    }

    if (col == -1) { timer->stop(); return; }

    QTableWidgetItem *ruleItem = ui->tableProgram->item(currentState, col);
    if (!ruleItem || ruleItem->text().isEmpty()) {
        timer->stop();
        ui->statusbar->showMessage("Нет, правла для q" + QString::number(currentState));
        return;
    }

    QStringList parts = ruleItem->text().split(",");
    if (parts.size() < 3) { timer->stop(); return; }

    QString symbolToWrite = parts[0].trimmed();
    if (symbolToWrite == "_") symbolToWrite = "λ";

    ui->tableTape->item(0, headPosition)->setData(Qt::BackgroundRole, QVariant());
    ui->tableTape->item(0, headPosition)->setText(symbolToWrite);

    QString dir = parts[1].trimmed().toUpper();
    if (dir == "R") headPosition++;
    else if (dir == "L") headPosition--;

    QString nextSt = parts[2].trimmed().toLower();
    if (nextSt == "stop" || nextSt == "!") {
        timer->stop();
        ui->statusbar->showMessage("Finished");
    } else {
        currentState = nextSt.remove("q").toInt();
    }

    if (headPosition >= 0 && headPosition < ui->tableTape->columnCount()) {
        ui->tableTape->item(0, headPosition)->setBackground(QColor(173, 216, 230));
        ui->tableTape->scrollToItem(ui->tableTape->item(0, headPosition), QAbstractItemView::PositionAtCenter);
        updateCarriagePosition();
    }
}

void TuringMachine::on_btnSetAlphabets_clicked() {
    QString mainAlpha = ui->lineAlphabetStr->text();
    QString extraAlpha = ui->lineAlphabetExtra->text();
    QString fullAlpha = mainAlpha + extraAlpha;
    if (!fullAlpha.contains("λ")) fullAlpha += "λ";

    ui->tableProgram->setColumnCount(fullAlpha.length());
    ui->tableProgram->setRowCount(1);
    for (int i = 0; i < fullAlpha.length(); ++i) {
        ui->tableProgram->setHorizontalHeaderItem(i, new QTableWidgetItem(QString(fullAlpha[i])));
    }
    ui->tableProgram->setVerticalHeaderItem(0, new QTableWidgetItem("q0"));
    ui->stackedWidget->setCurrentIndex(1);
}

void TuringMachine::on_btnAddState_clicked() {
    int rowCount = ui->tableProgram->rowCount();
    ui->tableProgram->insertRow(rowCount);
    ui->tableProgram->setVerticalHeaderItem(rowCount, new QTableWidgetItem("q" + QString::number(rowCount)));
}

void TuringMachine::on_btnRemoveState_clicked() {
    int rowCount = ui->tableProgram->rowCount();
    if (rowCount > 1) ui->tableProgram->removeRow(rowCount - 1);
}

void TuringMachine::on_btnSetString_clicked() {
    QString input = ui->lineTapeInput->text();
    int tapeSize = 101;
    ui->tableTape->setColumnCount(tapeSize);
    ui->tableTape->setRowCount(1);
    for (int i = 0; i < tapeSize; ++i) {
        ui->tableTape->setItem(0, i, new QTableWidgetItem("λ"));
        ui->tableTape->item(0, i)->setTextAlignment(Qt::AlignCenter);
    }
    int startPos = tapeSize / 2 - input.length() / 2;
    for (int i = 0; i < input.length(); ++i) ui->tableTape->item(0, startPos + i)->setText(QString(input[i]));
    headPosition = startPos;
    currentState = 0;
    ui->tableTape->item(0, headPosition)->setBackground(QColor(173, 216, 230));
    updateCarriagePosition();
}

void TuringMachine::on_btnBackToAlpha_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void TuringMachine::updateCarriagePosition() {
    QTableWidgetItem *item = ui->tableTape->item(0, headPosition);
    if (!item) return;
    QRect cellRect = ui->tableTape->visualItemRect(item);
    int globalX = ui->tableTape->x() + cellRect.center().x() - (ui->labelCarriage->width() / 2);
    ui->labelCarriage->move(globalX, ui->labelCarriage->y());
}