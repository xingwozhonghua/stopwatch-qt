#include "main-window.h"

#include <iomanip>
#include <sstream>

#include <QScrollBar>
#include <QAbstractSlider>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
  Qt::WindowFlags m_flags = windowFlags();
  setWindowFlags(m_flags | Qt::WindowStaysOnTopHint);

  setWindowTitle("Stopwatch");
  root_layout_ = new QVBoxLayout;

  elapsed_time_layout_ = new QHBoxLayout;
  elapsed_time_name_label_ = new QLabel("Elapsed time:");
  elapsed_time_label_ = new QLabel("00:000 s");
  elapsed_time_layout_->addWidget(elapsed_time_name_label_);
  elapsed_time_layout_->addWidget(elapsed_time_label_);

  button_layout_ = new QHBoxLayout;
  start_button_ = new QPushButton("Start");
  stop_button_ = new QPushButton("Stop");
  stop_button_->setEnabled(false);
  clear_button_ = new QPushButton("Clear");
  clear_button_->setEnabled(false);

  button_layout_->addWidget(start_button_);
  button_layout_->addWidget(stop_button_);
  button_layout_->addWidget(clear_button_);

  text_browser_layout_ = new QHBoxLayout;
  m_text_browser_ = new QTextBrowser();
  text_browser_layout_->addWidget(m_text_browser_);

  root_layout_->addLayout(elapsed_time_layout_);
  root_layout_->addLayout(button_layout_);
  root_layout_->addLayout(text_browser_layout_);

  setLayout(root_layout_);

  m_process_base_ = new QProcess(this);

  stopwatch_ = new Stopwatch;
  connect(start_button_, SIGNAL(clicked()), this, SLOT(OnStartButtonCliked()));
  connect(clear_button_, SIGNAL(clicked()), this, SLOT(OnClearClicked()));
  connect(stop_button_, SIGNAL(clicked()), this, SLOT(OnStopButtonClicked()));
  connect(this, SIGNAL(Start()), stopwatch_, SLOT(Start()));
  connect(this, SIGNAL(Restart()), stopwatch_, SLOT(Restart()));
  connect(this, SIGNAL(RequestInterruption()), stopwatch_,
          SLOT(RequestInterruption()), Qt::DirectConnection);
  connect(stopwatch_, SIGNAL(Elapsed(qint64)), this,
          SLOT(UpdateElapsedTime(qint64)));

  connect(m_process_base_, SIGNAL(readyReadStandardOutput()), this, SLOT(ReadBashStandardOutputInfo()));
  connect(m_process_base_, SIGNAL(readyReadStandardError()), this, SLOT(ReadBashStandardErrorInfo()));

}

void MainWindow::UpdateElapsedTime(qint64 elapsed_msec) {
    std::stringstream ss;
    qint64 seconds = elapsed_msec / 1000;
    qint64 msec = elapsed_msec % 1000;
    ss << seconds << ":" << std::setw(3)
       << msec << " s";
    elapsed_time_label_->setText(ss.str().c_str());
}

void MainWindow::OnStartButtonCliked() {
  if (!stopwatch_thread_.isRunning()) {
    stopwatch_->moveToThread(&stopwatch_thread_);
    start_button_->setEnabled(false);
    clear_button_->setEnabled(true);
    stop_button_->setEnabled(true);
    stopwatch_thread_.start();
    emit Start();

    m_process_base_->start("bash");
    m_process_base_->waitForStarted();
    m_process_base_->write(QString("sudo libinput debug-events").toLocal8Bit()+"\n");
  }
}

void MainWindow::OnRestartButtonClicked() {
  if (!stopwatch_thread_.isRunning()) {
    stopwatch_->moveToThread(&stopwatch_thread_);
    start_button_->setEnabled(false);
    clear_button_->setEnabled(true);
    stop_button_->setEnabled(true);
    stopwatch_thread_.start();
    emit Restart();
  }
}

void MainWindow::OnStopButtonClicked() {
  if (stopwatch_thread_.isRunning()) {
    start_button_->setEnabled(true);
    clear_button_->setEnabled(true);
    stop_button_->setEnabled(false);
    emit RequestInterruption();
    stopwatch_thread_.quit();
    m_process_base_->close();
  }
}

void MainWindow::OnClearClicked()
{
    m_text_browser_->clear();
}

void MainWindow::ReadBashStandardOutputInfo() {
    QByteArray cmdout = m_process_base_->readAllStandardOutput();
    if(!cmdout.isEmpty()){
        m_text_browser_->append(QString::fromLocal8Bit(cmdout));
    }
    QScrollBar* scroll = m_text_browser_->verticalScrollBar();
    scroll->setSliderPosition(scroll->maximum());
}

void MainWindow::ReadBashStandardErrorInfo() {
    QByteArray cmdout = m_process_base_->readAllStandardError();
    if(!cmdout.isEmpty()){
        m_text_browser_->append(QString::fromLocal8Bit(cmdout));
    }
    QScrollBar* scroll = m_text_browser_->verticalScrollBar();
    scroll->setSliderPosition(scroll->maximum());
}
