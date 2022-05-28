#include "Towa_main.h"

static const struct fuse_operations Towa_oper = {
	.init		= Towa_init,
	.getattr	= Towa_getattr,
	.readdir	= Towa_readdir,
	.open		= Towa_open,
	.read		= Towa_read,
	.write	    = Towa_write,
	.mknod      = Towa_mknod,
	.mkdir		= Towa_mkdir,
	.rmdir		= Towa_rmdir,
};

struct inode{
	//0 for dir 1 for file
	int node_type;
	//first son inode
	struct inode * son_inode;
	//brother inode
	struct inode * next_inode;
	//name
	char * name;
	//contents
	char * content;
};
struct inode *root;

int cnt;
struct inode * newinode(int ty,struct inode *son,struct inode *next_inode,char *name){
	struct inode * ret = (struct inode *) malloc(sizeof(struct inode));
	ret->node_type = ty;
	ret->son_inode	= son;
	ret->next_inode = next_inode;
	if (name!=NULL){
		int len = strlen(name);
		//printf("%d\n",(len+1)*sizeof(char));
		ret->name = (char *) malloc((len+1) * sizeof(char));
		strcpy(ret->name,name);
	}
	else ret->name=NULL;
	return ret;
}

int next(struct inode **p,const char *name){
	struct inode * q = (*p)->son_inode;
	while (q!=NULL){
		if (strcmp(q->name,name)==0){
			(*p) = q;
			return 0;
		}
		q = q->next_inode;
	}
	return -1;
}

int pathwalk(struct inode **p,const char *path){
	static char tmp[256];
	*p = root;
	int la = 1;// first '/'
	for (int i=0; path[i]!='\0'; ++i){
		if (path[i+1]=='\0'||path[i+1]=='/'){
			strncpy(tmp,path+la,i-la+1);
			tmp[i-la+2]='\0';
			la = i+2;
			if (next(p,tmp)<0) return -1;
		}
	}
	return 0;
}

void split_la_name(char *p,char *q){
	int la=1,laq=0;
	for (int i=0; q[i]!='\0'; ++i){
		//printf("q :%c %d\n",q[i+1],(int)q[i+1]);
		if (q[i+1]=='\0'||q[i+1]=='/'){
			strncpy(p,q+la,i-la+1);
			p[i-la+2]='\0';
			la=i+2;
		}
		if (q[i+1]=='/')
			laq=i+1;
	}
	q[laq]='\0';
}
void * Towa_init (struct fuse_conn_info *conn,struct fuse_config *cfg){
	printf("OPE %d: init\n",++cnt);
	root = newinode(0,NULL,NULL,NULL);
	return NULL;
}

int Towa_getattr (const char* path, struct stat* stbuf,struct fuse_file_info *fi){
	printf("OPE %d: getattr %s\n",++cnt,path);
	static struct inode * p;
	int ret = pathwalk(&p,path);
	if (ret<0)
		return -ENOENT;
	
	int res = 0;
	memset(stbuf, 0, sizeof(struct stat));
	if (p->node_type==0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (p->node_type==1) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		if (p->content!=NULL)
			stbuf->st_size = strlen(p->content);
		else
			stbuf->st_size = 0;
	} else
		res = -ENOENT;
	return res;

}

int Towa_mkdir (const char *path, mode_t mode){
	printf("OPE %d: mkdir\n",++cnt);
	static char tmp[256],tmp2[256];
	static struct inode * p;
	strcpy(tmp2,path);
	split_la_name(tmp,tmp2);
	printf("%s %s %d\n",tmp,tmp2,strlen(tmp2));
	int ret = pathwalk(&p,tmp2);
	if (ret < 0)
		return -ENOENT;
	printf("%d\n",ret);
	//empty before
	if (p->son_inode==NULL){
		p->son_inode=newinode(0,NULL,NULL,tmp);
		return 0;
	}
	p = p->son_inode;
	while (p->next_inode!=NULL)
		p = p->next_inode;
	p->next_inode=newinode(0,NULL,NULL,tmp);
	return 0;
}

int Towa_mknod (const char* path, mode_t mode, dev_t rdev){
	printf("OPE %d: mknod path : %s\n",++cnt,path);
	static char tmp[256],tmp2[256];
	static struct inode * p;
	strcpy(tmp2,path);
	split_la_name(tmp,tmp2);
	int ret = pathwalk(&p,tmp2);
	if (ret < 0)
		return -ENOENT;
	//empty before
	if (p->son_inode==NULL){
		p->son_inode=newinode(1,NULL,NULL,tmp);
		return 0;
	}
	p = p->son_inode;
	while (p->next_inode!=NULL)
		p = p->next_inode;
	p->next_inode=newinode(1,NULL,NULL,tmp);
	return 0;
}


int Towa_rmdir (const char *path){
	printf("OPE %d: rmdir\n",++cnt);
	return 0;
}

int Towa_readdir (const char *path, void *buffer, fuse_fill_dir_t filler, off_t oTowaet, struct fuse_file_info *fi, enum fuse_readdir_flags fl){
	printf("OPE %d: readdir\n",++cnt);
	return 0;
}

int Towa_open (const char *path, struct fuse_file_info *fi){
	static struct inode * p;
	int ret = pathwalk(&p,path);
	if (ret<0)
		return -ENOENT;
	return 0;
}

/*static int fioc_do_write(const char *buf, size_t size, off_t offset)
{
	if (fioc_expand(offset + size))
		return -ENOMEM;

	memcpy(fioc_buf + offset, buf, size);

	return size;
}*/

/*static int fioc_write(const char *path, const char *buf, size_t size,
		      off_t offset, struct fuse_file_info *fi)
{
	(void) fi;

	if (fioc_file_type(path) != FIOC_FILE)
		return -EINVAL;

	return fioc_do_write(buf, size, offset);
}*/
char * expand(char *p,size_t len){
	char * ret = (char *) malloc((len+1) * sizeof(char));
	if (p!=NULL){
		strcpy(ret,p);
		free(p);
	}
	return ret;
}
int Towa_do_write(struct inode *p,const char *buf,size_t size,off_t offset){
	if (p->content==NULL||strlen(p->content)<=size+offset){
		p->content=expand(p->content,size+offset);
	}
	memcpy(p->content+offset,buf,size);
	return size;
}

int Towa_write (const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info* fi){
	//printf("OPE %d: write\n",++cnt);
	static struct inode * p;
	int ret = pathwalk(&p,path);
	if (ret<0)
		return -ENOENT;
	if (p->node_type==0)
		return -EINVAL;
	return Towa_do_write(p,buffer,size,offset);
	//printf("%s %s\n",p->content,buffer+offset);
}
/*static int fioc_do_read(char *buf, size_t size, off_t offset)
{
	if (offset >= fioc_size)
		return 0;

	if (size > fioc_size - offset)
		size = fioc_size - offset;

	memcpy(buf, fioc_buf + offset, size);

	return size;
}
static int fioc_read(const char *path, char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	(void) fi;

	if (fioc_file_type(path) != FIOC_FILE)
		return -EINVAL;

	return fioc_do_read(buf, size, offset);
}
*/
int Towa_do_read(struct inode * p,char *buf, size_t size, off_t offset)
{
	if (p->node_type==0||p->content==NULL||offset >= strlen(p->content))
		return 0;
	int sz = strlen(p->content);
	
	if (size > sz - offset)
		size = sz - offset;

	memcpy(buf, p->content + offset, size);

	return size;
}
int Towa_read (const char* path, char* buffer, size_t size, off_t offset, struct fuse_file_info* fi){
	printf("OPE %d: read\n",++cnt);
	static struct inode * p;
	int ret = pathwalk(&p,path);
	if (ret<0)
		return -ENOENT;
	return Towa_do_read(p,buffer,size,offset);
}

int main(int argc, char *argv[])
{
	printf("%d ",argc);
	for (int i=0; i<argc; ++i)
		printf("%s ",argv[i]);
	printf("\n");

	int ret = fuse_main(argc, argv, &Towa_oper, NULL);

	return ret;
}
