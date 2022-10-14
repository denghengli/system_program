#后台运行两个程序
bin/lock_write aaaaa test/lock_write.txt lock &
bin/lock_write AAAAA test/lock_write.txt lock &

