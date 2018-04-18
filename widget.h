#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include "ui_widget.h"
#include <QTableWidgetItem> //表格条目
#include <QTableWidgetSelectionRange>   //表格选中区域
#include <QString>
#include <QTableView>
#include <QHeaderView>
#include <QFile>
#include <QIODevice>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTextStream>
#include <QMessageBox>

#include <assembler_core.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::Widget *ui;
    void init_table();
    void load_asm_file();
    void load_bin_file();
    void set_table_row(int n_row, QString str_address, QString str_bin_code);
    int asm2bin_run();
    int bin2asm_run();
};



#endif // WIDGET_H
