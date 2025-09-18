#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <regex>

// 链接GDI+库
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
using namespace std;

// 解析命令行参数
struct CommandLineArgs {
    string imagePath;
    int fontSize = 12;
    string colorCode = "000000";
    string position = "bottom-right";
    bool showHelp = false;
};

// 解析命令行参数的函数
CommandLineArgs parseCommandLine(int argc, wchar_t* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; i++) {
        wstring arg = argv[i];
        string argStr(arg.begin(), arg.end());
        
        if (argStr == "--help" || argStr == "-h") {
            args.showHelp = true;
        }
        else if (argStr.substr(0, 11) == "--font-size") {
            if (i + 1 < argc) {
                args.fontSize = stoi(string(argv[i + 1], argv[i + 1] + wcslen(argv[i + 1])));
                i++;
            }
        }
        else if (argStr.substr(0, 7) == "--color") {
            if (i + 1 < argc) {
                args.colorCode = string(argv[i + 1], argv[i + 1] + wcslen(argv[i + 1]));
                i++;
            }
        }
        else if (argStr.substr(0, 10) == "--position") {
            if (i + 1 < argc) {
                args.position = string(argv[i + 1], argv[i + 1] + wcslen(argv[i + 1]));
                i++;
            }
        }
        else if (args.imagePath.empty()) {
            args.imagePath = argStr;
        }
    }
    
    return args;
}

// 显示帮助信息
void showHelp() {
    cout << "使用方法: photo_watermark <image_path> [options]\n" << endl;
    cout << "选项:\n";
    cout << "  --font-size <size>    设置水印字体大小，默认值为12\n";
    cout << "  --color <color_code>  设置水印颜色，格式为RGB十六进制值（如FF0000表示红色），默认值为黑色（000000）\n";
    cout << "  --position <position> 设置水印位置，可选值为：top-left（左上角）、center（居中）、bottom-right（右下角）等，默认值为右下角\n";
    cout << "  --help               显示帮助信息\n";
}

// 从EXIF信息中提取拍摄时间
string getDateTimeFromExif(const wstring& imagePath) {
    string dateTime = "";
    
    // 初始化GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    
    Image* image = new Image(imagePath.c_str());
    
    if (image) {
        UINT count = 0;
        UINT size = 0;
        
        // 获取属性项数量
        image->GetPropertyCount(&count);
        
        if (count > 0) {
            // 获取所有属性项
            PropertyItem* items = new PropertyItem[count];
            image->GetAllPropertyItems(size, &count, items);
            
            // 查找日期时间属性
            for (UINT i = 0; i < count; i++) {
                // EXIF DateTimeOriginal属性ID为0x9003
                if (items[i].id == 0x9003) {
                    dateTime = reinterpret_cast<char*>(items[i].value);
                    break;
                }
            }
            
            delete[] items;
        }
        
        delete image;
    }
    
    // 关闭GDI+
    GdiplusShutdown(gdiplusToken);
    
    // 提取年月日部分 (格式: YYYY:MM:DD HH:MM:SS)
    if (!dateTime.empty()) {
        regex dateRegex("(\\d{4}):(\\d{2}):(\\d{2})");
        smatch match;
        if (regex_search(dateTime, match, dateRegex)) {
            if (match.size() >= 4) {
                return match[1].str() + "-" + match[2].str() + "-" + match[3].str();
            }
        }
    }
    
    // 如果没有EXIF信息，返回当前日期
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buffer[20];
    sprintf_s(buffer, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
    return buffer;
}

// 将十六进制颜色代码转换为GDI+ Color对象
Color hexToColor(const string& hexCode) {
    // 默认返回黑色
    if (hexCode.length() != 6) {
        return Color(255, 0, 0, 0);
    }
    
    int r = stoi(hexCode.substr(0, 2), nullptr, 16);
    int g = stoi(hexCode.substr(2, 2), nullptr, 16);
    int b = stoi(hexCode.substr(4, 2), nullptr, 16);
    
    return Color(255, r, g, b);
}

// 添加水印到图片
bool addWatermarkToImage(const wstring& imagePath, const string& watermarkText, int fontSize, const Color& color, const string& position) {
    // 初始化GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    
    bool result = false;
    
    Image* image = new Image(imagePath.c_str());
    
    if (image && image->GetLastStatus() == Ok) {
        // 创建图形对象
        Bitmap* bitmap = new Bitmap(image->GetWidth(), image->GetHeight(), PixelFormat32bppARGB);
        Graphics* graphics = Graphics::FromImage(bitmap);
        
        // 绘制原图
        graphics->DrawImage(image, 0, 0);
        
        // 创建字体
        Font* font = new Font(L"Arial", fontSize, FontStyleRegular, UnitPixel);
        
        // 创建画刷
        SolidBrush* brush = new SolidBrush(color);
        
        // 计算文本大小
        RectF boundingBox;
        graphics->MeasureString(wstring(watermarkText.begin(), watermarkText.end()).c_str(), watermarkText.length(), font, PointF(0, 0), &boundingBox);
        
        // 确定水印位置
        PointF textPosition;
        int margin = 20;
        
        if (position == "top-left") {
            textPosition.X = margin;
            textPosition.Y = margin;
        }
        else if (position == "center") {
            textPosition.X = (image->GetWidth() - boundingBox.Width) / 2;
            textPosition.Y = (image->GetHeight() - boundingBox.Height) / 2;
        }
        else if (position == "bottom-right") {
            textPosition.X = image->GetWidth() - boundingBox.Width - margin;
            textPosition.Y = image->GetHeight() - boundingBox.Height - margin;
        }
        else {
            // 默认右下角
            textPosition.X = image->GetWidth() - boundingBox.Width - margin;
            textPosition.Y = image->GetHeight() - boundingBox.Height - margin;
        }
        
        // 绘制文本水印
        graphics->DrawString(wstring(watermarkText.begin(), watermarkText.end()).c_str(), 
                           watermarkText.length(), 
                           font, 
                           textPosition, 
                           brush);
        
        // 创建保存目录
        wstring imageDir, imageName, imageExt;
        size_t lastSlash = imagePath.find_last_of(L"\\/");
        size_t lastDot = imagePath.find_last_of(L".");
        
        if (lastSlash != wstring::npos) {
            imageDir = imagePath.substr(0, lastSlash + 1);
        }
        
        if (lastDot != wstring::npos) {
            imageName = imagePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
            imageExt = imagePath.substr(lastDot);
        }
        else {
            imageName = imagePath.substr(lastSlash + 1);
        }
        
        // 创建输出目录
        wstring outputDir = imageDir + L"_watermark";
        CreateDirectoryW(outputDir.c_str(), NULL);
        
        // 保存带有水印的图片
        wstring outputPath = outputDir + L"\\" + imageName + L"_watermark" + imageExt;
        CLSID clsid;
        
        // 设置图片格式
        if (imageExt == L".jpg" || imageExt == L".jpeg") {
            CLSIDFromString(L"{557cf400-1a04-11d3-9a73-0000f81ef32e}", &clsid); // JPEG
        }
        else if (imageExt == L".png") {
            CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &clsid); // PNG
        }
        else {
            // 默认使用JPEG格式
            CLSIDFromString(L"{557cf400-1a04-11d3-9a73-0000f81ef32e}", &clsid);
            outputPath = outputDir + L"\\" + imageName + L"_watermark.jpg";
        }
        
        if (bitmap->Save(outputPath.c_str(), &clsid, NULL) == Ok) {
            // 输出结果信息
            cout << "处理成功！" << endl;
            cout << "原图路径：" << string(imagePath.begin(), imagePath.end()) << endl;
            cout << "水印图片保存路径：" << string(outputPath.begin(), outputPath.end()) << endl;
            cout << "水印内容：" << watermarkText << endl;
            result = true;
        }
        else {
            cout << "保存图片失败！" << endl;
        }
        
        // 释放资源
        delete brush;
        delete font;
        delete graphics;
        delete bitmap;
    }
    else {
        cout << "无法打开图片文件！" << endl;
    }
    
    delete image;
    
    // 关闭GDI+
    GdiplusShutdown(gdiplusToken);
    
    return result;
}

int wmain(int argc, wchar_t* argv[]) {
    // 解析命令行参数
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    // 显示帮助信息
    if (args.showHelp || args.imagePath.empty()) {
        showHelp();
        return 0;
    }
    
    // 转换图片路径为宽字符
    wstring imagePath(args.imagePath.begin(), args.imagePath.end());
    
    // 检查文件是否存在
    if (GetFileAttributesW(imagePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        cout << "文件不存在：" << args.imagePath << endl;
        return 1;
    }
    
    // 从EXIF信息中提取拍摄时间
    string watermarkText = getDateTimeFromExif(imagePath);
    
    // 转换颜色代码
    Color color = hexToColor(args.colorCode);
    
    // 添加水印到图片
    bool success = addWatermarkToImage(imagePath, watermarkText, args.fontSize, color, args.position);
    
    return success ? 0 : 1;
}