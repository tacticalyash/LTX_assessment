#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_upload_csv_button_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open CSV File"), QString(), tr("CSV Files (*.csv)"));
    if (filePath.isEmpty()) {
        QMessageBox::warning(nullptr, "Error", "No file selected");
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, "Error", "Failed to open file");
        return;
    }

    QString csv_data = QString::fromUtf8(file.readAll());
    file.close();


     flightDataVec = utility::parse_to_flight_data(csv_data);

    if (!flightDataVec.empty()) {
        QMessageBox::information(nullptr, "INFO", "File Uploaded and Sorted!");
    } else {
        QMessageBox::warning(nullptr, "Error", "Failed to parse CSV data");
    }


}


void MainWindow::on_pushButton_sort_data_clicked()
{
    if (flightDataVec.empty()) {
        QMessageBox::warning(nullptr, "Error", "No flight data available to sort");
        return;
    }


    utility::sort_flightdata_by_time(flightDataVec);


    QMessageBox::information(nullptr, "INFO", "Flight data sorted by time");
}




bool MainWindow::parseAndCheck(const QString& csv_data)
{

    flightDataVec = utility::parse_to_flight_data(csv_data);


    if (!flightDataVec.empty()) {
        return true;
    } else {
        QMessageBox::warning(nullptr, "Error", "Failed to parse CSV data");
        return false;
    }
}

void MainWindow::on_pushButton_write_JSON_clicked()
{
    if (flightDataVec.empty()) {
        QMessageBox::warning(nullptr, "Error", "No flight data available to export");
        return;
    }


     json_data = utility::export_to_json(flightDataVec);

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save JSON File"), QString(), tr("JSON Files (*.json)"));
    if (filePath.isEmpty()) {
        QMessageBox::warning(nullptr, "Error", "No file selected");
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr, "Error", "Failed to open file for writing");
        return;
    }

    QTextStream out(&file);
    out << json_data;
    file.close();


    QMessageBox::information(nullptr, "INFO", "Flight data exported to JSON file");


}


void MainWindow::on_pushButton_load_db_clicked()
{

    QString dbName = QFileDialog::getSaveFileName(this, tr("Save DB File"), "", tr("SQLite Database Files (*.db)"));
    if (dbName.isEmpty()) {

        return;
    }

    if (!QFileInfo::exists(dbName)) {
        // Create an empty database file
        QFile file(dbName);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to create database file";
            QMessageBox::critical(this, tr("Error"), tr("Failed to create database file!"));
            return;
        }
        QMessageBox::information(this, tr("Info"), tr("Database file created!"));
        file.close();
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("The selected file already exists. Please choose a different location."));
        return;
    }


    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        qDebug() << "Database connection failed";
        return;
    }


    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS Data ("
               "Time TEXT PRIMARY KEY,"
               "Latitude REAL,"
               "Longitude REAL,"
               "Course INTEGER,"
               "Kts INTEGER,"
               "Mph INTEGER,"
               "Altitude INTEGER,"
               "Reporting_facility TEXT)");

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json_data.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << parseError.errorString();
        return;
    }

    if (!jsonDoc.isArray()) {
        qDebug() << "JSON data is not an array";
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();
    for (const auto& item : jsonArray) {
        if (!item.isObject()) {
            qDebug() << "JSON array Invalid!";
            continue;
        }

        QJsonObject obj = item.toObject();
        QString Time = obj["Time"].toString();
        double Latitude = obj["Latitude"].toDouble();
        double Longitude = obj["Longitude"].toDouble();
        int Course = obj["Course"].toInt();
        int Kts = obj["kts"].toInt();
        int Mph = obj["mph"].toInt();
        int Altitude = obj["Altitude"].toInt();
        QString Reporting_facility = obj["Reporting Facility"].toString();

        QString insertQuery = QString("INSERT INTO Data VALUES ('%1', %2, %3, %4, %5, %6, %7, '%8')")
                                  .arg(Time).arg(Latitude).arg(Longitude).arg(Course).arg(Kts).arg(Mph).arg(Altitude).arg(Reporting_facility);
        if (!query.exec(insertQuery)) {
            qDebug() << "Failed to insert data:" << query.lastError().text();
        }
    }




    QSqlQuery selectQuery("SELECT * FROM Data");
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(8);


    QStringList headers;
    headers << "Time" << "Latitude" << "Longitude" << "Course" << "Kts" << "Mph" << "Altitude" << "Reporting Facility";
    model->setHorizontalHeaderLabels(headers);

    int row = 0;
    while (selectQuery.next()) {
        for (int col = 0; col < 8; ++col) {
            QString value = selectQuery.value(col).toString();
            model->setItem(row, col, new QStandardItem(value));
        }
        ++row;
    }

    db.close();


    QMainWindow *tableViewWindow = new QMainWindow;
    QTableView *tableView = new QTableView(tableViewWindow);
    tableView->setModel(model);
    tableViewWindow->setCentralWidget(tableView);
    tableViewWindow->setWindowTitle("Table data");

    tableViewWindow->resize(900, 700);
    tableViewWindow->show();
}




