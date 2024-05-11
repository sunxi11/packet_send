# packet_send
使用dpdk 发送/接受 自定义格式的数据包

## 编译

```shell
git clone https://github.com/sunxi11/packet_send.git
git checkout tm_dpdk_send
mkdir build
cd build
cmake ..
make
```

## 数据包发送
默认一个核绑定一个队列
```shell
cd build
sudo ./packet_send --legacy-mem -a 0000:3b:00.0 -c 0xfffffffff
```

使用 3b:00.0 设备，绑定内核 0 1 2 3

```angular2html
/home/zju/sunxi-workspace/packet_send/cmake-build-dpu-host-root/packet_send --legacy-mem -a 0000:3b:00.0 -c 0xfffffffff
EAL: Detected CPU lcores: 40
EAL: Detected NUMA nodes: 2
EAL: Static memory layout is selected, amount of reserved memory can be adjusted with -m or --socket-mem
EAL: Detected shared linkage of DPDK
EAL: Multi-process socket /var/run/dpdk/rte/mp_socket
EAL: Selected IOVA mode 'PA'
EAL: VFIO support initialized
EAL: Probe PCI driver: mlx5_pci (15b3:a2dc) device: 0000:3b:00.0 (socket 0)
TELEMETRY: No legacy callbacks, legacy socket not created
used cores: 36
Port 0 MAC: a0 88 c2 32 00 54
core_id: 1 queue id: 0
core_id: 3 queue id: 2
core_id: 2 queue id: 1
core_id: 4 queue id: 3
core_id: 5 queue id: 4
core_id: 6 queue id: 5
core_id: 7 queue id: 6
core_id: 8 queue id: 7
core_id: 9 queue id: 8
core_id: 10 queue id: 9
core_id: 11 queue id: 10
core_id: 13 queue id: 12
core_id: core_id: 14 queue id: 1312 queue id: 11

core_id: 15 queue id: 14
core_id: 16 queue id: 15
core_id: 17 queue id: 16
core_id: 18 queue id: 17
core_id: 19 queue id: 18
core_id: core_id: 21 queue id: 2020 queue id: 19

core_id: 23 queue id: 22
core_id: 22 queue id: 21
core_id: 24 queue id: 23
core_id: 25 queue id: 24
core_id: 26 queue id: 25
core_id: 27 queue id: 26
core_id: 28 queue id: 27
core_id: 29 queue id: 28
core_id: 31 queue id: 30
core_id: 30 queue id: 29
core_id: 32 queue id: 31
start to send data
core_id: 33 queue id: 32
core_id: 35 queue id: 34
core_id: 34 queue id: 33
^CCaught signal 2, exiting...
Core 24 send 49812381696 bytes
Core 28 send 49697894400 bytes
Core 20 send 49763180544 bytes
Core 32 send 36142571520 bytes
Core 25 send 49755070464 bytes
Core 31 send 49694515200 bytes
Core 33 send 49705328640 bytes
Core 23 send 49851445248 bytes
Core 21 send 49883480064 bytes
Core 29 send 49726955520 bytes
Core 30 send 49685999616 bytes
Core 26 send 49788727296 bytes
Core 35 send 49793728512 bytes
Core 34 send 49754529792 bytes
Core 22 send 49830764544 bytes
Core 27 send 49811030016 bytes
Core 16 send 6590791680 bytes
Core 8 send 6606876672 bytes
Core 7 send 6500093952 bytes
Core 6 send 6595522560 bytes
Core 12 send 6575923200 bytes
Core 19 send 6533210112 bytes
Core 17 send 6540374016 bytes
Core 2 send 6636613632 bytes
Core 14 send 6553485312 bytes
Core 18 send 6560243712 bytes
Core 1 send 6460895232 bytes
Core 4 send 6579032064 bytes
Core 9 send 6456840192 bytes
Core 13 send 6411018240 bytes
Core 11 send 6470221824 bytes
Core 5 send 6481305600 bytes
Core 10 send 6584573952 bytes
Core 3 send 6453866496 bytes
Core 15 send 6417506304 bytes
total time: 75.6686 s
Total Throughput: 95.86 Gbps
```

## 数据包接收
```shell
cd build
sudo ./packet_recv -a auxiliary/mlx5_core.sf.2
```


