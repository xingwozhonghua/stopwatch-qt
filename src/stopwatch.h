#ifndef STOPWARCH_H_
#define STOPWARCH_H_

#include <QMutex>
#include <QObject>
#include <QElapsedTimer>

class Stopwatch : public QObject {

  Q_OBJECT

 public slots:
  void Start();
  void Restart();
  void RequestInterruption();

 signals:
  void Elapsed(qint64 elapsed_msec);

 private:
  void ClearInterruptionRequest();
  bool IsInterruptionRequested();

  QElapsedTimer* elapsed_timer_;
  QMutex mutex_;
  qint64 elapsed_time_;
  bool interruption_requested_;
};

#endif /* STOPWARCH_H_ */
