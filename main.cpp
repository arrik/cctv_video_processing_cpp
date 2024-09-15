#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/trace.hpp>
using namespace cv;
using namespace cv::dnn;
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

string CLASSES[] = {"background", "aeroplane", "bicycle", "bird", "boat",
                    "bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
                    "dog", "horse", "motorbike", "person", "pottedplant", "sheep",
                    "sofa", "train", "tvmonitor"};

//int detectAndDraw(const HOGDescriptor &hog, Mat &img)
//{
//    vector<Rect> found, found_filtered;
//    double time = (double) getTickCount();
//    hog.detectMultiScale(img, found, 0, Size(8,8), Size(16,16), 1.07, 2);
//    time = (double) getTickCount() - time;
//    cout << "detection time = " << (time*1000./cv::getTickFrequency()) << " ms" << endl;
//
//    for(size_t i = 0; i < found.size(); i++ )
//    {
//        Rect r = found[i];
//        size_t j;
//        // Do not add small detections inside a bigger detection.
//        for ( j = 0; j < found.size(); j++ )
//            if ( j != i && (r & found[j]) == r )
//                break;
//        if ( j == found.size() )
//            found_filtered.push_back(r);
//    }
//    for (size_t i = 0; i < found_filtered.size(); i++)
//    {
//        Rect r = found_filtered[i];
//        // The HOG detector returns slightly larger rectangles than the real objects,
//        // so we slightly shrink the rectangles to get a nicer output.
//        r.x += cvRound(r.width*0.1);
//        r.width = cvRound(r.width*0.8);
//        r.y += cvRound(r.height*0.07);
//        r.height = cvRound(r.height*0.8);
//        rectangle(img, r.tl(), r.br(), cv::Scalar(0,255,0), 3);
//    }
//    return found_filtered.size();
//}

int dnnDetectAndDraw( Net &net, Mat &frame ) {
    Mat frame2;
    resize(frame, frame2, Size(300,300));
    Mat inputBlob = blobFromImage(frame2, 0.007843, Size(300,300), Scalar(127.5, 127.5, 127.5), false);

    net.setInput(inputBlob, "data");
    Mat detection = net.forward("detection_out");
    Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
    ostringstream ss;
    float confidenceThreshold = 0.5;
    for (int i = 0; i < detectionMat.rows; i++)
    {
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > confidenceThreshold)
        {
            int idx = static_cast<int>(detectionMat.at<float>(i, 1));
            int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
            int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
            int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
            int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

            Rect object((int)xLeftBottom, (int)yLeftBottom,
                        (int)(xRightTop - xLeftBottom),
                        (int)(yRightTop - yLeftBottom));

            rectangle(frame, object, Scalar(0, 255, 0), 2);

            cout << CLASSES[idx] << ": " << confidence << endl;

            ss.str("");
            ss << confidence;
            String conf(ss.str());
            String label = CLASSES[idx] + ": " + conf;
            int baseLine = 0;
            Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
            putText(frame, label, Point(xLeftBottom, yLeftBottom),
                    FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0));
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    cout << getBuildInformation();
    cout << "Built with OpenCV " << CV_VERSION << endl;
    string window_name = "RGB Camera";
    const string rtsp_url = "rtsp://admin:03011995@192.168.1.2:5543/live/channel0";
//    HOGDescriptor hog;
//    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    CV_TRACE_FUNCTION();
    String modelTxt = "<path>/MobileNetSSD_deploy.prototxt";
    String modelBin = "<path>/MobileNetSSD_deploy.caffemodel";
    Net net = dnn::readNetFromCaffe(modelTxt, modelBin);
    if (net.empty())
    {
        std::cerr << "Can't load network by using the following files: " << std::endl;
        std::cerr << "prototxt:   " << modelTxt << std::endl;
        std::cerr << "caffemodel: " << modelBin << std::endl;
        exit(-1);
    }

    // Load the video file
    VideoCapture cap; // Declare capture stream
//    cap.open(rtsp_url);
    cap.open(0, CAP_AVFOUNDATION);

    // Check if the video file was opened successfully!
    if (!cap.isOpened())
    {
        cerr << "Error: could not open video file" << endl;
        return -1;
    }

    // Create a window to display the video frames
    namedWindow("Video", WindowFlags::WINDOW_KEEPRATIO); // WINDOW_NORMAL

    // Loop over the video frames and display them in the window
    while (true)
    {

        // Read the next frame from the video file
        Mat frame;
        cap.read(frame);

        // Check if the frame was read successfully
        if (frame.empty())
        {
            break;
        }

//        //returns the number of people detected from detect and draw function
//        int num_people = detectAndDraw(hog, frame);
//        cout << "People count: " << num_people << endl;

        //returns the number of dnn object detected from detect and draw function
        int num_object = dnnDetectAndDraw(net, frame);
        if (num_object > 0)
        {
            cout << "Object count: " << num_object << endl;
        }

        // Display the current frame in the window
        imshow("Video", frame);

        // Wait for a key press (or 30 milliseconds) to allow the frame to be displayed
        if (waitKey(30) >= 0)
        {
            break;
        }
    }

    // Release the video file and destroy the window
    cap.release();
    destroyAllWindows();

    return 0;
}