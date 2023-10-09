# 图形学第二次作业

## 作业要求：

在本次作业中，你需要修改 **main.cpp** 中的的函数：

**bezier：**该函数实现绘制 Bézier 曲线的功能。它使用一个控制点序列和一个

OpenCV::Mat 对象作为输入，没有返回值。它会使 t 在 0 到 1 的范围内进行

迭代，并在每次迭代中使 t 增加一个微小值。对于每个需要计算的 t，将调用

另一个函数 recursive_bezier，然后该函数将返回在 Bézier 曲线上 t 处的点。

最后，将返回的点绘制在 OpenCV::Mat 对象上。

**recursive_bezier：**该函数使用一个控制点序列和一个浮点数 t 作为输入，实

现 de Casteljau 算法来返回 Bézier 曲线上对应点的坐标。

## 作业完成情况：

### 1.实现recursive_bezier函数

bezier函数中需要调用函数 recursive_bezier，所以先实现该函数的功能，该函数需要使用到De Casteljau 算法

#### De Casteljau 算法:

1.考虑一个 p0, p1, ... pn 为控制点序列的 Bézier 曲线。首先，将相邻的点连接起来以形成线段。

2.用 t : (1 − t) 的比例细分每个线段，并找到该分割点。

3.得到的分割点作为新的控制点序列，新序列的长度会减少 1

4.如果序列只包含一个点，则返回该点并终止。否则，使用新的控制点序列并 转到步骤 1。使用 [0,1] 中的多个不同的 t 来执行上述算法，你就能得到相应的 Bézier 曲线

t从0到1进行插值得到曲线。递归重复上述过程。
其核心就是多次的线性插值，并在生成的新的顶点所连接构成的线段之上递归的执行这个过程，直到得到最后一个顶点。

以下为贝塞尔曲线为4个控制点的时的公式

![贝塞尔曲线4点公式](C:\Users\Administrator\Desktop\作业2\贝塞尔曲线4点公式.png)

其中作业自带的naive_bezier函数就是直接将四点代入该公式中直接获取结果。

```
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
```



而根据说明中对于De Casteljau的描述，我们需要利用到递归算法

##### 根据要求1，生成一个控制点序列

该容器用于储存新生成的控制点序列

```std::vector<cv::Point2f>control_points_temp;```

##### 根据要求2，用 t : (1 − t) 的比例细分每个线段，并找到该分割点

其中***push_back()***函数的用法

函数将一个新的元素加到vector的最后面，位置为当前最后一个元素的下一个元素

```
for (int i = 0; i < control_points.size() - 1; i++)
{
    //(1 − t) 的比例细分每个线段，并找到该分割点。
    control_points_temp.push_back(control_points[i] * (1 - t) + control_points[i + 1] * t);
}
```

以下算术式来利用到了二阶贝塞尔公式

```control_points[i] * (1 - t) + control_points[i + 1] * t)```



如下图所示，由二阶贝塞尔公式（2个控制点）求得三阶贝塞尔公式（3个控制点）的过程，通过观察可以发现该公式具有一定的规律，利用递归可以实现题目所要求的四个控制点的贝塞尔曲线

![](C:\Users\Administrator\Desktop\作业2\贝塞尔公式3点案例.png)

##### 根据要求3，得到的分割点作为新的控制点序列，新序列的长度会减少 1。

control_points_temp为

```    return recursive_bezier(control_points_temp, t);```

##### 根据要求4，其中如果序列只包含一个点，则返回该点并终止

当算法递归到序列只剩下一个点时函数结束

在函数开头加入以下判断

``` if (control_points.size() == 1) return control_points[0];```

以下为recursive_bezier函数的完整代码段

```
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
```

### 2.实现bezier函数

#### 函数要求：

该函数实现绘制 Bézier 曲线的功能。它使用一个控制点序列和一个

OpenCV::Mat 对象作为输入，没有返回值。它会使 t 在 0 到 1 的范围内进行

迭代，并在每次迭代中使 t 增加一个微小值。对于每个需要计算的 t，将调用

另一个函数 recursive_bezier，然后该函数将返回在 Bézier 曲线上 t 处的点。

最后，将返回的点绘制在 OpenCV::Mat 对象上。

#### 函数实现：

它会使 t 在 0 到 1 的范围内进行迭代，并在每次迭代中使 t 增加一个微小值。

由此可以得到，应建立一个以t为变量的循环并将0.001作为微小的量进行迭代

在循环中调用recursive_bezier将返回的点变量存于二维点变量point中。

根据作业提示

**注：你可以使用** **window.at<cv::Vec3b>(point.y, point.x)[1] = 255** **将绿色的线画在屏幕上。**

通过该代码直接将所得点坐标信息对应地址进行填色

```
for (float t = 0.0; t <= 1.0; t = t + 0.001) 
{
    cv::Point2f point = recursive_bezier(control_points, t);
    window.at<cv::Vec3b>(point.y, point.x)[1] = 255;
    
}
```

根据作业中的要求：

**注释掉 main 函数中while 循环内调用 naive_bezier 函数的行，**
**并取消对 bezier 函数的注释。要求你的实现将Bézier 曲线绘制为绿色**

运行后得到如下绿色走样的贝塞尔曲线

![](C:\Users\Administrator\Desktop\102101342_蔡嘉钦_作业2\走样绿色贝塞尔曲线.png)

根据作业要求：

**如果要确保实现正确，请同时调用 naive_bezier 和 bezier 函数，如果实现正确，则两者均应写入大致相同的像素，因此该曲线将表现为黄色。**

运行后的到如下黄色曲线

![](C:\Users\Administrator\Desktop\102101342_蔡嘉钦_作业2\走样黄色贝塞尔曲线.png)

根据作业要求：

**在这之后，你需要使用不同数量的控制点 (大于四个)，来查看不同的 Bézier 曲线。**

绘制五点贝塞尔曲线

![](C:\Users\Administrator\Desktop\102101342_蔡嘉钦_作业2\5点贝塞尔曲线.png)

#### 贝塞尔曲线的反走样：

作业要求如下：

**你需要实现对 Bézier 曲线的反走样。因为用零散的像素点来描述曲线，会因为点之间的距离或一些错位导致走样问题。所以对于一个曲线上的点，不只把它对应于一个像素，你需要根据到像素中心的距离来考虑与它相邻的像素的颜色**

此要求与作业一中的抗锯齿类似

每得到一个曲线上的点时，我们就根据它到自己周围的3×3个像素中心的距离d来为这些像素填色以达到平滑过渡的效果(**每个像素的颜色是255\*ratio，d的范围是[0,3/√2]，ratio的范围是[0,1]，那么ratio关于d的函数就是ratio=1-√2/3d**)其中d为每个点各自到像素中心的距离，离得越远颜色越淡。

通过循环遍历像素中心周围九格，对线段每一个像素点进行反走样，重复计算的点选择颜色最大值即可避免暗点出现导致锯齿感明显

```python
for (int i = -1; i <= 1; i++)
{
    for (int j = -1; j <= 1; j++)
    {
        auto pixel_x = floor(point.x) + i;
        auto pixel_y = floor(point.y) + j;
        auto dis_x = abs(point.x - pixel_x - 0.5);
        auto dis_y = abs(point.y - pixel_y - 0.5);
        auto d = sqrt(dis_x * dis_x + dis_y * dis_y);
        window.at<cv::Vec3b>(pixel_y, pixel_x)[1] = std::max(255 * (1 - d * (1.414 / 3.0)),
            (double)window.at<cv::Vec3b>(pixel_y, pixel_x)[1]);
    }
}
```

运行后结果如下：

![](C:\Users\Administrator\Desktop\102101342_蔡嘉钦_作业2\反走样贝塞尔曲线.png)

参考链接如下：https://blog.csdn.net/ycrsw/article/details/124117190?ops_request_misc=&request_id=&biz_id=102&utm_term=game101%20B%C3%A9zier%20%E6%9B%B2%E7%BA%BF%E5%8F%8D%E8%B5%B0%E6%A0%B7&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-0-124117190.142

