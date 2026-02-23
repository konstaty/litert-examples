#ifndef __LITERT_EXAMPLES_GUI__
#define __LITERT_EXAMPLES_GUI__


#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QPixmap>
#include <QMessageBox>

#include <opencv2/opencv.hpp>

#include "core/core.hpp"


using namespace std;


QImage matToQImage(const cv::Mat &mat);


class ScalableLabel: public QLabel
{
    Q_OBJECT
public:
    explicit ScalableLabel(QWidget *parent = nullptr) : QLabel(parent)
    {
        setAlignment(Qt::AlignCenter);
        setMinimumSize(64, 64);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    }

    void setFullPixmap(const QPixmap &pix)
    {
        original = pix;
        rescale();
    }

protected:
    void resizeEvent(QResizeEvent *e) override
    {
        QLabel::resizeEvent(e);
        rescale();
    }

private:
    void rescale()
    {
        if( original.isNull() ){ return; };

        QPixmap scaled = original.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        if( original.width() <= scaled.width() )
        {
            QLabel::setPixmap(original);
        }
        else
        {
            QLabel::setPixmap(scaled);
        }
    }

    QPixmap original;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Model &model, QWidget *parent = nullptr): QMainWindow(parent), model(model)
    {
        setWindowTitle("LiteRT Viewer");
        resize(900, 600);

        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(20);

        // Horizontal layout for the two images side-by-side
        QHBoxLayout *imagesLayout = new QHBoxLayout();
        imagesLayout->setSpacing(30);

        // Create the combo box (dropdown)
        QHBoxLayout *controlLayout = new QHBoxLayout();
        controlLayout->setSpacing(20);

        combo = new QComboBox(centralWidget);
        for( auto const & model_opt : model.models() )
        {
            combo->addItem(model_opt.c_str());
        }

        combo->setCurrentIndex(0);
        controlLayout->addWidget(combo);

        mainLayout->addLayout(controlLayout);

        // First image
        label1 = new ScalableLabel(centralWidget);
        //QMessageBox::warning(this, "Error", "Failed to load image1.jpg");
        label1->setScaledContents(false);
        label1->setAlignment(Qt::AlignCenter);

        // Second image
        label2 = new ScalableLabel(centralWidget);
        //QMessageBox::warning(this, "Error", "Failed to load image2.jpg");
        label2->setScaledContents(false);
        label2->setAlignment(Qt::AlignCenter);

        imagesLayout->addWidget(label1);
        imagesLayout->addWidget(label2);

        mainLayout->addLayout(imagesLayout);

        // Buttons area (horizontal)
        QHBoxLayout *buttonsLayout = new QHBoxLayout();
        buttonsLayout->setSpacing(40);

        QPushButton *btn1 = new QPushButton("Process", centralWidget);
        QPushButton *btn2 = new QPushButton("Next", centralWidget);

        connect(btn1, &QPushButton::clicked, this, &MainWindow::process);
        connect(btn2, &QPushButton::clicked, this, &MainWindow::next);

        //buttonsLayout->addWidget(btn1);
        //buttonsLayout->addWidget(btn2);
        //buttonsLayout->addStretch(1);

        controlLayout->addWidget(btn1);
        controlLayout->addWidget(btn2);

        //mainLayout->addLayout(buttonsLayout);

        controlLayout->addStretch(1);

        setCentralWidget(centralWidget);

        label1->setVisible(true);
        label2->setVisible(true);
    }

private slots:
    void process()
    {
        auto images = model.process(combo->currentText().toStdString(), model.image());
        QImage qimg1 = matToQImage(images[0]);
        label1->setFullPixmap(QPixmap::fromImage(qimg1));

        QImage qimg2 = matToQImage(images[1]);
        label2->setFullPixmap(QPixmap::fromImage(qimg2));
    }

    void next()
    {
        auto images = model.process(combo->currentText().toStdString(), model.next_image());
        QImage qimg1 = matToQImage(images[0]);
        label1->setFullPixmap(QPixmap::fromImage(qimg1));

        QImage qimg2 = matToQImage(images[1]);
        label2->setFullPixmap(QPixmap::fromImage(qimg2));
    }

private:
    ScalableLabel *label1 = nullptr;
    ScalableLabel *label2 = nullptr;
    QComboBox *combo = nullptr;
    Model &model;
};


#endif  // __LITERT_EXAMPLES_GUI__