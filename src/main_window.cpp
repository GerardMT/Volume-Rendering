
#include "histogram_widget.h"
#include "main_window.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    HistogramWidget *histogram = new HistogramWidget();
    histogram->setObjectName("histogram");
    histogram->setMinimumWidth(200);
    histogram->setMaximumWidth(200);

    findChild<QVBoxLayout *>("histogramContainer")->addWidget(histogram);
}

void MainWindow::on_actionOpen_triggered()
{
    ui.glwidget->pause();

    QString filename = QFileDialog::getExistingDirectory(this, "Choose a directory.", ".", QFileDialog::Option::ShowDirsOnly);
    if (!filename.isNull()) {
        try {
            ui.glwidget->loadVolumeData(filename.toStdString());
        } catch (...) {
            QMessageBox::warning(this, tr("Error"), tr("The selected volume could not be opened."));
        }
    }

    ui.glwidget->play();
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}
