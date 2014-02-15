#include <QThread>
#include <QVector>

/*! Class for doing work (sorting) in a worker thread */
class Sorter : public QThread {
  Q_OBJECT

public:
  /*! Default constructor */
  Sorter(QObject *parent = 0);

  /*! Sorts asynchronously a vector in a worker thread */
  void sortAsync(const QVector<int> &list);

signals:
  /*!
   * Internal signal used to communicate with
   * the worker thread.
   */
  void sortingRequested(const QVector<int> &list);
  /*! Signal emitted once the vector is sorted */
  void vectorSorted(const QVector<int> &list);

protected:
  void run();

private:
  /*!
   * Boolean indicating if the worker thread is ready
   * to process requests.
   */
  bool m_ready;
};
