/*프로그램명 : ImageViewerSW
파일명 : prescription.cpp
설명 : 서버에서 받아온 환자 정보들을 treeWidget과 tableWidget을 통해 대기 환자 리스트와 환자 정보로 구성하였고
진료 시작 버튼, 촬영 의뢰 버튼을 통해 타 SW들과 연동되어 있는 클래스
작성자 : 이정연
최종 수정 날짜 : 2023.02.11*/


#include "prescription.h"
#include "ui_prescription.h"

#include <QDate>
#include <QMessageBox>
#include <QFileDialog>

Prescription::Prescription(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Prescription)
{
    ui->setupUi(this);
    this->setFixedSize(592, 714);
}

Prescription::~Prescription()
{
    delete ui;
}

//처방전 버튼 클릭 시 의사 정보, 환자 정보가 이미 작성된 채로 띄워지도록 설정
void Prescription::receivePrescription(QString DID, QString DName, QString PID, QString PName, QString PSex)
{
    //오늘 날짜를 자동적으로 입력되게 하기 위한 변수 (형식 : 2023-02-28)
    CurrentDate = QDate::currentDate().toString("yyyy-MM-dd");

    DoctorID = DID;
    DoctorName = DName;
    PatientID = PID;
    PatientName = PName;
    PatientSex = PSex;

    ui->PatientName->setReadOnly(true);
    ui->PatientID->setReadOnly(true);
    ui->DoctorName->setReadOnly(true);
    ui->Date->setReadOnly(true);

    ui->PatientName->setText(PatientName);
    ui->PatientID->setText(PatientID);
    ui->DoctorName->setText(DoctorName);
    ui->Date->setText(CurrentDate);
    ui->PatientSex->setText(PatientSex);
}

//처방전 작성 완료 버튼 클릭 시 처방전에 작성된 내용을 서버로 전송
void Prescription::on_Finish_clicked()
{
    QString DID = DoctorID;
    QString DName = DoctorName;
    QString PID = PatientID;
    QString PName = PatientName;
    QString Date = CurrentDate;
    QString Context = ui->textEdit->toPlainText();

    //VNT<CR>PID<CR>환자이름|의사번호|의사이름|진료날짜|진료내용(처방내용)
    QString Data = "SEN^VNT<CR>" + PID + "<CR>" + PName + "|" + DID + "|" + DName + "|" + Date + "|" + Context;

    //처방전 작성 완료 시 서버로 해당 데이터를 전송
    emit sendPrescriptionFinish(Data);

}

//처방전 작성이 완료되면 DB에 저장 여부를 서버로부터 수신
void Prescription::receivePrescriptionYesOrNot(QString signal)
{
    if(signal == "ACK")
    {
        QMessageBox::information(NULL, "저장 완료", "처방전 작성이 완료되었습니다.");
        ui->PatientName->clear();
        ui->PatientID->clear();
        ui->textEdit->clear();
    }

    //signal이 ERR를 받으면 DB에 처방전이 저장되지 않았으므로 로컬에 저장
    else {
        QMessageBox msgBox;
        msgBox.setText("처방전이 저장되지 않았습니다."
                       "이미지로 저장하시겠습니까?");
//        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        switch (ret) {
          case QMessageBox::Yes: {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                            tr("Images (*.png *.xpm *.jpg)"));
            if(fileName.isNull())
                return;

            QPixmap buf = this->grab();
            QImage image = buf.toImage();
            image.save(fileName);
        }

              break;
          case QMessageBox::No:
              break;

          default:
              break;
        }
    }
}

