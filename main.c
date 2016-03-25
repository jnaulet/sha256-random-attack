/*
 * sha256-random-attack. Simple sha256 attack using /dev/urandom
 * (c) 2016 Joachim Naulet <jnaulet@rdinnovation.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <openssl/sha.h>

#define DEFAULT_SIZE 256 /* 2048 bits */

static unsigned char *parse_digest(const char *digest,
				   unsigned char *md) {

  int i;
  size_t size = 0;
  unsigned int byte;
  
  while(*digest){
    if(sscanf(digest, "%02x", &byte)){
      md[size++] = 0xff & byte;
      digest += 2;
    }
  }

  /* Some debug */
  fprintf(stderr, "Attack on ");
  for(i = 0; i < size; i++)
    fprintf(stderr, "%02x", md[i]);
  fprintf(stderr, "\n");
  
  return md;
}

static void usage(const char *argv0) {

  fprintf(stderr, "Usage : %s -d <sha256-digest> [-s size]\n", argv0);
  exit(1);
}

int main(int argc, char **argv) {

  time_t __t, t;
  char *digest = NULL;
  int c, fd, i = 0, ret = -1;
  size_t size = DEFAULT_SIZE;
  unsigned char *buffer, md[32], hash[32];
  unsigned long long __total = 0, total = 0;

  while((c = getopt(argc, argv, "s:d:")) != -1){
    switch(c){
    case 's' : size = atoi(optarg); break;
    case 'd' : digest = optarg; break;
    }
  }

  /* Check opts */
  if(!digest)
    usage(argv[0]);

  /* Init */
  parse_digest(digest, hash);
  if(!(buffer = malloc(size))){
    perror("buffer");
    goto err_malloc;
  }
  if((fd = open("/dev/urandom", O_RDONLY)) < 0){
    perror("urandom");
    goto err_init;
  }
  
  /* Prepare stats */
  __t = time(NULL);
  
  /* Main algorithm */
  for(;;){
    /* Get data */
    if(read(fd, &buffer[i], sizeof(unsigned char)) <= 0){
      perror("read");
      goto err;
    }

    /* Hash */
    SHA256(buffer, size, md);
    if(!memcmp(md, hash, sizeof md)){
      /* We found a collision */
      ret = 0;
      break;
    }
    
    /* Prepare next round */
    i = (i + 1) % size;
    total++;
    
    /* Get some statistics */
    t = time(NULL);
    if(t != __t){
      /* How many treated */
      int n = (total - __total);
      /* Reset counters */
      __total = total;
      __t = t;
      /* Display */
      fprintf(stderr, "hash generation : %10d/s - Total %llu\r", n, total);
    }
  }

  /* Display computed key */
  write(STDOUT_FILENO, buffer, size);
  
  /* Finish */
 err:
  close(fd);
 err_init:
  free(buffer);
 err_malloc:
  return ret;
}
