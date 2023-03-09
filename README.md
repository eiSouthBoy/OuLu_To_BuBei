# OuLu_To_BuBei

## 一、简单介绍

程序目标是将欧陆词典中的生词本导出，经过数据转换后，将新的生词本导入不背单词中。

具体过程如下：

`欧陆词典导出csv文件` --> `csv文件转换为txt文件` --> `不背单词导入txt文件`

##  二、使用介绍

1️⃣ **config.ini文件**

```ini
[Configuration]
;输入需要转换的csv文件路径
;处理完成后的文件名：D:\document\02_Visual_Studio_2017\OuLu2BuBei_CsvData_Project\1.txt
file_path_in=D:\document\02_Visual_Studio_2017\OuLu2BuBei_CsvData_Project\1.csv
```

2️⃣ **1.csv是测试输入文件，1.txt是测试输出文件**