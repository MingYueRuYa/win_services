#pragma once

#include <QtCore/QString>

struct MsgInfo
{
  // 1�����½�� 2������Ŀ¼ 3����־����·�� 4��exe����Ŀ¼  5�����µĽű�
  bool update_result;
  QString code_top_dir;
  QString app_log_path;
  QString app_start_dir;
  QString update_script_path;
};
