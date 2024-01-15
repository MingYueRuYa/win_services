#include "QtGuiApplication1.h"
#include "msg_info.h"
#include "debughelper.h"
#include "json.hpp"
#include "stringhelper.h"
#include "utils.h"

#include <fstream>
#include <Windows.h>
#include <QtWidgets/QStyle>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices> 
#include <WtsApi32.h>

using json = nlohmann::json;
using XIBAO::DebugHelper;

QtGuiApplication1::QtGuiApplication1(const QString &config, QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	  bool result = WTSRegisterSessionNotification((HWND)this->winId(),
                                                 NOTIFY_FOR_ALL_SESSIONS);
		read_config(config);
}

QtGuiApplication1::~QtGuiApplication1() {
  WTSUnRegisterSessionNotification((HWND)this->winId());
}

void QtGuiApplication1::update_ui()
{
	QString result_str = msg_info.update_result ? "update code successful" : "update code error, please read log.";
  ui.lbl_info->setText(result_str);
	ui.lbl_info->setProperty("error", !msg_info.update_result);
	ui.lbl_info->style()->unpolish(ui.lbl_info);
	ui.lbl_info->style()->polish(ui.lbl_info);
  ui.lbl_update_path->setText(msg_info.code_top_dir);
}

bool QtGuiApplication1::read_config(const QString &config)
{
	std::ifstream input(config.toStdString().c_str());
	if (!input.is_open())
	{
		DebugHelper::OutputDebugString(XIBAO::StringHelper::to_string(L"没有找到配置config.json文件"));
		return false;
	}

		json j;
		input >> j;
		string str;

		double version = 0.1;
		if (j.contains("version"))
		{
			version = j["version"].get<double>();
		}
		if (j.contains("update_result"))
		{
			msg_info.update_result = j["update_result"].get<bool>();
		}

		if (j.contains("app_start_dir"))
		{
			str = j["app_start_dir"].get<string>();
			msg_info.app_start_dir = QString::fromStdWString(XIBAO::StringHelper::to_wstring(str));
		}

		if (j.contains("code_top_dir"))
		{
			str = j["code_top_dir"].get<string>();
			QString q_str = QString::fromStdWString(XIBAO::StringHelper::to_wstring(str));
			QDir dir(q_str);
			msg_info.code_top_dir = dir.absolutePath();
		}

		if (j.contains("app_log_path"))
		{
			str = j["app_log_path"].get<string>();
			msg_info.app_log_path = QString::fromStdWString(XIBAO::StringHelper::to_wstring(str));
		}

		if (j.contains("update_script_path"))
		{
			str = j["update_script_path"].get<string>();
			msg_info.update_script_path = QString::fromStdWString(XIBAO::StringHelper::to_wstring(str));
		}

	return true;
}

bool QtGuiApplication1::nativeEvent(const QByteArray& eventType, void* message,
	long* result) {

	PMSG msg = (PMSG)message;

  if (msg->message == WM_WTSSESSION_CHANGE) {
      // 处理用户登录和注销消息
      switch (msg->wParam) {
        case WTS_SESSION_LOGON:
        case WTS_SESSION_UNLOCK:
          // 用户注销
          OutputDebugStringW(L"检测到用户登录");
          TriggerAppExecute(L"");
          break;
      }
	}
	
	return false;
}

void QtGuiApplication1::on_btn_close_clicked()
{
	this->close();
 }

void QtGuiApplication1::on_btn_open_log_clicked()
{
	ShellExecuteW(NULL, L"open", msg_info.app_log_path.toStdWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void QtGuiApplication1::on_btn_open_log_dir_clicked()
{
	ShellExecuteW(NULL, L"open", msg_info.app_start_dir.toStdWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void QtGuiApplication1::on_btn_re_update_clicked()
{
	ShellExecuteW(NULL, L"open", msg_info.update_script_path.toStdWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
}
