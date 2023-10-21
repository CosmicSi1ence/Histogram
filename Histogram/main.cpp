#include <windows.h>
#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "INIReader.h"

using namespace std;
using namespace cv;

Mat img;
string path = "sadfgasg";
int w = -1;
int h = -1;

//  ФУНКЦИЯ ЧТЕНИЯ INI-ФАЙЛА:
bool get_ini_params(const string& config) {

    setlocale(LC_NUMERIC, "en_US.UTF-8");
    INIReader reader(config);       //  Создаем объект Ридера

    if (reader.ParseError() < 0) {
        cout << "Can't load '" << config << "'\n";
        return 0;
    }

    // ------------------------------ [images]: ----------------------------

    path = reader.Get("img", "path", "");
    if (path == "") {
        cout << "path not declared" << endl;
        return 0;
    }
    else {
        cout << "img_path = " << path << endl;
    }

    w = reader.GetInteger("img", "img_w", -1);
    h = reader.GetInteger("img", "img_h", -1);
    if (w <= 0 || h <= 0) {
        cout << "img_size error" << endl;
        return 0;
    }
    else {
        cout << "img_size = " << w << " x " << h << endl << endl;
    }

    return true;
}

int main() {

    //  Читаем INI-файл:
    bool is_ok = get_ini_params("img_data.ini");
    if (!is_ok) {
        cout << "INI-file error" << endl;
        return 0;
    }

    img = imread(path, IMREAD_GRAYSCALE);   //  читаем картинку по пути

    int gist[256] = { 0 };

    for (int i = 0; i < w*h; ++i) { //  Заполнение гистограммы
        ++gist[img.at<uchar>(i)];
    }

    int max = 0;
    for (int i = 0; i < 256; ++i) { //  Нахождение максимума
        if (gist[i] > max) {
            max = gist[i];
        }
    }
    
    int hist_h = 256/2;
    Mat histogram = Mat(Size(256, hist_h), CV_8UC1);
    for (int x = 0; x < 256; ++x) {
        for (int y = histogram.rows - 1; y >= 0; --y) {
            if (y >= histogram.rows - 1 - hist_h * gist[x] / max) {
                histogram.at<uchar>(y, x) = 0;
            }
            else {
                histogram.at<uchar>(y, x) = 255;
            }
        }
    }

    double scale = 5.5;
    resize(histogram, histogram, Size((int)(histogram.cols * scale), (int)(histogram.rows * scale)));
    namedWindow("hist", WINDOW_AUTOSIZE);
    imshow("hist", histogram);
    waitKey();

    return 0;
}
