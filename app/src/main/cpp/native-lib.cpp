#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/native_window_jni.h>
using namespace cv;

class CascadeDetectorAdapter : public DetectionBasedTracker::IDetector {
public:
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector) :
            IDetector(),
            Detector(detector) {
        CV_Assert(detector);
    }

    void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) {
        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize,
                                   maxObjSize);
    }
    virtual ~CascadeDetectorAdapter() {}
private:
    CascadeDetectorAdapter();
    cv::Ptr<cv::CascadeClassifier> Detector;
};

DetectionBasedTracker *tracker = 0;
ANativeWindow *window = 0;

extern "C"
JNIEXPORT void JNICALL
Java_com_louis_facetracing_MainActivity_setSurface(JNIEnv *env, jobject thiz, jobject surface) {
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env, surface);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_louis_facetracing_MainActivity_init(JNIEnv *env, jobject thiz, jstring model_) {
    const char *model = env->GetStringUTFChars(model_,0);

    if (tracker) {
        tracker->stop();
        delete tracker;
        tracker = 0;
    }
    //智能指针
    Ptr<CascadeClassifier> classifier = makePtr<CascadeClassifier>(model);
    //创建一个跟踪适配器
    Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(classifier);

    Ptr<CascadeClassifier> classifier1 = makePtr<CascadeClassifier>(model);
    //创建一个跟踪适配器
    Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(classifier1);
    //拿去用的跟踪器
    DetectionBasedTracker::Parameters DetectorParams;
    tracker = new DetectionBasedTracker(mainDetector, trackingDetector, DetectorParams);
    //开启跟踪器
    tracker->run();

    env->ReleaseStringUTFChars(model_, model);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_louis_facetracing_MainActivity_release(JNIEnv *env, jobject thiz) {
    if (tracker) {
        tracker->stop();
        delete tracker;
        tracker = 0;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_louis_facetracing_MainActivity_postData(JNIEnv *env, jobject thiz, jbyteArray data_, jint w,
                                                 jint h, jint camera_id) {
    //nv21的数据
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    //mat data->Mat
    //1、高 2、宽
    Mat src(h + h/2, w, CV_8UC1);
    env->ReleaseByteArrayElements(data_, data, 0);
}