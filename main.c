#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{

  int var;
  int check = 0x04030201;
  char buf[40];

  // fgets(buf,45,stdin);
  gets(buf);
  // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\xef\xbe\xad\xde
  // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAﾭ�
  // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAï¾­Þ
  // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����

  // printf("Before %p\n", (void*)&check);

  // memset(buf, 'A', 40);


  // buf[44] = '\xef';
  // buf[45] = '\xbe';
  // buf[46] = '\xad';
  // buf[47] = '\xde';
  // buf[48] = '\0';

  printf("After %p\n", (void*)&check);
  

  printf("\n[buf]: %s\n", buf);
  printf("[check] %p\n", check);

  if ((check != 0x04030201) && (check != 0xdeadbeef))
    printf ("\nYou are on the right way!\n");

  if (check == 0xdeadbeef)
   {
     printf("Yeah dude! You win!\nOpening your shell...\n");
     setreuid(geteuid(), geteuid());
     system("/bin/bash -i");
     printf("Shell closed! Bye.\n");
   }
   return 0;
}


// #include <stdio.h>
// #include <string.h>

// int main(void)
// {
//     char buff[15];
//     int pass = 0;

//     printf("\n Enter the password : \n");
//     gets(buff);

//     if(strcmp(buff, "thegeekstuff"))
//     {
//         printf ("\n Wrong Password \n");
//     }
//     else
//     {
//         printf ("\n Correct Password \n");
//         pass = 1;
//     }

//     if(pass)
//     {
//        /* Now Give root or admin rights to user*/
//         printf ("\n Root privileges given to the user \n");
//     }

//     return 0;
// }