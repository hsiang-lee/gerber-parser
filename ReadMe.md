<h1 align="center">Welcome to GerberParser 👋</h1>

![GerberParser](./img/logo.png)

[English](./ReadMe_English.md)

<p>
  <img alt="Version" src="https://img.shields.io/badge/version-v1.0-blue.svg?cacheSeconds=2592000" />
  <a href="https://github.com/hsiang-lee/gerber-parser" target="_blank">
    <img alt="Documentation" src="https://img.shields.io/badge/documentation-yes-brightgreen.svg" />
  </a>
  <a href="https://github.com/hsiang-lee/gerber-parser/blob/master/LICENSE" target="_blank">
    <img alt="License: LICENSE" src="https://img.shields.io/badge/License-LICENSE-yellow.svg" />
  </a>

[![MacOS](https://github.com/hsiang-lee/gerber-parser/actions/workflows/macos.yml/badge.svg)](https://github.com/hsiang-lee/gerber-parser/actions/workflows/macos.yml)
[![Ubuntu](https://github.com/hsiang-lee/gerber-parser/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/hsiang-lee/gerber-parser/actions/workflows/ubuntu.yml)
[![Windows](https://github.com/hsiang-lee/gerber-parser/actions/workflows/windows.yml/badge.svg)](https://github.com/hsiang-lee/gerber-parser/actions/workflows/windows.yml)
</p>

GerberParser是一个gerber文件解析库。采用标准c++编写，支持rs274x标准。gerber文件解析部分提供预编译好的动态链接库，包含Windows,Macos和Linux三大平台的。如果你需要其他平台或环境下的库，请联系作者(联系方式见后文)。图像渲染部分默认提供Qt版本。你可以直接绘制到Qt的QWidget上，也可以绘制到Qt的图像上，然后导出成图片文件。Qt支持的图像格式都支持，包括BMP、GIF、JPEG、PNG等，利用QSvgGenerator和QPDFWriter，你也可以将gerber文件绘制到svg格式的图像或pdf格式的文件中，具体请查阅Qt的文档。

仓库example目录下提供了两个示例程序。一个用来将gerber文件导出成图像文件，支持超高分辨率的导出，对于低分辨率的会导出成一个文件，对于高分辨率(分辨率高到超出了限制)的，会自动切割，导出成多个片段。另外一个是gerber_viewer，它是一个交互式的界面程序，用来预览gerber文件，支持简单的放大缩小的交互。如果你想编译example程序，请在CMake时设置"BUILD_EXAMPLES"的选项。

### 渲染样图
![gerber image](./img/gerber.png)

### ✨ [Demo](https://github.com/hsiang-lee/gerber-parser/tree/master/example)

## 安装
### 依赖库
Qt，请在你的电脑上预先安装Qt库,请到Qt官网下载安装。目前默认使用的是Qt6。


### 克隆代码：
```
git clone https://github.com/hsiang-lee/gerber-parser //克隆代码仓库到本地
git submodule update --recursive --init //拉取子模块，会自动拉取依赖的第三方库gfalg、gtest等
```

### CMake: 
你可以使用下面的命令来cmake。或者你也可以使用cmake gui程序来cmake，cmake时如果找不到Qt，请手动设置Qt的路径。
```
cd gerber-parser
mkdir build
cd build
cmake .. -G "NMake Makefiles" -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=OFF -DQt6_DIR=path/of/Qt
```

### 构建:
如果你在终端中编译，使用下面的命令即可。或者你也可以使用IDE来编译(visual studio，vs code，xCode等)。
Linux(Ubuntu) or Mac:
```make```

Windows:
```nmake```

## 使用示例
```
auto parser = std::make_shared<GerberParser>(gerber_file_path);
auto gerber = parser->GetGerber();
//Construct a Gerber object，gerber file will be parsed here。

auto image = std::make_unique<QBitmap>(1920， 1280);//Create a QBitmap(for export bitmap image).Other images can also be exported, eg: QPixmap or QImage.
auto engine = std::make_unique<QPainterEngine>(image.get());//Construct a QtEngine
engin->RenderGerber(gerber);//Render the gerber file to a bitmap image created above

image->save(path_you_want_to_save_image);//Save the image rendered to file.
```
也可参考example中的代码。

## 授权
使用gerber-parser需要获取license授权。未获取license的版本可以使用，但会出现随机错误，并且运行效率较低。获取授权请联系作者，可以获取试用版授权，或永久授权。

## 关于作者

👤 **leehsiang**

* email: leehsiang@hotmail.com
* Website: https://www.cnblogs.com/leehsiang
* Github: [@hsiang-lee](https://github.com/hsiang-lee)
* Wechat: ![lee_hsiang](./img/wechat.jpeg)

## 🤝 贡献代码

欢迎提交issue或功能请求 <br />Feel free to check [issues page](https://github.com/hsiang-lee/gerber-parser/issues). You can also take a look at the [contributing guide](https://github.com/hsiang-lee/gerber-parser).

## 支持作者

记得点赞，给星⭐️ 哟~

### Stargazers
[![Stargazers repo roster for @hsiang-lee/gerber-parser](https://reporoster.com/stars/hsiang-lee/gerber-parser)](https://github.com/hsiang-lee/gerber-parser/stargazers)

### Forkers
[![Forkers repo roster for @hsiang-lee/gerber-parser](https://reporoster.com/forks/hsiang-lee/gerber-parser)](https://github.com/hsiang-lee/gerber-parser/network/members)

## 📝 License

Copyright © 2022 [leehsiang](https://github.com/hsiang-lee).<br />
This project is [MIT license](https://github.com/hsiang-lee/gerber-parser/blob/master/LICENSE) licensed.
