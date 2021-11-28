wsl使用代理方法

启动代理客户端中的允许LAN，并确认客户端中提供的代理端口

export http_proxy='http://<windows ip>:<port>'

export https_proxy='http://<windows ip>:<port>'

windows ip应为使用无线局域网的ip，用ipconfig查询即可



设置sudo不需要使用密码

```bash
ls -l
# 第一个字母：d意味着是目录，-意味着是文件
# 之后每三个字母代表一组权限，即前三个字母代表所有者的权限，中间三个字母代表用户组的权限，最后三个字母代表其他用户的权限
# 第二列标识有多少链接指向这个文件
# 第三列和第四列标明文件的所有者和用户组
# 第五列标明大小（以字节为单位）
# 标明最后修改时间，文件名等

ls -lh
# -h可以让输出结果中的第五列使用k，M，G等常见单位

chmod [u/g/o/a]+/-[r/w/x] <file/dir>
# 更改文件权限
# u：所有者，g：用户组，o：其他用户
# r：读，w：写，x：执行
```

编辑/etc/sudoers

改为%sudo	ALL=(ALL:ALL)	NOPASSWD:ALL

或者添加一行：usrname	ALL=(ALL:ALL)	NOPASSWD:ALL



apt update的意义是更新apt源，而apt upgrade则是更新软件包

通常使用apt update & apt upgrade更新所有包



GNU是一种操作系统(GNU's Not Unix!)，gcc原本只是C语言编译器，即GNU C Compiler，但随着gcc逐渐发展，能够处理的编程语言越来越多，gcc也随之改名为GNU编译器套件，即GNU Compiler Collection，缩写为GCC（尽管名字中含有collection，但它不是很多个编译器的集合，他只是能编译多种语言而已），GCC现在已经被大多数Unix操作系统采用为标准编译器

gcc（GNU Compiler Collection）和g++本身实际上是某种驱动器，完成预处理、编译、汇编、链接过程

对于 .c和.cpp文件，gcc分别当做C和C++文件编译（C++语法强度比C更高）

对于 .c和.cpp文件，g++则统一当做C++文件编译

使用g++编译文件时，g++会自动链接标准库STL，而gcc不会自动链接STL

gcc在编译C文件时，可使用的预定义宏是比较少的

g++等同于gcc -xc++ -lstdc++ -shared-libgcc，在语法规范的前提下，完全可以使用g++来编译C源文件，g++按照C++标准来编译源文件时，语法强度会比gcc要高

如果是C源文件，在预处理时，会生成后缀为.i的纯C代码，编译时会生成.s结尾的汇编代码，之后再进行汇编，即可生成.o结尾的二进制代码，最后进行链接，把二进制代码与库文件和其他目标代码链接到一起，生成可执行文件

如果是C++源文件，需要在预编译时指明产生的是cpp文件

gcc可以根据后缀名判断使用什么方式编译



- 对于c文件，gcc与g++在**代码规范**的情况下，是**完全等价**的。
- 对于cpp文件，在预处理、编译、汇编这三部分，gcc和g++也是等价的，前提是这三个步骤一起做，或者你在中间继续能够体现cpp的文件名。
- 在涉及c++的标准库时，gcc无法链接到这些库，必须加上-lstdc++ 选项，相比之下，不如直接使用g++来得方便快捷。
- **g++就是gcc以默认c++的方式进行编译，然后链接的时候加上了一些c++的库。**



- -lstdc++ 要放在后面，因为编译器是从右向左调用和处理变量的（根据编译器版本的不同，有些版本可能支持放在前面）
- -l 后面加的是动态库libstdc++.-l加的时候，把"lib"三个字符省略，例如链接libtest.so你就需要加 -ltest ,一般这个库在usr/lib下可以找到
- gcc可以编译c++文件，也可以编译c文件，但默认是编译c文件的，加-lstdc++表示编译c++文件，即链接c++库，加-lc表示链接c库，默认情况下就是链接c库，所以如果编译c文件可以不加-lc。
- --shared-libgcc 链接动态libgcc库



其他操作系统上的编译器：Clang主要用于MacOS，而MinGW是Minimalist GNU for Windows的简称

GDB是The GNU Project Debugger的简称

GNU Make是一个构建工具，即可以用来控制编译过程的工具，Make本身并不具有编译和链接功能，当一整个项目中含有多个源文件时，如果逐个调用编译器去编译，就会很混乱，而且工作量很大，这时就需要使用Make，Make会执行Makefile中的命令，Makefile中就包含了调用编译器去编译某个文件等命令

CMake是一个跨平台的构建工具，可以跨平台生成对应平台所需的Makefile，使用起来也比Make更方便，能更快速地生成Makefile



虽然我们大多数时候都把Linux称作一个操作系统，但实际上，Linux只是指操作系统中的内核，内核是用来给其他计算机程序分配硬件资源的，真正意义上的操作系统还包含了诸多其他部分，当今我们所谓的Linux操作系统，实际上应该是GNU/Linux操作系统

GNU原本作为一个开发计划，目标是开发一个完整的操作系统，他们做了许多工作，比如开发出了一套编译器，附带的软件，图形界面，甚至bash等等，但GNU唯独缺一个内核，直到1992年，Linus Torvalds让Linux内核问世，Linux内核外加GNU所开发的软件，才构成了一个较为完整的类Unix操作系统

如今，还有其他很多的GNU/Linux操作系统，不但使用了Linux内核以及GNU软件，也使用了很多非GNU软件，一并构成了一个庞大的操作系统，这就是我们一般认为的Linux发行版





/usr目录的含义是Unix System Resource，而不是User

/bin目录的含义是二进制文件，即Binary的缩写
