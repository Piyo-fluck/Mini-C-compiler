#include <stdio.h>
#include <pthread.h>
int counter = 1000;
   void *
   print_message (void *thread)
   {
  while (1)
    {
        counter++;
        printf("thread=%-13p count=%d\n", thread, counter);
        sleep(1);
      }
  }
  int main (int argc, char **argv){
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, print_message, &thread1);
    pthread_create(&thread2, NULL, print_message, &thread2);
   ________(thread1, NULL);
   ________(thread2, NULL);
   }