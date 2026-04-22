#include "turingmachine.h"
#include "ui_turingmachine.h"
#include <QMessageBox>

TuringMachine::TuringMachine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TuringMachine)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TuringMachine::makeStep);
    ui->tableTape->horizontalHeader()->setDefaultSectionSize(45);
    ui->tableTape->verticalHeader()->setDefaultSectionSize(45);

    ui->tableTape->horizontalHeader()->setVisible(false);
    ui->tableTape->verticalHeader()->setVisible(false);

    ui->tableTape->setEditTriggers(QAbstractItemView::NoEditTriggers);
    if (ui->stackedWidget) {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

TuringMachine::~TuringMachine()
{
    delete ui;
}
void TuringMachine::on_btnSetAlphabets_clicked() {
    QString mainAlpha = ui->lineAlphabetStr->text();
    QString extraAlpha = ui->lineAlphabetExtra->text();
    QString fullAlpha = mainAlpha + extraAlpha;

    if (fullAlpha.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Алфавит не может быть пустым!");
        return;
    }

    ui->tableProgram->setColumnCount(fullAlpha.length());
    ui->tableProgram->setRowCount(1);

    for (int i = 0; i < fullAlpha.length(); ++i) {
        ui->tableProgram->setHorizontalHeaderItem(i, new QTableWidgetItem(QString(fullAlpha[i])));
    }
    ui->tableProgram->setVerticalHeaderItem(0, new QTableWidgetItem("q0"));

    ui->stackedWidget->setCurrentIndex(1);
    ui->tableProgram->setEnabled(true);
}

void TuringMachine::on_btnAddState_clicked() {
    int rowCount = ui->tableProgram->rowCount();
    ui->tableProgram->insertRow(rowCount);
    ui->tableProgram->setVerticalHeaderItem(rowCount, new QTableWidgetItem("q" + QString::number(rowCount)));
}

void TuringMachine::on_btnRemoveState_clicked() {
    int rowCount = ui->tableProgram->rowCount();
    if (rowCount > 1) {
        ui->tableProgram->removeRow(rowCount - 1);
    }
}

void TuringMachine::on_btnSetString_clicked() {
    QString input = ui->lineTapeInput->text();
    QString allowed = ui->lineAlphabetStr->text();

    // 1. Проверка на вхождение в алфавит
    for (const QChar &c : input) {
        if (!allowed.contains(c)) {
            QMessageBox::critical(this, "Ошибка", QString("Символ '%1' не входит в алфавит!").arg(c));
            return;
        }
    }

    // 2. Настройка размеров ленты
    int tapeSize = 101;
    ui->tableTape->setColumnCount(tapeSize);
    ui->tableTape->setRowCount(1);

    // Убираем заголовки, если они еще видны
    ui->tableTape->verticalHeader()->hide();
    ui->tableTape->horizontalHeader()->hide();

    // 3. Заполнение лямбдами
    QString lambda = "λ";
    for (int i = 0; i < tapeSize; ++i) {
        ui->tableTape->setItem(0, i, new QTableWidgetItem(lambda));
        ui->tableTape->item(0, i)->setTextAlignment(Qt::AlignCenter);
    }

    // 4. Установка строки по центру
    int startPos = tapeSize / 2 - input.length() / 2;
    for (int i = 0; i < input.length(); ++i) {
        ui->tableTape->item(0, startPos + i)->setText(QString(input[i]));
    }

    // Обновляем глобальную позицию головы (та, что в private в .h)
    headPosition = startPos;

    // 5. Визуальное выделение ячейки
    QColor lightBlue(173, 216, 230);
    ui->tableTape->item(0, headPosition)->setBackground(lightBlue);

    // Центрируем саму таблицу
    ui->tableTape->scrollToItem(ui->tableTape->item(0, headPosition), QAbstractItemView::PositionAtCenter);

    // 6. МАГИЯ: Двигаем треугольник (labelCarriage)
    // Используем QTimer, чтобы дать Qt миллисекунду отрисовать таблицу,
    // иначе координаты ячейки могут вернуться нулевыми.
    QTimer::singleShot(50, this, [this]() {
        // Получаем координаты прямоугольника текущей ячейки
        QRect cellRect = ui->tableTape->visualItemRect(ui->tableTape->item(0, headPosition));

        // Вычисляем центр ячейки относительно окна
        // Берем X таблицы + X ячейки + половина ширины ячейки - половина ширины треугольника
        int targetX = ui->tableTape->x() + cellRect.x() + (cellRect.width() / 2) - (ui->labelCarriage->width() / 2);

        // Перемещаем каретку только по горизонтали
        ui->labelCarriage->move(targetX, ui->labelCarriage->y());
    });

    if (ui->statusbar) {
        ui->statusbar->showMessage("Лента готова к работе", 3000);
    }
}

void TuringMachine::on_btnBackToAlpha_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}


void TuringMachine::on_btnPause_2_clicked() {

}

void TuringMachine::makeStep() {

    headPosition++;
    if (headPosition >= ui->tableTape->columnCount()) {
        timer->stop();
        return;
    }

    ui->tableTape->scrollToItem(ui->tableTape->item(0, headPosition), QAbstractItemView::PositionAtCenter);

    for(int i = 0; i < ui->tableTape->columnCount(); ++i) {
        ui->tableTape->item(0, i)->setBackground(Qt::white);
    }
    ui->tableTape->item(0, headPosition)->setBackground(Qt::yellow);
}

void TuringMachine::on_btnStart_clicked() {

    timer->start(500);
}

void TuringMachine::updateCarriagePosition() {
    // Получаем визуальный прямоугольник ячейки, где сейчас "голова"
    QRect cellRect = ui->tableTape->visualItemRect(ui->tableTape->item(0, headPosition));

    // Вычисляем координаты X (центр ячейки)
    // Нам нужно прибавить X таблицы, чтобы переместить label относительно окна
    int x = ui->tableTape->x() + cellRect.center().x() - (ui->labelCarriage->width() / 2);

    // Передвигаем! (Y оставляем как был или подправляем под таблицу)
    ui->labelCarriage->move(x, ui->labelCarriage->y());
}