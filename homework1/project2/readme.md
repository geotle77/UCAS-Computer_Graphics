## 项目名称
利用OPENGL库实现基本图形绘制，本项目实现的是一个带颜色填充的小电脑图案。

## 函数说明

代码中定义了一些基本的图形绘制函数，如绘制直线、矩形、圆形、圆角矩形、多边形、星形、梯形等。最后在display函数中，使用这些基本的图形绘制函数，组合成一个计算机的图形。

以下是代码的详细说明：

- 首先，代码定义了一些颜色常量，如GREY、DIMGREY、GOLDEN、RED和BLUE。

- drawLineBresenham函数使用Bresenham算法绘制一条直线。

- drawRectangle函数绘制一个矩形。

- drawArc函数绘制一个圆弧。

- drawRoundedRectangle函数绘制一个圆角矩形。

- drawSymmetricPolygon函数绘制一个对称的多边形。

- drawStar函数绘制一个星形。比如说五角星和七角星等，N必须为奇数，如果是偶数，效果则是和多边形一致。

- drawCircle函数绘制一个圆形。

- drawTrapezoid函数绘制一个梯形。

- displaychar函数在屏幕上显示一串字符。

- display函数是主要的绘图函数，它首先设置背景颜色，然后使用上述的图形绘制函数，绘制出一个计算机的图形。

## 使用方法
安装好相应库后，利用```g++ interactivewindow.cpp -lglut -lGLU -lGL -o interactivewindow```编译程序，执行程序后可以看到绘制的图案。

## 注意事项
本函数实现的是一个带颜色填充的绘制程序，可以通过更改display中对应的绘制颜色和绘制模式来更改其为填充或是线框，为例使图案边界更加明显，这里两种绘制模式都使用了。

注意，绘制的图案没有任何引申意味，不代表任何含义！！！
## 更新说明
- 2023/11/7完成初版