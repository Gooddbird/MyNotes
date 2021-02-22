# Git版本控制常用命令

## 创建版本库
```
cd dir
git init
```
git init 即将当前目录初始化为git的版本库，运行后该目录下出现一个隐藏目录 .git 目录。

## 添加文件
```
git add file1 
```
即将文件 file1 添加到git仓库

## 提交文件
```
git commit -m "添加文件file1"
```

## 版本控制

### 查看当前仓库状态
```
git status
```

### 查看修改
```
git diff file1
```

