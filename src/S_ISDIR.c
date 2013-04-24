#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>
int
main(int argc, char **argv){
  struct stat buf;
  if(stat(argv[1],&buf)) {
    printf("[stat]:error!/n");
    return -1;
  }
  printf("st_mode:%d S_ISDIR:%d/n",buf.st_mode,S_ISDIR(buf.st_mode));
  printf("st_nlink:%d/n",buf.st_nlink);
  printf("st_uid:%d/n",buf.st_uid);
  printf("st_gid:%d/n",buf.st_gid);
  printf("st_blksize:%lu/n",buf.st_blksize);
  printf("st_blocks:%lu/n",buf.st_blocks);
  printf("st_atime:%ld/n",buf.st_atime);
  printf("st_mtime:%ld/n",buf.st_mtime);
  printf("st_ctime:%ld/n",buf.st_ctime);
  return 0;
}
