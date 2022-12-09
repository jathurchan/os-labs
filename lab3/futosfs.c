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


const char* fileSystem = "test_tosfs_files";
int fileSize;

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

struct tosfs_superblock initSuperblocks(int* castedPtr) {

    struct tosfs_superblock superblock = {
            .magic          = *(castedPtr),
            .block_bitmap   = *(castedPtr + 1),
            .inode_bitmap   = *(castedPtr + 2),
            .block_size     = *(castedPtr + 3),
            .blocks         = *(castedPtr + 4),
            .inodes         = *(castedPtr + 5),
            .root_inode     = *(castedPtr + 6)
    };
    return superblock;
}

struct tosfs_inode initInode(short* shortCasterPtr, int* intCasterPtr, int i) {

    struct tosfs_inode inode = {
            .inode     = *(intCasterPtr + 1024 + i*5),
            .block_no  = *(intCasterPtr + 1025 + i*5),
            .uid       = *(shortCasterPtr + 2052 + i*10),
            .gid       = *(shortCasterPtr + 2053 + i*10),
            .mode      = *(shortCasterPtr + 2054 + i*10),
            .perm      = *(shortCasterPtr + 2055 + i*10),
            .size      = *(shortCasterPtr + 2056 + i*10),
            .nlink     = *(shortCasterPtr + 2057 + i*10)
    };
    return inode;
}

struct tosfs_dentry initDentry(int* intCasterPtr, char* charCasterPtr, int numberBlock, int i) {

    struct tosfs_dentry dentry = {
            .inode  = *(intCasterPtr + 1024*numberBlock + 9*i),
            .name   = (charCasterPtr + 4096*numberBlock + 4 + 36*i)
    };
    return dentry;
}


/*static int futosfs_stat(fuse_ino_t ino, struct stat *stbuf)
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

static struct fuse_lowlevel_ops futosfs_ll_oper = {
        //.lookup		= hello_ll_lookup,
        .getattr	= futosfs_ll_getattr,
        //.readdir	= hello_ll_readdir,
        //.open		= hello_ll_open,
        //.read		= hello_ll_read,
};*/

int main(int argc, char *argv[])
{
    void* mapPointer = mapFileSystem();
    int* intCasterPtr = (int*) mapPointer;
    short* shortCasterPtr = (short*) mapPointer;
    char* charCasterPtr = (char*) mapPointer;

    struct tosfs_superblock superblock = initSuperblocks(intCasterPtr);

    struct tosfs_inode inode[superblock.inodes];
    for (int i = 0; i<superblock.inodes; i++) {
        inode[i] = initInode(shortCasterPtr, intCasterPtr, i + 1);
    }
    struct tosfs_dentry dentry = initDentry(intCasterPtr,charCasterPtr,inode[superblock.root_inode-1].block_no,0);
    printf("%d\n", dentry.inode);
    printf("%s\n", dentry.name);
    struct tosfs_dentry dentry1 = initDentry(intCasterPtr,charCasterPtr,inode[superblock.root_inode-1].block_no,1);
    printf("%d\n", dentry1.inode);
    printf("%s\n", dentry1.name);
    struct tosfs_dentry dentry2 = initDentry(intCasterPtr,charCasterPtr,inode[superblock.root_inode-1].block_no,2);
    printf("%d\n", dentry2.inode);
    printf("%s\n", dentry2.name);
    struct tosfs_dentry dentry3 = initDentry(intCasterPtr,charCasterPtr,inode[superblock.root_inode-1].block_no,3);
    printf("%d\n", dentry3.inode);
    printf("%s\n", dentry3.name);
    //for (int i = 2048; i<2100; i++)
    //printf("%d\n",*(intCasterPtr+i));//*inode[superblock.root_inode-1].block_no);

    /*struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct fuse_chan *ch;
    char *mountpoint;
    int err = -1;

    if (fuse_parse_cmdline(&args, &mountpoint, NULL, NULL) != -1 &&
        (ch = fuse_mount(mountpoint, &args)) != NULL) {
        struct fuse_session *se;

        se = fuse_lowlevel_new(&args, &futosfs_ll_oper,
                               sizeof(futosfs_ll_oper), NULL);
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

    return err ? 1 : 0;*/
}