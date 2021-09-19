#include "stopwatch.h"

#include <QThread>

class Sleeper : public QThread {
 public:
  static void msleep(unsigned long msecs) { QThread::msleep(msecs); }
};

void Stopwatch::Start() {
  ClearInterruptionRequest();
  elapsed_timer_ = new QElapsedTimer;
  elapsed_time_ = 0;
  elapsed_timer_->start();
  while (!IsInterruptionRequested()) {
    Sleeper::msleep(1);
    elapsed_time_ = elapsed_timer_->elapsed();
    emit Elapsed(elapsed_time_);
  }
  delete elapsed_timer_;
}

void Stopwatch::Restart() {
  ClearInterruptionRequest();
  elapsed_timer_ = new QElapsedTimer;
  elapsed_timer_->start();
  qint64 tmp;
  while (!IsInterruptionRequested()) {
    Sleeper::msleep(1);
    tmp = elapsed_timer_->elapsed() + elapsed_time_;
    emit Elapsed(tmp);
  }
  elapsed_time_ = tmp;
  delete elapsed_timer_;
}

void Stopwatch::RequestInterruption() {
  mutex_.lock();
  interruption_requested_ = true;
  mutex_.unlock();
}

void Stopwatch::ClearInterruptionRequest() {
  mutex_.lock();
  interruption_requested_ = false;
  mutex_.unlock();
}

bool Stopwatch::IsInterruptionRequested() {
  bool flag;
  mutex_.lock();
  flag = interruption_requested_;
  mutex_.unlock();
  return flag;
}
