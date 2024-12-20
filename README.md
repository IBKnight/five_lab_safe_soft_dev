Для запуска потребуется `gcc` компилятор. В данной лабораторной работе используется `gcc version 12.2.0 (Debian 12.2.0-14)`
Для динамической отладки используется `GNU Debugger(gdb)`.

1. Динамическая отладка программного обеспечения с бинарной уязвимостью.
	Для начала немного видеизменим программу, для удобства, чтобы не нужно было вручную заполнять значение для переполнения буффера. Переполнение буффера происходит в 2 этапа:
	1. Заполнение буффера через `memset`
		```C
		memset(buf, 'A', 40);
		```
		![start_debugging](images/start_debugging.png)

		![filling_buffer](images/filling_buffer.png)
	2. Переполнение буффера специальными символами.
        Мы используем уязвимость переполнения буфера, заполняя буфер определённой последовательностью байтов. Цель заключается в том, чтобы изменить значение переменной `check` на 0xDEADBEEF. Для этого мы заполняем буфер фиктивными данными до тех пор, пока не достигнем области памяти, где хранится переменная `check`. После этого мы записываем в эту область нужное значение `(0xDEADBEEF)` путём непосредственного размещения байтов в порядке, соответствующем логике архитектуры (например, порядок little-endian для x86/x64).
		![after_buffer_overflow](images/after_buffer_overflow.png)
		
		После переполнения буфера в переменной чек мы получаем значение `0xdeadbeef`

		![check_value](images/check_value.png)
		
		Соответственно, у нас проходит проверка, после чего происходит открытие терминала.

		![opening_console](images/opening_console.png)

        После чего программа завершается.
		
		### **Как работает переполнение буфера?**
		Буфер — это область памяти, куда временно записываются данные. Например, массив `char buf[40]` выделяет 40 байт под хранение текста.
		
		**Что происходит при переполнении?**  
		Если программа ожидает 40 байт, но мы передаёте, например, 70 байт, то **первые 40 байт записываются в буфер**, а **оставшиеся 30 байт начинают затирать память за буфером**.  
		В памяти могут быть:
		- **Переменные программы** (переменная `check` в задании).
		- **Адрес возврата из функции** (вызов функции `return address`).
		- **Управляющие данные стека**.
		
		**К чему это приводит?**
		- **Изменение переменных** (значение переменной `check` изменилось на `check = 0xDEADBEEF`).
		- **Подмена адреса возврата** — программа может вернуться не в то место, откуда была вызвана функция, а в код злоумышленника.
		- **Запуск произвольного кода** — возможность запустить свою команду, как, например, я запустил терминал.

2. Анализ кода и поиск уязвимостей.
	Код для анализа:
	```C
	#include <unistd.h>
	#include <sys/types.h>
	#include <stdlib.h>
	#include <stdio.h>
	
	int main()
	{
	
	  int var;
	  int check = 0x04030201;
	  char buf[40];
	
	  fgets(buf,45,stdin);
	
	  printf("\n[buf]: %s\n", buf);
	  printf("[check] %p\n", check);
	
	  if ((check != 0x04030201) && (check != 0xdeadbeef))
	    printf ("\nYou are on the right way!\n");
	
	  if (check == 0xdeadbeef)
	   {
	     printf("Yeah dude! You win!\nOpening your shell...\n");
	     setreuid(geteuid(), geteuid());
	     system("/bin/bash");
	     printf("Shell closed! Bye.\n");
	   }
	   return 0;
	}
	```
	**Анализ слабых мест:**
	
	**1. Переполнение буффера:**
	Использование `fgets(buf, 45, stdin)` при объявлении буфера размером 40 байт может привести к переполнению буфера, если пользователь введет более 40 символов. Это уязвимость, которая позволяет злоумышленнику переписать данные в памяти, включая переменные check.
	
	**2. Неинициализированная переменная `var`:**
	Переменная `var` объявлена, но не используется. Это может сбивать с толку и должно быть удалено.

	**3. Тип переменной `check`:**
	Для хранения значения `0xdeadbeef` логичнее использовать `unsigned int`, чтобы избежать проблем с отображением знака.
	
	**4. Вывод адреса вместо значения:**
	`printf("[check] %p\n", check);` выводит адрес, а не значение переменной `check`. Формат `%p` предназначен для указателей. Для вывода целого числа следует использовать `%x` или `%d`.
	
	**5. Использование небезопасной функции system:**
	Функция `system("/bin/bash")` открывает командную оболочку, что создает риск выполнения нежелательного кода. Это особенно опасно в потенциально скомпрометированном коде. Поэтому стоить заменить её на более безопасный аналог.
	
	**6. Недостаточная проверка результата `fgets`:**
	Отсутствует проверка возвращаемого значения `fgets`. Это может привести к обработке данных, даже если ввод завершился ошибкой.
	
	**7. Потенциальные проблемы с `setreuid`:**
	Использование `setreuid` небезопасно, если права доступа к процессу не настроены корректно.

	**Улучшенный код:**
	```C
	#include <unistd.h>
	#include <sys/types.h>
	#include <stdlib.h>
	#include <stdio.h>

	int main() {
		unsigned int check = 0x04030201; //unsigned int для переменной check
		char buf[40];

		// Проверка результата fgets и предотвращение переполнения буфера
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			fprintf(stderr, "Ошибка ввода\n");
			return 1;
		}

		printf("\n[buf]: %s\n", buf);
		printf("[check]: 0x%x\n", check); // Корректный вывод значения check

		if ((check != 0x04030201) && (check != 0xdeadbeef)) {
			printf("\nYou are on the right way!\n");
		}

		if (check == 0xdeadbeef) {
			printf("Yeah dude! You win!\nOpening your shell...\n");
			if (setreuid(geteuid(), geteuid()) != 0) {
				perror("setreuid failed");
				return 1;
			}
			execl("/bin/bash", "bash", NULL); // Более безопасная альтернатива system
			perror("execl failed");
		}

		printf("Shell closed! Bye.\n");
		return 0;
	}

	```
