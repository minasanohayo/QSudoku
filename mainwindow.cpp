#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QElapsedTimer>
float w = 540;
float h = 540;
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_Timer.start(30);
    uvcCap.open(0);
    connect(&m_Timer, &QTimer::timeout, this, &MainWindow::process);

    sudoku.solution();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::process()
{
    QElapsedTimer timer;
    timer.start();

    cv::Mat frame = read_a_frame().clone();
    if (frame.empty())
        return;

    cv::Mat _edge;
    cv::Mat _bin;
    pretreatment(frame, _edge, _bin);
    qint64 pretreatmentelapsed = timer.elapsed(); // 返回毫秒
    timer.start();

    std::vector<cv::Point> conPoly;

    cv::Mat _drawContour = frame;
    get_contour(_edge, _drawContour, conPoly);

    qint64 get_contourelapsed = timer.elapsed(); // 返回毫秒
    timer.start();

    static cv::Mat _warp;
    get_warp(frame, _warp, conPoly);
    qint64 get_warpelapsed = timer.elapsed(); // 返回毫秒
    timer.start();

    std::vector<std::vector<cv::Mat>> cells;
    cells.clear();

    pretreatmentBINARY(_warp, _bin);
    get_cels(_bin, cells);

    cv::Mat _cellsMap = print_cels(cells);

    ui->graphicsView_1->setPixmap(cvMatToQPixmap(_drawContour));
    ui->graphicsView_2->setPixmap(cvMatToQPixmap(_edge));
    ui->graphicsView_3->setPixmap(cvMatToQPixmap(_cellsMap));
    // ui->graphicsView_4->setPixmap(cvMatToQPixmap(_cellsMap));

    frame.release();
    qDebug() << pretreatmentelapsed << " " << get_contourelapsed << " " << get_warpelapsed;
}

cv::Mat MainWindow::read_a_frame()
{
    cv::Mat frame;

    int originalWidth = img.cols;
    int originalHeight = img.rows;
    // uvcCap >> frame;
    if (originalWidth > w + 150 || originalHeight > h + 150) {
        double scale = w / static_cast<double>(std::max(originalWidth, originalHeight));
        int newWidth = static_cast<int>(originalWidth * scale);
        int newHeight = static_cast<int>(originalHeight * scale);

        cv::resize(img, img, cv::Size(newWidth, newHeight));
    }
    return img;
}

int MainWindow::pretreatment(cv::Mat& src, cv::Mat& dstEdge, cv::Mat& bin)
{

    // 1. 转为灰度
    cv::Mat gray;
    if (src.channels() == 3)
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    else
        gray = src;

    // 2. 高斯模糊
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0.5);

    // 3. 初次 Canny 边缘
    cv::Mat edge1;
    cv::Canny(blurred, edge1, 25, 80);

    // 4. 闭运算消除小孔
    int k = ((std::min(gray.rows, gray.cols) / 10) / 2) * 2 + 1;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(k, k));
    cv::Mat closed;
    cv::morphologyEx(gray, closed, cv::MORPH_CLOSE, kernel);

    // 5. 光照归一化： gray / closed
    cv::Mat normf;
    gray.convertTo(normf, CV_32F);
    cv::Mat closedf;
    closed.convertTo(closedf, CV_32F);
    cv::Mat tmp = normf / closedf;
    cv::normalize(tmp, tmp, 0, 255, cv::NORM_MINMAX);
    cv::Mat gray_closs;
    tmp.convertTo(gray_closs, CV_8U);

    // 6. 再次模糊
    cv::GaussianBlur(gray_closs, gray_closs, cv::Size(3, 3), 0.5);

    // 7. Otsu 二值化

    cv::threshold(gray_closs, bin, 150, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // 8. 第二次 Canny + 闭运算

    cv::Canny(bin, dstEdge, 35, 180);
    cv::morphologyEx(dstEdge, dstEdge, cv::MORPH_CLOSE,
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)),
        cv::Point(-1, -1), 1);
    return 0;
}

int MainWindow::pretreatmentBINARY(cv::Mat& src, cv::Mat& bin)
{
    // 1. 转为灰度
    cv::Mat gray;
    if (src.channels() == 3)
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    else
        gray = src.clone();

    // 2. 高斯模糊
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0.5);

    // 4. 闭运算消除小孔
    int k = ((std::min(gray.rows, gray.cols) / 10) / 2) * 2 + 1;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(k, k));
    cv::Mat closed;
    cv::morphologyEx(gray, closed, cv::MORPH_CLOSE, kernel);

    // 5. 光照归一化： gray / closed
    cv::Mat normf;
    gray.convertTo(normf, CV_32F);
    cv::Mat closedf;
    closed.convertTo(closedf, CV_32F);
    cv::Mat tmp = normf / closedf;
    cv::normalize(tmp, tmp, 0, 255, cv::NORM_MINMAX);
    cv::Mat gray_closs;
    tmp.convertTo(gray_closs, CV_8U);

    // 6. 再次模糊
    cv::GaussianBlur(gray_closs, gray_closs, cv::Size(3, 3), 0.5);

    // 7. Otsu 二值化

    cv::threshold(gray_closs, bin, 150, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    return 0;
}

int MainWindow::get_contour(cv::Mat& edge, cv::Mat& dst, std::vector<cv::Point>& conPoly)
{

    // 查找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edge, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 筛选并多边形逼近
    int H = edge.rows, W = edge.cols;
    conPoly.clear();
    for (auto& c : contours) {
        double area = cv::contourArea(c);
        if (area > H * W * 0.1) {
            double peri = cv::arcLength(c, true);
            std::vector<cv::Point> approx;
            cv::approxPolyDP(c, approx, 0.05 * peri, true);
            // 仅记录凸包（可根据需要去掉以下判断）
            if (cv::isContourConvex(approx)) {
                conPoly = approx;
                // 🔴 画出轮廓
                cv::drawContours(dst, std::vector<std::vector<cv::Point>> { approx }, -1, cv::Scalar(255, 0, 255), 3);
                return 0;
            }
        }
    }
    return -1;
}

int MainWindow::get_warp(cv::Mat& img, cv::Mat& dst, std::vector<cv::Point> points)
{

    cv::Mat imgWarp;

    // 对于凸四边形的顶点排序
    std::sort(points.begin(), points.end(), [](const auto& a, const auto& b) { return a.y < b.y; });
    if (points[0].x > points[1].x)
        swap(points[0], points[1]);
    if (points[2].x > points[3].x)
        swap(points[2], points[3]);

    cv::Point2f srcPoint[4] = { points[0], points[1], points[2], points[3] };

    cv::Point2f dstPoint[4] = {
        { 0.0f, 0.0f },
        { w, 0.0f },
        { 0.0f, h },
        { w, h }
    };

    cv::Mat matrix = getPerspectiveTransform(srcPoint, dstPoint);

    cv::warpPerspective(img, dst, matrix, cv::Point(w, h));

    return 0;
}

int MainWindow::get_cels(cv::Mat& frame, std::vector<std::vector<cv::Mat>>& cells)
{
    cells.clear();
    int w = frame.cols / 9;
    int h = frame.rows / 9;
    for (int i = 0; i < 9; i++) {
        std::vector<cv::Mat> row;
        for (int j = 0; j < 9; j++) {
            int x = fmin(i * w, frame.cols);
            int y = fmin(j * h, frame.rows);
            cv::Rect roi(x, y, w, h);

            // row.push_back(frame(roi).clone()); // 对的

            cv::Mat bit;
            cv::threshold(frame, bit, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // 二值化为0-255图
            row.push_back(bit(roi).clone()); //  不行
        }
        cells.push_back(row);
    }
    return 0;
}

cv::Mat MainWindow::print_cels(std::vector<std::vector<cv::Mat>>& cells)
{
    cv::Mat ret(cv::Size(590, 590), CV_8UC3, cv::Scalar(255, 0, 255));
    int x = 5;
    int y = 5;
    // qDebug() << cells.size();
    cv::Mat dst;
    for (int i = 0; i < cells.size(); i++) {
        x = 5 + i * (5 + 60);

        for (int j = 0; j < cells.size(); j++) {
            y = 5 + j * (5 + 60);
            if (!cells[i][j].empty()) {
                dst = cells[i][j].clone();
                blobAnalyze(cells[i][j], dst); // dst应该也是0-255二值图
                // cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);

                dst.copyTo(ret(cv::Rect(x, y, cells[i][j].cols, cells[i][j].rows)));
            }
        }
    }

    return ret;
}

int MainWindow::blobAnalyze(cv::Mat& binary, cv::Mat& dst)
{
    // 必须是单通道8位图像
    if (binary.empty() || binary.type() != CV_8UC1) {
        qDebug() << "Input image is invalid or not binary.";
        return 0;
    }

    cv::bitwise_not(binary, binary);
    // 统计信息
    cv::Mat labels, stats, centroids;
    int numLabels = cv::connectedComponentsWithStats(binary, labels, stats, centroids);
    cv::cvtColor(binary, dst, cv::COLOR_GRAY2BGR);
    int validBlobCount = 0;
    cv::Rect box;
    for (int i = 1; i < numLabels; ++i) { // i=0 是背景
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        if (area < 50)
            continue; // 忽略小区域（可调整）
        float cx = centroids.at<double>(i, 0);
        float cy = centroids.at<double>(i, 1);
        float width = binary.size().width;
        float height = binary.size().height;

        if (cx < width * 0.3 || cx > width * 0.7 || cy < height * 0.3 || cy > height * 0.7)
            continue;
        int x = fmax(0, stats.at<int>(i, cv::CC_STAT_LEFT) - 2);
        int y = fmax(0, stats.at<int>(i, cv::CC_STAT_TOP) - 2);
        int w = fmin(width, stats.at<int>(i, cv::CC_STAT_WIDTH) + 3);
        int h = fmin(height, stats.at<int>(i, cv::CC_STAT_HEIGHT) + 4);
        if (w == width || h == height)
            continue;

        if (box.empty() || box.width >= w || box.height >= h)
            box = cv::Rect(x, y, w, h);

        ++validBlobCount;
    }

    if (!box.empty()) {
        cv::rectangle(dst, box, cv::Scalar(0, 255, 0), 2);
        keepOnlyBoxArea(dst, box);
    }
    // 再次尝试
    if (validBlobCount == 0) {

        double whitePixels = cv::countNonZero(binary);
        double totalPixels = binary.rows * binary.cols;

        double ratio = whitePixels / totalPixels;

        if (ratio < 0.3)
            return 0;

        cv::Mat _temp = (binary(cv::Rect(5, 5, binary.size().width - 10, binary.size().height - 10))).clone();

        int borderSize = 5;
        cv::copyMakeBorder(_temp, _temp,
            borderSize, borderSize, borderSize, borderSize,
            cv::BORDER_CONSTANT, cv::Scalar(0)); // 边界像素值为0

        numLabels = cv::connectedComponentsWithStats(_temp, labels, stats, centroids);
        for (int i = 1; i < numLabels; ++i) { // i=0 是背景
            int area = stats.at<int>(i, cv::CC_STAT_AREA);
            if (area < 50)
                continue; // 忽略小区域（可调整）
            float cx = centroids.at<double>(i, 0);
            float cy = centroids.at<double>(i, 1);
            float width = binary.size().width;
            float height = binary.size().height;

            if (cx < width * 0.3 || cx > width * 0.7 || cy < height * 0.3 || cy > height * 0.7)
                continue;
            int x = stats.at<int>(i, cv::CC_STAT_LEFT) - 2;
            int y = stats.at<int>(i, cv::CC_STAT_TOP) - 2;
            int w = stats.at<int>(i, cv::CC_STAT_WIDTH) + 3;
            int h = stats.at<int>(i, cv::CC_STAT_HEIGHT) + 4;
            keepOnlyBoxArea(dst, cv::Rect(x, y, w, h));
            cv::rectangle(dst, cv::Rect(x, y, w, h), cv::Scalar(0, 255, 0), 2);

            ++validBlobCount;
            return 1;
        }
    }

    return validBlobCount;
}

// 在图像上找到每个字符框的区域并画出，存入boxes
int MainWindow::get_boxes(cv::Mat& frame, cv::Mat& dst, std::vector<cv::Rect> boxes, bool draw)
{
    return 1;
}

int MainWindow::recognise(std::vector<std::vector<cv::Mat>>& cells, std::vector<std::vector<char>>& sudoku)
{
    for (int i = 0; i < cells.size(); i++) {
        for (int j = 0; j < cells[0].size(); j++) {
            if (cells[i][j].empty()) {
                sudoku[i][j] = 0;
            }
        }
    }
    return 1;
}

void MainWindow::keepOnlyBoxArea(cv::Mat& mat, const cv::Rect& box)
{
    CV_Assert(mat.data); // 检查是否为空

    // 创建与 mat 相同大小、同类型的全 0 图像
    cv::Mat mask = cv::Mat::zeros(mat.size(), mat.type());

    // 将 box 区域复制回去
    mat(box).copyTo(mask(box));

    // 用结果替换原图
    mat = mask;
}

void MainWindow::matToQImg(cv::Mat& input, QImage& output, QImage::Format format)
{
    if (input.type() == CV_8UC1)
        output = QImage(input.data, input.cols, input.rows, input.step, QImage::Format_Grayscale8);
    else if (input.type() == CV_8UC3)
        output = QImage(input.data, input.cols, input.rows, input.step, QImage::Format_RGB888);
    else if (input.type() == CV_8UC4)
        output = QImage(input.data, input.cols, input.rows, input.step, QImage::Format_RGBA8888);

    if (output.isNull())
        std::cerr << "MatToQImg fails" << std::endl;

    if (input.type() == CV_8UC3 || input.type() == CV_8UC4)
        output = output.rgbSwapped(); // 将 BGR 转为 RGB
}

QPixmap MainWindow::cvMatToQPixmap(const cv::Mat& input)
{
    // 1. 检查图像是否为空
    if (input.empty()) {
        return QPixmap();
    }

    // 2. 区分图像通道数
    QImage image;
    switch (input.type()) {
    case CV_8UC1: { // 灰度图
        image = QImage(input.data, input.cols, input.rows, input.step, QImage::Format_Grayscale8);
        break;
    }
    case CV_8UC3: { // BGR 彩图，需转为 RGB

        cv::cvtColor(input, input, cv::COLOR_BGR2RGB);
        image = QImage(input.data, input.cols, input.rows, input.step, QImage::Format_RGB888);

        break;
    }
    case CV_8UC4: { // BGRA，Qt 支持 ARGB32 格式

        image = QImage(input.data, input.cols, input.rows, input.step, QImage::Format_ARGB32);
        break;
    }
    default:
        qWarning("Unsupported cv::Mat image type for conversion to QPixmap.");
        return QPixmap();
    }

    // 3. 拷贝数据，防止原始 mat 释放后指针失效
    return QPixmap::fromImage(image.copy());
}

void MainWindow::on_pushButton_2_clicked()
{
    img = cv::imread("C:/Users/commander/Pictures/QQ20250520-220312.png");
}

void MainWindow::on_pushButton_5_clicked()
{
    img = cv::imread("C:/Users/commander/Pictures/QQ20250521-112811.png");
}

void MainWindow::on_pushButton_6_clicked()
{
    img = cv::imread("C:/Users/commander/Pictures/QQ20250521-211557.png");
}

void MainWindow::on_pushButton_7_clicked()
{
    img = cv::imread("C:/Users/commander/Pictures/QQ20250517-194343.png");
}

void MainWindow::on_pushButton_8_clicked()
{
    img = cv::imread("C:/Users/commander/Pictures/QQ20250520-190236.png");
}

void MainWindow::on_pushButton_9_clicked()
{
    img = cv::imread("C:/Users/commander/Pictures/QQ20250517-150657.png");
}
