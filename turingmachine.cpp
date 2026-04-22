#include "turingmachine.h"
#include "ui_turingmachine.h"
#include <QMessageBox>

TuringMachine::TuringMachine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TuringMachine)
{
    ui->setupUi(this);

    // Сбрасываем на первое окно при запуске
    if (ui->stackedWidget) {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

TuringMachine::~TuringMachine()
{
    delete ui;
}

// 1. Кнопка "Задать алфавиты"
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

// 2. Кнопки + и - для состояний
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

// 3. Кнопка "Задать строку"
void TuringMachine::on_btnSetString_clicked() {
    QString input = ui->lineTapeInput->text();
    QString allowed = ui->lineAlphabetStr->text();

    for (const QChar &c : input) {
        if (!allowed.contains(c)) {
            QMessageBox::critical(this, "Ошибка", QString("Символ '%1' не входит в алфавит!").arg(c));
            return;
        }
    }

    if (ui->statusbar) {
        ui->statusbar->showMessage("Строка успешно проверена", 3000);
    }
}

// 4. Кнопка "Назад"
void TuringMachine::on_btnBackToAlpha_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// 5. Заглушка для кнопки паузы
void TuringMachine::on_btnPause_2_clicked() {
    // Пока ничего не делаем
}