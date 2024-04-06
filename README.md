# maze
C++ 控制台 迷宫  使用了自动生成迷宫算法，实现迷宫基本玩法

内部使用了windows API SetConsoleCursorPosition来重绘
目的是为了不使用清屏重绘，解决闪烁问题
所以该代码只能在windows下编译

程序参数可以在代码头部设置，例如迷宫大小和输出区域大小等

需要合适的字体才能正常显示


![游玩示例](play.gif)
