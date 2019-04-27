cd ~
echo "开始创建文件夹src" >> /root/log.txt
mkdir src
echo "创建文件夹 src 完成！" >> /root/log.txt
cd src
echo "正在下载内核 5.0.6，请确保你的机型是 CentOS7.X !" >> /root/log.txt
wget https://mirrors.tuna.tsinghua.edu.cn/kernel/v5.x/linux-5.0.6.tar.xz
echo "解压内核文件" >> /root/log.txt
xz -d linux-5.0.6.tar.xz
tar xvf linux-5.0.6.tar
echo "解压完成！" >> /root/log.txt
echo "开始下载所需安装包......" >> /root/log.txt
yum -y install ncurses-devel
echo "第一个完成..." >> /root/log.txt
yum -y install gcc
echo "第二个完成..." >> /root/log.txt
yum -y install flex
echo "第三个完成..." >> /root/log.txt
yum -y install bison
echo "第四个完成..." >> /root/log.txt
yum -y install openssl-devel
echo "第五个完成..." >> /root/log.txt
yum -y install elfutils-libelf-devel
echo "第六个完成..." >> /root/log.txt
echo "安装工作结束！开始清理垃圾文件..." >> /root/log.txt
rm -f linux-5.0.6.tar
cd linux-5.0.6
make clean
echo "清理完成！所有工作结束！！！感谢使用！" >> /root/log.txt
echo "检查日志:"
history -c
cat /root/log.txt
make menuconfig


cd ~/src/linux-5.0.6.tar/
make -j4
echo "内核编译完成..." >> /root/log.txt
make modules
echo "模块编译完成..." >> /root/log.txt
make modules_install
echo "模块安装完成..." >> /root/log.txt
make install
echo "内核安装完成..." >> /root/log.txt
grub2-mkconfig -o /etc/grub2.cfg
echo "grub 引导完成..." >> /root/log.txt
grub2-set-default 0
history -c
echo "设置系统默认启动项完成...开始重启" >> /root/log.txt
reboot



