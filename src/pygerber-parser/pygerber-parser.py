import ctypes

lib = ctypes.cdll.LoadLibrary('./libpygerber-parser.so')

def gerber2image(gerber_file:str):
    result = lib.RenderGerber2Image(ctypes.c_char_p(bytes(gerber_file, encoding="utf-8")))