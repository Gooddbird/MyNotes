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

在Git中，使用HEAD指针指向当前版本, HEAD^代表上一个版本。

### 查看当前仓库状态
```
git status
```

### 查看修改
```
git diff file1
```
### 查看日志
```
git log

git reflog // 查看每一次版本变化
```
### 版本重置
```
git reset --hard xxxx
其中 xxxx为版本号
```

### 撤销修改
```
git checkout -- file1
```
撤销工作区的更改，将文件file1恢复到最近一次git commit 或者 git add 的状态。实际上就是用版本库里的文件替换掉工作区的文件。注意 -- 符号必须存在，不然就变成了切换分支命令。

```

git reset HEAD file
```
撤销掉暂存区的修改，重新放回工作区。即现在暂存区是干净的，但工作区是修改的。相当于撤销掉上一次 git add 操作。


### 版本控制原理
Git使用了一个名为暂存区的东西。在Git的版本库 .git 中，存在一个 stage 的暂存区。
![image](../images/stage.jpg)


## 远程版本库
### 关联远程仓库
注意需事先将此电脑key公钥上传到GitHub的个人SSH公钥中。
```
git remote add origin git@github.com:Gooddbird/xxx.git
```

### 推送
```
git push -u origin master // 关联后第一次推送master分支到远程仓库

git push origin master // 之后的推送
```
注意第一次的 -u参数，因为远程仓库开始时空的，使用 -u参数不仅将本地的master分支推送到远程仓库的master分支，而且还将这两个master分支关联起来。

### 从远程克隆
```
git clone git@github.com:Gooddbird/MyNotes.git 
```

## 分支
### 创建分支
```
git branch 分支名 // 创建分支
git checkout 分支名 // 切换到这个分支
```
### 查看分支
```
git branch
```
### 删除分支
```
git branch -d 分支名
```

## 标签
```
git tag commitid 
```
在某个commit上打上标签

```
git tag
查看所有标签 

git tag -d 标签名字
删除标签

git push origin 标签名字
将某个标签推送到远程

git push origin --tags
推送所有标签

```


