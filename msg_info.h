#pragma once

#include <QtCore/QString>

struct MsgInfo
{
  // 1、更新结果 2、顶级目录 3、日志绝对路径 4、exe启动目录  5、更新的脚本
  bool update_result;
  QString code_top_dir;
  QString app_log_path;
  QString app_start_dir;
  QString update_script_path;
};
