#include <QMetaType>
#include <QDebug>
#include "sorter.h"
#include "sorterworker.h"

Sorter::Sorter(QObject *parent):
  QThread(parent), m_ready(false) {
  qDebug() << Q_FUNC_INFO << QThread::currentThreadId(); // Main Thread
  // Start the worker thread
  start();
  // Wait for the worker thread to be ready;
  while(!m_ready) msleep(50);
}

void Sorter::sortAsync(const QVector<int> &v)
{
  qDebug() << Q_FUNC_INFO << QThread::currentThreadId(); // Main Thread
  emit sortingRequested(v);
}

void Sorter::run()
{
  qDebug() << Q_FUNC_INFO << QThread::currentThreadId(); // Worker Thread
  // This QObject lives in the worker thread
  //SorterWorker worker; // DO NOT define 'this' pointer as parent
  // We need to register QList<int> because it is not known
  // to Qt's meta-object system
  qRegisterMetaType< QVector<int> >("QVector<int>");
  // Pass sorting requests to SorterWorker in the worker thread
  //connect(this, SIGNAL(sortingRequested(QVector<int>)), &worker, SLOT(doSort(QVector<int>))/*, Qt::QueuedConnection*/);
  // Forward the signal to the clients
  //connect(&worker, SIGNAL(vectorSorted(QVector<int>)), this, SIGNAL(vectorSorted(QVector<int>))/*, Qt::QueuedConnection*/);
  // Mark the worker thread as ready
  m_ready = true;
  // Event loop (necessary to process signals)
  exec();
}
