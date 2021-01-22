max@max-Swift:~/Рабочий стол/ДА/lab4/DA_lab_4-main$ make bench
g++ -Wall -o lab4b.exe bench.cpp
g++ -Wall -o lab4bg.exe banchmark_test_gen.cpp
max@max-Swift:~/Рабочий стол/ДА/lab4/DA_lab_4-main$ ./lab4b.exe < test_a
^C
max@max-Swift:~/Рабочий стол/ДА/lab4/DA_lab_4-main$ ./lab4bg.exe 5 100000000
done| 9400ms
max@max-Swift:~/Рабочий стол/ДА/lab4/DA_lab_4-main$ ./lab4b.exe < test_a
search complete| 6324ms
max@max-Swift:~/Рабочий стол/ДА/lab4/DA_lab_4-main$ ./lab4bg.exe 5 50000000
done| 4695ms
max@max-Swift:~/Рабочий стол/ДА/lab4/DA_lab_4-main$ ./lab4b.exe < test_a
search complete| 3148ms
Ошибка сегментирования (стек памяти сброшен на диск)
max@max-Swift:~/Рабочий стол/ДА/lab4/DA_lab_4-main$ make bench
g++ -Wall -o lab4b.exe bench.cpp

