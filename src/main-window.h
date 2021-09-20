#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QThread>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <QProcess>

#include <QTextBrowser>
#include "stopwatch.h"

class MainWindow : public QWidget {

  Q_OBJECT

 public:
  MainWindow(QWidget* parent = 0);

 private slots:
  void UpdateElapsedTime(qint64 elapsed_msec);
  void OnStartButtonCliked();
  void OnRestartButtonClicked();
  void OnStopButtonClicked();
  void ReadBashStandardOutputInfo();
  void ReadBashStandardErrorInfo();
  void OnClearClicked();

 signals:
  void Start();
  void Restart();
  void RequestInterruption();

 private:
  QThread stopwatch_thread_;
  Stopwatch* stopwatch_;

  QVBoxLayout* root_layout_;
  QHBoxLayout* elapsed_time_layout_;
  QLabel* elapsed_time_name_label_;
  QLabel* elapsed_time_label_;
  QHBoxLayout* button_layout_;
  QPushButton* start_button_;
  QPushButton* stop_button_;
  QPushButton* clear_button_;

  QProcess* m_process_base_;

  QHBoxLayout* text_browser_layout_;
  QTextBrowser* m_text_browser_;
};

#endif /* MAIN_WINDOW_H_ */
