#!/usr/bin/env python3
"""
Python绑定测试脚本
用于验证Gerber Parser的Python接口是否正常工作
"""

import sys
import os

# 添加Python绑定路径
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build/src/pygerber-parser'))

def test_basic_import():
    """测试基本导入功能"""
    try:
        import pygerber_parser
        print("✓ pygerber_parser模块导入成功")
        return True
    except ImportError as e:
        print(f"✗ 导入失败: {e}")
        return False

def test_module_info():
    """测试模块信息"""
    try:
        import pygerber_parser
        print(f"✓ 模块文档: {pygerber_parser.__doc__}")
        return True
    except Exception as e:
        print(f"✗ 获取模块信息失败: {e}")
        return False

def test_gerber_info_function():
    """测试Gerber信息函数"""
    try:
        import pygerber_parser
        
        # 测试一个不存在的文件
        info = pygerber_parser.get_gerber_info("nonexistent.gerber")
        print(f"✓ GerberInfo结构测试: {info}")
        print(f"  文件名: {info.filename}")
        print(f"  是否有效: {info.is_valid}")
        print(f"  错误信息: {info.error_message}")
        return True
    except Exception as e:
        print(f"✗ GerberInfo测试失败: {e}")
        return False

def test_gerber_parser_class():
    """测试GerberParser类"""
    try:
        import pygerber_parser
        
        # 创建解析器实例
        parser = pygerber_parser.GerberParser("test.gerber")
        print(f"✓ GerberParser类创建成功: {parser}")
        
        # 测试方法
        is_valid = parser.is_valid()
        print(f"  文件有效性: {is_valid}")
        
        return True
    except Exception as e:
        print(f"✗ GerberParser类测试失败: {e}")
        return False

def main():
    """主测试函数"""
    print("开始测试Python绑定...")
    print("=" * 50)
    
    tests = [
        ("基本导入", test_basic_import),
        ("模块信息", test_module_info),
        ("Gerber信息函数", test_gerber_info_function),
        ("GerberParser类", test_gerber_parser_class),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n测试: {test_name}")
        print("-" * 30)
        if test_func():
            passed += 1
    
    print("\n" + "=" * 50)
    print(f"测试完成: {passed}/{total} 通过")
    
    if passed == total:
        print("🎉 所有测试通过！Python绑定工作正常。")
        print("\n下一步:")
        print("1. 构建项目: mkdir build && cd build && cmake .. && make")
        print("2. 运行此测试脚本: python3 test_python_binding.py")
        print("3. 使用真实Gerber文件进行完整测试")
    else:
        print("⚠️ 部分测试失败，需要检查构建配置。")

if __name__ == "__main__":
    main()