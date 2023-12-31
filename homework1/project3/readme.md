# interactivewindow.cpp 代码说明

这份代码是用于在 OpenGL 环境中绘制几何图形。它包含了一些控制OPENGL窗口的功能，会通过处理鼠标点击事件和移动事件来实现不同的功能

## 主要功能


1. **绘制线**：遍历 `lines` 数组中的每一条线，并使用 OpenGL 的 `GL_LINES `或者` GL_LINE_STRIP` 模式来绘制线。

2. **绘制矩形**：遍历 `rectangles` 数组中的每一个矩形，并使用 OpenGL 的`GL_POLYGON`模式来绘制矩形。

3. **绘制三角形**：遍历 `triangles` 数组中的每一个三角形，并使用 OpenGL 的 `GL_POLYGON` 模式来绘制三角形。

4. **绘制圆形**： 遍历 `circles` 数组中的每一个圆形，并使用 OpenGL 的 `GL_POLYGON`和`GL_LINE_LOOP` 模式来绘制圆形。

5. **鼠标操作**：`mouse` 函数用于处理鼠标点击事件，将点击的屏幕坐标转换为 OpenGL 的世界坐标，并根据当前的绘制模式和形状来添加点或创建新的几何图形。
`motion`函数则是通过鼠标的移动来记录相关的点，最后一起绘制出来，从而实现连续的直线或曲线。

6. **菜单操作**：`menu` 函数用于处理菜单的点击事件，根据用户的选择来改变背景颜色、绘制颜色、绘制模式和绘制形状，或清除所有的几何图形。

## 菜单说明

这份代码中的 `menu` 函数用于创建一个右键菜单，包含以下子菜单和选项：

1. **背景颜色**：用户可以选择红色、绿色、蓝色、白色、黑色、黄色、洋红色和青色作为背景颜色。

2. **选择绘制颜色**：用户可以选择白色、红色、橙色、黄色、绿色、蓝色、靛蓝色和紫罗兰色作为绘制颜色。

3. **选择形状**：用户可以选择矩形、三角形和圆形作为绘制的形状。
### 数据结构说明

- **线（Line）**：线由一系列的点和颜色以及绘制模式组成。点是由一对浮点数表示的二维坐标，存储在一个向量中。颜色和绘制模式都是整数。

- **矩形（Rectangle）**：矩形由左上角和右下角的坐标以及颜色组成。坐标是由一对浮点数表示的二维坐标。颜色是一个整数。

- **三角形（Triangle）**：三角形由三个点的坐标和颜色组成。每个点的坐标是由一对浮点数表示的二维坐标。颜色是一个整数。

- **圆形（Circle）**：圆形由一个中心点的坐标、一个半径和颜色组成。中心点的坐标是由一对浮点数表示的二维坐标。半径是一个浮点数。颜色是一个整数。

```
struct Line {
    std::vector<std::pair<float, float>> points;  // 线由一系列的点组成，点是由一对浮点数表示的二维坐标，存储在一个向量中。
    int color;  // 颜色是一个整数。
    int drawmod;  // 绘制模式是一个整数。
};

struct Rectangle {
    std::pair<float, float> topLeft;  // 矩形的左上角坐标，由一对浮点数表示的二维坐标。
    std::pair<float, float> bottomRight;  // 矩形的右下角坐标，由一对浮点数表示的二维坐标。
    int color;  // 颜色是一个整数。
};

struct Triangle {
    std::pair<float, float> point1;  // 三角形的第一个点的坐标，由一对浮点数表示的二维坐标。
    std::pair<float, float> point2;  // 三角形的第二个点的坐标，由一对浮点数表示的二维坐标。
    std::pair<float, float> point3;  // 三角形的第三个点的坐标，由一对浮点数表示的二维坐标。
    int color;  // 颜色是一个整数。
};

struct Circle {
    std::pair<float, float> center;  // 圆形的中心点的坐标，由一对浮点数表示的二维坐标。
    float radius;  // 半径是一个浮点数。
    int color;  // 颜色是一个整数。
};
```
- 绘制矩形是通过点击后拖动鼠标记录一条对角线，最后通过这条对角线来实现矩形的绘制，绘制矩形时通过对应的数据结构来保存。

- 绘制三角形是通过点击三个点，从而记录位置后，填充即可生成一个三角形。

- 绘制圆和矩形一样，通过拖动产生的直线定义其半径，从而绘制出一个圆形。但是它会额外绘制一个边框，以显示其轮廓。

- 另外还有直线绘制和自由绘制模式，这两个模式共用线的数据结构，但是是不同的绘制模式，从而实现绘制直线和任意曲线。

4. **清屏**：用户可以选择这个选项来清除屏幕上的所有图形。

5. **绘制模式**：用户可以选择线条绘制、自由绘制或禁用绘制。禁用绘制后将不能绘制任何图形。

菜单通过 GLUT 的 `glutCreateMenu`、`glutAddMenuEntry`、`glutAddSubMenu` 和 `glutAttachMenu` 函数来创建。它会在用户右键点击时显示。

## 使用方法

在已经配置好了相关库和环境后。利用```g++ interactivewindow.cpp -lglut -lGLU -lGL -o interactivewindow1```命令来编译该cpp文件，并执行该程序来创建一个交互式窗口。

## 注意事项

- 注意自由绘制和直线绘制两种模式是互斥的，也就是它们互相切换时会清除原本的线，这是因为它们共用一个数据结构。

- 由于diplay模式下，是按顺序绘制各种图形的，因此线可能会被其他图形覆盖，无法显示出来。这一点可以通过重新设置display内的绘制逻辑来更改，但由于大局已定，我就没有添加这个功能了。

- 绘制圆模式下，开启菜单点击某些按钮时可能会默认绘制一个圆，只可能是圆绘制设计上的问题，目前我仍未找到原因。

- 由于代码是不断往后扩展的，需要新功能就直接增加新功能，因此某些变量可能并没有作用，可以忽略不计！也就是它仍然有许多bug，但是我还还没有修复完毕。

## 版本说明
- 2023/11/7 完成第一版