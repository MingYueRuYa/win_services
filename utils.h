#pragma once


#include <Windows.h>
#include <wtsapi32.h>
#include <userenv.h>
#include <tlhelp32.h>
#include <string>
#include <QDesktopWidget>

#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "Wtsapi32.lib")

using std::wstring;
class QWidget;
class QDesktopWidget;

DWORD GetActiveSessionID();

const wstring GetAppPath();

BOOL TriggerAppExecute(std::wstring wstrCmdLine /*, INT32& n32ExitResult*/);
 
// �ƶ��������½�
void move_window(QWidget* widget);