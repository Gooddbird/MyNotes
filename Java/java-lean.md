# JAVA核心技术笔记

## Java基本数据类型（共8种）
Java是强类型语言。
### 整形：:w

- int 四字节
- short 二字节
- long 8字节，后缀L。
- byte 1字节
- 在java中，整形的范围与运行Java的机器无关。方便移植。
十六进制前缀0x
八进制前缀0
二进制前缀0b
Java没有任何无符号形式的int long shrot byte
### 浮点型：
- float：4字节，后缀F或f
- double: 8字节，不加后缀默认为double
<br>三个特殊浮点数： 正无穷大、负无穷大、NaN(不是一个数字，如除数为0得到)

### 字符型：
char 
### 布尔类型
boolean: 只有两个值 true 和 false

**整形与布尔类型不能转换**

## 变量

java不像C++区分变量的声明和定义

```
C/C++:
int i = 1; 是定义
extern int i; 是声明
```
## 常量
Java使用 **final**关键字声明常量
```
final int a = 1;
```
## 位运算符
```
& and
| or
^ 异或
~ 非
>> << 左移右移
```
## 枚举类型

```
enum Size {
    SMALL,
    MID,
    BIG
};

Size s = Size.SMALL;
Size类型变量只能存储枚举值或者null值
```
## 字符串String

- substring（a, b）方法：提取下标为a,到下标b的子串。
- 不可变： java字符串是不可变的。但字符串变量是可变的。
```
String a = "11";
a = "22";
字符串"11"是不可变的 但a变量是可变的。
```
- 不可变字符串的优点是可以共享字符串。
- s1.equal(s2): 检查s1 s2是否相等。相等返回true。s1 s2可以使字符串变量，也可以是字符串字面量
- 不要使用 == 判断两个字符串。这个运算符**只能判断两个字符串是否放在同一个位置，而不是判断其是否相等**
- 空串""：长度为0的串
- null串：表示没有对象与该变量关联（str==null）

## 输入输出 

```
import java.util.*;

Scanner in = new Scanner(System.in);
String name = in.nextLine();
int age = in.nextInt();
double x = 1.1;
System.out.println("%.2f", x);
```






## 大数值
```
BigInteger a = BigInter.valueOf(100);
BigInteger b = a.add(a); // b=a+a
BigInteger c = a.multiply(b); // c=a*b

```
## 数组
```
int[] a;
或 int a[];

int[] a = new int[100];
```
- 创建一个数值数组元素都初始化为0。boolean数组初始化为false.对象数组如字符串数组全初始化为null
- 数组一旦创建其大小不可改变 
- java数组分配在堆上

## 类与对象
```
public class A {

};

// A 是一个类
A a; // 注意这里a不是对象, a是一个对象变量
new A(); // new A()才是一个对象 不过这是一个临时对象
A a = new A();  // 这里将一个对象绑定到a这个对象变量上。这又称之为引用。

```
- 局部变量不会自动初始化为null，必须使用new或将他们初始化null。

- 只有一个类有main方法。
### 静态域
- 每个类只有一个静态域（static）,这个类的所有对象共享这个静态域

## 方法
- 对于基本数据类型，Java采用值传递方式。
- 对于对象类型，也是采用值传递的。也就是说swap(a, b)是不起作用的。
- 一个方法不能修改一个基本数据类型的参数。
- 一个方法可以改变一个对象参数的状态
- 一个方法不能让对象参数引用一个新的对象。
- java中可以在一个构造函数中调用该类的另一个构造函数
-   
