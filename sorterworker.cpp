#include <QObject>
#include <QVector>
#include <QThread>
#include <QDebug>

/*! Class doing the actual work (sorting) */
class SorterWorker: public QObject {
  Q_OBJECT

signals:
  /*! Signal emitted once the vector is sorted */
  void vectorSorted(const QVector<int> &v);

public slots:
  /*! Method taking care of the actual sorting */
  void doSort(const QVector<int> &v) {
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId(); // Worker Thread
    QVector<int> v_sorted = v;
    qSort(v_sorted);
    emit vectorSorted(v_sorted);
  }
};
