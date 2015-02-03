#ifndef WRITER_H
#define WRITER_H

#include <QRunnable>

class KinectCapture;

class WriterBase : public QRunnable
{
public:
    explicit WriterBase(KinectCapture* kc, void* ctx);
    ~WriterBase();

    //void InitializeMessageQueue();
    //bool IsStopped();
    //void ReleaseMessageQueue();

protected:
    void run();

private:
    KinectCapture *_kc;

    void* _ctx;
    //void* _sock;

//    char _buf[8];

    enum MaxFrameNumber
    {
        FrameCount = 900
    };
};

#endif // WRITER_H
