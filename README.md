# Sender-receiver_foreach

The complain info:

/work/chuanqiu/senderReceiver/workhpx/test/foreach_async.cpp:118:86: error: ‘hpx::this_thread::experimental’ has not been declared
  118 |         hpx::for_each(par(task).on(exec), c.begin(), c.end(), f) | hpx::this_thread::experimental::sync_wait();
      |                                                                                      ^~~~~~~~~~~~
make[2]: *** [CMakeFiles/foreach_async.dir/build.make:76: CMakeFiles/foreach_async.dir/foreach_async.cpp.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:83: CMakeFiles/foreach_async.dir/all] Error 2
