#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_table();
    create_actions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    asm2bin_run();
}

void MainWindow::on_pushButton_2_clicked()
{
    bin2asm_run();
}


void MainWindow::create_actions()
{
// open file
    QToolBar *fileToolBar = addToolBar(tr("File"));
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    const QIcon open_icon = QIcon(":/icon/open.png");
    QAction *open_action = new QAction(open_icon, tr("&Open..."),this);
    open_action->setShortcut(QKeySequence::Open);
    open_action->setStatusTip(tr("Plz open an existing file."));
    connect(open_action, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(open_action);
    fileToolBar->addAction(open_action);

// save file
    fileToolBar = addToolBar(tr("Save"));
    fileMenu = menuBar()->addMenu(tr("Save"));
    const QIcon save_icon = QIcon(":/icon/save.png");
    QAction *save_action = new QAction(save_icon, tr("&Save..."), this);
    save_action->setShortcut(QKeySequence::Save);
    connect(save_action, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(save_action);
    fileToolBar->addAction(save_action);
}

// judge the save state
bool MainWindow::save()
{
    if (cur_file.isEmpty()) {
        return save_as();
    } else {
        return save_file(cur_file);
    }
}

// save file function
bool MainWindow::save_file(QString &file_name)
{
    QFile file(file_name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),tr("Cannot write file %1:\n%2.").arg(QDir::toNativeSeparators(file_name),file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

// check the choice
QString asm_code_choice = "assembly code";
QString bin_code_choice = "machine code";
if(ui->comboBox->currentText() == asm_code_choice)
{
    out << ui->plainTextEdit->toPlainText();
}else if (ui->comboBox->currentText() == bin_code_choice)
{
    for(int i = 0; i < line_num; i++)
    {
            out << ui->tableWidget->item(i, 1)->text();
            out << QString("\n");
    }
}

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    set_cur_file(file_name);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}


// save as function
bool MainWindow::save_as()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return save_file(dialog.selectedFiles().first());
}

// set the param of the file
void MainWindow::set_cur_file(QString &file_name)
{
    cur_file = file_name;
    ui->plainTextEdit->document()->setModified(false);
//    textEdit->document()->setModified(false);
    setWindowModified(false);
    QString show_name = cur_file;
    if (cur_file.isEmpty())
        show_name = "untitled.txt";
    setWindowFilePath(show_name);
}

// open-action function
void MainWindow::open()
{
    cur_file_path = QFileDialog::getOpenFileName(this,
            tr("Open File"), ".");
    if(cur_file_path.length() == 0)
        QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    else
    {
        QMessageBox::information(NULL, tr("Path"), tr("You selected ") + cur_file_path);
    }

    QString asm_code_choice = "assembly code";
    QString bin_code_choice = "machine code";
    if(ui->comboBox_2->currentText() == asm_code_choice)
    {

        load_asm_file(cur_file_path);
    }else if (ui->comboBox_2->currentText() == bin_code_choice)
    {
        load_bin_file(cur_file_path);
    }
    return ;
}

void MainWindow::init_table()
{
    ui->tableWidget->setRowCount(1000);
    ui->tableWidget->setColumnCount(2);
    QStringList listHeaders;
    listHeaders << tr("Address") << tr("Machine Language");
    ui->tableWidget->setHorizontalHeaderLabels(listHeaders);
    ui->tableWidget->QTableView::verticalHeader()->hide();
    ui->tableWidget->setColumnWidth(0, 150);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    return;
}

void MainWindow::set_table_row(int n_row, QString str_address, QString str_bin_code)
{
    QTableWidgetItem *itemName = new QTableWidgetItem(str_address);
    ui->tableWidget->setVerticalHeaderItem(n_row, itemName);
    QTableWidgetItem *item_address = new QTableWidgetItem(str_address);
    ui->tableWidget->setItem(n_row, 0, item_address);
    QTableWidgetItem *item_bin_code = new QTableWidgetItem(str_bin_code);
    ui->tableWidget->setItem(n_row, 1, item_bin_code);
}

//qt界面输出汇编文件

void MainWindow::load_asm_file(QString filename)
{

//    QString filename = "/home/qiu/code_qt/assembler_qt5/test_assemble.s";
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "open file", tr("cannot open %1:\n %2").arg(filename).arg(file.errorString()));
    }
    QTextStream in(&file);
    ui->plainTextEdit->setPlainText(in.readAll());
}


//qt界面输出二进制文件

void MainWindow::load_bin_file(QString file_name)
{
    QFile file(file_name);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "open file", tr("cannot open %1:\n %2").arg(file_name).arg(file.errorString()));
    }

    QTextStream in(&file);
    QString line;
    int bin_line_num = 0;
    while (!in.atEnd())
    {
        line = in.readLine();
        QString key = QString("%1").arg(bin_line_num * 4, 8, 16, QLatin1Char('0'));
        //k为int型或char型都可
        set_table_row(bin_line_num, "0X " + key, line);
        bin_line_num++;
    }
}

// assembly lanuage ==> machine language
int MainWindow::asm2bin_run()
{
    std::string asm_code;
    std::string bin_code;
    std::ifstream inf;
    std::vector<string> v;
    char delimiter[10] = "\n ,\t;:";
    inf.open(cur_file_path.toStdString());
    find_loop(inf);
    inf.close();
    pc = 0;
    line_num = 0;
    inf.open(cur_file_path.toStdString());
    while (getline(inf, asm_code))
    {
        split(asm_code, delimiter, 5, v);
        if (v.size() != 0 && v.size() != 1)
        {
            bin_code = asm2bin(v);
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


//启动二进制文件==>汇编
int MainWindow::bin2asm_run()
{
    std::string bin_code;
    std::string asm_code;
    std::ifstream inf;
//    pc = 0;
    inf.open(cur_file_path.toStdString());

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
