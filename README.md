# OS_4sem, HW05

Выполнены задания 1-2 из последней домашней работы (без fork).

Для проверки работоспособности программы выполнить следующие инструкции:

- Выполнить make в initramfs/user_elf
- Запустить make_initramfs.sh с параметрами initramfs initrd
- Выполнить make всего
- Выполнить qemu-system-x86_64 -kernel kernel -serial stdio -s -initrd initrd

Описание основных функций:

*elf.c*

"read_buffer" -- функция для чтения из файла в буфер

"load_program_header" -- функция загрузки program header в память

"create_stack" -- функция, которая создает стек

"load_elf" -- функция, котрая читает elf файл, загружает его в память и настраивает стек

"run_elf" -- функция, запускающая по пути path файл elf

"elf_syscall" -- функция - системный вызов: если rax = 0, то работает как write, а если rax = 1, то как fork(вернее, не работает, как fork)


*threads.c*

"setup_userspace" -- функция, настраивающая TSS


*entry.S*

"elf_syscall_wrapper" -- обертка для elf_syscall


*interrupt.c*

"setup_ints" -- в него добавилось новое прерывание для elf_syscall


*initramfs/user_elf/elf.c*

Тут находится код для userspace 
