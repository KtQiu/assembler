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


// #include <iostream>
// #include "vector"
// #include "map"
// #include "string"
// #include "algorithm"
// #include "fstream"

// using namespace std;

// void find_loop(istream & inf);

// void split(const string & code, char * delimiter, int n_delimiter, vector<string>& v);

// int bin2dec_5bit(string array);

// string dec2bin(int dec_num, int len_bin);

// string asm2bin(vector<string>& v);

// string bin2asm(vector<string> &v);


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
//    QTextEdit *showText;
    void init_table();
    void load_file();
    void set_table_row(int n_row, QString str_address, QString str_bin_code);
    int asm2bin_run();
    int bin2asm_run();
};



#endif // WIDGET_H
