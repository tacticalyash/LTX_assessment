#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <vector>
#include <algorithm>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include "utility.h"
#include <QVBoxLayout>
#include <QTableView>
#include <QStandardItem>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool parseAndCheck(const QString& csv_data);
private slots:
    void on_upload_csv_button_clicked();

    void on_pushButton_sort_data_clicked();

    void on_pushButton_write_JSON_clicked();

    void on_pushButton_load_db_clicked();

    void on_pushButton_display_table_clicked();

private:
    Ui::MainWindow *ui;
     std::vector<flight_data> flightDataVec;
    QString json_data;
};
#endif // MAINWINDOW_H
