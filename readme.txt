# Sender-receiver_foreach

2022/10/18
cdw/senderReceiver

///////////////////////////////build HPX//////////////////////////////////////////////////////////
On compute node medusa:

1. chuanqiu@rostam1:/work/chuanqiu/senderReceiver$ srun -p medusa -N 1 --pty /bin/bash -l
2. mkdir workhpx
3. chuanqiu@medusa01:/work/chuanqiu/senderReceiver/workhpx$ git clone https://github.com/STEllAR-GROUP/hpx.git
4. cd hpx
5. chuanqiu@medusa01:/work/chuanqiu/senderReceiver/workhpx/hpx$ mkdir build && cd build

// check module loaded: module av

// loaded module has yellow highlight(L)

// chuanqiu@medusa01:/work/chuanqiu/sender/compile/hpx/build$ module savelist
list :
  1) boost  2) cmake323  3) gcc12  4) git236  5) hwloc271  6) openmpi414
  
6. chuanqiu@medusa01:/work/chuanqiu/senderReceiver/workhpx/hpx/build$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/work/chuanqiu/senderReceiver/installhpx -DHPX_WITH_FETCH_ASIO=ON -DHPX_WITH_MALLOC=system ..

HPX will be installed to /work/chuanqiu/senderReceiver/installhpx

-- Configuring done
-- Generating done
-- Build files have been written to: /work/chuanqiu/senderReceiver/workhpx/hpx/build
   
7. chuanqiu@medusa01:/work/chuanqiu/senderReceiver/workhpx/hpx/build$  make -j20
8. chuanqiu@medusa01:/work/chuanqiu/senderReceiver/workhpx/hpx/build$  make install


///////////////////////////////do test//////////////////////////////////////////////////////////

1. chuanqiu@medusa01:/work/chuanqiu/senderReceiver/workhpx/test$ cmake -DCMAKE_BUILD_TYPE=Release -DHPX_DIR=/work/chuanqiu/senderReceiver/installhpx/lib64/cmake/HPX -DHPX_IGNORE_COMPILER_COMPATIBILITY=on .
2. chuanqiu@medusa01:/work/chuanqiu/senderReceiver/workhpx/test$ make 
