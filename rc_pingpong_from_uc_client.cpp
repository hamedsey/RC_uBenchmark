// TO COMPILE
//TO RUN
//./RCC_Client -d mlx5_0 -s 20 -r 20 -g 3 -l 1000000 192.168.1.5 -z 5 -n 8 -t 1
//./RCC_Server -d mlx5_0 -s 20 -r 20 -g 4 -n 16 -t 1

//./RCC_Client -d mlx5_0 -s 20 -r 20 -g 3 -l 30000000 192.168.1.5 -z 5 -m 0 -n 12 -t 12
//./RCC_Server -d mlx5_0 -s 20 -r 20 -g 4 -n 12 -t 12

//gives 16M with no server service time
//./RCC_Server -d mlx5_0 -s 20 -r 200 -g 4 -n 12 -t 12
//./RCC_Client -d mlx5_0 -s 20 -r 200 -g 3 -l 40000000 192.168.1.5 -z 5 -m 0 -n 12 -t 12

//works up to 4092 queues with 12 threads - don't go beyond 14000000
//./RCC_Client -d mlx5_0 -s 20 -r 20 -g 3 -l 14000000 192.168.1.5 -z 5 -m 0 -n 12 -t 12
//./RCC_Server -d mlx5_0 -s 20 -r 20 -g 4 -n 12 -t 12
/*
 * Copyright (c) 2005 Topspin Communications.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//#define _GNU_SOURCE
//#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <malloc.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <pthread.h> 
#include <sched.h>
#include <array>
#include <set>
//#include "pingpong.h"
#include <infiniband/verbs.h>
#include <linux/types.h>  //for __be32 type

#include "rdma_uc.cc"

#include <algorithm>
#include <random>
#define SERVICE_TIME_SIZE 0x8000

#define MAX_QUEUES 1024
#define SHARED_CQ 1
#define RC 1

#define MEASURE_LATENCIES_ON_CLIENT 1
#define RUNTIME 10

enum {
	FIXED = 0,
	NORMAL = 1,
	UNIFORM = 2,
	EXPONENTIAL = 3,
    BIMODAL = 4,
};

enum {
	FB =  5,
	PC = 6,
	SQ = 7,
	NC = 8,
	EXP = 9,
	PCR = 10,
	RR = 11,
	TWENTYP = 12,
	TENP = 13,
	CONTWENTYP = 14,
	CONTENP = 15,
	FIFTYP = 16,
	CONFIFTYP = 17,
	MIXED = 18,
	TWENTYPRAND = 19,
	TENPRAND = 20
};

struct ibv_device      **dev_list;
struct ibv_device	*ib_dev;
char                    *ib_devname = NULL;
char                    *servername = NULL;
unsigned int             port = 18515;
int                      ib_port = 1;
unsigned int             size = 4096;
enum ibv_mtu		 	 mtu = IBV_MTU_1024;
uint64_t             rx_depth = 8192;
uint64_t             window_per_thread = 100;

uint64_t             iters = 1000;
int                      num_cq_events = 0;
int                      sl = 0;
int			 gidx = -1;
char			 gid[33];

uint64_t numConnections = 48;

uint32_t * all_rcnts;
uint32_t * all_scnts;
uint64_t **countPriority;
uint64_t **countSendPriority;
double *rps;
uint64_t goalLoad = 0;
uint64_t numThreads = 0;

uint64_t mean = 1000;
uint64_t bimodal_ratio = 0;
uint64_t long_query_percent = 0;
uint64_t distribution_mode = 0;
uint64_t priority_distribution_mode = 0;

uint64_t connPerThread = 0;
uint8_t *expectedSeqNum;


#if MEASURE_LATENCIES_ON_CLIENT
uint64_t ***latencies;
uint64_t **sendDistribution;
uint64_t **recvDistribution;

struct timespec starttime;
struct timespec **nowtime;
uint16_t **whichconn;
uint16_t **whichcore;

#endif

#if SHARED_CQ
struct ibv_cq **sharedCQ;
#endif

char* output_dir;

uint8_t numEpochs = 1;
std::set<uint16_t> uniqueNumbers20P;
std::set<uint16_t> uniqueNumbers10P;



uint64_t gen_latency(int mean, int mode, int isMeasThread, uint64_t *serviceTime) {
    
	if(isMeasThread == 1) return mean;
	//else if (mode == FIXED) return mean;
	else {
		static uint64_t index = 0;
		uint64_t result = serviceTime[index];
		//printf("index = %lu, OR  = %lu \n",index,SERVICE_TIME_SIZE | index);
		//if( (SERVICE_TIME_SIZE | index) == 0xFFFF) {
		if(index == SERVICE_TIME_SIZE - 1) {
			//printf("worked!, index = %lu \n",index);
			index = 0;
		}
		else {
			//printf("index = %lu \n",index);
			index++;
		}

		//if(result != 1000) printf("index = %lu, wrong value = %d \n", index, result);
		#if MEAS_GEN_LAT 
			printf("gen lat = %d \n",result); 
		#endif
		//if (result < 0) result = 0;
		return result;
	}
}

inline uint16_t gen_priority(uint16_t *priorityID) {

	static uint16_t index = 0;
	uint16_t result = priorityID[index];
	//printf("index = %lu, OR  = %lu \n",index,SERVICE_TIME_SIZE | index);
	if(index == SERVICE_TIME_SIZE - 1) {
		//printf("worked!, index = %lu \n",index);
		index = 0;
	}
	else {
		//printf("index = %lu \n",index);
		index++;
	}

	/*
	uint64_t * countofPriority = (uint64_t*)malloc(numConnections*(sizeof(uint64_t)));
	for(int o = 0; o < SERVICE_TIME_SIZE; o++) countofPriority[priorityID[o]]++;

	for(int o = 0; o < numConnections; o++) printf("%llu  ", countofPriority[o]);
	printf("\n");
	*/

	return result;
}

/*
uint64_t gen_arrival_time(uint64_t *arrivalSleepTime) {
    
	static uint16_t index = 0;
	uint64_t result = arrivalSleepTime[index];
	//printf("index = %lu, OR  = %lu \n",index,SERVICE_TIME_SIZE | index);
	//if( (SERVICE_TIME_SIZE | index) == 0xFFFF) {
	if(index == SERVICE_TIME_SIZE - 1) {
		//printf("worked!, index = %lu \n",index);
		index = 0;
	}
	else {
		//printf("index = %lu \n",index);
		index++;
	}
	return result;
}
*/

double ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}

int pp_get_port_info(struct ibv_context *context, int port,
		     struct ibv_port_attr *attr)
{
	return ibv_query_port(context, port, attr);
}

void wire_gid_to_gid(const char *wgid, union ibv_gid *gid)
{
	char tmp[9];
	__be32 v32;
	int i;
	uint32_t tmp_gid[4];

	for (tmp[8] = 0, i = 0; i < 4; ++i) {
		memcpy(tmp, wgid + i * 8, 8);
		sscanf(tmp, "%x", &v32);
		tmp_gid[i] = be32toh(v32);
	}
	memcpy(gid, tmp_gid, sizeof(*gid));
}

void gid_to_wire_gid(const union ibv_gid *gid, char wgid[])
{
	uint32_t tmp_gid[4];
	int i;

	memcpy(tmp_gid, gid, sizeof(tmp_gid));
	for (i = 0; i < 4; ++i)
		sprintf(&wgid[i * 8], "%08x", htobe32(tmp_gid[i]));
}

enum {
	PINGPONG_RECV_WRID = 1,
	PINGPONG_SEND_WRID = 2,
};

static int page_size;


struct pingpong_dest {
	int lid;
	int qpn;
	int psn;
	union ibv_gid gid;
	uint64_t addr;   // buffer address
    uint32_t rkey;   // remote key
};

struct pingpong_context {
	struct ibv_context	*context;
	struct ibv_comp_channel *channel;
	struct ibv_pd		*pd;
	struct ibv_mr		*mr_send;
	struct ibv_mr		**mr_recv;
	struct ibv_cq		*cq;
	struct ibv_qp		*qp;
	char				*buf_send;
	char				**buf_recv;
	int			 size;
	int			 send_flags;
	uint64_t			 rx_depth;
	int			 pending;
	struct ibv_port_attr     portinfo;
	int                      routs;
	struct pingpong_dest     my_dest;
	struct pingpong_dest    *rem_dest;
	uint64_t id;
	//double rps;
	unsigned int rcnt = 0;
	unsigned int scnt = 0;
	unsigned int souts = 0;
	    
	struct ibv_mr *mr_write;                  // MR handle for buf
    volatile char *buf_write;                 // memory buffer pointer, used for
	uint8_t nextSequenceNumToSend = 0;
};
struct ibv_context	*globalcontext;

struct pingpong_context **ctx;

struct thread_data {
	struct pingpong_context **ctxs;
	uint64_t thread_id;

	#if MEASURE_LATENCIES_ON_CLIENT
	uint64_t** latencyArr;
	uint64_t* receives;

	struct timespec *nowtime;
	uint16_t *whichconn;
	uint16_t *whichcore;

	#endif
};

inline void my_sleep(uint64_t n) {
	//printf("mysleep = %llu \n",n);
	struct timespec ttime,curtime;
	clock_gettime(CLOCK_MONOTONIC,&ttime);
	
	while(1){
		clock_gettime(CLOCK_MONOTONIC,&curtime);
		uint64_t elapsed = (curtime.tv_sec-ttime.tv_sec )/1e-9 + (curtime.tv_nsec-ttime.tv_nsec);

		if (elapsed >= n) {
			//printf("elapsed = %llu \n",elapsed);
			break;
		}
	}
}

static int pp_connect_ctx(struct pingpong_context *ctx, int port, int my_psn,
			  enum ibv_mtu mtu, int sl,
			  struct pingpong_dest *dest, int sgid_idx)
{
	struct ibv_qp_attr attr;// = {
	memset(&attr, 0, sizeof(attr));

	attr.qp_state		= IBV_QPS_RTR;
	attr.path_mtu		= mtu;
	attr.dest_qp_num		= dest->qpn;
	attr.rq_psn		= dest->psn;
	#if RC
		attr.max_dest_rd_atomic = 1;
		attr.min_rnr_timer      = 0;
	#endif
	attr.ah_attr.is_global	= 0;
	attr.ah_attr.dlid		= dest->lid;
	attr.ah_attr.sl		= sl;
	attr.ah_attr.src_path_bits	= 0;
	attr.ah_attr.port_num	= port;

	if (dest->gid.global.interface_id) {
		attr.ah_attr.is_global = 1;
		attr.ah_attr.grh.hop_limit = 1;
		attr.ah_attr.grh.dgid = dest->gid;
		attr.ah_attr.grh.sgid_index = sgid_idx;
	}
	if (ibv_modify_qp(ctx->qp, &attr,
			#if RC
			  IBV_QP_MAX_DEST_RD_ATOMIC |
			  IBV_QP_MIN_RNR_TIMER 		|
			#endif
			  IBV_QP_STATE              |
			  IBV_QP_AV                 |
			  IBV_QP_PATH_MTU           |
			  IBV_QP_DEST_QPN           |
			  IBV_QP_RQ_PSN				
			  )) {
		fprintf(stderr, "Failed to modify QP to RTR\n");
		return 1;
	}

	attr.qp_state	= IBV_QPS_RTS;
	#if RC
		attr.timeout    = 1;
		attr.retry_cnt  = 7;
		attr.rnr_retry  = 7;
		attr.max_rd_atomic  = 1;
	#endif
	attr.sq_psn	    = my_psn;

	if (ibv_modify_qp(ctx->qp, &attr,
			#if RC
			  IBV_QP_TIMEOUT            |
			  IBV_QP_RETRY_CNT          |
			  IBV_QP_RNR_RETRY          |
			  IBV_QP_MAX_QP_RD_ATOMIC   |
			#endif
  			  IBV_QP_STATE                |
			  IBV_QP_SQ_PSN				  
			  )) {
		fprintf(stderr, "Failed to modify QP to RTS\n");
		return 1;
	}

	return 0;
}

static struct pingpong_dest *pp_client_exch_dest(const char *servername, int port, const struct pingpong_dest *my_dest)
{
	struct addrinfo *res, *t;
	struct addrinfo hints;// = {
	memset(&hints, 0, sizeof hints);
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	char *service;
	char msg[sizeof "0000:000000:000000:0000000000000000:00000000:00000000000000000000000000000000"];
	int n;
	int sockfd = -1;
	struct pingpong_dest *rem_dest = NULL;
	//memset(rem_dest, 0, sizeof(struct pingpong_dest));


	char gid[33];

	if (asprintf(&service, "%d", port) < 0)
		return NULL;

	n = getaddrinfo(servername, service, &hints, &res);

	if (n < 0) {
		fprintf(stderr, "%s for %s:%d\n", gai_strerror(n), servername, port);
		free(service);
		return NULL;
	}

	for (t = res; t; t = t->ai_next) {
		sockfd = socket(t->ai_family, t->ai_socktype, t->ai_protocol);
		if (sockfd >= 0) {
			if (!connect(sockfd, t->ai_addr, t->ai_addrlen))
				break;
			close(sockfd);
			sockfd = -1;
		}
	}

	freeaddrinfo(res);
	free(service);

	if (sockfd < 0) {
		fprintf(stderr, "Couldn't connect to %s:%d\n", servername, port);
		return NULL;
	}

	//printf("port number = %llu \n", port);
	gid_to_wire_gid(&my_dest->gid, gid);
	sprintf(msg, "%04x:%06x:%06x:%p:%08x:%s", my_dest->lid, my_dest->qpn,
							my_dest->psn, my_dest->addr, my_dest->rkey, gid);
	if (write(sockfd, msg, sizeof msg) != sizeof msg) {
		fprintf(stderr, "Couldn't send local address\n");
		goto out;
	}

	if (read(sockfd, msg, sizeof msg) != sizeof msg ||
	    write(sockfd, "done", sizeof "done") != sizeof "done") {
		perror("client read/write");
		fprintf(stderr, "Couldn't read/write remote address\n");
		goto out;
	}


	rem_dest = (struct pingpong_dest *)malloc(sizeof *rem_dest);
	if (!rem_dest)
		goto out;

	//printf("msg = %s \n", msg);
	//for(int z = 0; z < n; z++) printf("%x", msg[z]);

	sscanf(msg, "%x:%x:%x:%p:%x:%s", &rem_dest->lid, &rem_dest->qpn,
						&rem_dest->psn, &rem_dest->addr, &rem_dest->rkey, gid);
	
	//printf("sscanf:  LID 0x%04x, QPN 0x%06x, PSN 0x%06x, ADDR %p, RKEY 0x%08x, GID %s \n", rem_dest->lid, rem_dest->qpn, rem_dest->psn, rem_dest->addr, rem_dest->rkey, gid);

	wire_gid_to_gid(gid, &rem_dest->gid);

out:
	close(sockfd);
	return rem_dest;
}

static struct pingpong_dest *pp_server_exch_dest(struct pingpong_context *ctx,
						 int ib_port, enum ibv_mtu mtu,
						 int port, int sl,
						 const struct pingpong_dest *my_dest,
						 int sgid_idx)
{
	struct addrinfo *res, *t;
	struct addrinfo hints;// = {
	memset(&hints, 0, sizeof hints);
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	char *service;
	char msg[sizeof "0000:000000:000000:0000000000000000:00000000:00000000000000000000000000000000"];
	int n;
	int sockfd = -1, connfd;
	struct pingpong_dest *rem_dest = NULL;
	char gid[33];

	if (asprintf(&service, "%d", port) < 0)
		return NULL;

	n = getaddrinfo(NULL, service, &hints, &res);

	if (n < 0) {
		fprintf(stderr, "%s for port %d\n", gai_strerror(n), port);
		free(service);
		return NULL;
	}

	for (t = res; t; t = t->ai_next) {
		sockfd = socket(t->ai_family, t->ai_socktype, t->ai_protocol);
		if (sockfd >= 0) {
			n = 1;

			setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof n);

			if (!bind(sockfd, t->ai_addr, t->ai_addrlen))
				break;
			close(sockfd);
			sockfd = -1;
		}
	}

	freeaddrinfo(res);
	free(service);

	if (sockfd < 0) {
		fprintf(stderr, "Couldn't listen to port %d\n", port);
		return NULL;
	}

	listen(sockfd, 1);
	connfd = accept(sockfd, NULL, NULL);
	close(sockfd);
	if (connfd < 0) {
		fprintf(stderr, "accept() failed\n");
		return NULL;
	}

	n = read(connfd, msg, sizeof msg);
	if (n != sizeof msg) {
		perror("server read");
		fprintf(stderr, "%d/%d: Couldn't read remote address\n", n, (int) sizeof msg);
		goto out;
	}

	rem_dest = (struct pingpong_dest *)malloc(sizeof *rem_dest);
	if (!rem_dest)
		goto out;

	sscanf(msg, "%x:%x:%x:%p:%x:%s", &rem_dest->lid, &rem_dest->qpn,
							&rem_dest->psn, &rem_dest->addr, &rem_dest->rkey, gid);
	wire_gid_to_gid(gid, &rem_dest->gid);

	if (pp_connect_ctx(ctx, ib_port, my_dest->psn, mtu, sl, rem_dest,
								sgid_idx)) {
		fprintf(stderr, "Couldn't connect to remote QP\n");
		free(rem_dest);
		rem_dest = NULL;
		goto out;
	}


	gid_to_wire_gid(&my_dest->gid, gid);
	sprintf(msg, "%04x:%06x:%06x:%p:%08x:%s", my_dest->lid, my_dest->qpn,
							my_dest->psn, my_dest->addr, my_dest->rkey, gid);
	if (write(connfd, msg, sizeof msg) != sizeof msg ||
	    read(connfd, msg, sizeof msg) != sizeof "done") {
		fprintf(stderr, "Couldn't send/recv local address\n");
		free(rem_dest);
		rem_dest = NULL;
		goto out;
	}

out:
	close(connfd);
	return rem_dest;
}

static struct pingpong_context *pp_init_ctx(struct ibv_device *ib_dev, int size, int rx_depth, int port)
{
	uint64_t totalBytes = 100;
	uint64_t bufOffset = 4096;
	struct pingpong_context *ctx;

	ctx = (pingpong_context*)calloc(1, sizeof *ctx);
	if (!ctx)
		return NULL;

	ctx->size       = size;
	//ctx->send_flags = IBV_SEND_INLINE;
	ctx->rx_depth   = rx_depth;

	ctx->buf_send = (char*)malloc(size*sizeof(char));
	//ctx->buf = memalign(page_size, size);
	if (!ctx->buf_send) {
		fprintf(stderr, "Couldn't allocate work buf.\n");
		goto clean_ctx;
	}
	/* FIXME memset(ctx->buf, 0, size); */
	memset(ctx->buf_send, 0x00, size);

	ctx->buf_recv = (char**)malloc(ctx->rx_depth*sizeof(char*));
	for(uint64_t z = 0; z < ctx->rx_depth; z++) {
		ctx->buf_recv[z] = (char*)malloc(size*sizeof(char));
		//ctx->buf = memalign(page_size, size);
		if (!ctx->buf_recv[z]) {
			fprintf(stderr, "Couldn't allocate work buf.\n");
			goto clean_ctx;
		}

		/* FIXME memset(ctx->buf, 0, size); */
		memset(ctx->buf_recv[z], 0x00, size);
	}


	static int poo = 0;
	if(poo == 0) {
		globalcontext = ibv_open_device(ib_dev);
		if (!globalcontext) {
			fprintf(stderr, "Couldn't get context for %s\n",
				ibv_get_device_name(ib_dev));
			goto clean_buffer;
		}
	}
	ctx->context = globalcontext;
	ctx->id = poo;

	ctx->channel = NULL;

	ctx->pd = ibv_alloc_pd(ctx->context);
	if (!ctx->pd) {
		fprintf(stderr, "Couldn't allocate PD\n");
		goto clean_comp_channel;
	}

	ctx->mr_send = ibv_reg_mr(ctx->pd, ctx->buf_send, size, IBV_ACCESS_LOCAL_WRITE);
	if (!ctx->mr_send) {
		fprintf(stderr, "Couldn't register MR\n");
		goto clean_pd;
	}

	ctx->mr_recv = (struct ibv_mr**)malloc(ctx->rx_depth*sizeof(struct ibv_mr*));
	for(uint64_t z = 0; z < ctx->rx_depth; z++) {
		ctx->mr_recv[z] = ibv_reg_mr(ctx->pd, ctx->buf_recv[z], size, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);
		if (!ctx->mr_recv[z]) {
			fprintf(stderr, "Couldn't register MR\n");
			goto clean_pd;
		}
	}

	#if SHARED_CQ
		if(ctx->id % connPerThread == 0) {
			//printf("created sharedCQ, connid = %llu \n", ctx->id);
			sharedCQ[ctx->id/(connPerThread)] = ibv_create_cq(ctx->context, (connPerThread)*(2*rx_depth + 1), NULL, ctx->channel, 0);
			if (!sharedCQ[ctx->id/(connPerThread)]) {
				fprintf(stderr, "Couldn't create CQ\n");
				goto clean_cq;
			}
		}
	#else
		ctx->cq = ibv_create_cq(ctx->context, 2*rx_depth + 1, NULL, ctx->channel, 0);
		if (!ctx->cq) {
			fprintf(stderr, "Couldn't create CQ\n");
			goto clean_cq;
		}
	#endif

	assert(posix_memalign((void**)(&ctx->buf_write), bufOffset, totalBytes) == 0);

	//printf("buf address %p \n",(ctx->buf_write));
	//ctx->buf_write[0] = 255;
 	//for (uint z = 0; z < totalBytes; z++) if((uint8_t)ctx->buf_write[z] != 0) printf("buf index %d = %d \n",z,(uint8_t)ctx->buf_write[z]); 

    ctx->mr_write = ibv_reg_mr(ctx->pd, (void *)ctx->buf_write, totalBytes, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);
    assert(ctx->mr_write != NULL);

	{
		struct ibv_qp_attr attr;
		struct ibv_qp_init_attr init_attr;// = {
		memset(&attr, 0, sizeof(attr));
		memset(&init_attr, 0, sizeof(init_attr));

		#if SHARED_CQ
			//printf("sharedCQ index = %llu \n",ctx->id/connPerThread);
			init_attr.send_cq = sharedCQ[ctx->id/connPerThread];
			init_attr.recv_cq = sharedCQ[ctx->id/connPerThread];
		#else
			init_attr.send_cq = ctx->cq;
			init_attr.recv_cq = ctx->cq;
		#endif
		init_attr.cap.max_send_wr  = 8192;//rx_depth;	
		init_attr.cap.max_recv_wr  = 8192;//rx_depth;
		init_attr.cap.max_send_sge = 1;
		init_attr.cap.max_recv_sge = 1;
		init_attr.sq_sig_all = 1;
		#if RC
			init_attr.qp_type = IBV_QPT_RC;
		#else 
			init_attr.qp_type = IBV_QPT_UC;
		#endif
		static uint64_t first = 1;
		if(first == 1) {
			first = 0;
			while(1)
			{				
				ctx->qp = ibv_create_qp(ctx->pd, &init_attr);
				if (!ctx->qp)  {
					fprintf(stderr, "Couldn't create QP\n");
					goto clean_cq;
				}
				if(ctx->qp->qp_num %(MAX_QUEUES) == 0) break;
				else ibv_destroy_qp(ctx->qp);
			}
		}
		else {
			ctx->qp = ibv_create_qp(ctx->pd, &init_attr);
				if (!ctx->qp)  {
					fprintf(stderr, "Couldn't create QP\n");
					goto clean_cq;
				}
		}

		ibv_query_qp(ctx->qp, &attr, IBV_QP_CAP, &init_attr);
		if (init_attr.cap.max_inline_data >= size) {
			ctx->send_flags |= IBV_SEND_INLINE;
		}
	}

	{
		struct ibv_qp_attr attr;// = {
		attr.qp_state        = IBV_QPS_INIT;
		attr.pkey_index      = 0;
		attr.port_num        = port;
		attr.qp_access_flags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE;

		if (ibv_modify_qp(ctx->qp, &attr,
				  IBV_QP_STATE              |
				  IBV_QP_PKEY_INDEX         |
				  IBV_QP_PORT               |
				  IBV_QP_ACCESS_FLAGS)) {
			fprintf(stderr, "Failed to modify QP to INIT\n");
			goto clean_qp;
		}
	}
	poo++;

	return ctx;

clean_qp:
	ibv_destroy_qp(ctx->qp);

clean_cq:
	ibv_destroy_cq(ctx->cq);

clean_mr:
	ibv_dereg_mr(ctx->mr_send);

clean_pd:
	ibv_dealloc_pd(ctx->pd);

clean_comp_channel:
	if (ctx->channel)
		ibv_destroy_comp_channel(ctx->channel);

clean_device:
	ibv_close_device(ctx->context);

clean_buffer:
	free(ctx->buf_send);

clean_ctx:
	free(ctx);

	return NULL;
}

static int pp_close_ctx(struct pingpong_context *ctx)
{
	//printf("destroy qp \n");
	if (ibv_destroy_qp(ctx->qp)) {
		fprintf(stderr, "Couldn't destroy QP\n");
		return 1;
	}
	//printf("qp \n");
	if (ibv_destroy_cq(ctx->cq)) {
		fprintf(stderr, "Couldn't destroy CQ\n");
		return 1;
	}
	//printf("cq \n");

	if (ibv_dereg_mr(ctx->mr_send)) {
		fprintf(stderr, "Couldn't deregister MR\n");
		return 1;
	}
	//printf("mrsend \n");

	for(uint64_t z = 0; z < ctx->rx_depth; z++) {
		if (ibv_dereg_mr(ctx->mr_recv[z])) {
			fprintf(stderr, "Couldn't deregister MR\n");
			return 1;
		}
	}

	//printf("mrrecv \n");

	if (ibv_dealloc_pd(ctx->pd)) {
		fprintf(stderr, "Couldn't deallocate PD\n");
		return 1;
	}
	//printf("pd \n");


	if (ctx->channel) {
		if (ibv_destroy_comp_channel(ctx->channel)) {
			fprintf(stderr, "Couldn't destroy completion channel\n");
			return 1;
		}
	}
	//printf("channel \n");

	if (ibv_close_device(ctx->context)) {
		fprintf(stderr, "Couldn't release context\n");
		return 1;
	}
	//printf("context \n");

	free(ctx->buf_send);
	for(uint64_t z = 0; z < ctx->rx_depth; z++) {
		free(ctx->buf_recv[z]);
	}
	//printf("bufs \n");

	free(ctx);
	//printf("ctx \n");

	return 0;
}

static int pp_post_recv(struct pingpong_context *ctx, uint64_t bufIndex)
{
	struct ibv_sge list;
	memset(&list, 0, sizeof(list));

	list.addr	= (uintptr_t) ctx->buf_recv[bufIndex];
	list.length = ctx->size;
	list.lkey	= ctx->mr_recv[bufIndex]->lkey;

	struct ibv_recv_wr wr;
	memset(&wr, 0, sizeof(wr));

	wr.wr_id	  = bufIndex;
	wr.sg_list    = &list;
	wr.num_sge    = 1;
	//};
	struct ibv_recv_wr *bad_wr;
	memset(&bad_wr, 0, sizeof(bad_wr));

	//printf("inside recv \n");
	return ibv_post_recv(ctx->qp, &wr, &bad_wr);
}

static int pp_post_send(struct pingpong_context *ctx, bool signal)
{
	struct ibv_sge list;// = {
	memset(&list, 0, sizeof(list));

	list.addr	= (uintptr_t) ctx->buf_send;
	list.length = ctx->size;
	list.lkey	= ctx->mr_send->lkey;

	struct ibv_send_wr wr;
	memset(&wr, 0, sizeof(wr));

	wr.wr_id	  = ctx->rx_depth;
	wr.sg_list    = &list;
	wr.num_sge    = 1;
	wr.opcode     = IBV_WR_SEND;
	wr.send_flags = ctx->send_flags | IBV_SEND_INLINE;

	if(signal == true) wr.send_flags = ctx->send_flags | IBV_SEND_SIGNALED; //need to have a signaled completion every signalInterval
	else wr.send_flags = ctx->send_flags;

	struct ibv_send_wr *bad_wr;
	memset(&bad_wr, 0, sizeof(bad_wr));

	return ibv_post_send(ctx->qp, &wr, &bad_wr);
}

static int pp_post_write(struct pingpong_context *ctx, bool signal, uint64_t offset)
{
	struct ibv_sge list;// = {
	memset(&list, 0, sizeof(list));

	list.addr	= (uintptr_t)(ctx->buf_write) + offset;
	//printf("buf index %d = %d \n",offset,(uint8_t)ctx->buf_write[offset]); 
	list.length = 1;
	list.lkey	= ctx->mr_write->lkey;

	struct ibv_send_wr wr;
	memset(&wr, 0, sizeof(wr));

	wr.wr_id	  = ctx->rx_depth;
	wr.sg_list    = &list;
	wr.num_sge    = 1;
	wr.opcode     = IBV_WR_RDMA_WRITE;
	//wr.send_flags = ctx->send_flags | IBV_SEND_INLINE;
	wr.next = NULL;

	//if(signal == true) wr.send_flags = ctx->send_flags | IBV_SEND_SIGNALED; //need to have a signaled completion every signalInterval
	wr.send_flags = ctx->send_flags;

	wr.wr.rdma.remote_addr = ctx->rem_dest->addr + offset;
	//printf("remote addr = %x \n", wr.wr.rdma.remote_addr);
	wr.wr.rdma.rkey = ctx->rem_dest->rkey;

	struct ibv_send_wr *bad_wr;
	memset(&bad_wr, 0, sizeof(bad_wr));

	return ibv_post_send(ctx->qp, &wr, &bad_wr);
}

static void usage(const char *argv0)
{
	printf("Usage:\n");
	printf("  %s            start a server and wait for connection\n", argv0);
	printf("  %s <host>     connect to server at <host>\n", argv0);
	printf("\n");
	printf("Options:\n");
	printf("  -p, --port=<port>      listen on/connect to port <port> (default 18515)\n");
	printf("  -d, --ib-dev=<dev>     use IB device <dev> (default first device found)\n");
	printf("  -i, --ib-port=<port>   use port <port> of IB device (default 1)\n");
	printf("  -s, --size=<size>      size of message to exchange (default 4096)\n");
	printf("  -m, --mtu=<size>       path MTU (default 1024)\n");
	printf("  -r, --rx-depth=<dep>   number of receives to post at a time (default 500)\n");
	printf("  -n, --iters=<iters>    number of exchanges (default 1000)\n");
	printf("  -l, --sl=<sl>          service level value\n");
	printf("  -e, --events           sleep on CQ events (default poll)\n");
	printf("  -g, --gid-idx=<gid index> local port gid index\n");
	printf("  -c, --chk              validate received buffer\n");
}

void* threadfunc(void* x) {
	//printf("hello \n");
	struct timeval           start, end;
	struct thread_data *tdata = (struct thread_data *)x;
	struct pingpong_context **ctxs = (struct pingpong_context **)(tdata->ctxs);
	uint64_t thread_id = tdata->thread_id;

	#if MEASURE_LATENCIES_ON_CLIENT
	uint64_t ** latencyArr = tdata->latencyArr;
	uint64_t *receives = tdata->receives;

	struct timespec *nowtime = tdata->nowtime;
	uint16_t *whichconn = tdata->whichconn;
	uint16_t *whichcore = tdata->whichcore;

	#endif

	uint64_t offset = thread_id % numConnections;
	unsigned int rcnt = 0, scnt = 0, souts = 0;
	//printf("thread id = %llu \n", thread_id);

	//printf("ctx->id = %llu \n", ctxs[thread_id]->id);
	cpu_set_t cpuset;
    CPU_ZERO(&cpuset);       				   //clears the cpuset
    if(thread_id < 12) CPU_SET(thread_id /*+2*//*12*/, &cpuset);  //set CPU 2 on cpuset
    else CPU_SET(thread_id + 12/*+2*//*12*/, &cpuset);  //set CPU 2 on cpuset
	sched_setaffinity(0, sizeof(cpuset), &cpuset);

	uint64_t singleThreadWait = 1000000000/goalLoad;
	uint64_t avg_inter_arr_ns = numThreads*singleThreadWait;

	if(thread_id == 0) printf("avg_inter_arr_ns = %llu \n",avg_inter_arr_ns);

	//uint64_t *arrivalSleepTime = (uint64_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));
	std::random_device rd{};
	std::mt19937 gen{rd()};
	std::exponential_distribution<double> exp{1/((double)avg_inter_arr_ns)};//-SEND_SERVICE_TIME)};
	//for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
	//	arrivalSleepTime[i] = exp(gen);
		//printf("arrivalSleepTime = %llu \n", arrivalSleepTime[i]);
	//}



	uint64_t *serviceTime = (uint64_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));
	uint16_t *priorityID = (uint16_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));

	uint16_t *priority50p = (uint16_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));
	uint16_t *priority20p = (uint16_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));
	uint16_t *priority10p = (uint16_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));



	if(distribution_mode == FIXED) {
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			serviceTime[i] = mean;
		}
	}
	else if(distribution_mode == EXPONENTIAL) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::exponential_distribution<double> exp{1/(double)mean};
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) serviceTime[i] = exp(gen);
	}
	else if(distribution_mode == BIMODAL) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({double(100-long_query_percent), double(long_query_percent)});
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			uint8_t result = bm(gen);
			//printf("result = %d \n",result);
			if (result == 0) serviceTime[i] = mean;
			else serviceTime[i] = mean*bimodal_ratio;
		}
	}
	else printf("Invalid service time distribution mode \n");
	std::uniform_int_distribution<uint64_t>* bm = nullptr;
	if(priority_distribution_mode == FB) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
    	bm = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, ((thread_id+1)*connPerThread)-1);
    	//std::uniform_int_distribution<uint64_t> bm(0, numConnections-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[i] =(*bm)(gen);
		}

		/*
		for (int i = 0; i < SERVICE_TIME_SIZE; ++i) {
			printf("%llu ",priorityID[i]);
			if ((i + 1) % 20 == 0) // Print 20 numbers per line for readability
				printf("\n");
		}
		*/
	}
	else if(priority_distribution_mode == PC) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});
		
		uint16_t numQueuesGettingMoreLoad = /*numConnections*/ connPerThread/5; //8/5;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);
		std::uniform_int_distribution<uint64_t>* ur80 = new std::uniform_int_distribution<uint64_t>(eightyPercent, ((thread_id+1)*connPerThread)-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			if(bm(gen) == 0) {
				priorityID[i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
				//if(twentyPercent == numQueuesGettingMoreLoad-1) twentyPercent = 0;
				//else twentyPercent++;
			}
			else {
				priorityID[i] = (*ur80)(gen);//eightyPercent;//8*eightyPercent;
				//if(eightyPercent == numConnections-1/*7*/) eightyPercent = numQueuesGettingMoreLoad;
				//else eightyPercent++;
			}
		}
	}
	else if(priority_distribution_mode == SQ) {
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			//priorityID[i] = numConnections-1;
			priorityID[i] = thread_id*connPerThread;
		}
	}
	else if(priority_distribution_mode == NC) {
		//traffic passes through 20 queues all the time and through the rest with a probability of 5%
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});
		
		uint16_t numQueuesGettingMoreLoad = 20;
		//uint16_t numQueuesGettingLessLoad = numConnections-numQueuesGettingMoreLoad;

		uint16_t twentyPercent = 0;
		uint16_t eightyPercent = numQueuesGettingMoreLoad;

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			if(bm(gen) == 0) {
				priorityID[i] = twentyPercent;
				if(twentyPercent == numQueuesGettingMoreLoad-1) twentyPercent = 0;
				else twentyPercent++;
			}
			else {
				priorityID[i] = eightyPercent;
				if(eightyPercent == numConnections-1) eightyPercent = numQueuesGettingMoreLoad;
				else eightyPercent++;
			}
		}
	}
	else if(priority_distribution_mode == EXP) {

		//generating 10000 item array of discrete exponentially distributed priorities
		const uint64_t arrayLength = 10000;
        std::array<uint64_t, arrayLength> arr;
		std::random_device rd{};
		std::mt19937 gen{rd()};
		double meanPriorities = numConnections/2;//4;
		//double lambda = 100; // Exponential distribution parameter
		std::exponential_distribution<double> exp{1/(double)meanPriorities};
		for (int i = 0; i < arr.size(); ++i) {
			// Generate a random number from the exponential distribution
			double randomNumber = exp(gen);

			// Scale the random number to the range [0, 255]
			uint64_t scaledNumber = static_cast<uint64_t>(randomNumber) % numConnections;

			arr[i] = (numConnections-1) - scaledNumber;
		}

		// Sort the array in ascending order
		std::sort(arr.begin(), arr.end());
		/*
		uint64_t count = 0;
		for(int i = 0; i < numConnections; i++) {
			count = 0;
			for(int j = 0; j < 10000; j++) {
				if(i == arr[j]) count++;
			}
			printf("i = %llu, count = %llu \n", i , count);
		}

		for (int i = 0; i < arr.size(); ++i) {
			std::cout << arr[i] << " ";
			if ((i + 1) % 20 == 0) // Print 20 numbers per line for readability
				std::cout << std::endl;
		}
		*/
    	std::uniform_int_distribution<uint64_t> bm(0, arrayLength-1);


		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[i] = arr[bm(gen)];
		}
		/*
		for (int i = 0; i < SERVICE_TIME_SIZE; ++i) {
			printf("%llu ",priorityID[i]);
			if ((i + 1) % 20 == 0) // Print 20 numbers per line for readability
				printf("\n");
		}
		*/
	}
	else if(priority_distribution_mode == PCR) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});
		
		uint16_t numQueuesGettingMoreLoad = numConnections/5; //8/5;
		uint16_t twentyPercent = 0;
		uint16_t eightyPercent = numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(0, numQueuesGettingMoreLoad-1);
		std::uniform_int_distribution<uint64_t>* ur80 = new std::uniform_int_distribution<uint64_t>(numQueuesGettingMoreLoad, numConnections-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			if(bm(gen) == 0) {
				priorityID[i] = numConnections-1-(*ur20)(gen);//twentyPercent;//8*twentyPercent;
				//if(twentyPercent == numQueuesGettingMoreLoad-1) twentyPercent = 0;
				//else twentyPercent++;
			}
			else {
				priorityID[i] = numConnections-1-(*ur80)(gen);//eightyPercent;//8*eightyPercent;
				//if(eightyPercent == numConnections-1/*7*/) eightyPercent = numQueuesGettingMoreLoad;
				//else eightyPercent++;
			}
		}
	}
	else if(priority_distribution_mode == RR) {
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[i] = i%numConnections;
			//priorityID[i] = thread_id*connPerThread;
		}
	}
	else if(priority_distribution_mode == TWENTYP) { //each thread sends to 20% of its fraction of queues
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = /*numConnections*/ connPerThread/5; //8/5;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == TENP) { //each thread sends to 10% of its fraction of queues
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = /*numConnections*/ connPerThread/10; //8/5;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == CONTWENTYP) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = (numConnections/5); //8/5; 200
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+connPerThread;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == CONTENP) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = (numConnections/10); //8/5; 200
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+connPerThread;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == FIFTYP) { //each thread sends to 50% of its fraction of queues
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = /*numConnections*/ connPerThread/2; //8/5;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == CONFIFTYP) { //each thread sends to an even fraction of the first 50% of queues
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = numConnections/2; //8/5;
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+connPerThread;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == MIXED) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
    	bm = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, ((thread_id+1)*connPerThread)-1);
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) priorityID[i] =(*bm)(gen);


		uint16_t numQueuesGettingMoreLoad = numConnections/2; //8/5;
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+connPerThread;
		std::uniform_int_distribution<uint64_t>* ur50 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) priority50p[i] = (*ur50)(gen);

		numQueuesGettingMoreLoad = (numConnections/5); //8/5; 200
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		twentyPercent = thread_id*connPerThread; //0
		eightyPercent = twentyPercent+connPerThread;
		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) priority20p[i] = (*ur20)(gen);

		numQueuesGettingMoreLoad = (numConnections/10); //8/5; 200
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		twentyPercent = thread_id*connPerThread; //0
		eightyPercent = twentyPercent+connPerThread;
		std::uniform_int_distribution<uint64_t>* ur10 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) priority10p[i] = (*ur10)(gen);
	}
	else if(priority_distribution_mode == TWENTYPRAND) { //each thread sends to 20% of its fraction of queues

		//uint16_t twentyPercent = thread_id*connPerThread; //0
		//uint16_t eightyPercent = twentyPercent+numQueuesGettingMoreLoad;

		uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers20P.size())/numThreads;
		printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			std::set<uint16_t>::iterator it = uniqueNumbers20P.begin();
			std::advance(it, (*ur20)(gen));
			priorityID[i] = *it;//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == TENPRAND) { //each thread sends to 20% of its fraction of queues

		uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers10P.size())/numThreads;
		printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
		std::uniform_int_distribution<uint64_t>* ur10 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			std::set<uint16_t>::iterator it = uniqueNumbers10P.begin();
			std::advance(it, (*ur10)(gen));
			priorityID[i] = *it;//twentyPercent;//8*twentyPercent;
		}
	}

	//reduce logic
	//get rid of send completions
	//poll less items
	
	//uint64_t prio_prev = 0;
	struct pingpong_context *ctx = ctxs[0];
	printf("rx_depth = %llu \n", ctx->rx_depth);

	uint16_t signalInterval = 8192 - ctx->rx_depth;
	printf("signalInterval = %llu \n", signalInterval);

	/*
	uint8_t *expectedSeqNum = (uint8_t*)malloc(numConnections*sizeof(uint8_t));
	for(uint64_t g = 0; g < numConnections; g++) expectedSeqNum[g] = 0;
	
	uint64_t *outOfOrderNum = (uint64_t*)malloc(numConnections*sizeof(uint64_t));
	for(uint64_t g = 0; g < numConnections; g++) outOfOrderNum[g] = 0;
	*/

	//uint8_t *firstOne = (uint8_t*)malloc(numConnections*sizeof(uint8_t));
	//for(uint64_t g = 0; g < numConnections; g++) firstOne[g] = 1;

	struct timespec ttime,curtime;
	clock_gettime(CLOCK_MONOTONIC,&ttime);
	uint64_t index = 0;

	/*
	uint64_t * countofPriority = (uint64_t*)malloc(numConnections*(sizeof(uint64_t)));
	for(int o = 0; o < SERVICE_TIME_SIZE; o++) countofPriority[priorityID[o]]++;

	for(int o = 0; o < numConnections; o++) printf("%llu  ", countofPriority[o]);
	printf("\n");
	*/

	uint32_t ingressTS = 0;
	uint32_t egressTS = 0;
	uint64_t latency = 0;
	uint8_t epochNumber = 0;
	uint16_t priority = 0;

	clock_gettime(CLOCK_MONOTONIC,&starttime);

	while(epochNumber != numEpochs) {

		if (gettimeofday(&start, NULL)) {
			perror("gettimeofday");
			return 0;
		}
		while (1) {
				//compare to previous version that was save
				//try adding shared CQ
				//try adding PC traffic
				//go back to doing send completions (didn't work)
				//lowering generated load (works to some extent) - works when capping load at 15000000
			//if(thread_id % 2 == 1) {
			//	sleep(10);
			//	break;
			//}
			//uint16_t priority = priorityID[index];/*(*bm)(gen);*/ //gen_priority(priorityID);
			//if(index == SERVICE_TIME_SIZE - 1) index = 0;
			//else index++;

			if(priority_distribution_mode == MIXED) {
				if(epochNumber == 0 || epochNumber == 1) priority = /*(*bm)(gen);*/ gen_priority(priorityID);
				else if(epochNumber == 2) priority = /*(*bm)(gen);*/ gen_priority(priority50p);
				else if(epochNumber == 3) priority = /*(*bm)(gen);*/ gen_priority(priority20p);
				else if(epochNumber == 4) priority = /*(*bm)(gen);*/ gen_priority(priority10p);
				else assert(false);
			}
			else priority = /*(*bm)(gen);*/ gen_priority(priorityID);

			//printf("T %llu , priority = %llu \n",thread_id, priority);
			//if(priority != prio_prev) 
			ctx = ctxs[priority];
			//printf("souts = %llu, rcnt = %llu, ctx->rx_depth = %llu \n", ctx->souts, ctx->rcnt, ctx->rx_depth);
			//if (servername && (ctx->souts - (ctx->scnt*signalInterval) < signalInterval)) {
			//prio_prev = priority;

			if (servername && (ctx->souts - ctx->rcnt < ctx->rx_depth)) { //&& (souts - rcnt < window_per_thread)) {

				uint64_t req_lat = mean;//gen_latency(mean, distribution_mode, 0, serviceTime);
				req_lat = req_lat >> 4;
				//printf("lat = %d \n",req_lat); 


				#if MEAS_GEN_LAT 
					printf("lat = %d \n",req_lat); 
				#endif
				//uint8_t lat_lower = req_lat & ((1u <<  8) - 1);//req_lat % 0x100;
				//uint8_t lat_upper = (req_lat >> 8) & ((1u <<  8) - 1);//req_lat / 0x100;f
				//printf("sleep_int_lower = %lu, sleep_int_upper = %lu, sleep_time = %lu \n", lat_lower, lat_upper, req_lat);

				//service time
				//if(firstOne[priority] == 1) {
				//	ctx->buf_send[0] = 128; //128, 192
				//	firstOne[priority] = 0;
				//}
				//else 
				ctx->buf_send[0] = 0; //0, 64

				ctx->buf_send[11] = (uint8_t)(req_lat & ((1u <<  8) - 1));
				ctx->buf_send[10] = (uint8_t)((req_lat >> 8) & ((1u <<  8) - 1));
				
				//printf("sleep_int_lower = %llu \n",(uint8_t)ctx->buf_send[11]);
				//printf("sleep_int_upper = %llu \n",(uint8_t)ctx->buf_send[10]);

				//sequence number
				ctx->buf_send[12] = ctx->nextSequenceNumToSend;
				//ctx->buf_send[19] = priority;
				if(ctx->nextSequenceNumToSend == 255) ctx->nextSequenceNumToSend = 0;
				else ctx->nextSequenceNumToSend++;

				//printf("buf_send \n");
				ctx->souts++;
				souts++;
				uint64_t success = pp_post_send(ctx, (ctx->souts%signalInterval == 0));
				if (success == EINVAL) printf("Invalid value provided in wr \n");
				else if (success == ENOMEM)	printf("Send Queue is full or not enough resources to complete this operation \n");
				else if (success == EFAULT) printf("Invalid value provided in qp \n");
				else if (success != 0) {
					printf("success = %d, \n",success);
					fprintf(stderr, "Couldn't post send 3 \n");
					//return 1;
				}
				else {
					//printf("send posted ... \n");
				}

				
				//success = pp_post_write(ctx, (ctx->souts%signalInterval == 0), 0);
				/*
				if(ctx->souts == 1) {
					//if(ctx->souts % 2 == 0) 
					(res.buf)[0] = 255;
					//else (res.buf)[0] = 255;
					success = post_send(&res, IBV_WR_RDMA_WRITE, priority);
					if (success == EINVAL) printf("Invalid value provided in wr \n");
					else if (success == ENOMEM)	printf("Send Queue is full or not enough resources to complete this operation \n");
					else if (success == EFAULT) printf("Invalid value provided in qp \n");
					else if (success != 0) {
						printf("success = %d, \n",success);
						fprintf(stderr, "Couldn't post send 3 \n");
						//return 1;
					}
					else {
						//printf("write posted ... \n");
					}
				}
				*/
				countSendPriority[thread_id][priority]++;
				//double interArrivalWaitTime = ran_expo(1/1000);

				if(priority_distribution_mode == MIXED) {
					if(epochNumber > 0) {
						uint64_t arrival_wait_time = exp(gen);//gen_arrival_time(arrivalSleepTime);
						//printf("%llu \n", arrival_wait_time);
						clock_gettime(CLOCK_MONOTONIC,&curtime);
						uint64_t elapsed = (curtime.tv_sec-ttime.tv_sec )/1e-9 + (curtime.tv_nsec-ttime.tv_nsec);
						if(elapsed < arrival_wait_time) my_sleep(arrival_wait_time - elapsed);
						//my_sleep(arrival_wait_time);
					}
				}
				else {
					uint64_t arrival_wait_time = exp(gen);//gen_arrival_time(arrivalSleepTime);
					//printf("%llu \n", arrival_wait_time);
					clock_gettime(CLOCK_MONOTONIC,&curtime);
					uint64_t elapsed = (curtime.tv_sec-ttime.tv_sec )/1e-9 + (curtime.tv_nsec-ttime.tv_nsec);
					if(elapsed < arrival_wait_time) my_sleep(arrival_wait_time - elapsed);
					//my_sleep(arrival_wait_time);
				}
			}

			clock_gettime(CLOCK_MONOTONIC,&ttime);

			//if(ctx->id == 1 && servername) break;
			if (gettimeofday(&end, NULL)) {
				perror("gettimeofday");
				return 0;
			}

			//runtime
			if(end.tv_sec - start.tv_sec > RUNTIME) {
				printf("epoch %llu done \n", epochNumber);
				epochNumber++;
				break;
			}

			#if SHARED_CQ
				struct ibv_wc wc[connPerThread*2*ctx->rx_depth];
			#else
				struct ibv_wc wc[2*ctx->rx_depth];
			#endif
			int ne, i;
			//printf("hello, thread id = %llu \n", thread_id);
			#if SHARED_CQ
				ne = ibv_poll_cq(sharedCQ[thread_id], connPerThread*2*ctx->rx_depth, wc);
				//printf("after poll cq \n");
			#else
				ne = ibv_poll_cq(ctx->cq, 2*ctx->rx_depth, wc);
			#endif
			//if(servername) printf("polling, ne = %llu \n", ne);
			if (ne < 0) {
				fprintf(stderr, "poll CQ failed %d\n", ne);
				return 0;
			}

			if (ne < 1) continue;

			for (i = 0; i < ne; ++i) {
				if (wc[i].status != IBV_WC_SUCCESS) {
					fprintf(stderr, "T%d - scnt = %llu, rcnt = %llu, Failed status %s (%d) for wr_id %d\n", thread_id, scnt, rcnt,
						ibv_wc_status_str(wc[i].status),
						wc[i].status, (int) wc[i].wr_id);
					return 0;
				}
				uint64_t wrID = (uint64_t) wc[i].wr_id;
				//printf("wrID = %llu \n", wrID);

				uint64_t connID = wc[i].qp_num & (MAX_QUEUES-1);

				#if SHARED_CQ
					//printf("connID = %llu \n", connID);
					ctx = ctxs[connID];
				#endif
				
				if(wrID == ctx->rx_depth) {
					ctx->scnt++;
					scnt++;
					//printf("send complete ... scnt = %llu \n", ctx->scnt);
				}
				else if (wrID >= 0 && wrID < ctx->rx_depth) {

					//printf("recv complete ... rcnt = %llu \n", ctx->rcnt);

					countPriority[thread_id][connID]++;
					//printf("connID = %llu , qpn = %llu \n", connID, wc[i].qp_num);

					//handling userspace sequence number
					/*
					if(expectedSeqNum[connID] != (uint8_t)ctx->buf_recv[wrID][12]) {
						printf("conn %llu ... received sequence %llu ... expected %llu \n", connID, (uint8_t)ctx->buf_recv[wrID][12], expectedSeqNum[connID]);
						//outOfOrderNum[connID]++;
					}
					else //printf("conn %llu ... received sequence %llu = expected %llu \n", connID, (uint8_t)ctx->buf_recv[wrID][12], expectedSeqNum[connID]);

					if(expectedSeqNum[connID] == 255) expectedSeqNum[connID] = 0;
					else expectedSeqNum[connID]++;
					*/
					//handling userspace sequence number



							egressTS =  ((uint8_t)ctx->buf_recv[wrID][5] << 24) + ((uint8_t)ctx->buf_recv[wrID][6] << 16) + ((uint8_t)ctx->buf_recv[wrID][7] << 8) + ((uint8_t)ctx->buf_recv[wrID][8]);
							ingressTS = ((uint8_t)ctx->buf_recv[wrID][1] << 24) + ((uint8_t)ctx->buf_recv[wrID][2] << 16) + ((uint8_t)ctx->buf_recv[wrID][3] << 8) + ((uint8_t)ctx->buf_recv[wrID][4]);

							//egressTS = (unsigned long *)strtoul(conn->buf_recv[bufID] + 46, NULL, 0);
							
							if(egressTS > ingressTS) latency = egressTS - ingressTS;
							else latency = /*4294967295*/ uint64_t(1099511627775) + (egressTS - ingressTS);

							//printf("ingressTS = %llu, egressTS = %llu, latency = %llu \n", ingressTS, egressTS, latency);

							#if MEASURE_LATENCIES_ON_CLIENT						
							if(rcnt > 1000) {
								//printf("priority = %llu \n", priorityID);
								latencyArr[connID][receives[connID]] = latency;
								//printf("wrote latency \n");
								//if(receives[priorityID] < goalLoad*RUNTIME*numEpochs - 1) 
								receives[connID]++;

								#if debug						
								if(conn->rcnt % INTERVAL == 0 && conn->rcnt > 0) {
									double curr_clock = now();
									printf("T%d - %f rps, rcnt = %d, scnt = %d \n",thread_num,INTERVAL/(curr_clock-prev_clock),conn->rcnt,conn->scnt);
									prev_clock = curr_clock;
								}
								#endif
							}

							clock_gettime(CLOCK_MONOTONIC,&nowtime[rcnt]);
							whichconn[rcnt] = connID;
							//assert((uint8_t)connID == (uint8_t)thread_id);
							whichcore[rcnt] = (uint8_t)ctx->buf_recv[wrID][9];
							//assert((uint8_t)ctx->buf_recv[wrID][9] == 0 || (uint8_t)ctx->buf_recv[wrID][9] == 1 || (uint8_t)ctx->buf_recv[wrID][9] == 2 || (uint8_t)ctx->buf_recv[wrID][9] == 3);
							#endif


					ctx->rcnt++;
					rcnt++;

					assert(pp_post_recv(ctx, wrID) == 0);
					if (ctx->routs < ctx->rx_depth) {
						fprintf(stderr,
							"Couldn't post receive (%d)\n", ctx->routs);
						return 0;
					}

					/*
					uint64_t success = pp_post_send(ctx);
					if (success == EINVAL) printf("Invalid value provided in wr \n");
					else if (success == ENOMEM)	printf("Send Queue is full or not enough resources to complete this operation \n");
					else if (success == EFAULT) printf("Invalid value provided in qp \n");
					else if (success != 0) {
						printf("success = %d, \n",success);
						fprintf(stderr, "Couldn't post send 3 \n");
						//return 1;
					}
					else {
						//printf("send posted ... sentCount = %llu, scnt = %llu \n", sentCount, conn->scnt);
					}
					*/
				}
				else {
					fprintf(stderr, "Completion for unknown wr_id %d\n",(int) wc[i].wr_id);
					return 0;
				}
			}
		}
	}

	ctx = ctxs[offset];

	float usec = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	//long long bytes = (long long) size * iters * 2;
		
	all_rcnts[thread_id] = rcnt;
	all_scnts[thread_id] = scnt;

	rps[thread_id] = rcnt/(usec/1000000.);
	printf("%d iters in %.5f seconds, rps = %f \n", rcnt, usec/1000000., rps[thread_id]);


	/*
	uint64_t * countofPriorityAgain = (uint64_t*)malloc(numConnections*(sizeof(uint64_t)));
	for(int o = 0; o < SERVICE_TIME_SIZE; o++) countofPriorityAgain[priorityID[o]]++;

	for(int o = 0; o < numConnections; o++) printf("%llu  ", countofPriorityAgain[o]);
	printf("\n");
	*/

	//printf("\n\nprinting sequence number stats \n.\n.\n.\n");
	/*
	uint64_t totalOutOfOrderNum = 0;
	for(uint64_t h = 0; h < numConnections; h++) {
		//printf("%llu  ", outOfOrderNum[h]);
		totalOutOfOrderNum += outOfOrderNum[h];
	}
	printf("\nT%d totalOutOfOrderNum = %llu \n", thread_id, totalOutOfOrderNum);
	*/
	//printf(".\n.\n.\nend of sequence number stats ... \n");

	//printf("%lld bytes in %.2f seconds = %.2f Mbit/sec\n", bytes, usec / 1000000., bytes * 8. / usec);
	//printf("%d iters in %.2f seconds = %.2f usec/iter\n", iters, usec / 1000000., usec / iters);

	//free(ctx->rem_dest);

	//end of thread operations
}

int main(int argc, char *argv[])
{
	while (1) {
		int c;

		static struct option long_options[] = {
			{ .name = "long_query_percent",     .has_arg = 1, .flag = NULL, .val = 'p' },
			{ .name = "ib-dev",   .has_arg = 1, .flag = NULL, .val = 'd' },
			{ .name = "ib-port",  .has_arg = 1, .flag = NULL, .val = 'i' },
			{ .name = "size",     .has_arg = 1, .flag = NULL, .val = 's' },
			{ .name = "distribution_mode",      .has_arg = 1, .flag = NULL, .val = 'm' },
			{ .name = "priority_distribution_mode",      .has_arg = 1, .flag = NULL, .val = 'z' },
			{ .name = "rx-depth", .has_arg = 1, .flag = NULL, .val = 'r' },
			{ .name = "iters",    .has_arg = 1, .flag = NULL, .val = 'n' },
			{ .name = "load",     .has_arg = 1, .flag = NULL, .val = 'l' },
			{ .name = "events",   .has_arg = 0, .flag = NULL, .val = 'e' },
			{ .name = "gid-idx",  .has_arg = 1, .flag = NULL, .val = 'g' },
			{ .name = "chk",      .has_arg = 0, .flag = NULL, .val = 'c' },
			{ .name = "bimodal_ratio",        .has_arg = 0, .flag = NULL, .val = 'x' },
			{ .name = "num-threads", .has_arg = 1, .flag = NULL, .val = 't' },
			{}
		};

		c = getopt_long(argc, argv, "p:d:i:s:m:z:r:n:l:eg:c:x:t:f:",
				long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'p':
			long_query_percent = strtoul(optarg, NULL, 0);
			/*
			port = strtoul(optarg, NULL, 0);
			if (port > 65535) {
				usage(argv[0]);
				return 1;
			}
			*/
			break;

		case 'd':
			ib_devname = strdupa(optarg);
			break;

		case 'i':
			ib_port = strtol(optarg, NULL, 0);
			if (ib_port < 1) {
				usage(argv[0]);
				return 1;
			}
			break;

		case 's':
			size = strtoul(optarg, NULL, 0);
			break;

		case 'm':
			distribution_mode = strtoul(optarg, NULL, 0);
			break;

		case 'z':
			priority_distribution_mode = strtoul(optarg, NULL, 0);
			break;

		case 'r':
			rx_depth = strtoul(optarg, NULL, 0);
			break;

		case 'n':
			numConnections = strtoul(optarg, NULL, 0);
			break;

		case 'l':
			goalLoad = strtol(optarg, NULL, 0);
			break;

		case 'g':
			gidx = strtol(optarg, NULL, 0);
			break;

		case 'x':
			bimodal_ratio = strtoul(optarg, NULL, 0);
			break;

		case 't':
			numThreads = strtoul(optarg, NULL, 0);
			break;
      
	  	case 'f':
        	output_dir = optarg;
        	break;
		
		default:
			usage(argv[0]);
			return 1;
		}
	}

	if (optind == argc - 1)
		servername = strdupa(argv[optind]);
	else if (optind < argc) {
		usage(argv[0]);
		return 1;
	}

	page_size = sysconf(_SC_PAGESIZE);

	dev_list = ibv_get_device_list(NULL);
	if (!dev_list) {
		perror("Failed to get IB devices list");
		return 1;
	}

	if (!ib_devname) {
		ib_dev = *dev_list;
		if (!ib_dev) {
			fprintf(stderr, "No IB devices found\n");
			return 1;
		}
	} else {
		int i;
		for (i = 0; dev_list[i]; ++i)
			if (!strcmp(ibv_get_device_name(dev_list[i]), ib_devname))
				break;
		ib_dev = dev_list[i];
		if (!ib_dev) {
			fprintf(stderr, "IB device %s not found\n", ib_devname);
			return 1;
		}
	}

	window_per_thread = rx_depth/numThreads;
	
	double totalRPS = 0;
	rps = (double*)malloc((numConnections)*sizeof(double));
	memset(rps, 0, numConnections*sizeof(double));

	connPerThread = numConnections/numThreads;
	printf("connections per thread = %llu \n", connPerThread);
	#if SHARED_CQ
		sharedCQ = (struct ibv_cq **)malloc(numThreads*sizeof(struct ibv_cq *));
	#endif
	
	ctx = (struct pingpong_context**)malloc(numConnections*sizeof(struct pingpong_context*));

	for(uint64_t y = 0; y < numConnections; y++) {
		my_sleep(uint64_t(5E6));
		//memset(ctx[y], 0, sizeof(struct pingpong_context*));
		//printf("y = %llu \n", y);
		ctx[y]  = pp_init_ctx(ib_dev, size, rx_depth, ib_port);
		if (!ctx[y])
			return 1;
		//printf("hello 22 \n");

		ctx[y]->id = y;
		ctx[y]->routs = 0;
		ctx[y]->souts = 0;
		ctx[y]->scnt = 0;
		ctx[y]->rcnt = 0;
		for(uint64_t z = 0; z < ctx[y]->rx_depth; z++) {
			//printf("hello 222 \n");
			assert(pp_post_recv(ctx[y], z) == 0);
			//printf("hello 3 \n");
			ctx[y]->routs++;
		}
		//printf("hello 2222 \n");

		if (ctx[y]->routs < ctx[y]->rx_depth) {
			fprintf(stderr, "Couldn't post receive (%d)\n", ctx[y]->routs);
			return 1;
		}
		
		//printf("hello 0 \n");

		if (pp_get_port_info(ctx[y]->context, ib_port, &ctx[y]->portinfo)) {
			fprintf(stderr, "Couldn't get port info\n");
			return 1;
		}
		//printf("hello 1 \n");

		ctx[y]->my_dest.lid = ctx[y]->portinfo.lid;
		if (ctx[y]->portinfo.link_layer != IBV_LINK_LAYER_ETHERNET &&
								!ctx[y]->my_dest.lid) {
			fprintf(stderr, "Couldn't get local LID\n");
			return 1;
		}

		if (gidx >= 0) {
			if (ibv_query_gid(ctx[y]->context, ib_port, gidx, &ctx[y]->my_dest.gid)) {
				fprintf(stderr, "can't read sgid of index %d\n", gidx);
				return 1;
			}
		} else
			memset(&ctx[y]->my_dest.gid, 0, sizeof ctx[y]->my_dest.gid);

		ctx[y]->my_dest.qpn = ctx[y]->qp->qp_num;
		ctx[y]->my_dest.psn = 0;//lrand48() & 0xffffff;
		inet_ntop(AF_INET6, &ctx[y]->my_dest.gid, gid, sizeof gid);
		ctx[y]->my_dest.addr = (uintptr_t)ctx[y]->buf_write;
		ctx[y]->my_dest.rkey = ctx[y]->mr_write->rkey;

		//printf("  local address:  LID 0x%04x, QPN 0x%06x, PSN 0x%06x, GID %s, ADDR %p, RKEY 0x%08x \n",
		//	ctx[y]->my_dest.lid, ctx[y]->my_dest.qpn, ctx[y]->my_dest.psn, gid, ctx[y]->my_dest.addr, ctx[y]->my_dest.rkey);


		if (servername)
			ctx[y]->rem_dest = pp_client_exch_dest(servername, port, &ctx[y]->my_dest);
		else
			ctx[y]->rem_dest = pp_server_exch_dest(ctx[y], ib_port, mtu, port, sl, &ctx[y]->my_dest, gidx);

		if (!ctx[y]->rem_dest)
			return 1;

		inet_ntop(AF_INET6, &ctx[y]->rem_dest->gid, gid, sizeof gid);
		//printf("  remote address: LID 0x%04x, QPN 0x%06x, PSN 0x%06x, GID %s, ADDR %p, RKEY 0x%08x\n",
		//	ctx[y]->rem_dest->lid, ctx[y]->rem_dest->qpn, ctx[y]->rem_dest->psn, gid, ctx[y]->rem_dest->addr, ctx[y]->rem_dest->rkey);

		if (servername)
			if (pp_connect_ctx(ctx[y], ib_port, ctx[y]->my_dest.psn, mtu, sl, ctx[y]->rem_dest,
						gidx))
				return 1;
		
		/*
		if (servername) {
			for(uint64_t z = 0; z < ctx[y]->rx_depth; z++) {
				uint64_t success = pp_post_send(ctx[y]);
				if (success == EINVAL) printf("Invalid value provided in wr \n");
				else if (success == ENOMEM)	printf("Send Queue is full or not enough resources to complete this operation \n");
				else if (success == EFAULT) printf("Invalid value provided in qp \n");
				else if (success != 0) {
					printf("success = %d, \n",success);
					fprintf(stderr, "Couldn't post send 3 \n");
					//return 1;
				}
				else {
					//printf("send posted ... z = %llu \n", z);
				}
				//double interArrivalWaitTime = ran_expo(1/1000);
				//printf("%f \n", interArrivalWaitTime);
			}
		}
		*/

		//if(servername) sleep(1);
		//ctx[y]->rcnt = ctx[y]->scnt = 0;

	}

	pthread_t pt[numConnections];
    int ret;

	//numThreads = numConnections;

    rps = (double*)malloc((numThreads)*sizeof(double));
	all_rcnts = (uint32_t*)malloc(numThreads*sizeof(uint32_t));
	all_scnts = (uint32_t*)malloc(numThreads*sizeof(uint32_t));
	countPriority = (uint64_t **)malloc(numThreads*sizeof(uint64_t *));

	for(int t = 0; t < numThreads; t++) {
		countPriority[t] = (uint64_t *)malloc(numConnections*sizeof(uint64_t));
		for(int g = 0; g < numConnections; g++) countPriority[t][g] = 0;
	}

	countSendPriority = (uint64_t **)malloc(numThreads*sizeof(uint64_t *));
	for(int t = 0; t < numThreads; t++) {
		countSendPriority[t] = (uint64_t *)malloc(numConnections*sizeof(uint64_t));
		for(int g = 0; g < numConnections; g++) countSendPriority[t][g] = 0;
	}

	expectedSeqNum = (uint8_t *)malloc(numConnections*sizeof(uint8_t));
	for(int c = 0; c < numConnections; c++) expectedSeqNum[c] = 0;


	#if MEASURE_LATENCIES_ON_CLIENT
	//latencies.resize(active_thread_num, vector<vector<uint64_t>>(numberOfPriorities, vector<uint64_t>(0,0)));

	recvDistribution = (uint64_t**)malloc(numThreads*sizeof(uint64_t*));
	for(int i = 0; i < numThreads; i++) {
		recvDistribution[i] = (uint64_t*)malloc(numConnections*sizeof(uint64_t));
		for(int j = 0; j < numConnections; j++) recvDistribution[i][j] = 0;
	}
	
	latencies = (uint64_t ***)malloc(numThreads*sizeof(uint64_t**));
	for(uint64_t b = 0; b < numThreads; b++) {
		latencies[b] = (uint64_t**)malloc(numConnections*sizeof(uint64_t*));
		for(uint64_t p = 0; p < numConnections; p++) {
			latencies[b][p] = (uint64_t*)malloc(goalLoad*RUNTIME*numEpochs*sizeof(uint64_t));
			for(uint64_t v = 0; v < goalLoad*RUNTIME*numEpochs; v++) {
				latencies[b][p][v] = 0;
			}
		}
	}	

	struct timespec **nowtime;

	whichconn = (uint16_t **)malloc(numThreads*sizeof(uint16_t*));
	for(uint64_t b = 0; b < numThreads; b++) {
		whichconn[b] = (uint16_t*)malloc(goalLoad*RUNTIME*numEpochs*sizeof(uint16_t));
		for(uint64_t v = 0; v < goalLoad*RUNTIME*numEpochs; v++) {
			whichconn[b][v] = 0;
		}
	}	

	whichcore = (uint16_t **)malloc(numThreads*sizeof(uint16_t*));
	for(uint64_t b = 0; b < numThreads; b++) {
		whichcore[b] = (uint16_t*)malloc(goalLoad*RUNTIME*numEpochs*sizeof(uint16_t));
		for(uint64_t v = 0; v < goalLoad*RUNTIME*numEpochs; v++) {
			whichcore[b][v] = 0;
		}
	}	

	nowtime = (struct timespec **)malloc(numThreads*sizeof(struct timespec *));
	for(uint64_t b = 0; b < numThreads; b++) {
		nowtime[b] = (struct timespec *)malloc(goalLoad*RUNTIME*numEpochs*sizeof(struct timespec));
		for(uint64_t v = 0; v < goalLoad*RUNTIME*numEpochs; v++) {
				nowtime[b][v].tv_sec = 0;
				nowtime[b][v].tv_nsec = 0;
		}
	}	

	#endif


	std::random_device rd{};
	std::mt19937 gen{rd()};

	uint16_t numQueuesGettingMoreLoad = numConnections/5;
	std::uniform_int_distribution<uint64_t>* rand = new std::uniform_int_distribution<uint64_t>(0, numConnections-1);

	// Generate unique numbers
	while (uniqueNumbers20P.size() < numQueuesGettingMoreLoad) {
		int num = (*rand)(gen);
		uniqueNumbers20P.insert(num);  // Insert only if it's unique
	}

	numQueuesGettingMoreLoad = numConnections/10;
	// Generate unique numbers
	while (uniqueNumbers10P.size() < numQueuesGettingMoreLoad) {
		int num = (*rand)(gen);
		uniqueNumbers10P.insert(num);  // Insert only if it's unique
	}



	do_uc(ib_devname, servername, port, ib_port, gidx, numConnections, numThreads);

	//if(servername == NULL) numThreads = 1;
    for(int x = 0; x < numThreads; x++) {
		struct thread_data *tdata = (struct thread_data *)malloc(sizeof(struct thread_data));

		tdata->ctxs = ctx;
		tdata->thread_id = x;

		#if MEASURE_LATENCIES_ON_CLIENT
		tdata->latencyArr = latencies[x];
		tdata->receives = recvDistribution[x];
		tdata->whichconn = whichconn[x];
		tdata->whichcore = whichcore[x];
		tdata->nowtime = nowtime[x];
		#endif

		ret = pthread_create(&pt[x], NULL, threadfunc, tdata); 
		assert(ret == 0);
	}

    for(int x = 0; x < numThreads; x++) {
		//printf("x = %llu \n", x);
        int ret = pthread_join(pt[x], NULL);
        assert(!ret);
    }

	uint32_t total_rcnt = 0;
	uint32_t total_scnt = 0;

	for(int i = 0; i < numThreads; i++){
		//printf("allrcnt[%d] = %llu \n", i, all_rcnts[i]);
		total_rcnt += all_rcnts[i];
		total_scnt += all_scnts[i];
	}


	uint64_t *totalPerThread = (uint64_t *)malloc(numThreads*sizeof(uint64_t));
	for(int t = 0; t < numThreads; t++) totalPerThread[t] = 0;

	printf("total received per connection ... \n");
	for(int g = 0; g < numConnections; g++) {
		printf("P%llu   ",g);

		for(int t = 0; t < numThreads; t++){
			totalPerThread[t] += countPriority[t][g];
			printf("%llu    ",countPriority[t][g]);
		}
		printf("\n");
	}		
	printf("\n");
	printf("total received per thread ... \n");
	for(int t = 0; t < numThreads; t++){
		printf("%llu   ",totalPerThread[t]);
	}

	printf("\n");
	printf("\n");
	
	uint64_t *totalSendPerThread = (uint64_t *)malloc(numThreads*sizeof(uint64_t));
	for(int t = 0; t < numThreads; t++) totalSendPerThread[t] = 0;

	printf("total sent per connection ... \n");
	for(int g = 0; g < numConnections; g++) {
		printf("P%llu   ",g);
		for(int t = 0; t < numThreads; t++){
			totalSendPerThread[t] += countSendPriority[t][g];
			printf("%llu    ",countSendPriority[t][g]);
		}
		printf("\n");
	}		
	printf("\n");
	printf("total sent per thread ... \n");
	for(int t = 0; t < numThreads; t++){
		printf("%llu   ",totalSendPerThread[t]);
	}



	printf("\n");
	printf("total rcnt = %d, total scnt = %d \n",(int)total_rcnt,(int)total_scnt);


	for(int i = 0; i < numThreads; i++){
		printf("rps = %f \n",rps[i]);
		totalRPS += rps[i];
	}
	printf("%f \n",totalRPS);

	
	#if MEASURE_LATENCIES_ON_CLIENT
	char* output_name_all;
	char* output_name_time;
	char* output_name_conn;
	char* output_name_core;

	char* output_name_time1;
	char* output_name_conn1;
	char* output_name_core1;

	if(priority_distribution_mode == FB) 	  {
		asprintf(&output_name_all, "%s/all_%d_%d_conn_FB.result", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time, "%s/all_%d_%d_conn_FB.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn, "%s/all_%d_%d_conn_FB.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core, "%s/all_%d_%d_conn_FB.core", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time1, "%s/all_%d_%d_conn_FB_1.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn1, "%s/all_%d_%d_conn_FB_1.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core1, "%s/all_%d_%d_conn_FB_1.core", output_dir, (int)totalRPS, numConnections);
	}
	else if(priority_distribution_mode == PC) {
		asprintf(&output_name_all, "%s/all_%d_%d_conn_PC.result", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time, "%s/all_%d_%d_conn_PC.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn, "%s/all_%d_%d_conn_PC.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core, "%s/all_%d_%d_conn_PC.core", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time1, "%s/all_%d_%d_conn_PC_1.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn1, "%s/all_%d_%d_conn_PC_1.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core1, "%s/all_%d_%d_conn_PC_1.core", output_dir, (int)totalRPS, numConnections);
	}
	else if(priority_distribution_mode == SQ) {
		asprintf(&output_name_all, "%s/all_%d_%d_conn_SQ.result", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time, "%s/all_%d_%d_conn_SQ.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn, "%s/all_%d_%d_conn_SQ.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core, "%s/all_%d_%d_conn_SQ.core", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time1, "%s/all_%d_%d_conn_SQ_1.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn1, "%s/all_%d_%d_conn_SQ_1.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core1, "%s/all_%d_%d_conn_SQ_1.core", output_dir, (int)totalRPS, numConnections);
	}
	else if(priority_distribution_mode == CONTWENTYP) {
		asprintf(&output_name_all, "%s/all_%d_%d_conn_CONTWENTYP.result", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time, "%s/all_%d_%d_conn_CONTWENTYP.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn, "%s/all_%d_%d_conn_CONTWENTYP.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core, "%s/all_%d_%d_conn_CONTWENTYP.core", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time1, "%s/all_%d_%d_conn_CONTWENTYP_1.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn1, "%s/all_%d_%d_conn_CONTWENTYP_1.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core1, "%s/all_%d_%d_conn_CONTWENTYP_1.core", output_dir, (int)totalRPS, numConnections);
	}
	else if(priority_distribution_mode == CONTENP) {
		asprintf(&output_name_all, "%s/all_%d_%d_conn_CONTENP.result", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time, "%s/all_%d_%d_conn_CONTENP.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn, "%s/all_%d_%d_conn_CONTENP.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core, "%s/all_%d_%d_conn_CONTENP.core", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time1, "%s/all_%d_%d_conn_CONTENP_1.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn1, "%s/all_%d_%d_conn_CONTENP_1.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core1, "%s/all_%d_%d_conn_CONTENP_1.core", output_dir, (int)totalRPS, numConnections);
	}
	else if(priority_distribution_mode == CONFIFTYP) {
		asprintf(&output_name_all, "%s/all_%d_%d_conn_CONFIFTYP.result", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time, "%s/all_%d_%d_conn_CONFIFTYP.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn, "%s/all_%d_%d_conn_CONFIFTYP.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core, "%s/all_%d_%d_conn_CONFIFTYP.core", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time1, "%s/all_%d_%d_conn_CONFIFTYP_1.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn1, "%s/all_%d_%d_conn_CONFIFTYP_1.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core1, "%s/all_%d_%d_conn_CONFIFTYP_1.core", output_dir, (int)totalRPS, numConnections);
	}
	else if(priority_distribution_mode == MIXED) {
		asprintf(&output_name_all, "%s/all_%d_%d_conn_MIXED.result", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time, "%s/all_%d_%d_conn_MIXED.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn, "%s/all_%d_%d_conn_MIXED.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core, "%s/all_%d_%d_conn_MIXED.core", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time1, "%s/all_%d_%d_conn_MIXED_1.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn1, "%s/all_%d_%d_conn_MIXED_1.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core1, "%s/all_%d_%d_conn_MIXED_1.core", output_dir, (int)totalRPS, numConnections);
	}
	else if(priority_distribution_mode == TWENTYPRAND) {
		asprintf(&output_name_all, "%s/all_%d_%d_conn_TWENTYPRAND.result", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time, "%s/all_%d_%d_conn_TWENTYPRAND.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn, "%s/all_%d_%d_conn_TWENTYPRAND.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core, "%s/all_%d_%d_conn_TWENTYPRAND.core", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time1, "%s/all_%d_%d_conn_TWENTYPRAND_1.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn1, "%s/all_%d_%d_conn_TWENTYPRAND_1.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core1, "%s/all_%d_%d_conn_TWENTYPRAND_1.core", output_dir, (int)totalRPS, numConnections);
	}
	else if(priority_distribution_mode == TENPRAND) {
		asprintf(&output_name_all, "%s/all_%d_%d_conn_TENPRAND.result", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time, "%s/all_%d_%d_conn_TENPRAND.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn, "%s/all_%d_%d_conn_TENPRAND.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core, "%s/all_%d_%d_conn_TENPRAND.core", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_time1, "%s/all_%d_%d_conn_TENPRAND_1.time", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_conn1, "%s/all_%d_%d_conn_TENPRAND_1.conn", output_dir, (int)totalRPS, numConnections);
		asprintf(&output_name_core1, "%s/all_%d_%d_conn_TENPRAND_1.core", output_dir, (int)totalRPS, numConnections);
	}
	else assert(false);
	
	FILE *f_all = fopen(output_name_all, "wb");
	FILE *f_time = fopen(output_name_time, "wb");
	FILE *f_conn = fopen(output_name_conn, "wb");
	FILE *f_core = fopen(output_name_core, "wb");

	FILE *f_time1 = fopen(output_name_time1, "wb");
	FILE *f_conn1 = fopen(output_name_conn1, "wb");
	FILE *f_core1 = fopen(output_name_core1, "wb");

	
	uint64_t countedLatencyZero = 0;
	printf("dumping latencies ... \n");
	for(uint i = 0; i < numConnections; i++){

		//char* output_name;
		//asprintf(&output_name, "%s/%d_%d.result", output_dir, i, (int)totalRPS);
		//FILE *f = fopen(output_name, "wb");

		for(uint j = 0; j < numThreads; j++) {
			uint64_t index = 0;
			//while(latencies[j][i][index] > 0) {
			for(uint index = 0; index < recvDistribution[j][i]; index++) {
				if(latencies[j][i][index] == 0) {
					countedLatencyZero++;
					continue;
				}
				//fprintf(f, "%llu \n", latencies[j][i][index]);
				fprintf(f_all, "%llu \n", latencies[j][i][index]);
				//index++;
			}
		}
		//fclose(f);
	}
	fclose(f_all);
	printf("count of zero latency = %llu \n",countedLatencyZero);
	
	uint64_t* numReqCompletedInEachSecond = (uint64_t*)malloc(2*numEpochs*RUNTIME*sizeof(uint64_t));
	for(int zz = 0; zz < 2*numEpochs*RUNTIME ; zz++) numReqCompletedInEachSecond[zz] = 0;
	uint64_t one_second_in_ns = 1000000000;  // 1 second = 1,000,000,000 nanoseconds
	//printf("hello \n");

	for(uint j = 0; j < numThreads; j++) {
		for(uint index = 0; index < goalLoad*RUNTIME*numEpochs; index++) {
			//printf("index = %llu \n", index);
			if(nowtime[j][index].tv_nsec == 0 && nowtime[j][index].tv_sec == 0) {
				continue;
			}
			uint64_t time_elapsed = (nowtime[j][index].tv_sec - starttime.tv_sec)/1e-9 + (nowtime[j][index].tv_nsec - starttime.tv_nsec);
			//assert(time_elapsed < 12e9);
			//assert(whichconn[j][index] == j);

			if(j == 0) {
				fprintf(f_time, "%llu \n", time_elapsed);
				fprintf(f_conn, "%llu \n", whichconn[j][index]);
				fprintf(f_core, "%llu \n", whichcore[j][index]);
			}
			else if(j == 1) {
				fprintf(f_time1, "%llu \n", time_elapsed);
				fprintf(f_conn1, "%llu \n", whichconn[j][index]);
				fprintf(f_core1, "%llu \n", whichcore[j][index]);
			}
			//else assert(false);

			//printf("%llu \n",time_elapsed/one_second_in_ns);
			//if(time_elapsed/one_second_in_ns > numEpochs*RUNTIME) {
			//	numReqCompletedInEachSecond[numEpochs*RUNTIME - 1]++;
			//}
			//else 
			numReqCompletedInEachSecond[time_elapsed/one_second_in_ns]++;
		}
	}

	for(int zz = 0; zz < 2*numEpochs*RUNTIME ; zz++) printf("%llu \n", numReqCompletedInEachSecond[zz]);

	fclose(f_time);
	fclose(f_conn);
	fclose(f_core);

	fclose(f_time1);
	fclose(f_conn1);
	fclose(f_core1);
	#endif
	
	
	//printf("closing connections \n");
	/*
	for(uint64_t z = 0; z < numConnections; z++) {
		if (pp_close_ctx(ctx[z]))
			return 1;
		//printf("z = %d \n", z);
	}
	*/
	//printf("closed connections \n");

	//ibv_free_device_list(dev_list);
	//printf("free device list \n");

	return 0;
}