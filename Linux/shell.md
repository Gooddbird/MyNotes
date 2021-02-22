#   Shell总结

## 变量
- 变量名称只能由字母数字组合而成，且**开头不能是数字**
- 变量值若有空格可用单引号或双引号包裹，但需注意：
  - 双引号中的特殊字符如 $ ,可以保持原来的含义。如【a="x is $LANG"】即代表【a="x is zh.UTF-8"】
  - 单引号中的特殊字符就是其文本。如【a="x is $LANG"】代表【a="x is $LANG"】
- 使用反斜杠可以将特殊字符（如 $ 空格 \ 等）变为一般字符
- 使用 \`指令\` 或者 $(指令) 可以取这个指令的值赋给变量，如

```
[hd@localhost ~]$ var=$(ls)
[hd@localhost ~]$ echo $var
build cmake-3.14.4 Desktop Documents Downloads muduo Muduo Music orange Pictures Public Reactor Templates valgrind Videos workspace
```

- 为变量增加内容
```
PATH=${PATH}:/home/bin
// 即将PATH变量的值再添加 /home/bin进去
// 注意一定要有花括号
```
- export 变量名称，可以将该变量变为环境变量
- unset 变量名称：取消变量


### 变量读取
echo $PATH  // 读取环境变量PATH的值打印出来
```
[hd@localhost ~]$ echo $PATH 
/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin:/home/hd/.local/bin:/home/hd/bin
```

### 变量设定
```
myname=GooddBird
```
**注意上式等号两端不能有空格**

### 显示变量

```
env 显示所有环境变量

set 显示所有变量（包括环境变量）
```

### 读取键盘输入
```
[hd@localhost ~]$ read -p "请输入变量" -t 30 myname
请输入变量goo^H^H
[hd@localhost ~]$ echo $myname
goo
```

### 变量类型声明
```
declare [-aixr] 变量

-a : 数组类型
-i ：int类型
-x : 设为环境变量
-r : 只读变量

如 
declare -i sum=1+2+3
echo $sum
输出 6
```

### 变量内容的删除、替换等
- 从前往后删除 使用#
```
#: 符合模式最短的一个
##：符合模式最长的一个

path=$PATH
echo $path
[ /usr/local/bin:/usr/bin:/usr/sbin: ]

echo ${path#/*bin:} 删除符合 【/*bin"】模式最短的一个，即删除了 [/usr/local/bin: ],删除后path为
[ /usr/bin:/usr/sbin: ]

```
- 从后往前删除 使用%

- 替换
```
echo ${path/sbin/SBIN} 替换path中的第一个sbin为SBIN
echo ${path//sbin/SBIN} 替换path中的全部sbin为SBIN
```
### 复合指令
```
cmd1 && cmd2  // cmd1先执行，cmd1执行成功才会执行cmd2

cmd1 || cmd2  // cmd1先执行，其执行失败后才会执行cmd2，cmd1执行成功就不执行cmd2
```

### PIPE管线命令
```
cmd1 | cmd2
将cmd1的标准输出作为cmd2的输入
```

## SHELL脚本

### 中括号判断
```
[ "$PATH" == $"HOME" ]
判断 PATH 和HOME两个变量是否相等
```
- **中括号的每个组件必须由空格相隔** 
- **中括号中的变量要用双引号括起来**
- **中括号的常量最好也用单双引号**

### 脚本入参
```
$# : 输入参数个数
$@ : 输入的参数
${1} : 取第一个参数
```
### 条件判断
```
if [ 条件判断 ]; then
    // 条件成立时候执行内容

elif [ 条件2 ]; then
    // ..

else
    // ....
fi

若有多个中括号可用 && || 连接
```
### 函数
由于shell脚本执行是从上到下从左到右，因此函数声明一定要在程序最前面
```
function () {
    // ...
}
```
### 循环
```
while [ 条件 ]
do
    // ...
done
```