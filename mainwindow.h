#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qtimer.h"
#include "sudoku.h"
#include <QImage>
#include <QMainWindow>
#include <opencv2/opencv.hpp>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    struct sudokuCell {
        cv::Mat img;
        char numb = 0;
    };

    void process();
    cv::Mat read_a_frame();
    int pretreatment(cv::Mat& src, cv::Mat& dst, cv::Mat& bin);
    int pretreatmentBINARY(cv::Mat& src, cv::Mat& bin);
    int get_contour(cv::Mat& src, cv::Mat& dst, std::vector<cv::Point>& conPoly);
    int get_warp(cv::Mat& frame, cv::Mat& dst, std::vector<cv::Point> points);
    int get_cels(cv::Mat& frame, std::vector<std::vector<cv::Mat>>& cells);
    cv::Mat print_cels(std::vector<std::vector<cv::Mat>>& cells);
    int blobAnalyze(cv::Mat& binary, cv::Mat& dst);
    // AAAAA
    int get_boxes(cv::Mat& frame, cv::Mat& dst, std::vector<cv::Rect> boxes, bool draw = false);
    int recognise(std::vector<std::vector<cv::Mat>>& cells, std::vector<std::vector<char>>& sudoku);
    void keepOnlyBoxArea(cv::Mat& mat, const cv::Rect& box);
    void matToQImg(cv::Mat& input, QImage& output, QImage::Format format);
    QPixmap cvMatToQPixmap(const cv::Mat& input);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

private:
    cv::VideoCapture uvcCap;
    Ui::MainWindow* ui;
    cv::Mat img;
    QTimer m_Timer;
    Sudoku sudoku;
};
#endif // MAINWINDOW_H
