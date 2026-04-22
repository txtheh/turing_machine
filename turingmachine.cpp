#include "turingmachine.h"
#include "ui_turingmachine.h"
#include <QMessageBox>

TuringMachine::TuringMachine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TuringMachine)
{
    ui->setupUi(this);
    ui->tableTape->horizontalHeader()->setDefaultSectionSize(45);
    ui->tableTape->verticalHeader()->setDefaultSectionSize(45);

    // Скрываем номера строк и столбцов (заголовки)
    ui->tableTape->horizontalHeader()->setVisible(false);
    ui->tableTape->verticalHeader()->setVisible(false);

    // Выключаем возможность редактировать ячейки ленты вручную кликом
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

    // 1. Проверка входных символов
    for (const QChar &c : input) {
        if (!allowed.contains(c)) {
            QMessageBox::critical(this, "Ошибка", QString("Символ '%1' не входит в алфавит!").arg(c));
            return;
        }
    }

    // 2. Настройка ленты
    int tapeSize = 101; // Нечетное число, чтобы был четкий центр
    ui->tableTape->setColumnCount(tapeSize);
    ui->tableTape->setRowCount(1);

    // 3. Заполняем ленту символом Лямбда
    QString lambda = "λ";
    for (int i = 0; i < tapeSize; ++i) {
        ui->tableTape->setItem(0, i, new QTableWidgetItem(lambda));
        ui->tableTape->item(0, i)->setTextAlignment(Qt::AlignCenter);
    }

    // 4. Размещаем строку пользователя по центру
    int startPos = tapeSize / 2 - input.length() / 2;
    for (int i = 0; i < input.length(); ++i) {
        ui->tableTape->item(0, startPos + i)->setText(QString(input[i]));
    }

    // 5. Подсветка каретки ГОЛУБЫМ цветом
    // Выбираем позицию первого символа строки
    int headPos = startPos;

    // Используем Qt::cyan (голубой) или точный RGB цвет
    QColor lightBlue(173, 216, 230); // Светло-голубой
    ui->tableTape->item(0, headPos)->setBackground(lightBlue);

    // Прокручиваем к каретке
    ui->tableTape->scrollToItem(ui->tableTape->item(0, headPos), QAbstractItemView::PositionAtCenter);

    if (ui->statusbar) {
        ui->statusbar->showMessage("Лента готова к работе", 3000);
    }
}

void TuringMachine::on_btnBackToAlpha_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}


void TuringMachine::on_btnPause_2_clicked() {

}