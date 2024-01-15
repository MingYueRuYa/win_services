#include "utils.h"

#include <QAPplication>

DWORD GetActiveSessionID() {
  DWORD dwSessionId = 0;
  PWTS_SESSION_INFO pSessionInfo = NULL;
  DWORD dwCount = 0;

  WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo,
                       &dwCount);

  for (DWORD i = 0; i < dwCount; i++) {
    WTS_SESSION_INFO si = pSessionInfo[i];
    if (WTSActive == si.State) {
      dwSessionId = si.SessionId;
      break;
    }
  }

  WTSFreeMemory(pSessionInfo);
  return dwSessionId;
}

const wstring GetAppPath() {
  static wstring szRootPath;
  if (szRootPath == (L"")) {
    TCHAR szPath[_MAX_PATH];
    GetModuleFileName(NULL, szPath, _MAX_PATH);
    wstring strFullPath = szPath;
    szRootPath = strFullPath;
  }
  return szRootPath;
}

BOOL TriggerAppExecute(std::wstring wstrCmdLine /*, INT32& n32ExitResult*/) {
  DWORD dwProcesses = 0;
  BOOL bResult = FALSE;

  DWORD dwSid = GetActiveSessionID();

  DWORD dwRet = 0;
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
  HANDLE hProcess = NULL, hPToken = NULL, hUserTokenDup = NULL;
  if (!WTSQueryUserToken(dwSid, &hPToken)) {
    PROCESSENTRY32 procEntry;
    DWORD dwPid = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
      OutputDebugStringW(L"01");

      return FALSE;
    }

    procEntry.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnap, &procEntry)) {
      do {
        if (_wcsicmp(procEntry.szExeFile, (L"explorer.exe")) == 0) {
          DWORD exeSessionId = 0;
          if (ProcessIdToSessionId(procEntry.th32ProcessID, &exeSessionId) &&
              exeSessionId == dwSid) {
            dwPid = procEntry.th32ProcessID;
            break;
          }
        }

      } while (Process32Next(hSnap, &procEntry));
    }
    CloseHandle(hSnap);

    // explorer进程不存在
    if (dwPid == 0) {
      OutputDebugStringW(L"02");
      return FALSE;
    }

    hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
    if (hProcess == NULL) {
      OutputDebugStringW(L"03");
      return FALSE;
    }

    if (!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS_P, &hPToken)) {
      OutputDebugStringW(L"04");
      CloseHandle(hProcess);
      return FALSE;
    }
  }

  if (hPToken == NULL) {
    OutputDebugStringW(L"05");
    return FALSE;
  }

  TOKEN_LINKED_TOKEN admin;
  bResult =
      GetTokenInformation(hPToken, (TOKEN_INFORMATION_CLASS)TokenLinkedToken,
                          &admin, sizeof(TOKEN_LINKED_TOKEN), &dwRet);

  if (!bResult)  // vista 以前版本不支持TokenLinkedToken
  {
    TOKEN_PRIVILEGES tp;
    LUID luid;
    if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
      tp.PrivilegeCount = 1;
      tp.Privileges[0].Luid = luid;
      tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    //复制token
    DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification,
                     TokenPrimary, &hUserTokenDup);
  } else {
    hUserTokenDup = admin.LinkedToken;
  }

  LPVOID pEnv = NULL;
  DWORD dwCreationFlags = CREATE_PRESERVE_CODE_AUTHZ_LEVEL;

  // hUserTokenDup为当前登陆用户的令牌
  if (CreateEnvironmentBlock(&pEnv, hUserTokenDup, TRUE)) {
    //如果传递了环境变量参数，CreateProcessAsUser的
    // dwCreationFlags参数需要加上CREATE_UNICODE_ENVIRONMENT，
    //这是MSDN明确说明的
    dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
  } else {
    //环境变量创建失败仍然可以创建进程，
    //但会影响到后面的进程获取环境变量内容
    pEnv = NULL;
  }

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_SHOW;
  ZeroMemory(&pi, sizeof(pi));

  bResult = CreateProcessAsUser(hUserTokenDup,         // client's access token
                                GetAppPath().c_str(),  // file to execute
                                (LPTSTR)wstrCmdLine.c_str(),  // command line
                                NULL,  // pointer to process SECURITY_ATTRIBUTES
                                NULL,  // pointer to thread SECURITY_ATTRIBUTES
                                FALSE,            // handles are not inheritable
                                dwCreationFlags,  // creation flags
                                pEnv,  // pointer to new environment block
                                NULL,  // name of current directory
                                &si,   // pointer to STARTUPINFO structure
                                &pi    // receives information about new process
  );

  if (!bResult) {
    OutputDebugStringW(
        (std::wstring(L"error code:") + std::to_wstring(GetLastError()))
            .c_str());
  } else {
    OutputDebugStringW((std::wstring(L"create as user ok:")).c_str());
  }

  if (pi.hProcess) {
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
  }

  if (hUserTokenDup != NULL) CloseHandle(hUserTokenDup);
  if (hProcess != NULL) CloseHandle(hProcess);
  if (hPToken != NULL) CloseHandle(hPToken);
  if (pEnv != NULL) DestroyEnvironmentBlock(pEnv);

  return TRUE;
}

// 移动窗口右下角
void move_window(QWidget* widget) {
  QDesktopWidget* desktop = QApplication::desktop();
  int screenCount = desktop->screenCount();
  if (screenCount > 0) {
    QRect screenGeometry =
        desktop->screenGeometry(0);  // 获取第一个桌面的几何信息
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int windowWidth = widget->width();
    int windowHeight = widget->height();
    int x = screenWidth - windowWidth - 30;
    int y = screenHeight - windowHeight - 80;
    widget->move(screenGeometry.x() + x,
                 screenGeometry.y() + y);  // 将窗口移动到桌面的右下角
  }

  widget->show();
}
