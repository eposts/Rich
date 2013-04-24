#define IS(a,b)  ((a&&b)?!strcasecmp(a, b):0)
#define ASSERT(e) do { if(!(e)) {fprintf(stderr, "AssertException: " #e \
" at %s:%d aborting..", __FILE__, __LINE__); abort(); } } while(0)
#define PATH_MAX 1024
struct filesystem {
        long   f_bsize;                               /* Transfer block size */
        long   f_blocks;                  /* Total data blocks in filesystem */
        long   f_blocksfree;       /* Free blocks available to non-superuser */
        long   f_blocksfreetotal;               /* Free blocks in filesystem */
        long   f_files;                    /* Total file nodes in filesystem */
        long   f_filesfree;                 /* Free file nodes in filesystem */
        char  *mntpath;          /* Filesystem file, directory or mountpoint */
        int    inode_percent;                  /* Used inode percentage * 10 */
        long   inode_total;                      /* Used inode total objects */
        int    space_percent;                  /* Used space percentage * 10 */
        long   space_total;                       /* Used space total blocks */
} ;

char *device_mountpoint(filesystem fsinfo, char *blockdev) {
  FILE *mntfd;
  struct mntent *mnt;

  ASSERT(fsinfo);
  ASSERT(blockdev);

  if ((mntfd = setmntent("/etc/mtab", "r")) == NULL) {
    fprintf(stderr, "Cannot open /etc/mtab file");
    return NULL;
  }
  while ((mnt = getmntent(mntfd)) != NULL) {
    char realpathbuf[PATH_MAX+1];
    /* Try to compare the the filesystem as is, if failed, try to use the symbolic link target */
    if (IS(blockdev, mnt->mnt_fsname) || (realpath(mnt->mnt_fsname, realpathbuf) && ! strcasecmp(blockdev, realpathbuf))) {
      endmntent(mntfd);
      fsinfo.mntpath = Str_dup(mnt->mnt_dir);
      return fsinfo.mntpath;
    }
  }
  endmntent(mntfd);
 fprintf(stderr, "Device %s not found in /etc/mtab", blockdev);
  return NULL;
}

/*
 * Linux filesystem usage statistics. In the case of success result is stored in
 * given information structure.
 */
int filesystem_usage(filesystem fsinfo) {
  struct statvfs usage;

  ASSERT(fsinfo);

  if (statvfs(fsinfo.mntpath, &usage) != 0) {
      fprintf(stderr, "Error getting usage statistics for filesystem '%s' -- %s", fsinfo.mntpath,strerror(errno));
    return FALSE;
  }
  fsinfo.f_bsize =           usage.f_bsize;
  fsinfo.f_blocks =          usage.f_blocks;
  fsinfo.f_blocksfree =      usage.f_bavail;
  fsinfo.f_blocksfreetotal = usage.f_bfree;
  fsinfo.f_files =           usage.f_files;
  fsinfo.f_filesfree =       usage.f_ffree;
    return TRUE;
}
char *device_path(filesystem fsinfo, char *object) {
  struct stat buf;

  ASSERT(fsinfo);
  ASSERT(object);

  if(stat(object, &buf) != 0) {
    fprintf(stderr, "Cannot stat '%s' -- %s", object,strerror(errno));
    return NULL;
  }

  if(S_ISREG(buf.st_mode) || S_ISDIR(buf.st_mode)) {
    fsinfo.mntpath = Str_dup(object);
    return fsinfo.mntpath;
  } else if(S_ISBLK(buf.st_mode) || S_ISCHR(buf.st_mode)) {
    return device_mountpoint(fsinfo, object);
  }
  fprintf(stderr, "Not file, directory or device: '%s'", object);
  return NULL;
}
