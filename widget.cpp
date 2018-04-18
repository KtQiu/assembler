#include "widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    init_table();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    asm2bin_run();
    load_asm_file();
}

void Widget::on_pushButton_2_clicked()
{
    load_bin_file();
    bin2asm_run();
}

void Widget::init_table()
{
    ui->tableWidget->setRowCount(1000);
    ui->tableWidget->setColumnCount(2);
    QStringList listHeaders;
    listHeaders << tr("Address") << tr("Machine Language");
    ui->tableWidget->setHorizontalHeaderLabels(listHeaders);
    ui->tableWidget->QTableView::verticalHeader()->hide();
    ui->tableWidget->setColumnWidth(0, 150);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //就是这个地方
    return;
}

void Widget::set_table_row(int n_row, QString str_address, QString str_bin_code)
{
    QTableWidgetItem *itemName = new QTableWidgetItem(str_address);
    ui->tableWidget->setVerticalHeaderItem(n_row, itemName);
    QTableWidgetItem *item_address = new QTableWidgetItem(str_address);
    ui->tableWidget->setItem(n_row, 0, item_address);
    QTableWidgetItem *item_bin_code = new QTableWidgetItem(str_bin_code);
    ui->tableWidget->setItem(n_row, 1, item_bin_code);
}

//qt界面输出汇编文件
void Widget::load_asm_file()
{

    QString filename = "/home/qiu/code_qt/assembler_qt5/test_assemble.s";
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "open file", tr("cannot open %1:\n %2").arg(filename).arg(file.errorString()));
    }

    QTextStream in(&file);
    ui->plainTextEdit->setPlainText(in.readAll());
}

//qt界面输出二进制文件
void Widget::load_bin_file()
{
    int line_num = 0;
    QString filename = "/home/qiu/code_qt/assembler_qt5/bin_code.bin";
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "open file", tr("cannot open %1:\n %2").arg(filename).arg(file.errorString()));
    }

    QTextStream in(&file);
    QString line;
    while (!in.atEnd())
    {
        line = in.readLine();
        line_num++;
        QString key = QString("%1").arg(line_num * 4, 8, 16, QLatin1Char('0')); //k为int型或char型都可
        set_table_row(line_num, "0X " + key, line);
    }
}

//启动汇编代码转二进制代码
int Widget::asm2bin_run()
{

    std::string asm_code;
    std::string bin_code;
    std::string input_file_name = "/home/qiu/code_qt/assembler_qt5/test_assemble.s";
    std::string output_file_name = "/home/qiu/code_qt/assembler_qt5/bin_code.bin";
    std::ifstream inf;
    std::ofstream outf(output_file_name);
    std::vector<string> v;
    char delimiter[10] = "\n ,\t;:";

    inf.open(input_file_name);
    find_loop(inf);
    inf.close();
    inf.open(input_file_name);
    pc = 0;

    int line_num = 0;
    while (getline(inf, asm_code))
    {
        split(asm_code, delimiter, 5, v);
        if (v.size() != 0 && v.size() != 1)
        {
            bin_code = asm2bin(v);
            outf << bin_code << endl;
            QString show_bin_code = QString::fromStdString(bin_code);
            int index = 0;
            for (index = 0; index < 36; index += 5)
                show_bin_code.insert(index, QString(" "));
            QString key;
            key = QString("%1").arg(pc - 4, 8, 16, QLatin1Char('0')); //k为int型或char型都可
            set_table_row(line_num, "0X " + key, show_bin_code);
            line_num++;
        }
        v.clear();
    }
    return 0;
}

//启动二进制文件转汇编
int Widget::bin2asm_run()
{
    std::string bin_code;
    std::string asm_code;
    std::string input_file_name = "/home/qiu/code_qt/assembler_qt5/bin_code.bin";
    std::string output_file_name = "/home/qiu/code_qt/assembler_qt5/bin2assemble.s";
    std::ifstream inf;
    std::ofstream outf;
    std::vector<string> v;
    inf.open(input_file_name);

    while (getline(inf, bin_code))
    {
        if (bin_code.length() != 0)
        {
            asm_code = bin2asm(bin_code);
            QString asm_code_show = QString::fromStdString(asm_code);
            ui->plainTextEdit->appendPlainText(asm_code_show);
        }
    }
    return 0;
}
