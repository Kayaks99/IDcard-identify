# -*-encoding:utf-8-*-
import pytesseract
from PIL import Image
from pathlib import Path
 
def func(fileName):
        my_file = Path(fileName + 'output.txt')
        if my_file.is_file():# 指定的文件存在
                with open(fileName + 'output.txt', "r+") as f:
                        read_data = f.read()
                        f.seek(0)
                        f.truncate()   #清空文件

        for i in range(7,0,-1):
                str_num = fileName+ str(i) + '.jpg'# + '(' + fileName + ')' 
                #print(str_num)
                file = Path(str_num)
                if file.is_file():
                        image = Image.open(str_num)
                        text = pytesseract.image_to_string(image,lang='normal') #使用简体中文解析图片
                        #print(text)
                        with open(fileName + 'output.txt', "a") as f: #将识别出来的文字存到本地
                                f.write(str(text))
                                f.write("\n");
              
if __name__ == '__main__':
        func('C:/Users/Siyun/Desktop/IDcard/Debug/')

