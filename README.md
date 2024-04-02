# packet_send
使用dpdk 发送/接受 自定义格式的数据包

## 编译

```shell
mkdir build
cd build
cmake ..
make
```

## 数据包发送
默认一个核绑定一个队列
```shell
cd build
sudo ./packet_send -a 0000:5e:00.0 -l 0-3
```

使用 5e:00.0 设备，绑定内核 0 1 2 3

```angular2html
zju@zju-PowerEdge-R740:~$ grep PCI_SLOT_NAME /sys/class/net/*/device/uevent
/sys/class/net/eno1/device/uevent:PCI_SLOT_NAME=0000:18:00.0
/sys/class/net/eno2/device/uevent:PCI_SLOT_NAME=0000:18:00.1
/sys/class/net/eno3/device/uevent:PCI_SLOT_NAME=0000:19:00.0
/sys/class/net/eno4/device/uevent:PCI_SLOT_NAME=0000:19:00.1
/sys/class/net/ens1f0np0/device/uevent:PCI_SLOT_NAME=0000:3b:00.0
/sys/class/net/ens1f1np1/device/uevent:PCI_SLOT_NAME=0000:3b:00.1
/sys/class/net/ens3f0np0/device/uevent:PCI_SLOT_NAME=0000:5e:00.0
/sys/class/net/ens3f1np1/device/uevent:PCI_SLOT_NAME=0000:5e:00.1
```

## 数据包接收
```shell
cd build
sudo ./packet_recv -a auxiliary/mlx5_core.sf.2
```


