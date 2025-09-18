# 图片水印命令行程序

## 项目简介

这是一个基于C++和GDI+开发的命令行工具，用于读取图片的EXIF信息中的拍摄时间，并将其作为水印添加到图片上。用户可以自定义水印的字体大小、颜色和位置，处理后的图片将保存在原目录的子目录中。

## 功能特点

- 读取图片文件的EXIF信息，提取拍摄时间（年月日部分）作为水印
- 自定义水印字体大小
- 自定义水印颜色（使用RGB十六进制值）
- 自定义水印位置（左上角、居中、右下角）
- 自动创建保存目录
- 保持原图片格式和质量
- 支持无EXIF信息的图片（使用当前日期作为水印）

## 编译方法

本项目使用Visual Studio编译器编译。请确保您的系统中已安装Visual Studio。

### 使用提供的批处理脚本编译

1. 打开命令提示符（CMD）
2. 导航到项目目录
3. 运行以下命令，替换路径为您的Visual Studio安装路径：
   ```
   build.bat "C:\Program Files\Microsoft Visual Studio\2022\Community"
   ```

### 手动编译

1. 打开Visual Studio开发人员命令提示符
2. 导航到项目目录
3. 运行以下命令：
   ```
   cl /EHsc photo_watermark.cpp gdiplus.lib user32.lib
   ```

## 使用说明

编译成功后，您将得到一个名为`photo_watermark.exe`的可执行文件。

### 基本命令格式

```
photo_watermark <image_path> [options]
```

### 参数说明

- `<image_path>`: 必需参数，指定图片文件路径
- `--font-size <size>`: 可选参数，设置水印字体大小，默认值为12
- `--color <color_code>`: 可选参数，设置水印颜色，格式为RGB十六进制值（如FF0000表示红色），默认值为黑色（000000）
- `--position <position>`: 可选参数，设置水印位置，可选值为：top-left（左上角）、center（居中）、bottom-right（右下角）等，默认值为右下角
- `--help`: 显示帮助信息

## 示例

### 基本使用（使用默认设置）

```
photo_watermark ./photos/sample.jpg
```

### 自定义字体大小和颜色

```
photo_watermark ./photos/sample.jpg --font-size 16 --color FF0000
```

### 自定义位置

```
photo_watermark ./photos/sample.jpg --position center
```

### 组合使用所有选项

```
photo_watermark ./photos/sample.jpg --font-size 16 --color FF0000 --position top-left
```

## 输出示例

处理成功后，程序将输出类似以下信息：

```
处理成功！
原图路径：./photos/sample.jpg
水印图片保存路径：./photos_watermark/sample_watermark.jpg
水印内容：2023-10-15
```

## 注意事项

1. 本程序目前仅支持Windows操作系统，因为它使用了Windows特有的GDI+库
2. 程序支持常见的图片格式，如JPEG、PNG等
3. 对于没有EXIF信息的图片，程序会使用当前日期作为水印
4. 程序需要读写文件的权限
5. 处理大图片时可能会消耗较多内存

## 开发说明

本项目是根据PRD文档实现的，严格遵循了不依赖额外需要安装的库的要求，仅使用了Windows系统自带的GDI+库和标准C++库。

## License

请参见项目根目录下的LICENSE文件。
