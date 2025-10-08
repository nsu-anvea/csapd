#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

int main() {
    cv::VideoCapture capture(0);
    if (!capture.isOpened()) {
        cerr << "Ошибка: не удалось открыть камеру!" << endl;
        return -1;
    }

    cv::Mat frame, transformedFrame;
    int frame_count = 0;
    int FPS_count = 0;

    double sum_percentage_time_frame_capture = 0;
    double sum_percentage_time_frame_conversion = 0;
    double sum_percentage_showing_converted_frame = 0; 
    double sum_percentage_waitkey = 0;

    auto FPS_timer = high_resolution_clock::now();
    auto start = high_resolution_clock::now();

    while (true) {
        auto start_frame_capture = high_resolution_clock::now();
        capture.read(frame);
        auto end_frame_capture = high_resolution_clock::now();
        duration<double> time_frame_capture = end_frame_capture - start_frame_capture;
        
        if (frame.empty()) break;

        auto start_frame_conversion = high_resolution_clock::now();
        cv::cvtColor(frame, transformedFrame, cv::COLOR_BGR2GRAY);
        auto end_frame_conversion = high_resolution_clock::now();
        duration<double> time_frame_conversion = end_frame_conversion - start_frame_conversion;

        auto start_showing_converted_frame = high_resolution_clock::now();
        cv::imshow("Camera", transformedFrame);
        auto end_showing_converted_frame = high_resolution_clock::now();
        duration<double> time_showing_converted_frame = end_showing_converted_frame - start_showing_converted_frame;

        auto start_waitkey = high_resolution_clock::now();
        if (cv::waitKey(1) == 'q') break;
        auto end_waitkey = high_resolution_clock::now();
        duration<double> time_waitkey = end_waitkey - start_waitkey;

        auto end_frame_processing = high_resolution_clock::now();
        duration<double> time_frame_processing = end_frame_processing - start_frame_capture;

        sum_percentage_time_frame_capture += (time_frame_capture / time_frame_processing) * 100;
        sum_percentage_time_frame_conversion += (time_frame_conversion / time_frame_processing) * 100;
        sum_percentage_showing_converted_frame += (time_showing_converted_frame / time_frame_processing) * 100;
        sum_percentage_waitkey += (time_waitkey / time_frame_processing) * 100;

        frame_count++;
        FPS_count++;

        auto current_time = high_resolution_clock::now();
        duration<double> time_since_last_FPS = current_time - FPS_timer;

        if (time_since_last_FPS.count() >= 1.0) {
            cout << "FPS за последнюю секунду: " << FPS_count << endl;
            FPS_count = 0;
            FPS_timer = current_time;
        }

        // cout << "Time frame capture: " << time_frame_capture.count() * 1000 << "ms" << endl << 
        // "Time frame conversion: " << time_frame_conversion.count() * 1000 << "ms" << endl <<
        // "Time show converted frame: " << time_showing_converted_frame.count() * 1000 << "ms" << endl <<
        // "Time waitKey: " << time_waitkey.count() * 1000 << "ms" << endl <<
        // "Time frame_processing: " << time_frame_processing.count() * 1000 << "ms" << endl <<
        // "------------------------" << endl;
    }
    auto end = high_resolution_clock::now();
    duration<double> total_time = end - start;

    double average_FPS = frame_count / total_time.count();
    double average_processing_time = (total_time.count() / frame_count) * 1000.0;

    cout << "Average time of frame capture: " << sum_percentage_time_frame_capture / frame_count << "%" << endl <<
    "Average time of frame conversion: " << sum_percentage_time_frame_conversion / frame_count << "%" << endl <<
    "Average time of showing converted frame: " << sum_percentage_showing_converted_frame / frame_count << "%" << endl <<
    "Average time of waitkey: " << sum_percentage_waitkey / frame_count << "%\n" << endl;

    cout << "Average time of FPS: " << average_FPS << endl;
    cout << "Average processing time: " << average_processing_time << "ms" << endl;

    capture.release();
    cv::destroyAllWindows();

    return 0;
}
