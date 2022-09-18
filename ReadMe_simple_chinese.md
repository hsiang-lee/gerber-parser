![GerberParser](./img/logo.png)

[English](./ReadMe.md)

# GerberParser

[![MacOS](https://github.com/hsiang-lee/gerber-parser/actions/workflows/macos.yml/badge.svg)](https://github.com/hsiang-lee/gerber-parser/actions/workflows/macos.yml)
[![Ubuntu](https://github.com/hsiang-lee/gerber-parser/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/hsiang-lee/gerber-parser/actions/workflows/ubuntu.yml)
[![Windows](https://github.com/hsiang-lee/gerber-parser/actions/workflows/windows.yml/badge.svg)](https://github.com/hsiang-lee/gerber-parser/actions/workflows/windows.yml)



RenderParser是一个开源的、支持跨平台编译、简单易用的Gerber文件渲染库，采用标准c++编写，支持rs274x标准。渲染引擎使用Qt编写，支持渲染Gerber文件到Qt支持的图像导出格式:


Format|Description|Qt's support
---|:--:|---:
BMP|Windows Bitmap|Read/write
GIF|Graphic Interchange Format (optional)|Read
JPG|Joint Photographic Experts Group|Read/write
JPEG|Joint Photographic Experts Group|Read/write
PNG|Portable Network Graphics|Read/write
PBM|Portable Bitmap|Read
PGM|Portable Graymap|Read
PPM|Portable Pixmap|Read/write
XBM|X11 Bitmap|Read/write
XPM|X11 Pixmap|Read/write

也支持直接渲染到QWidget窗口上。

另外，Qt还提供了QSvgGenerator和QPdfWriter，它们也继承自QPaintDevice。理论上也可以渲染成sgv和pdf导出，不过目前没有直接支持。
除了渲染和导出静态的图像，GerberParser库也支持一些交互式的操作。比如移动图像的位置，缩放图像和选中某一点高亮显示。这些功能可以用于交互式Gerber显示，参考example/gerber_viewer。也可用于较大图像的分片段渲染和导出，example/gerber2image提供了示例。对于分辨率较高的图像，要导出成一张图像会受到诸多限制，example/gerber2image通过分段导出，导出成多张图像，每张分辨率最高20000，合起来构成了完整的图像。



# 支持平台
GerberParser采用标准C++编写，依赖的Qt5、glog、gflags都支持跨平台编译。目前已测试过在Windows和Mac上正常编译过，Ubuntu上还存在一些编译问题。



# 渲染样图
![gerber image](./logo/gerber.png)



# 如何构建？

## 依赖库：
- Qt5(没测过其他版本，不能确定支不支持，看Qt的兼容性)
- glog(用于记录日志)
- gflags(用于example中的示例程序解析cui参数，如果关闭“BUILD_EXAMPLE”选项则不需要这个依赖)
- googletest(用于测试，如果关闭选项“BUILD_TESTS”则不需要这个依赖)
其中Qt5需要外部安装，CMake时指定Qt安装路径，或通过CMake-GUI设置Qt安装路径，或者机器上设置了Qt的环境变量，CMake能自动找到。
glog、gflags和googletest已通过git的submodule自包含，无需外部提供。


## 克隆仓库：
```
git clone https://github.com/hsiang-lee/gerber-parser
git submodule update --recursive --init
```


## CMake:
```
cd gerber-parser
mkdir build
cd build
cmake .. -G "NMake Makefiles" -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=ON -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=OFF -DQt5_DIR=C:/Qt/Qt5.14.1/5.14.1/msvc2017_64/lib/cmake/Qt5
```


## 构建
```nmake```



# 如何使用？
```
auto parser = std::make_shared<GerberParser>(gerber_file_path);
auto gerber = parser->GetGerber();
//构建一个Gerber，这里会解析gerber文件。

auto image = std::make_unique<QBitmap>(1920， 1280);//创建一张二值位图
auto engine = std::make_unique<QtEngine>(image.get());//构建一个QtEngine
engin->RenderGerber(gerber);//渲染gerber文件，结果会渲染到上面创建的二值位图上

image->save(path_you_want_to_save_image);//保存图像
```



# 附带工具
GerberParser也附带了一些简单的工具，在example目录下。CMake时设置BUILD_EXAMPLES=ON打开构建example下的工具。
* gerber_viewer	一个简单的gerber预览工具，可以缩放，拖动，选中元素(存在问题，尚未完全支持)
* gerber2image	一个导出gerber文件到二值位图的工具，提供cui接口，通过“--help”选项可以查看帮助

# 捐赠
如果这个开源仓库帮助到了你，你也可以请作者喝瓶饮料🥤，谢谢！
![alipay](./img/alipay.jpeg)
![wechatpay](./img/wechatpay.jpeg)
