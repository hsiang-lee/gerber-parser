# Python绑定使用指南

本文档介绍如何构建和使用Gerber Parser的Python绑定。

## 🛠️ 构建Python绑定

### 系统要求

- Python 3.6+
- CMake 3.20+
- C++17兼容编译器
- Qt 6.0+
- NumPy (用于图像处理)
- Pillow (用于图像保存)

### 构建步骤

1. **创建构建目录**
   ```bash
   mkdir build
   cd build
   ```

2. **配置项目**
   ```bash
   cmake .. -DBUILD_EXAMPLES=ON
   ```

3. **编译项目**
   ```bash
   make -j$(nproc)
   ```

4. **安装Python模块** (可选)
   ```bash
   make install
   ```

### 验证构建

运行测试脚本验证绑定是否正常工作：

```bash
python3 test_python_binding.py
```

## 📚 Python API参考

### 基本函数

#### `gerber2image(gerber_file, width=800, height=600)`

将Gerber文件渲染为图像数据。

**参数:**
- `gerber_file` (str): Gerber文件路径
- `width` (int): 图像宽度，默认800
- `height` (int): 图像高度，默认600

**返回:**
- `list`: RGBA格式的图像字节数据

**示例:**
```python
import pygerber_parser

# 渲染Gerber文件
image_data = pygerber_parser.gerber2image("board.gerber", 1024, 768)
```

#### `get_gerber_info(gerber_file)`

获取Gerber文件的基本信息。

**参数:**
- `gerber_file` (str): Gerber文件路径

**返回:**
- `GerberInfo`: 包含文件信息的对象

**示例:**
```python
import pygerber_parser

# 获取文件信息
info = pygerber_parser.get_gerber_info("board.gerber")
print(f"文件: {info.filename}")
print(f"尺寸: {info.width} x {info.height}")
print(f"是否有效: {info.is_valid}")
```

### GerberParser类

#### 构造函数
```python
parser = GerberParser(filename)
```

#### 方法

- `is_valid()`: 检查文件是否有效
- `get_bounds()`: 获取边界框 [left, top, right, bottom]
- `get_width()`: 获取宽度
- `get_height()`: 获取高度
- `render_to_image(width=800, height=600)`: 渲染为图像数据

**示例:**
```python
import pygerber_parser

# 创建解析器
parser = pygerber_parser.GerberParser("board.gerber")

# 检查有效性
if parser.is_valid():
    # 获取尺寸信息
    bounds = parser.get_bounds()
    width = parser.get_width()
    height = parser.get_height()
    
    print(f"边界框: {bounds}")
    print(f"尺寸: {width} x {height}")
    
    # 渲染图像
    image_data = parser.render_to_image(1024, 768)
else:
    print("Gerber文件无效")
```

## 🎯 完整使用示例

### 示例1: 基本渲染和保存

```python
import pygerber_parser
import numpy as np
from PIL import Image

def render_gerber_to_png(gerber_file, output_file, width=800, height=600):
    """将Gerber文件渲染为PNG图像"""
    
    # 渲染图像
    image_data = pygerber_parser.gerber2image(gerber_file, width, height)
    
    # 转换为PIL图像
    arr = np.array(image_data, dtype=np.uint8)
    
    # 假设为RGBA格式
    if len(arr) == width * height * 4:
        arr = arr.reshape((height, width, 4))
        img = Image.fromarray(arr, 'RGBA')
    else:
        # 尝试灰度图像
        arr = arr.reshape((height, width))
        img = Image.fromarray(arr, 'L')
    
    # 保存图像
    img.save(output_file)
    print(f"图像已保存: {output_file}")

# 使用示例
render_gerber_to_png("board.gerber", "board.png", 1024, 768)
```

### 示例2: 批量处理

```python
import pygerber_parser
import os

def batch_process_gerber_files(directory, output_dir):
    """批量处理目录中的所有Gerber文件"""
    
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    for filename in os.listdir(directory):
        if filename.lower().endswith(('.gerber', '.gbr', '.gtl', '.gbl', '.gto')):
            filepath = os.path.join(directory, filename)
            
            try:
                # 检查文件有效性
                info = pygerber_parser.get_gerber_info(filepath)
                
                if info.is_valid:
                    print(f"处理: {filename} ({info.width}x{info.height})")
                    
                    # 渲染图像
                    output_file = os.path.join(output_dir, f"{os.path.splitext(filename)[0]}.png")
                    render_gerber_to_png(filepath, output_file)
                else:
                    print(f"跳过无效文件: {filename} - {info.error_message}")
                    
            except Exception as e:
                print(f"处理失败 {filename}: {e}")

# 使用示例
batch_process_gerber_files("gerber_files/", "output_images/")
```

### 示例3: 集成到Web应用

```python
from flask import Flask, send_file, request
import pygerber_parser
import numpy as np
from PIL import Image
import io

app = Flask(__name__)

@app.route('/render-gerber', methods=['POST'])
def render_gerber():
    """Web API: 渲染Gerber文件为图像"""
    
    if 'file' not in request.files:
        return "No file uploaded", 400
    
    file = request.files['file']
    if file.filename == '':
        return "No file selected", 400
    
    # 保存上传的文件
    temp_path = f"/tmp/{file.filename}"
    file.save(temp_path)
    
    try:
        # 渲染图像
        width = int(request.form.get('width', 800))
        height = int(request.form.get('height', 600))
        
        image_data = pygerber_parser.gerber2image(temp_path, width, height)
        
        # 转换为PNG
        arr = np.array(image_data, dtype=np.uint8).reshape((height, width, 4))
        img = Image.fromarray(arr, 'RGBA')
        
        # 返回图像
        img_io = io.BytesIO()
        img.save(img_io, 'PNG')
        img_io.seek(0)
        
        return send_file(img_io, mimetype='image/png')
        
    except Exception as e:
        return f"Error: {e}", 500
    
    finally:
        # 清理临时文件
        if os.path.exists(temp_path):
            os.remove(temp_path)

if __name__ == '__main__':
    app.run(debug=True)
```

## 🔧 故障排除

### 常见问题

1. **导入错误: ModuleNotFoundError**
   - 确保已正确构建项目
   - 检查Python路径是否包含构建目录

2. **Qt相关错误**
   - 确保已安装Qt6开发包
   - 检查Qt环境变量设置

3. **图像格式问题**
   - 当前绑定返回RGBA格式的字节数据
   - 需要手动转换为合适的图像格式

### 调试建议

1. 运行测试脚本验证基本功能
2. 检查系统依赖是否完整
3. 查看构建日志中的警告和错误
4. 使用小尺寸Gerber文件进行测试

## 📈 性能优化

- 对于大尺寸Gerber文件，适当调整渲染尺寸
- 批量处理时使用多线程
- 缓存解析结果避免重复解析

## 🤝 贡献

欢迎为Python绑定贡献代码！主要改进方向：

- 添加更多API绑定
- 改进错误处理
- 优化性能
- 添加单元测试
- 完善文档