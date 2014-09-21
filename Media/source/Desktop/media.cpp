#include <media.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QString>
#include <QFileDialog>
#include <QPixmap>
#include <QDateTime>
#include <cv.h>
#include <highgui.h>

class GMEDIA
{
public:
	GMEDIA()
	{
		gid_ = g_NextId();

	}

	~GMEDIA()
	{

		gevent_RemoveEventsWithGid(gid_);
    }
	
	bool isCameraAvailable()
	{
        CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
        if (capture) {
            IplImage* frame = cvQueryFrame( capture );
            if(frame)
            {
                return true;
            }
            cvReleaseCapture( &capture );
        }
        return false;
    }
	
	void takePicture()
	{
        CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
        if (capture) {
            IplImage* frame = cvQueryFrame( capture );
            if(frame)
            {
                for (int i = 0; i < 10; i++)
                {
                    frame = cvQueryFrame(capture);
                }
                QDateTime createdDate = QDateTime::currentDateTime();
                QString format = "jpg";
                QString fileName = getAppPath() + "/" + createdDate.toString("yyyyMMdd_HHmmss") + "_gideros." + format;
                cvSaveImage(fileName.toStdString().c_str(),frame);
                onMediaReceived(fileName.toStdString().c_str());
            }
            cvReleaseCapture( &capture );
        }
	}

    void takeScreenshot()
    {
        QDateTime createdDate = QDateTime::currentDateTime();
        //QPixmap originalPixmap = QPixmap::grabWindow(QApplication::activeWindow()->winId());
        QWidget *widget = QApplication::activeWindow();
        QPixmap originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
        QPixmap windowPixmap = originalPixmap.copy(widget->geometry().x(), widget->geometry().y(), widget->width(), widget->height());
        QString format = "png";
        QString fileName = getAppPath() + "/" + createdDate.toString("yyyyMMdd_HHmmss") + "_gideros." + format;
        windowPixmap.save(fileName, format.toAscii());
        onMediaReceived(fileName.toStdString().c_str());
    }
	
	void getPicture()
	{
        QString fileName = QFileDialog::getOpenFileName(0, QObject::tr("Open File"),"",QObject::tr("Images (*.png *.jpeg *.jpg)"));
        if(fileName.isNull())
            onMediaCanceled();
        else{
            QFileInfo info(fileName);
            QDateTime createdDate = QDateTime::currentDateTime();
            QString destName = getAppPath() + "/" + createdDate.toString("yyyyMMdd_HHmmss") + "_gideros." + info.suffix();
            if(QFile::exists(destName))
            {
                QFile::remove(destName);
            }
            QFile::copy(fileName, destName);
            onMediaReceived(destName.toStdString().c_str());
        }
	}
	
    void savePicture(const char* path)
	{
        QFileInfo info(path);
        QString format = info.suffix();
        QString initialPath = getAppPath() + "/" + info.fileName();

        QString fileName = QFileDialog::getSaveFileName(0, QObject::tr("Save As"),
                                        initialPath,
                                        QObject::tr("%1 Files (*.%2);;All Files (*)")
                                        .arg(format.toUpper())
                                        .arg(format));
        if (!fileName.isEmpty())
        {
            if(QFile::exists(fileName))
            {
                QFile::remove(fileName);
            }

            QFile::copy(path, fileName);
        }
	}

    void playVideo(const char* path, bool force)
    {

    }

    QString getAppPath(){
        QDir dir = QDir::temp();
        dir.mkdir("gideros");
        dir.cd("gideros");
        dir.mkdir("mediafiles");
        dir.cd("mediafiles");
        return dir.absolutePath();
    }
	
    void onMediaReceived(const char* path)
	{

		gmedia_ReceivedEvent *event = (gmedia_ReceivedEvent*)gevent_CreateEventStruct1(
			sizeof(gmedia_ReceivedEvent),
            offsetof(gmedia_ReceivedEvent, path), path);
		gevent_EnqueueEvent(gid_, callback_s, GMEDIA_RECEIVED_EVENT, event, 1, this);
	}

    void onMediaCompleted()
    {
        gevent_EnqueueEvent(gid_, callback_s, GMEDIA_COMPLETED_EVENT, NULL, 1, this);
    }
	
	void onMediaCanceled()
    {
		gevent_EnqueueEvent(gid_, callback_s, GMEDIA_CANCELED_EVENT, NULL, 1, this);
	}
	
	g_id addCallback(gevent_Callback callback, void *udata)
	{
		return callbackList_.addCallback(callback, udata);
	}
	void removeCallback(gevent_Callback callback, void *udata)
	{
		callbackList_.removeCallback(callback, udata);
	}
	void removeCallbackWithGid(g_id gid)
	{
		callbackList_.removeCallbackWithGid(gid);
	}

private:
	static void callback_s(int type, void *event, void *udata)
	{
		((GMEDIA*)udata)->callback(type, event);
	}

	void callback(int type, void *event)
	{
		callbackList_.dispatchEvent(type, event);
	}

private:
	gevent_CallbackList callbackList_;

private:
    g_id gid_;
};


static GMEDIA *s_gmedia = NULL;

extern "C" {

void gmedia_init()
{
    s_gmedia = new GMEDIA;
}

void gmedia_cleanup()
{
    delete s_gmedia;
    s_gmedia = NULL;
}

int gmedia_isCameraAvailable()
{
    return s_gmedia->isCameraAvailable();
}

void gmedia_takePicture(){
    s_gmedia->takePicture();
}

void gmedia_takeScreenshot(){
    s_gmedia->takeScreenshot();
}

void gmedia_getPicture(){
    s_gmedia->getPicture();
}

void gmedia_savePicture(const char* path){
    s_gmedia->savePicture(path);
}

void gmedia_playVideo(const char* path, int force){
    s_gmedia->playVideo(path, force);
}

g_id gmedia_addCallback(gevent_Callback callback, void *udata)
{
    return s_gmedia->addCallback(callback, udata);
}

void gmedia_removeCallback(gevent_Callback callback, void *udata)
{
    s_gmedia->removeCallback(callback, udata);
}

void gmedia_removeCallbackWithGid(g_id gid)
{
    s_gmedia->removeCallbackWithGid(gid);
}

}
