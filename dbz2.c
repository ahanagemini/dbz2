#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/sysinfo.h>
#include<string.h>
#include<sys/time.h>
#include<sys/resource.h>
#include "mpi.h"
#include<unistd.h>

struct block_info{
                long length;
                unsigned long offset;
       		int rank; 
		int sno;	
	};
struct block_info** my_blocks;
int blocks_processed=0;
char **a;
unsigned long my_prev_blocks=0;
unsigned long blocks_pn_pw;
int bc_size;
unsigned long block_size;
long blocks_done=0;
int wave_blocks;
long tot_blocks;
char fname[50];
int fd;
int fd_out;
unsigned long my_tot_blocks=0;
void DBz2_enqueue(CIRCLE_handle *handle)
{
	char* newop;	
  	for(int i=0; i<wave_blocks;i++)
	{
		newop=(char*)malloc(sizeof(char)*10);
	 	long block_no=(long)i+(long)blocks_done;	
 		if(block_no>=tot_blocks)
			break;	
		sprintf(newop,10, "%l",block_no);
		handle->enqueue(newop);
	}
}
void DBz2_dequeue(CIRCLE_handle *handle)
{
	char newop[10];	
 	if(blocks_pn_pw<=blocks_processed)
	{
		handle->mem_full=1;
		return;
	}	
	handle->dequeue(newop);
	long block_no=atoi(newop);
	lseek(fd,block_no*block_size,SEEK_SET);
	char* ibuf=(char*)malloc(1*block_size);	
	read(fd,ibuf,(size_t)block_size);
	int outSize=(int) ((inSize*1.01)+600);	
	a[blocks_processed]=malloc(sizeof(char)*outSize); 
	ret=BZ2_bzBuffToBuffCompress(out_buff,&outSize, ibuf, bc_size,0,30);
        struct block_info this_block; 	
	my_blocks[my_tot_blocks].sno=block_no;
	my_blocks[my_tot_blocks].length=outSize;
	my_blocks[my_tot_blocks].rank=rank;
	blocks_processed++;
	my_tot_blocks++:	
	if(blocks_pn_pw<=blocks_processed)
		handle->mem_full=1;
}
void find_wave_size(int b_size)
{
	bc_size=b_size;	
	block_size=b_size*100*1024;
  	struct sysinfo info;
  	int i,rank,size;
	long k;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	struct rlimit limit;
	getrlimit(RLIMIT_DATA,&limit);
	printf("The limit is %lld %lld\n",limit.rlim_cur,limit.rlim_max);
	sysinfo(&info);
	printf("%lu,%lu,%ld\n",info.freeram,info.totalram,block_size);
	unsigned long wave_size_approx=info.freeram-(unsigned long)info.totalram*5/100-(unsigned long)8*block_size-400*102400-128-block_size;
	unsigned long comp_buff_size=(unsigned long)1.01*block_size+600;
	unsigned long waves_blocks_approx=wave_size_approx/comp_buff_size;
	printf("Expected blocks:%lu,%lu\n",wave_size_approx,wave_size_approx/comp_buff_size);
	unsigned long wave_size=wave_size_approx-2*waves_blocks_approx*sizeof(struct block_info);
	printf("Number of possible blocks in a wave=%lu,%lu\n",wave_size,wave_size/comp_buff_size);
	blocks_pn_pw=0.8*wave_size/comp_buff_size;
	printf("Each wave shall have %lu blocks per node",blocks_pn_pw);
	a=(char**)malloc(sizeof(char *)*blocks_pn_pw);
	for(i=0;i<blocks_pn_pw;i++)
	{
		a[i]=(char *)malloc(comp_buff_size*sizeof(char));
		memset(a[i],1,comp_buff_size*sizeof(char));
		if(a[i]==NULL)
		{
   			printf("Out of memory\n");
   			exit(1);
		}
	}
	int blocks_n_w=(int)blocks_pn_pw;

	printf("Total number of blocks=%d",blocks_pn_pw*size);
	MPI_Reduce(&blocks_pn_pw,&wave_blocks,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	printf("The total number of blocks in a wave=%d",wave_blocks);
}

int compress(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int rcount[size];
	int b_size=atoi(argv[1]);
	find_wave_size(b_size);
	fd=open(argv[3],O_RDONLY);
	char fname[20];
	strcpy	
	fd_out=open(strcat	
	struct stat st;
	if(stat(argv[3],&st)!=0)
	{		
		if(st.st_size%block_size==0)
			tot_blocks=(unsigned long)(st.st_size)/(unsigned long)block_size;		
		else
			tot_blocks=(unsigned long)(st.st_size)/(unsigned long)block_size +1;
	}	
	if(tot_blocks%wave_blocks==0)
		num_waves=tot_blocks/wave_blocks;
	else
		num_waves=tot_blocks/wave_blocks+1;
	struct block_info my_blocks[wave_blocks*blocks_pn_pw];	
	if(rank==0)
	{	
		struct block_info *this_wave_blocks[blocks_pn_pw];	
		unsigned long last_offset=0;
	}	
	MPI_Datatype metatype, oldtype[3];	
	MPI_Aint offsets[3],extent;
	offsets[0]=0;
	oldtypes[0]= MPI_LONG;
	blockcounts[0]=1;
	MPI_Type_extent(MPI_LONG, &extent);
	offsets[1]=extent;
	oldtypes[1]=MPI_UNSIGNED_LONG;
	blockcounts[1]=1;
	MPI_Type_extent(MPI_UNSIGNED_LONG, &extent);
	offsets[2]=extent+offsets[1];
	oldtypes[2]=MPI_INT;
	blockcounts[2]=2;
	MPI_Type_struct(3, blockcounts, offsets,oldtypes, &metatype);
	MPI_Type_commit(&metatype);
	struct block_info rbuf[blocks_pn_pw];		
	for(blocks_done=0;blocks_done<tot_blocks;blocks_done+=wave_blocks)	
	{	CIRCLE_init(0,NULL,CIRCLE_DEFAULT_FLAGS);
		CIRCLE_cb_create(&DBz2_Enqueue);
		CIRCLE_cb_process(&DBz2_Dequeue);
		CIRCLE_begin();
		CIRCLE_finalize();
		MPI_Gather(&blocks_processed,1,MPI_INT,rcount,1,MPI_INT,0,comm);	
		long displs[size];	
		displs[0]=0;	
		for(int k=1;k<size,k++)
			displs[k]=displ[k-1]+rcount[k-1]*sizeof(struct block_info);	
		MPI_Gatherv(&my_blocks[my_tot_blocks-1],blocks_processed,metatype,rbuf,rcount,displs,metatype,0,MPI_COMM_WORLD);
		if(rank==0)
		{	
			for(int k=0;k<blocks_pn_pw;k++)
			{
				this_wave_blocks[rbuf[k].sno]=rbuf[k];
			}
			this_wave_blocks[0]->offset=last_offset;	
			for(int k=1;k<blocks_pn_pw;k++)
			{
				this_wave_blocks[k]->offset=this_wave_blocks[k-1]->offset+this_wave_blocks[k-1]->length;
			}
			last_offset=this_wave_blocks[k-1]->offset+this_wave_blocks[k-1]->length;	
		}	
		my_prev_blocks=my_tot_blocks;	
		blocks_processed=0;
	}	
	MPI_Finalize();
}
int main(int argc , char ** argv)
{
	if(strcmp(argv[2],"c")==0)
	{
		compress(argc,&argv);
	}
	else
	{
		decompress(argc, &argv);
	}
} 
