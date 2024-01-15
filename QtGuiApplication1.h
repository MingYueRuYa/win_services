#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplication1.h"
#include "msg_info.h"

class QtGuiApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtGuiApplication1(const QString &config, QWidget *parent = Q_NULLPTR);
 ~QtGuiApplication1();
    void update_ui();

private:
  bool read_config(const QString &config);

protected:
  bool nativeEvent(const QByteArray &eventType, void *message, long *result);

protected slots:
  void on_btn_close_clicked();
  void on_btn_open_log_clicked();
  void on_btn_open_log_dir_clicked();
  void on_btn_re_update_clicked();


private:
    Ui::QtGuiApplication1Class ui;
    MsgInfo msg_info;
};
