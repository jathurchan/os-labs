#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tosfs.h"
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <fuse_lowlevel.h>

const char* fileSystem = "test_tosfs_files";
int fileSize;
int blockSize = 4096;


void* mapFileSystem() {

    int openFileDescriptor;
    char* mapPointer;
    struct stat statBuffer;

    if ((openFileDescriptor = open(fileSystem, O_RDONLY, 0)) == -1) // Open for reading and writing
        exit(EXIT_FAILURE);
    if (fstat(openFileDescriptor, &statBuffer) == -1)
        exit(EXIT_FAILURE);

    fileSize = (int) statBuffer.st_size;

    if ((mapPointer = mmap(NULL, fileSize, PROT_READ, MAP_SHARED, openFileDescriptor, 0)) == MAP_FAILED)
        exit(EXIT_FAILURE);

    return mapPointer;
}

struct tosfs_superblock initSuperblocks(void* mapPointer) {
    struct tosfs_superblock* superblockPtr = (struct tosfs_superblock*) mapPointer;
    return *superblockPtr;
}

struct tosfs_inode initInode(void* mapPointer, int blockIndex) {
    struct tosfs_inode* inode = (struct tosfs_inode*) (mapPointer + blockSize + sizeof(struct tosfs_inode)*(blockIndex+1));
    return *inode;
}

struct tosfs_dentry initDentry(void* mapPointer, int blockIndex) {
    struct tosfs_dentry* dentry = (struct tosfs_dentry*) (mapPointer + blockSize*2 + sizeof(struct tosfs_dentry)*blockIndex);
    return *dentry;
}

void displayFilesystemInfo(void* mapPointer) {

    printf("Superblock:\n   Magic number: %d  |  block bitmap: %d  |  inode bitmap: %d  |  block size: %d  \n   blocks number: %d  |  inodes number: %d  |  root inode: %d\n\n",
           initSuperblocks(mapPointer).magic, initSuperblocks(mapPointer).block_bitmap,
           initSuperblocks(mapPointer).inode_bitmap, initSuperblocks(mapPointer).block_size,
           initSuperblocks(mapPointer).blocks, initSuperblocks(mapPointer).inodes,
           initSuperblocks(mapPointer).root_inode);

    for (int i = 0; i < 3; i++)
        printf("Inode %d:\n   inode number: %d  |  block number: %d  |  user id: %d  |  group id: %d  \n   mode: %d  |  permissions: %d  |  size: %d  |  number of hardlink: %d\n\n",
               i, initInode(mapPointer, i).inode, initInode(mapPointer, i).block_no, initInode(mapPointer, i).uid,
               initInode(mapPointer, i).gid, initInode(mapPointer, i).mode, initInode(mapPointer, i).perm,
               initInode(mapPointer, i).size, initInode(mapPointer, i).nlink);


    for (int i = 0; i < 4; i++)
        printf("Inode %d:\n   inode number: %d  |  name of file: %s\n\n",
               i, initDentry(mapPointer, i).inode, initDentry(mapPointer, i).name);
}

static int futosfs_stat(fuse_ino_t ino, struct stat *stbuf)
{
    stbuf->st_ino = ino;
    switch (ino) {
        case 1:
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            break;

        case 2:
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = strlen(hello_str);
            break;

        default:
            return -1;
    }
    return 0;
}

static void futosfs_ll_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    struct stat stbuf;

    (void) fi;

    memset(&stbuf, 0, sizeof(stbuf));
    if (hello_stat(ino, &stbuf) == -1)
        fuse_reply_err(req, ENOENT);
    else
        fuse_reply_attr(req, &stbuf, 1.0);
}



static int hello_stat(fuse_ino_t ino, struct stat *stbuf)
{
    stbuf->st_ino = ino;
    switch (ino) {
        case 1:
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            break;

        case 2:
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = strlen(hello_str);
            break;

        default:
            return -1;
    }
    return 0;
}

static void hello_ll_getattr(fuse_req_t req, fuse_ino_t ino,
                             struct fuse_file_info *fi)
{
    struct stat stbuf;

    (void) fi;

    memset(&stbuf, 0, sizeof(stbuf));
    if (hello_stat(ino, &stbuf) == -1)
        fuse_reply_err(req, ENOENT);
    else
        fuse_reply_attr(req, &stbuf, 1.0);
}

static void hello_ll_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    struct fuse_entry_param e;

    if (parent != 1 || strcmp(name, hello_name) != 0)
        fuse_reply_err(req, ENOENT);
    else {
        memset(&e, 0, sizeof(e));
        e.ino = 2;
        e.attr_timeout = 1.0;
        e.entry_timeout = 1.0;
        hello_stat(e.ino, &e.attr);

        fuse_reply_entry(req, &e);
    }
}

struct dirbuf {
    char *p;
    size_t size;
};

static void dirbuf_add(fuse_req_t req, struct dirbuf *b, const char *name,
                       fuse_ino_t ino)
{
    struct stat stbuf;
    size_t oldsize = b->size;
    b->size += fuse_add_direntry(req, NULL, 0, name, NULL, 0);
    b->p = (char *) realloc(b->p, b->size);
    memset(&stbuf, 0, sizeof(stbuf));
    stbuf.st_ino = ino;
    fuse_add_direntry(req, b->p + oldsize, b->size - oldsize, name, &stbuf,
                      b->size);
}

#define min(x, y) ((x) < (y) ? (x) : (y))

static int reply_buf_limited(fuse_req_t req, const char *buf, size_t bufsize,
                             off_t off, size_t maxsize)
{
    if (off < bufsize)
        return fuse_reply_buf(req, buf + off,
                              min(bufsize - off, maxsize));
    else
        return fuse_reply_buf(req, NULL, 0);
}

static void hello_ll_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                             off_t off, struct fuse_file_info *fi)
{
    (void) fi;

    if (ino != 1)
        fuse_reply_err(req, ENOTDIR);
    else {
        struct dirbuf b;

        memset(&b, 0, sizeof(b));
        dirbuf_add(req, &b, ".", 1);
        dirbuf_add(req, &b, "..", 1);
        dirbuf_add(req, &b, hello_name, 2);
        reply_buf_limited(req, b.p, b.size, off, size);
        free(b.p);
    }
}

static void hello_ll_open(fuse_req_t req, fuse_ino_t ino,
                          struct fuse_file_info *fi)
{
    if (ino != 2)
        fuse_reply_err(req, EISDIR);
    else if ((fi->flags & 3) != O_RDONLY)
        fuse_reply_err(req, EACCES);
    else
        fuse_reply_open(req, fi);
}

static void hello_ll_read(fuse_req_t req, fuse_ino_t ino, size_t size,
                          off_t off, struct fuse_file_info *fi)
{
    (void) fi;

    assert(ino == 2);
    reply_buf_limited(req, hello_str, strlen(hello_str), off, size);
}

static struct fuse_lowlevel_ops hello_ll_oper = {
        .lookup		= hello_ll_lookup,
        .getattr	= hello_ll_getattr,
        .readdir	= hello_ll_readdir,
        .open		= hello_ll_open,
        .read		= hello_ll_read,
};

int main(int argc, char *argv[])
{
    void* mapPointer = mapFileSystem();
    displayFilesystemInfo(mapPointer);

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct fuse_chan *ch;
    char *mountpoint;
    int err = -1;

    if (fuse_parse_cmdline(&args, &mountpoint, NULL, NULL) != -1 &&
        (ch = fuse_mount(mountpoint, &args)) != NULL) {
        struct fuse_session *se;

        se = fuse_lowlevel_new(&args, &hello_ll_oper,
                               sizeof(hello_ll_oper), NULL);
        if (se != NULL) {
            if (fuse_set_signal_handlers(se) != -1) {
                fuse_session_add_chan(se, ch);
                err = fuse_session_loop(se);
                fuse_remove_signal_handlers(se);
                fuse_session_remove_chan(ch);
            }
            fuse_session_destroy(se);
        }
        fuse_unmount(mountpoint, ch);
    }
    fuse_opt_free_args(&args);

    return err ? 1 : 0;
}
