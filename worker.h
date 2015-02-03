#ifndef WORKER_H
#define WORKER_H

#include <QRunnable>


class KinectCapture;

class Worker : public QRunnable
{
public:
    explicit Worker(KinectCapture *kc, void* ctx)
        :_ctx(ctx)
    {
        _kc = kc;
    }

    ~Worker();

protected:
    void run();

private:
    KinectCapture *_kc;
    void* _ctx;
};

#endif // WORKER_H
