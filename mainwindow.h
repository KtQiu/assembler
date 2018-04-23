#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QWidget>
#include <QTableWidgetItem>
#include <QTableWidgetSelectionRange>
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


#include "ui_mainwindow.h"
#include"QFileDialog"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void open();
    bool save();
    bool save_as();

private:
    void create_actions();
    Ui::MainWindow *ui;
    QAction *open_action;
    QAction *save_action;
    QString cur_file;
    QString cur_file_path;
    int line_num;
    void set_cur_file(QString &file_name);
    bool save_file(QString &file_name);

    void init_table();
    void load_asm_file(QString filename);
    void load_bin_file(QString file_name);
    void set_table_row(int n_row, QString str_address, QString str_bin_code);
    int asm2bin_run();
    int bin2asm_run();
};

#endif // MAINWINDOW_H
