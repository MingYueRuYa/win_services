#include "QtGuiApplication1.h"
#include "msg_info.h"
#include "utils.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDesktopWidget>

#include <Windows.h>
#include <string>


int main(int argc, char *argv[])
{
    OutputDebugStringW(L"start here 00");
    QApplication app(argc, argv);
    QStringList args = app.arguments();
    if (args.length() > 1 && args[1] == "start_by_task")
    {
      args.removeAt(1);
      OutputDebugStringW(L"start here 01");
      bool result = TriggerAppExecute(args.join(" ").toStdWString());
      // 如果成功 直接返回，失败
      if (result) {
        return 0; 
      }
    }
    OutputDebugStringW(L"start here");
    
    QString config_path = args.length() > 1 ? args[1]: QCoreApplication::applicationDirPath() + "/config.json";
    QtGuiApplication1 w(config_path);
    w.show();
    w.setWindowFlags(Qt::WindowStaysOnTopHint);
    w.update_ui();
    move_window(&w);
    return app.exec();
}
