#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
            << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }
}

void naive_bezier(const std::vector<cv::Point2f>& points, cv::Mat& window)
{
    auto& p_0 = points[0];
    auto& p_1 = points[1];
    auto& p_2 = points[2];
    auto& p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
            3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f>& control_points, float t)
{
    // TODO: Implement de Casteljau's algorithm
    //该函数使用一个控制点序列和一个浮点数 t 作为输入，
    //实现 de Casteljau 算法来返回 Bézier 曲线上对应点的坐标。

    //如果序列只包含一个点，则返回该点并终止。
    if (control_points.size() == 1) return control_points[0];

    
    //以下为递归算法
    std::vector<cv::Point2f>control_points_temp;
    for (int i = 0; i < control_points.size() - 1; i++)
    {
        //(1 − t) 的比例细分每个线段，并找到该分割点。
        control_points_temp.push_back(control_points[i] * (1 - t) + control_points[i + 1] * t);
    }
    //对算出的点进行递归,得到的新分割点作为新的控制点序列，新序列的长度会减少 1
    return recursive_bezier(control_points_temp, t);

    //return cv::Point2f();

}

void bezier(const std::vector<cv::Point2f>& control_points, cv::Mat& window)
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    //该函数实现绘制 Bézier 曲线的功能。它使用一个控制点序列
    //和一个OpenCV::Mat 对象作为输入，没有返回值。
    //它会使 t 在 0 到 1 的范围内进行迭代，
    //并在每次迭代中使 t 增加一个微小值。对于每个需要计算的 t，
    //将调用另一个函数 recursive_bezier，然后该函数将返回在 Bézier曲线上 t 处的点。
    //最后，将返回的点绘制在 OpenCV::Mat 对象上
    for (float t = 0.0; t <= 1.0; t = t + 0.001) 
    {
        cv::Point2f point = recursive_bezier(control_points, t);
        window.at<cv::Vec3b>(point.y, point.x)[1] = 255;
        //使用 window.at<cv::Vec3b>(point.y, point.x)[1] = 255 将绿色的线画在屏幕上。

        //反走样（抗锯齿）
        bool Antialiasing = true;
        if (Antialiasing)
        {
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    auto pixel_x = floor(point.x) + i;
                    auto pixel_y = floor(point.y) + j;
                    auto dis_x = abs(point.x - pixel_x - 0.5);
                    auto dis_y = abs(point.y - pixel_y - 0.5);
                    auto d = sqrt(dis_x * dis_x + dis_y * dis_y);
                    //(每个像素的颜色是255*ratio，d的范围是[0,3/√2]，ratio的范围是[0,1]，
                    // 那么ratio关于d的函数就是ratio=1-√2/3d)
                    // max distance is 3/sqrt(2)
                    window.at<cv::Vec3b>(pixel_y, pixel_x)[1] = std::max(255 * (1 - d * (1.414 / 3.0)),
                        (double)window.at<cv::Vec3b>(pixel_y, pixel_x)[1]);
                }
            }
        }
        



    }


}

int main()
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27)
    {
        window.setTo(0);
        for (auto& point : control_points)
        {
            cv::circle(window, point, 3, { 255, 255, 255 }, 3);
        }

        if (control_points.size() >= 4)
        {
            //注释掉 main 函数中while 循环内调用 naive_bezier 函数的行，
            // 并取消对 bezier 函数的注释。要求你的实现将Bézier 曲线绘制为绿色
            //naive_bezier(control_points, window);
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(1);

        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

    return 0;
}
