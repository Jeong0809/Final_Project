#include "loginform.h"
#include "ui_loginform.h"

#include <QMessageBox>

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    QPixmap pixmap("./Icon/login.png");
    ui->Icon->setPixmap(pixmap.scaled(200, 200));
}

LoginForm::~LoginForm()
{
    delete ui;
}

//로그인 창에서 입력한 의사 ID와 의사명이 DB 내에 존재하는 정보인지 확인
void LoginForm::receiveLogInCheck(QString data)
{
    Check = data;

    //의사 ID, 의사 명으로 로그인 되었을 때
    if(Check == "True")
    {
        QString DoctorID = ui->DoctorID->text();
        QString DoctorName = ui->DoctorName->text();
        emit LogInSuccess(DoctorID, DoctorName);
    }

    //의사 ID, 의사 명으로 로그인 안되었을 때
    else if(Check == "False")
    {
        QMessageBox:: critical(this, "경고", "아이디/이름을 잘못 입력했습니다");
        ui->DoctorID->clear();
        ui->DoctorName->clear();
    }
}

//로그인 버튼 클릭 시 서버로 해당 의사 정보가 일치하는지 확인하기 위해 시그널 전송
void LoginForm::on_LogIn_clicked()
{
    QString DoctorID = ui->DoctorID->text();
    QString DoctorName = ui->DoctorName->text();

    if(DoctorID == "" || DoctorName == "")
    {
        QMessageBox::critical(this, "경고", "아이디/이름을 입력하세요");
    }

    QString Data = "SEN^VLG<CR>" + DoctorID + "<CR>" + DoctorName;
    //로그인 페이지에서 입력한 의사 ID, 의사 명을 서버에서 체크하기 위해 데이터 전송
    emit SendDoctorCheck(Data);
}

