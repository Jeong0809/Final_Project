#include "ui_prescription.h"
#include <QSplitter>
#include <QListWidget>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QColorDialog>
#include <QMouseEvent>
#include <QMessageBox>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

#include "imagealbum.h"
#include "ui_imagealbum.h"
#include "imageview.h"
#include "imagescene.h"
#include "prescription.h"

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX:(n<0) ? 0 : n

ImageAlbum::ImageAlbum(QWidget *parent)
    : QWidget(parent), ui(new Ui::ImageAlbum)
{
    ui->setupUi(this);

    imageView = new ImageView(this);
    imageScene = new ImageScene(this);
    m_prescription = new Prescription(0);

    imageView->setGeometry(6, 6, 600, 600);
    imageView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    imageView->setDragMode(QGraphicsView::NoDrag);
    ui->gridLayout->addWidget(imageView);
    imageView->setAlignment(Qt::AlignCenter);


    connect(imageScene, SIGNAL(changed(const QList<QRectF> &)), imageScene, SLOT(updateSceneeee()));

    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));

    connect(ui->ZoomIn, SIGNAL(clicked()), this, SLOT(ZoomIn()));
    connect(ui->ZoomOut, SIGNAL(clicked()), this, SLOT(ZoomOut()));
    connect(ui->LeftRotate, SIGNAL(clicked()), this, SLOT(LeftRotate()));
    connect(ui->RightRotate, SIGNAL(clicked()), this, SLOT(RightRotate()));
    connect(ui->Brush, SIGNAL(clicked()), this, SLOT(BrushColor()));
    connect(ui->OrigImage, SIGNAL(clicked()), this, SLOT(OrigImage()));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(Brightness(int)));
    connect(ui->Sobel, SIGNAL(clicked()), this, SLOT(Sobel()));
    connect(ui->VReverse, SIGNAL(clicked()), this, SLOT(VReverse()));
    connect(ui->HReverse, SIGNAL(clicked()), this, SLOT(HReverse()));
    connect(ui->Blur, SIGNAL(clicked()), this, SLOT(Blur()));
    connect(ui->Reverse, SIGNAL(clicked()), this, SLOT(Reverse()));
    connect(ui->Contrast, SIGNAL(valueChanged(double)), this, SLOT(Contrast(double)));
    connect(ui->HistEqual, SIGNAL(clicked()), this, SLOT(HistEqual()));
    connect(ui->Sharpening, SIGNAL(clicked()), this, SLOT(Sharpening()));
    connect(ui->Thickness, SIGNAL(valueChanged(int)), this, SLOT(Thickness(int)));
    connect(ui->Lines, SIGNAL(clicked()), this, SLOT(Lines()));
    connect(ui->Freehand, SIGNAL(clicked()), this, SLOT(Freehand()));
    connect(ui->Triangle, SIGNAL(clicked()), this, SLOT(Triangle()));

    /*GraphicsView??? ??? ??????, ??? ??????, ????????? ??????????????? ???????????? ????????? ??????*/
    connect(this, SIGNAL(SendThickness(int)), imageScene, SLOT(ReceiveThickness(int)));
    connect(this, SIGNAL(SendBrushColor(QColor)), imageScene, SLOT(ReceiveBrushColor(QColor)));
    connect(this, SIGNAL(SendType(int)), imageScene, SLOT(ReceiveType(int)));

    //????????? ?????? ?????? ?????? ??? ????????? ???????????? ?????? ??????, ?????? ????????? ??????
    connect(this, SIGNAL(sendPrescription(QString, QString, QString, QString)),
            m_prescription, SLOT(receivePrescription(QString, QString, QString, QString)));

    //????????? ??????????????? ????????? ?????? ?????? ?????? ?????? ????????? ????????? ???????????? ?????? ??????
    connect(m_prescription, SIGNAL(sendPrescriptionFinish(QString)), this, SLOT(receivePrescriptionFinish(QString)));

    reloadImages();
}

ImageAlbum::~ImageAlbum()
{

}

void ImageAlbum::reloadImages()
{
    QDir dir(".");
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp" << "*.gif";
    QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    ui->listWidget->clear();
    for(int i = 0; i < fileInfoList.count(); i++) {
        QListWidgetItem* item = new QListWidgetItem(QIcon(fileInfoList.at(i).fileName()), NULL, ui->listWidget); //, QListWidgetItem::UserType);
        item->setStatusTip(fileInfoList.at(i).fileName());
        ui->listWidget->addItem(item);
    };
}

void ImageAlbum::Triangle()
{
    emit SendType(1);
}

void ImageAlbum::Lines()
{
    emit SendType(0);
}

void ImageAlbum::Freehand()
{
    emit SendType(2);
}

void ImageAlbum::Thickness(int value)
{
    penThickness = value;
    emit SendThickness(penThickness);
}

void ImageAlbum::BrushColor()
{
    paintColor = QColorDialog::getColor(paintColor, this);
    emit SendBrushColor(paintColor);
}

void ImageAlbum::ZoomIn()
{
    imageView->scale(1.2, 1.2);
}

void ImageAlbum::ZoomOut()
{
    imageView->scale(0.8, 0.8);
}

void ImageAlbum::LeftRotate()
{
    imageView->rotate(-45);
}

void ImageAlbum::RightRotate()
{
    imageView->rotate(45);
}

void ImageAlbum::OrigImage()
{
    imageView->resetTransform();
    imageScene->clear();
    imageScene->setBackgroundBrush(Qt::white);
    ui->horizontalSlider->setSliderPosition(0);
    ui->Contrast->setValue(1.0);

    *selectImage = QPixmap(orignal->statusTip()).toImage();
    imageScene->addPixmap(QPixmap(orignal->statusTip()).scaled(imageView->width(), imageView->height(),
                                                                               Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImageAlbum::selectItem(QListWidgetItem* item)
{
    orignal = item;
    origImage = new QImage(ui->listWidget->currentItem()->statusTip());
    selectImage = new QImage(ui->listWidget->currentItem()->statusTip());
    imageView->resetTransform();
    imageScene->clear();

    QSize size = imageView->viewport()->size();
    QGraphicsItem *i = imageScene->addPixmap(QPixmap(item->statusTip()).scaled(size, Qt::KeepAspectRatio));
    imageView->setAlignment(Qt::AlignCenter);
    imageScene->setSceneRect(i->sceneBoundingRect());
    imageView->setScene(imageScene);

    qDebug() << selectImage->width();
}

void ImageAlbum::VReverse()
{
    imageScene->clear();
    selectImage->mirror(true, false);

    QPixmap buf = QPixmap::fromImage(*selectImage);
//    imageView->setScene(imageScene);
    imageScene->addPixmap(buf.scaled(imageView->width(), imageView->height(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImageAlbum::HReverse()
{
    imageScene->clear();
    selectImage->mirror(false, true);

    QPixmap buf = QPixmap::fromImage(*selectImage);
//    imageView->setScene(imageScene);
    imageScene->addPixmap(buf.scaled(imageView->width(), imageView->height(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImageAlbum::Brightness(int value)
{
    imageScene->clear();
    QImage* image = selectImage;
    Mat out;

    *image = image->convertToFormat(QImage::Format_BGR888);

    cv::Mat in(
                image->height(),
                image->width(),
                CV_8UC3,    //uchar
                image->bits(),
                image->bytesPerLine());

    cvtColor(in, out, cv::COLOR_BGR2GRAY);
    out = out + value;

    QImage image_brightness(
                out.data,
                out.cols,
                out.rows,
                out.step,
                QImage::Format_Grayscale8);

    QPixmap buf = QPixmap::fromImage(image_brightness);
//    imageView->setScene(imageScene);
    imageScene->addPixmap(buf.scaled(imageView->width(), imageView->height(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));

//    *selectImage = image_brightness.convertToFormat(QImage::Format_BGR888);
}

void ImageAlbum::HistEqual()
{
    imageScene->clear();
    QImage* image = selectImage;

    *image = image->convertToFormat(QImage::Format_BGR888);

    cv::Mat in = cv::Mat(
                image->height(),
                image->width(),
                CV_8UC3,
                image->bits(),
                image->bytesPerLine());
    Mat out, Hist;
    cvtColor(in, out, cv::COLOR_BGR2GRAY);
    equalizeHist(out, Hist);

    QImage image_Histogram(
                Hist.data,
                Hist.cols,
                Hist.rows,
                Hist.step,
                QImage::Format_Grayscale8);

    QPixmap buf = QPixmap::fromImage(image_Histogram);
//    imageView->setScene(imageScene);
    imageScene->addPixmap(buf.scaled(imageView->width(), imageView->height(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
    *selectImage = image_Histogram.convertToFormat(QImage::Format_BGR888);
}

void ImageAlbum::Reverse()
{
    imageScene->clear();
    QImage* image = selectImage;

    *image = image->convertToFormat(QImage::Format_BGR888);
    cv::Mat in = cv::Mat(
                image->height(),
                image->width(),
                CV_8UC3,
                image->bits(),
                image->bytesPerLine());
    Mat out;
    cvtColor(in, out, cv::COLOR_BGR2GRAY);
    out = 255 - out;

    QImage image_Reverse(
                out.data,
                out.cols,
                out.rows,
                out.step,
                QImage::Format_Grayscale8);

    QPixmap buf = QPixmap::fromImage(image_Reverse);
//    imageView->setScene(imageScene);
    imageScene->addPixmap(buf.scaled(imageView->width(), imageView->height(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
    *selectImage = image_Reverse.convertToFormat(QImage::Format_BGR888);
}

void ImageAlbum::Contrast(double value)
{
    imageScene->clear();
    QImage* image = selectImage;

    *image = image->convertToFormat(QImage::Format_BGR888);
    cv::Mat in = cv::Mat(
                image->height(),
                image->width(),
                CV_8UC3,
                image->bits(),
                image->bytesPerLine());
    Mat out;
    cvtColor(in, out, cv::COLOR_BGR2GRAY);
    out = out * value ;

    QImage image_Contrast(
                out.data,
                out.cols,
                out.rows,
                out.step,
                QImage::Format_Grayscale8);

    QPixmap buf = QPixmap::fromImage(image_Contrast);
//    imageView->setScene(imageScene);
    imageScene->addPixmap(buf.scaled(imageView->width(), imageView->height(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
//    *selectImage = image_Contrast.convertToFormat(QImage::Format_BGR888);
}

void ImageAlbum::Sobel()
{
    imageScene->clear();
    QImage *image = new QImage(ui->listWidget->currentItem()->statusTip());

    int scale = 1;
    int delta = 0;
    int ddepth = CV_32FC1;

    *image = image->convertToFormat(QImage::Format_RGB888);
    Mat in = cv::Mat(
                image->height(),
                image->width(),
                CV_8UC3,
                image->bits(),
                image->bytesPerLine());

    Mat out, grad_x, grad_y;

    //color->gray??? ?????? ??????
    cvtColor(in, out, cv::COLOR_BGR2GRAY);
    cv::Sobel(out, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_CONSTANT);
    cv::Sobel(out, grad_y, ddepth, 1, 0, 3, scale, delta, cv::BORDER_CONSTANT);

    Mat float_mag, mag;
    magnitude(grad_x, grad_y, float_mag);
    float_mag.convertTo(mag, CV_8UC1);

    QImage image_sobel(
                mag.data,
                mag.cols,
                mag.rows,
                mag.step,
                QImage::Format_Grayscale8);

    QPixmap buf = QPixmap::fromImage(image_sobel);

//    imageView->setScene(imageScene);
    imageScene->addPixmap(buf.scaled(imageView->width(), imageView->height(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImageAlbum::Blur()
{
    imageScene->clear();
    QImage* image = selectImage;

    //OpenCV?????? ????????? ????????? ?????? ????????? Matrix ???????????? ????????? ????????? ????????? ?????? ????????? ????????????.
    *image = image->convertToFormat(QImage::Format_BGR888);
    cv::Mat mat = cv::Mat(
                image->height(),
                image->width(),
                CV_8UC3,
                image->bits(),
                image->bytesPerLine());

    // OpenCV??? ?????? blur() ????????? ???????????????. imgproc ?????? ?????? ?????? ???????????????.
    // ??? ?????? ????????? ?????? ?????????, ??? ?????? ????????? blur ??? ?????????, ??? ?????? ????????? ????????? ???????????????. ???????????? ?????????
    // OpenCV?????? ????????? ????????? ?????? ?????? ?????? ?????? ?????? ????????? ???????????? ?????? ???????????? ????????? ???????????????.

    cv::Mat tmp;
    blur(mat, tmp, cv::Size(18, 18));
    mat = tmp;

    //?????? ??? ??? ?????? ????????? ???????????? ??????????????????.
    QImage image_Blur(
                mat.data,
                mat.cols,
                mat.rows,
                mat.step,
                QImage::Format_RGB888);

    QPixmap buf = QPixmap::fromImage(image_Blur);

//    imageView->setScene(imageScene);
    imageScene->addPixmap(buf.scaled(imageView->width(), imageView->height(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
    *selectImage = image_Blur.convertToFormat(QImage::Format_BGR888);
}

void ImageAlbum::Sharpening()
{
    imageScene->clear();
    QImage* image = selectImage;

    //OpenCV?????? ????????? ????????? ?????? ????????? Matrix ???????????? ????????? ????????? ????????? ?????? ????????? ????????????.
    *image = image->convertToFormat(QImage::Format_BGR888);
    cv::Mat mat = cv::Mat(
                image->height(),
                image->width(),
                CV_8UC3,
                image->bits(),
                image->bytesPerLine());

    Mat blurred, out;
    cvtColor(mat, out, cv::COLOR_BGR2GRAY);
    GaussianBlur(out, blurred, Size(), 8);

    float alpha = 1.f;
    Mat dst = (1 + alpha) * out - alpha * blurred;

    QImage image_Sharpen(
                dst.data,
                dst.cols,
                dst.rows,
                dst.step,
                QImage::Format_Grayscale8);

    QPixmap buf = QPixmap::fromImage(image_Sharpen);

//    imageView->setScene(imageScene);
    imageScene->addPixmap(buf.scaled(imageView->width(), imageView->height(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
    *selectImage = image_Sharpen.convertToFormat(QImage::Format_BGR888);
}


void ImageAlbum::on_Prescription_clicked()
{
    //????????? ?????? ?????? ?????? ??? ?????? ??????, ?????? ????????? ????????? ???????????? ??????
    emit sendPrescription(DoctorID, DoctorName, PatientID, PatientName);
    m_prescription->show();
}

void ImageAlbum::receivePatientInfo(QString ID, QString Name)
{
    PatientID = ID;
    PatientName = Name;
}

void ImageAlbum::receiveDoctorInfo(QString ID, QString Name)
{
    DoctorID = ID;
    DoctorName = Name;
}

void ImageAlbum::receivePrescriptionFinish(QString Data)
{
    emit sendPrescriptiontoServer(Data);
    m_prescription->close();
}

void ImageAlbum::on_EndTreatment_clicked()
{
    QString Data = "VTF<CR>" + PatientID + "<CR>" + PatientName;
    emit sendEndTreatment(Data);
}

