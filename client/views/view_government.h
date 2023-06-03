/**************************************************************************
 Copyright (c) 1996-2023 Freeciv21 and Freeciv contributors. This file is
 part of Freeciv21. Freeciv21 is free software: you can redistribute it
 and/or modify it under the terms of the GNU  General Public License  as
 published by the Free Software Foundation, either version 3 of the
 License,  or (at your option) any later version. You should have received
 a copy of the GNU General Public License along with Freeciv21. If not,
 see https://www.gnu.org/licenses/.
**************************************************************************/
#pragma once

// utility
#include "fc_types.h"
// common
#include "government.h"
// client
#include "repodlgs_g.h"

class QComboBox;
class QGridLayout;
class QStackedLayout;
class QLabel;
class QMouseEvent;
class QObject;
class QPaintEvent;
class QScrollArea;
class progress_bar;

class audit_button : public QPushButton {
  Q_OBJECT

public:
  audit_button();
  int get_audit_id() const;
  void set_audit_id(int id);
  void set_audit_info(const struct packet_government_audit_info *info);

signals:
  void audit_selected(int id);

private:
  int audit_id = -1;
};

/****************************************************************************
  Widget embedded as tab on game view (F7 default)
  Uses string "GOV" to mark it as opened
  You can check it using if (queen()->is_repo_dlg_open("GOV"))
****************************************************************************/
class government_report : public QWidget {
  Q_OBJECT

  QStackedLayout *layout;
  QScrollArea *m_recent_decisions_scroll;
  int m_auditing_count = MAX_AUDIT_NUM;
  audit_button **m_auditing_buttons;

  int cached_last_message_id = -1;
  int cached_last_audit_id = -1;

public:
  void init(bool raise);
  void redraw();

protected:
  static government_report *_instance;

  void show_audit_screen(int id);

public:
  static government_report *instance();

  void update_info();
  void update_news(int id, int turn, const QString &news);
  void update_audit_info(const struct packet_government_audit_info *info);

private:
  government_report();
  ~government_report();
  government_report(const government_report &other) = delete;
  government_report &operator=(const government_report &other) = delete;
  int index{0};
};