﻿Linux Kernel Module simple device
Простой модуль, регистрирующий драйвер в системе и создающей связанный с ним файл /dev/simple_device.
При считывании из этого файла выводится "Hello world".
При записи в файл, введенный текст записывается в лог, его можно увидеть в файле /var/log/kern.log или командой dmesg.
