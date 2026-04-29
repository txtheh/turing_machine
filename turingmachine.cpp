#include "turingmachine.h"
#include "ui_turingmachine.h"
#include <QMessageBox>
#include <QScrollBar>

TuringMachine::TuringMachine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TuringMachine)
{
    ui->setupUi(this);

    headPosition = 0;
    currentState = 0;
    tickSpeed    = 500;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TuringMachine::makeStep);

    ui->tableTape->horizontalHeader()->setDefaultSectionSize(45); //лента
    ui->tableTape->verticalHeader()->setDefaultSectionSize(45);
    ui->tableTape->horizontalHeader()->setVisible(false);
    ui->tableTape->verticalHeader()->setVisible(false);
    ui->tableTape->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableTape->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableTape->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(ui->tableProgram, &QTableWidget::itemChanged,this, &TuringMachine::onProgramCellChanged); //вызов ячейки
    ui->stackedWidget->setCurrentIndex(0);
}

TuringMachine::~TuringMachine()
{
    delete ui;
}

void TuringMachine::on_btnSpeedUp_clicked()
{
    if (tickSpeed > 100) {
        tickSpeed -= 100;
        if (timer->isActive())
            timer->start(tickSpeed);
        ui->statusbar->showMessage("Скорость: " + QString::number(tickSpeed) + " мс", 1500);
    }
}

void TuringMachine::on_btnSpeedDown_clicked()
{
    if (tickSpeed < 2000) {
        tickSpeed += 100;
        if (timer->isActive())
            timer->start(tickSpeed);
        ui->statusbar->showMessage("Скорость: " + QString::number(tickSpeed) + " мс", 1500);
    }
}


void TuringMachine::on_btnPlay_clicked()
{
    if (!hasStopRule()) {
        QMessageBox::warning(this, "Ошибка","В программе нет ни одного правила остановки.\n");
        return;
    }
    setRunningState(true);
    timer->start(tickSpeed);
    ui->statusbar->showMessage("Работатет...");
}

void TuringMachine::on_btnStep_clicked()
{
    timer->stop();
    makeStep();
}

void TuringMachine::on_btnPause_2_clicked()
{
    timer->stop();
    setRunningState(false);
    ui->statusbar->showMessage("Пауза");
}

void TuringMachine::on_btnStop_clicked()
{
    timer->stop();
    setRunningState(false);
    on_btnSetString_clicked();
    ui->statusbar->showMessage("Сброшено");
}

void TuringMachine::makeStep()
{
    if (headPosition < 0 || headPosition >= ui->tableTape->columnCount()) {
        timer->stop();
        setRunningState(false);
        ui->statusbar->showMessage("Головка улетела");
        return;
    }
    QString currentSymbol = ui->tableTape->item(0, headPosition)->text(); //чит символ


    int col = -1;
    for (int i = 0; i < ui->tableProgram->columnCount(); ++i) { // ищем нужную программу
        QTableWidgetItem *h = ui->tableProgram->horizontalHeaderItem(i);
        if (h && h->text() == currentSymbol) {
            col = i;
            break;
        }
    }

    if (col == -1) {
        timer->stop();
        setRunningState(false);
        ui->statusbar->showMessage("Напишите правило для символа: «" + currentSymbol + "»");
        return;
    }

    QTableWidgetItem *ruleItem = ui->tableProgram->item(currentState, col);
    if (!ruleItem || ruleItem->text().isEmpty()) {
        timer->stop();
        setRunningState(false);
        ui->statusbar->showMessage("Нет правила для q" + QString::number(currentState) + ", символ «" + currentSymbol + "»");
        return;
    }

    QStringList parts = ruleItem->text().split(",");
    if (parts.size() < 3) {
        timer->stop();
        setRunningState(false);
        ui->statusbar->showMessage("правило: " + ruleItem->text());
        return;
    }


    QString symbolToWrite = parts[0].trimmed(); //пуст символ
    if (symbolToWrite == "/\\" || symbolToWrite == "Λ" || symbolToWrite == "/")
        symbolToWrite = "λ";
    if (symbolToWrite == " " || symbolToWrite.isEmpty())
        symbolToWrite = "λ";


    ui->tableTape->item(0, headPosition)->setData(Qt::BackgroundRole, QVariant()); //подсветка
    ui->tableTape->item(0, headPosition)->setText(symbolToWrite);


    QString direction = parts[1].trimmed().toUpper();//движение
    if (direction == "R")
        headPosition++;
    else if (direction == "L")
        headPosition--;


    QString nextState = parts[2].trimmed().toLower();//состояние
    if (nextState == "stop" || nextState == "!") { //подсветка состояния
        if (headPosition >= 0 && headPosition < ui->tableTape->columnCount()) {
            ui->tableTape->item(0, headPosition)->setBackground(QColor(173, 216, 230));
            scrollTapeToHead();
            updateCarriagePosition(true);
        }
        highlightCurrentState();
        timer->stop();
        setRunningState(false);
        ui->statusbar->showMessage("Готово");
        return;
    } else {
        currentState = nextState.remove("q").toInt();
    }

    if (headPosition >= 0 && headPosition < ui->tableTape->columnCount()) {//подсветка и сдвиг
        ui->tableTape->item(0, headPosition)->setBackground(QColor(173, 216, 230));
        scrollTapeToHead();
        updateCarriagePosition(true);
    }

    highlightCurrentState();
}


void TuringMachine::scrollTapeToHead()
{

    QTableWidgetItem *item = ui->tableTape->item(0, headPosition); //координаты ячейки
    if (!item) return;

    QRect cellRect = ui->tableTape->visualItemRect(item);
    int viewportWidth = ui->tableTape->viewport()->width();

    int threshold = viewportWidth / 4; //отступы от края для ленты

    bool needScroll = false;
    int currentScrollVal = ui->tableTape->horizontalScrollBar()->value();
    int newScrollVal = currentScrollVal;

    if (cellRect.right() > viewportWidth - threshold) {
        newScrollVal = currentScrollVal + viewportWidth / 3; //сдвиг ленты пр
        needScroll = true;
    } else if (cellRect.left() < threshold) {
        newScrollVal = currentScrollVal - viewportWidth / 3; //сдвиг ленты лв
        needScroll = true;
    }

    if (needScroll) {
        ui->tableTape->horizontalScrollBar()->setValue(newScrollVal); //сдвиг
    }
}



void TuringMachine::updateCarriagePosition(bool animate)
{
    QTableWidgetItem *item = ui->tableTape->item(0, headPosition);
    if (!item) return;

    QRect cellRect = ui->tableTape->visualItemRect(item); //координаты
    QPoint cellCenterInViewport = cellRect.center(); //координаты центра ячейки для каретки
    QPoint cellCenterInParent = ui->tableTape->viewport()->mapTo(
        ui->labelCarriage->parentWidget(),
        cellCenterInViewport
        );

    int targetX = cellCenterInParent.x() - ui->labelCarriage->width() / 2;
    int targetY = ui->labelCarriage->y(); // у фикс

    if (animate) {
        QPropertyAnimation *anim = new QPropertyAnimation(ui->labelCarriage, "pos", this);
        anim->setDuration(qMax(50, tickSpeed - 50));
        anim->setStartValue(ui->labelCarriage->pos());
        anim->setEndValue(QPoint(targetX, targetY));
        anim->setEasingCurve(QEasingCurve::InOutQuad);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        ui->labelCarriage->move(targetX, targetY);
    }
}

void TuringMachine::on_btnSetAlphabets_clicked()
{
    mainAlphabet       = ui->lineAlphabetStr->text();
    QString extraAlpha = ui->lineAlphabetExtra->text();
    QString fullAlpha  = mainAlphabet + extraAlpha;

    if (!fullAlpha.contains("λ"))
        fullAlpha += "λ";

    disconnect(ui->tableProgram, &QTableWidget::itemChanged,
               this, &TuringMachine::onProgramCellChanged);//перерыв пока снова заполняем таблицу

    ui->tableProgram->setColumnCount(fullAlpha.length());
    ui->tableProgram->setRowCount(1);

    for (int i = 0; i < fullAlpha.length(); ++i) {
        ui->tableProgram->setHorizontalHeaderItem(
            i, new QTableWidgetItem(QString(fullAlpha[i]))
            );
    }
    ui->tableProgram->setVerticalHeaderItem(0, new QTableWidgetItem("q0"));

    connect(ui->tableProgram, &QTableWidget::itemChanged,
            this, &TuringMachine::onProgramCellChanged);

    ui->stackedWidget->setCurrentIndex(1);
}

void TuringMachine::on_btnBackToAlpha_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void TuringMachine::on_btnAddState_clicked()
{
    int rowCount = ui->tableProgram->rowCount();
    ui->tableProgram->insertRow(rowCount);
    ui->tableProgram->setVerticalHeaderItem(
        rowCount, new QTableWidgetItem("q" + QString::number(rowCount))
        );
}

void TuringMachine::on_btnRemoveState_clicked()
{
    int rowCount = ui->tableProgram->rowCount();
    if (rowCount > 1)
        ui->tableProgram->removeRow(rowCount - 1);
}


void TuringMachine::on_btnSetString_clicked() // создание строки и ленты
{
    QString input = ui->lineTapeInput->text();

    // Проверяем символы строки
    if (!mainAlphabet.isEmpty()) {
        for (QChar ch : input) { //ошибки
            if (!mainAlphabet.contains(ch)) {
                QMessageBox::warning(this, "Ошибка ввода",
                                     QString("Символ «%1» не входит в алфавит строки.").arg(ch));
                return;
            }
        }
    }

    int tapeSize = 101;
    ui->tableTape->setColumnCount(tapeSize);
    ui->tableTape->setRowCount(1);

    for (int i = 0; i < tapeSize; ++i) {
        ui->tableTape->setItem(0, i, new QTableWidgetItem("λ"));
        ui->tableTape->item(0, i)->setTextAlignment(Qt::AlignCenter);
    }

    int startPos = tapeSize / 2 - input.length() / 2;
    for (int i = 0; i < input.length(); ++i)
        ui->tableTape->item(0, startPos + i)->setText(QString(input[i]));

    headPosition = startPos;
    currentState = 0;

    ui->tableTape->item(0, headPosition)->setBackground(QColor(173, 216, 230));

    ui->tableTape->scrollToItem( //двигаем ленту, потом каретку
        ui->tableTape->item(0, headPosition),
        QAbstractItemView::PositionAtCenter
        );

    QTimer::singleShot(0, this, [this]() { //задержка
        updateCarriagePosition(false);
    });

    highlightCurrentState();
}

void TuringMachine::onProgramCellChanged(QTableWidgetItem *cellItem)
{
    if (!cellItem) return;
    QString text = cellItem->text().trimmed();
    if (text.isEmpty()) return;

    // Собираем допустимые символы алфавита
    QString fullAlpha = mainAlphabet + ui->lineAlphabetExtra->text();
    if (!fullAlpha.contains("λ")) fullAlpha += "λ";

    QStringList parts = text.split(",");

    bool valid = true;
    QString errorMsg;

    if (parts.size() < 3) {
        valid = false;
        errorMsg = "Правило должно иметь формат: символ,направление,состояние\n"
                   "Например: a,R,q1  или  λ,R,stop";
    } else {
        QString sym = parts[0].trimmed();
        QString dir = parts[1].trimmed().toUpper();
        QString state = parts[2].trimmed().toLower();//проверяем символ
        if (sym == "/\\" || sym == "Λ" || sym == "/") sym = "λ";
        if (sym == " ") sym = "λ";
        if (!fullAlpha.isEmpty() && !fullAlpha.contains(sym)) {
            valid = false;
            errorMsg = QString("Символ «%1» не входит в алфавит.").arg(sym);
        }
        if (dir != "R" && dir != "L" && dir != "N" && dir != "S") {
            valid = false;
            errorMsg += "\nНаправление должно быть R, L или N."; //проер направления
        }

        if (state != "stop" && state != "!") { //провер сост
            QString stateNum = state;
            stateNum.remove("q");
            bool ok;
            stateNum.toInt(&ok);
            if (!ok) {
                valid = false;
                errorMsg += "\nСостояние должно быть q0, q1, ... или stop/!";
            }
        }
    }

    if (!valid) { //подсветка красн
        cellItem->setBackground(QColor(255, 200, 200));
        ui->statusbar->showMessage("⚠ " + errorMsg, 3000);
    } else {
        cellItem->setData(Qt::BackgroundRole, QVariant()); // сбрасываем красный
    }
}

void TuringMachine::setRunningState(bool running)
{
    ui->tableProgram->setEnabled(!running);
    ui->lineTapeInput->setEnabled(!running);
    ui->lineAlphabetStr->setEnabled(!running);
    ui->lineAlphabetExtra->setEnabled(!running);
    ui->btnSetString->setEnabled(!running);
    ui->btnSetAlphabets->setEnabled(!running);
    ui->btnBackToAlpha->setEnabled(!running);
    ui->btnAddState->setEnabled(!running);
    ui->btnRemoveState->setEnabled(!running);
    ui->btnPlay->setEnabled(!running);
    ui->btnStep->setEnabled(!running);
}

bool TuringMachine::hasStopRule() const
{
    for (int row = 0; row < ui->tableProgram->rowCount(); ++row) {
        for (int col = 0; col < ui->tableProgram->columnCount(); ++col) {
            QTableWidgetItem *cell = ui->tableProgram->item(row, col);
            if (!cell) continue;
            QStringList parts = cell->text().toLower().split(",");
            if (parts.size() >= 3) {
                QString ns = parts[2].trimmed();
                if (ns == "stop" || ns == "!")
                    return true;
            }
        }
    }
    return false;
}

void TuringMachine::highlightCurrentState()
{
    for (int row = 0; row < ui->tableProgram->rowCount(); ++row) {
        QTableWidgetItem *header = ui->tableProgram->verticalHeaderItem(row);
        if (!header) continue;

        QFont font = header->font();
        if (row == currentState) {
            font.setBold(true);
            header->setForeground(QColor(Qt::blue));
        } else {
            font.setBold(false);
            header->setForeground(QColor(Qt::black));
        }
        header->setFont(font);
    }
}