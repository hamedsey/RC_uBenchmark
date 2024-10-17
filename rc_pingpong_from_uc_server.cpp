//byte vector better than sharedCQ
// TO COMPILE
//TO RUN
//./RCC_Client -d mlx5_0 -s 20 -r 20 -g 3 -l 1000000 192.168.1.5 -z 5 -n 8 -t 1
//./RCC_Server -d mlx5_0 -s 20 -r 20 -g 4 -n 16 -t 1

//./RCC_Client -d mlx5_0 -s 20 -r 20 -g 3 -l 30000000 192.168.1.5 -z 5 -m 0 -n 12 -t 12
//./RCC_Server -d mlx5_0 -s 20 -r 20 -g 4 -n 12 -t 12

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

//#include "pingpong.h"
#include <infiniband/verbs.h>
#include <linux/types.h>  //for __be32 type
#include <errno.h>

#include "rdma_uc.cc"

//#include <algorithm>
//#include <random>
#define SERVICE_TIME_SIZE 0x8000

#define MAX_QUEUES 1024
#define SHARED_CQ 1
#define RC 1
#define ENABLE_SERV_TIME 1
#define FPGA_NOTIFICATION 0
#define debugFPGA 0
#define bitVector 1

#define SHARE_SHAREDCQs 0

struct ibv_device      **dev_list;
struct ibv_device	*ib_dev;
char                    *ib_devname = NULL;
char                    *servername = NULL;
unsigned int             port = 18515;
int                      ib_port = 1;
unsigned int             size = 4096;
enum ibv_mtu		 	 mtu = IBV_MTU_1024;
uint64_t             rx_depth = 8192;
uint64_t             iters = 1000;
int                      num_cq_events = 0;
int                      sl = 0;
int			 gidx = -1;
char			 gid[33];

uint64_t numConnections = 48;

uint32_t * all_rcnts;
uint32_t * all_scnts;
uint64_t **countPriority;
double *rps;
uint64_t goalLoad = 0;
uint64_t numThreads = 0;

uint64_t connPerThread = 0;

uint8_t *expectedSeqNum;

#if SHARED_CQ
struct ibv_cq **sharedCQ;
#endif


uint64_t gen_arrival_time(uint64_t *arrivalSleepTime) {
    
	static uint16_t index = 0;
	uint64_t result = arrivalSleepTime[index];
	//printf("index = %lu, OR  = %lu \n",index,SERVICE_TIME_SIZE | index);
	if( (SERVICE_TIME_SIZE | index) == 0xFFFF) {
		//printf("worked!, index = %lu \n",index);
		index = 0;
	}
	else {
		//printf("index = %lu \n",index);
		index++;
	}
	return result;
}

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

	struct ibv_mr *mr_write;                  // MR handle for buf
    volatile char *buf_write;                 // memory buffer pointer, used for
};
struct ibv_context	*globalcontext;

struct pingpong_context **ctx;

struct thread_data {
	struct pingpong_context **ctxs;
	uint64_t thread_id;
};

void my_sleep(uint64_t n) {
	//if(n == 10000) printf("mysleep = %llu \n",n);
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

	sscanf(msg, "%x:%x:%x:%p:%x:%s", &rem_dest->lid, &rem_dest->qpn,
						&rem_dest->psn, &rem_dest->addr, &rem_dest->rkey, gid);
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
	/*
	if(connfd != 0 )
	{
		char buffer[256];
		strerror_r(errno, buffer, 256 ); // get string message from errno, XSI-compliant version
		printf("Error %s", buffer);
		// or
		char * errorMsg = strerror_r( errno, buffer, 256 ); // GNU-specific version, Linux default
		printf("Error %s", errorMsg); //return value has to be used since buffer might not be modified
		// ...
	}

	if (connfd < 0) {
		if(connfd == EBADF) printf("1 \n");
		else if(connfd == EFAULT) printf("2 \n");
		else if(connfd == EINVAL) printf("3 \n");
		else if(connfd == ENOBUFS) printf("4 \n");
		else if(connfd == EOPNOTSUPP) printf("5 \n");
		else if(connfd == EWOULDBLOCK) printf("6 \n");
		else printf("7 \n");

		printf("connfd = %d \n", connfd);
		fprintf(stderr, "accept() failed\n");
		return NULL;
	}
	*/
	n = read(connfd, msg, sizeof msg);
	if (n != sizeof msg) {
		perror("server read");
		fprintf(stderr, "%d/%d: Couldn't read remote address\n", n, (int) sizeof msg);
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
			//printf("created sharedCQ,  %llu \n", ctx->id);
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
			goto clean_mr;
		}
	#endif

	assert(posix_memalign((void**)(&ctx->buf_write), bufOffset, totalBytes) == 0);
	//printf("buf address %p \n",(ctx->buf_write));
 
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
		init_attr.sq_sig_all = 0;

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

	wr.wr_id	    = ctx->rx_depth;
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

/*
static int pp_post_write(struct pingpong_context *ctx, bool signal, uint64_t offset)
{
	struct ibv_sge list;// = {
	memset(&list, 0, sizeof(list));

	list.addr	= (uintptr_t) ctx->buf_write + offset;
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

	if(signal == true) wr.send_flags = ctx->send_flags | IBV_SEND_SIGNALED; //need to have a signaled completion every signalInterval
	else wr.send_flags = ctx->send_flags;

	wr.wr.rdma.remote_addr = ctx->rem_dest->addr + offset;
	//printf("remote addr = %x \n", wr.wr.rdma.remote_addr);
	wr.wr.rdma.rkey = ctx->rem_dest->rkey;

	struct ibv_send_wr *bad_wr;
	memset(&bad_wr, 0, sizeof(bad_wr));

	return ibv_post_send(ctx->qp, &wr, &bad_wr);
}
*/

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

	//#if SHARED_CQ
	//uint64_t offset = thread_id;
	//#else
	uint64_t offset = (numConnections/numThreads) * thread_id;
	//#endif
	unsigned int rcnt = 0, scnt = 0;
	uint64_t wrID;
	struct pingpong_context *ctx = ctxs[0];
	//OFFSET IS THE CONNECTION ID

	struct ibv_wc wc[1];
	
	int ne, i;

	//printf("ctx->id = %llu \n", ctxs[thread_id]->id);
	cpu_set_t cpuset;
    CPU_ZERO(&cpuset);       				   //clears the cpuset
    if(thread_id < 12) CPU_SET(thread_id /*+2*//*12*/, &cpuset);  //set CPU 2 on cpuset
    else CPU_SET(thread_id + 12/*+2*//*12*/, &cpuset);  //set CPU 2 on cpuset
	sched_setaffinity(0, sizeof(cpuset), &cpuset);

	/*
	//if(servername) {
		uint64_t singleThreadWait = 1000000000/goalLoad;
		uint64_t avg_inter_arr_ns = numThreads*singleThreadWait;

		//if(thread_num == 6) avg_inter_arr_ns = 10000;

		uint64_t *arrivalSleepTime = (uint64_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::exponential_distribution<double> exp{1/((double)avg_inter_arr_ns)};//-SEND_SERVICE_TIME)};
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			arrivalSleepTime[i] = exp(gen);
			//printf("arrivalSleepTime = %llu \n", arrivalSleepTime[i]);
		}
	//}
	*/

	uint64_t mean = (uint64_t)2E3;
	uint64_t sleep_time = 1000;
	uint64_t foundEmptyQueue = 0;

	#if FPGA_NOTIFICATION
		/*
		uint32_t *processBufB4Polling = (uint32_t*) malloc(numConnections*sizeof(uint32_t)); 
		for(int q = 0; q < numConnections; q++) {
			//processBufB4Polling[q] = (uint32_t*) malloc(sizeof(uint32_t));
			processBufB4Polling[q] = NULL;
		}
		//uint32_t *processBufB4PollingSrcQP = (uint32_t*) malloc(NUM_QUEUES*sizeof(uint32_t)); 
		//for(int q = 0; q < NUM_QUEUES; q++) {
			//processBufB4PollingSrcQP[q] = (uint32_t*) malloc(sizeof(uint32_t));
		//	processBufB4PollingSrcQP[q] = NULL;
		//}
		*/
		//uint32_t offset = 0;		
		unsigned long long leadingZeros = 0;
		//alignas(64) uint64_t result[8] = {0,0,0,0,0,0,0,0};
		//uint8_t sequenceNumberInBV;
		//bool need2PollAgain = false;

		//__m512i sixtyfour = _mm512_setr_epi64(64, 64, 64, 64, 64, 64, 64, 64);
		//uint64_t log2;
		//uint64_t value;
		//uint64_t byteOffset;
		//uint64_t leadingZerosinValue;
		//uint8_t resultIndexWorkFound;
		//uint64_t bufID;
		//uint64_t received = false;

		//uint64_t startOffset = 4096*thread_id;
		//uint64_t endOffset = startOffset + numConnections;
		//uint64_t leftOff = startOffset;

		uint64_t prevOffset = 0;
		uint64_t notificationExistsButNoPacket = 0;
		uint64_t idlePollsBeforeFindingWork = 0;
		//bool haveNotPolled = true;

		//bool currentState = false;

		uint64_t lzcntsaysworkbutactuallynotthere = 0;
		bool foundWork = false;
		uint64_t ii = 0;
		uint64_t counting = 0;
		uint64_t no_work_found_idle_polls = 0;
		//uint64_t alliterations = 0;

		bool exit = false;
		ctx = ctxs[prevOffset];
	#endif

	if (gettimeofday(&start, NULL)) {
		perror("gettimeofday");
		return 0;
	}

	uint16_t signalInterval = (8192 - ctx->rx_depth)/4;
	printf("signalInterval = %llu \n", signalInterval);

	uint64_t connection8Active = 0;
	uint64_t connection9Active = 0;

#if SHARE_SHAREDCQs
	uint64_t sharedCQIndex = thread_id;
#endif

	while (1) { //exit == false) {

			//my_sleep(uint64_t(3E2)); //647kRPS

			//alliterations++;
			if (gettimeofday(&end, NULL)) {
				perror("gettimeofday");
				return 0;
			}

			//runtime
			if(end.tv_sec - start.tv_sec > 10) {
				//printf("finished 10 seconds \n");	
				//exit = true;
				break;
			}

			#if !SHARED_CQ && !FPGA_NOTIFICATION
			if(servername == NULL) {
				ctx = ctxs[offset];
			}
			#endif

			
			//for(uint64_t y = 0; y < 4 ; y++) printf("%x  ", (res.buf)[y]);
			//printf("\n\n");

			#if FPGA_NOTIFICATION
			/////////////////////////////////Multi Queue - lzcnt //////////////////////////////////////////////
			foundWork = false;

			//for (int i = 0; i < endOffset; i += 8) {
			#if bitVector
			//ii+=8;
			if(ii*8 >= numConnections) { //compare against upperbound for numConnections*8 to make code identical
				ii = 0;
				//if(numConnections > 512) __asm__ __volatile__ ("clflush (%0)" :: "r"(res.buf));
			}
			#else
			//ii+=8;
			if(ii >= numConnections) ii = 0;
			//rewrite code for byte vector like bit vector
			/*
			if(numConnections > 64 && (ii & 0x0000003F) == 0 && ii < numConnections - 64 ) {
				__asm__ __volatile__ ("clflush (%0)" :: "r"(res.buf + ii));
				//printf("invalidated cache line. ii = %llu \n", ii);
			}
			*/
			#endif
				
			unsigned long long value = htonll(*reinterpret_cast<volatile unsigned long long*>(res.buf + ii + (4096*thread_id)));
						
			//printf("ii = %llu \n", ii);
			//printf("T %llu = %llu \n", ii);
			//if(value == 0xC000000000000000) printf("T%llu \n", thread_id);
				
			//bool bothareone = false;
			//if((uint8_t)res.buf[0] == 128 && (uint8_t)res.buf[4096] == 128) bothareone = true;
			//printf("thread %llu - bothareone = true \n", thread_id);
			//assert(bothareone == true);

			//printf("T%llu: 
			//for(int pp = 0; pp < numConnections; pp++)
			
			//if((uint8_t)res.buf[(4096*thread_id)] != 0) printf("T%llu: %llu \n",thread_id, (uint8_t)res.buf[(4096*thread_id)]);
			//if( ((uint8_t)res.buf[4096] == 255 && (uint8_t)res.buf[0] == 255) || ((uint8_t)res.buf[4097] == 255 && (uint8_t)res.buf[1] == 255)) {
			//	printf("hi \n");
			//	assert(false);
			//}


			#if FPGA_NOTIFICATION
			while(value != 0) {
			#endif

			__asm__ __volatile__ ("lzcnt %1, %0" : "=r" (leadingZeros) : "r" (value):);  //register allocated
			//__asm__ __volatile__ ("clflush (%0)" :: "r"(res.buf));

			//the reason the gap does not exist is that the expensive operation of getting the cache block from memory is the same for both bit and byte vector
			//128 queues
			
			//Byte vector 128 byte (two cache blocks)
				//fetch from memory, bring into cache, 4x instructions + 4x instructions
			//Bit vector 128 bits (one cache block)
				//fetch from memory, bring into cache, x instructions
			//both access one "slow" cache block
			//if we invalidate the cache block that's not being changed we'd expect the gap to grow between bit/byte vector

			//leadingZeros = __builtin_clzll(value);

			//my_sleep(uint64_t(3E2)); //656kRPS
			//if(uint8_t(res.buf[8]) == 0xFF) connection8Active++;
			//if(uint8_t(res.buf[9]) == 0xFF) connection9Active++;
			
			#if bitVector
			//printf("ii = %llu , value = %llu, leadingzeros = %llu \n", ii, (uint64_t)value, leadingZeros);
			value = value & (uint64_t)(0x7FFFFFFFFFFFFFFF >> leadingZeros);
			//uint64_t shiftStartValue = 0x7FFFFFFFFFFFFFFF;

			//int result;

			 // Right shift 'shiftStartValue' by 'leadingZeros'
			 // Output: shiftStartValue is updated after the shift
			 // Input: leadingZeros specifies how much to shift

			/*
			__asm__ __volatile__ (
				"shr %[leadingZeros], %[shiftStartValue]"  
				: [shiftStartValue] "+r" (shiftStartValue)         
				: [leadingZeros] "r" ((uint8_t)leadingZeros) 
			);
			*/


			/*
			__asm__ __volatile__ (
				"mov %[leadingZeros], %%cl;"  				// Move shift amount to 'cl'
				"shr %%cl, %[shiftStartValue]"         		// Shift 'shiftStartValue' right by the amount in 'cl'
				: [shiftStartValue] "=r" (shiftStartValue)      		// Output: result after the shift
				: "[shiftStartValue]" (shiftStartValue),    // Input: shiftStartValue to shift
				[leadingZeros] "r" (leadingZeros) 			// Input: shift amount
				: "%cl"                      				// cl is modified, so we need to mention it here
			);
			*/
			// Inline assembly block
			// Shift 'shiftStartValue' right by 'leadingZeros', shiftStartValue = shiftStartValue >> leadingZeros
			// Perform 'value = value & shiftStartValue'
			// Output: result is stored back in 'value'
			// Input: shiftStartValue that will be used as the mask after the shift
			// Input: leadingZeros
			/*
			__asm__ __volatile__ (
				"shr %[leadingZeros], %[shiftStartValue];"   
				"and %[shiftStartValue], %[value];"         
				: [value] "+r" (value)           			
				: [shiftStartValue] "r" (shiftStartValue),  
				[leadingZeros] "r" (leadingZeros) 			
			);
			*/

			//0x3FFFFFFFFFFFFFFF
			//0x1FFFFFFFFFFFFFFF
			//0x0FFFFFFFFFFFFFFF
			//if(leadingZeros != 64) {
				offset = (ii << 3) + leadingZeros;

				foundWork = true;
				//printf("offset = %llu \n", offset);
				//if(prevOffset != offset) {
					ctx = ctxs[offset];
				//	prevOffset = offset;
					//printf("hi \n");
				//}
			//}
			/*
			else {
				//printf("assert \n");
				assert(true);
				//ii = ii+8;//%numConnections;
				//foundWork = false;
				//no_work_found_idle_polls++;
				continue;
			}
			*/
			#else

			//printf("ii = %llu, leading zeros = %llu \n", ii, leadingZeros);
			/*
			if(leadingZeros != 64) {
			//if(uint8_t(res.buf[0]) == 255) {
				offset = ii + (leadingZeros >> 3);// - startOffset; //this value will change depending on leading zeros
				//printf("i is %llu \n", i);
				//printf("startOffset is %llu \n", startOffset);
				//printf("offset is %llu \n", offset);
			
				//my_sleep(uint64_t(3E2)); //648kRPS

				foundWork = true;
				counting++;
				ii = ii+(leadingZeros >> 3)+1;//%numConnections;
				//printf("thread %d - found work, offset is %llu \n", thread_id , offset);

				ctx = ctxs[offset];

				//leftOff = offset + startOffset+ 1;
				//if(leftOff >= endOffset) leftOff = startOffset;
				
				//break;
			}
			else {
				//my_sleep(sleep_time);  //backing off for 1ns
				//usleep(1);
				//my_sleep(uint64_t(3E2)); //444RPS
				ii = ii+8;//%numConnections;
				foundWork = false;
				no_work_found_idle_polls++;
				continue;
			}
			*/

			
			//printf("ii = %llu , value = %llu, leadingzeros = %llu \n", ii, (uint64_t)value, leadingZeros);

			value = value & (uint64_t)(0x00FFFFFFFFFFFFFF >> leadingZeros);
			offset = ii + (leadingZeros >> 3);// - startOffset; //this value will change depending on leading zeros

			foundWork = true;
			//printf("thread %d - found work, offset is %llu \n", thread_id , offset);

			ctx = ctxs[offset];
			


			assert(foundWork == true);
			#endif
			#endif

			//printf("after lzcnt for loop \n");
			
			//CPU doesn't see update value... add prints or cut the statements in the code
			//if using bits instead of bytes, need to use a bit mask to check the next queue in that read byte which has work (read 8 byte value once only)

			//my_sleep(uint64_t(3E2)); //245RPS, 213kRPS

		
			//if(foundWork == false) {
				/*
				if(currentState == true) {
					//printf("1 -> 0 \n");
					currentState = false;
				}
				*/
				//printf("did not find work \n");
				//my_sleep(sleep_time);

				//my_sleep(uint64_t(3E2)); //118RPS
			//	continue;
			//}


			#if SHARED_CQ
				//ne = ibv_poll_cq(sharedCQ[thread_id], connPerThread*2*ctx->rx_depth, wc);
							
				#if SHARE_SHAREDCQs
					ne = ibv_poll_cq(sharedCQ[sharedCQIndex], 1, wc);
					sharedCQIndex++;
					if(sharedCQIndex == numThreads) sharedCQIndex = 0;
				#else
					ne = ibv_poll_cq(sharedCQ[thread_id], 1, wc);
				#endif
				//printf("after poll cq \n");
			#else
				//ne = ibv_poll_cq(ctx->cq, 2*ctx->rx_depth, wc);
				ne = ibv_poll_cq(ctx->cq, 1, wc);
			#endif

			//printf("after polling \n");

			//if(servername) printf("polling \n");

			//my_sleep(uint64_t(3E2)); //620kRPS

			if (ne < 0) {
				fprintf(stderr, "poll CQ failed %d\n", ne);
				return 0;
			}

			#if FPGA_NOTIFICATION
			if(foundWork == true && ne == 0) lzcntsaysworkbutactuallynotthere++;
			#endif

			for (i = 0; i < ne; ++i) {
				if (wc[i].status != IBV_WC_SUCCESS) {
					fprintf(stderr, "Failed status %s (%d) for wr_id %d\n", ibv_wc_status_str(wc[i].status),wc[i].status, (int) wc[i].wr_id);
					return 0;
				}
				wrID = (uint64_t) wc[i].wr_id;

				#if SHARED_CQ
					uint64_t offset = wc[i].qp_num & (MAX_QUEUES-1);
					//printf("offset = %llu , qpn = %llu \n", offset, wc[i].qp_num);
					ctx = ctxs[offset];
				#endif

				if(wrID == ctx->rx_depth) {
					//++ctx->scnt;
					//scnt++;
					//printf("send complete ... scnt = %llu \n", ctx->scnt);
					//my_sleep(uint64_t(3E2)); //75RPS
				}
				else if (wrID >= 0 && wrID < ctx->rx_depth) {

					ctx->rcnt++;
					rcnt++;

					#if FPGA_NOTIFICATION
						//received = true;
						//printf("thread %d - found work in first poll, offset is %llu \n", thread_id , offset);

						//printf("found work in poll, offset = %llu \n", offset);
						#if debugFPGA
						printf("T%d - FIRST POLL - found work, offset = %llu \n", thread_id, offset);
						#endif
					#endif

					#if SHARED_CQ
						countPriority[thread_id][offset]++;
					#else
						countPriority[thread_id][offset]++;
					#endif
					//uint8_t sleep_int_lower = (uint8_t)ctx->buf_recv[wrID][11];
					//uint8_t sleep_int_upper = (uint8_t)ctx->buf_recv[wrID][10];	
					//printf("recv complete ... rcnt = %llu \n", ctx->rcnt);

					if(servername == NULL) {
						sleep_time = ((uint8_t)ctx->buf_recv[wrID][11] + (uint8_t)ctx->buf_recv[wrID][10] * 0x100) << 4;
						//my_sleep((uint64_t)2E3);

						/*
						if(offset != (uint8_t)ctx->buf_recv[wrID][19]) {
							printf("offset = %lu , priority = %lu \n", offset, (uint8_t)ctx->buf_recv[wrID][19]);
							assert(offset == (uint8_t)ctx->buf_recv[wrID][19]);
						}
						*/

						if ((uint8_t)ctx->buf_recv[wrID][0] >= 128) foundEmptyQueue++; //printf("T%llu - conn %llu is empty \n", thread_id, offset);

					
						#if ENABLE_SERV_TIME
						//printf("sleep_time = %llu \n",sleep_time);
						//printf("sleep_int_lower = %llu \n",(uint8_t)ctx->buf_recv[wrID][11]);
						//printf("sleep_int_upper = %llu \n",(uint8_t)ctx->buf_recv[wrID][10]);
						assert(sleep_time == 992);
						my_sleep(sleep_time);
						memcpy(ctx->buf_send,ctx->buf_recv[wrID],size);
						#endif
					}

					assert(pp_post_recv(ctx, wrID) == 0);
					if (ctx->routs < ctx->rx_depth) {
						fprintf(stderr,
							"Couldn't post receive (%d)\n", ctx->routs);
						return 0;
					}

					//printf("offset = %llu , qpn = %llu \n", offset, wc[i].qp_num);

					//handling userspace sequence number
					/*
					if(expectedSeqNum[offset] != (uint8_t)ctx->buf_recv[wrID][12]) {
						printf("conn %llu ... received sequence %llu ... expected %llu \n", offset, (uint8_t)ctx->buf_recv[wrID][12], expectedSeqNum[offset]);
						//outOfOrderNum[offset]++;
					}
					else //printf("conn %llu ... received sequence %llu = expected %llu \n", offset, (uint8_t)ctx->buf_recv[wrID][12], expectedSeqNum[offset]);

					if(expectedSeqNum[offset] == 255) expectedSeqNum[offset] = 0;
					else expectedSeqNum[offset]++;
					*/
					//handling userspace sequence number

					//for(int hi = 0; hi < 1; hi++) {
						//if(offset == 0) {
							uint64_t success = pp_post_send(ctx, (ctx->scnt%signalInterval == 0));
							assert(success == 0);
						//}
						/*
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
						scnt++;
						ctx->scnt++;
						//my_sleep(uint64_t(3E2)); //635kRPS
					//}
				}
				else {
					fprintf(stderr, "Completion for unknown wr_id %d\n",(int) wc[i].wr_id);
					return 0;
				}
			}

			#if !SHARED_CQ && !FPGA_NOTIFICATION		
			if(servername == NULL) {
				if(offset == numConnections-1) offset = 0;
				else offset++;

				//if(offset == ((numConnections/numThreads) * (thread_id+1)) -1) offset = (numConnections/numThreads) * thread_id;
				//else offset++;
				
			}
			#endif
					
			#if FPGA_NOTIFICATION
			//printf("hi \n");
			}
			ii+=8;
			//if(ii >= numConnections) ii = 0;
			
			#endif
	}

	ctx = ctxs[offset];

	float usec = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	//long long bytes = (long long) size * iters * 2;
		
	all_rcnts[thread_id] = rcnt;
	all_scnts[thread_id] = scnt;

	rps[thread_id] = rcnt/(usec/1000000.);
	#if FPGA_NOTIFICATION
	printf("T %d, %d iters in %.5f seconds, rps = %f ..... notificationExistsButNoPacket = %llu , idlePollsBeforeFindingWork = %llu \n", thread_id, rcnt, usec/1000000., rps[thread_id],notificationExistsButNoPacket, idlePollsBeforeFindingWork);
	#else
	printf("T %d, %d iters in %.5f seconds, rps = %f  \n", thread_id, rcnt, usec/1000000., rps[thread_id]);
	#endif
	
	printf("T%llu found an empty queue %llu times \n", thread_id, foundEmptyQueue);
	//printf("connection 8 active = %llu \n", connection8Active);
	//printf("connection 9 active = %llu \n", connection9Active);

	/*
	//printf("\n\nprinting sequence number stats \n.\n.\n.\n");
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

	#if FPGA_NOTIFICATION
	//printf("counting = %llu \n", counting);
	//printf("lzcntsaysworkbutactuallynotthere = %llu \n", lzcntsaysworkbutactuallynotthere);
	//printf("no_work_found_idle_polls = %llu \n", no_work_found_idle_polls);
	//printf("alliterations = %llu \n", alliterations);
	#endif

	return 0;
}

int main(int argc, char *argv[])
{
	while (1) {
		int c;

		static struct option long_options[] = {
			{ .name = "port",     .has_arg = 1, .flag = NULL, .val = 'p' },
			{ .name = "ib-dev",   .has_arg = 1, .flag = NULL, .val = 'd' },
			{ .name = "ib-port",  .has_arg = 1, .flag = NULL, .val = 'i' },
			{ .name = "size",     .has_arg = 1, .flag = NULL, .val = 's' },
			{ .name = "mtu",      .has_arg = 1, .flag = NULL, .val = 'm' },
			{ .name = "rx-depth", .has_arg = 1, .flag = NULL, .val = 'r' },
			{ .name = "iters",    .has_arg = 1, .flag = NULL, .val = 'n' },
			{ .name = "load",     .has_arg = 1, .flag = NULL, .val = 'l' },
			{ .name = "events",   .has_arg = 0, .flag = NULL, .val = 'e' },
			{ .name = "gid-idx",  .has_arg = 1, .flag = NULL, .val = 'g' },
			{ .name = "chk",      .has_arg = 0, .flag = NULL, .val = 'c' },
			{ .name = "num-threads", .has_arg = 1, .flag = NULL, .val = 't' },
			{}
		};

		c = getopt_long(argc, argv, "p:d:i:s:m:r:n:l:eg:c:t:",
				long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'p':
			port = strtoul(optarg, NULL, 0);
			if (port > 65535) {
				usage(argv[0]);
				return 1;
			}
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

		case 't':
			numThreads = strtoul(optarg, NULL, 0);
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
		//memset(ctx[y], 0, sizeof(struct pingpong_context*));
		//printf("y = %llu \n", y);
		ctx[y]  = pp_init_ctx(ib_dev, size, rx_depth, ib_port);
		if (!ctx[y])
			return 1;
		//printf("hello 22 \n");

		ctx[y]->id = y;
		ctx[y]->routs = 0;
		//ctx[y]->souts = 0;
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
		
		if(y  == 0) printf("ready to connect! \n");
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

	do_uc(ib_devname, servername, port, ib_port, gidx, numConnections, numThreads);

	pthread_t pt[numConnections];
    int ret;

	//numThreads = 1;//numConnections;

    rps = (double*)malloc((numThreads)*sizeof(double));
	all_rcnts = (uint32_t*)malloc(numThreads*sizeof(uint32_t));
	all_scnts = (uint32_t*)malloc(numThreads*sizeof(uint32_t));
	countPriority = (uint64_t **)malloc(numThreads*sizeof(uint64_t *));

	expectedSeqNum = (uint8_t *)malloc(numConnections*sizeof(uint8_t));
	for(int c = 0; c < numConnections; c++) expectedSeqNum[c] = 0;

	for(uint16_t t = 0; t < numThreads; t++) {
		countPriority[t] = (uint64_t *)malloc(numConnections*sizeof(uint64_t));
		for(uint16_t g = 0; g < numConnections; g++) countPriority[t][g] = 0;
	}

	//if(servername == NULL) numThreads = 1;
    for(uint16_t x = 0; x < numThreads; x++) {
		struct thread_data *tdata = (struct thread_data *)malloc(sizeof(struct thread_data));

		tdata->ctxs = ctx;
		tdata->thread_id = x;
		ret = pthread_create(&pt[x], NULL, threadfunc, tdata); 
		assert(ret == 0);
	}

    for(uint16_t x = 0; x < numThreads; x++) {
        int ret = pthread_join(pt[x], NULL);
        assert(!ret);
    }

	uint32_t total_rcnt = 0;
	uint32_t total_scnt = 0;

	for(uint16_t i = 0; i < numThreads; i++){
		//printf("allrcnt[%d] = %llu \n", i, all_rcnts[i]);
		total_rcnt += all_rcnts[i];
		total_scnt += all_scnts[i];
	}


	uint64_t *totalPerThread = (uint64_t *)malloc(numThreads*sizeof(uint64_t));
	for(uint16_t t = 0; t < numThreads; t++) totalPerThread[t] = 0;

	for(uint16_t g = 0; g < numConnections; g++) {
		printf("P%llu   ",g);

		for(uint16_t t = 0; t < numThreads; t++){
			totalPerThread[t] += countPriority[t][g];
			printf("%llu    ",countPriority[t][g]);
		}
		printf("\n");
	}		
	printf("\n");
	printf("total per thread ... \n");
	for(uint16_t t = 0; t < numThreads; t++){
		printf("%llu   ",totalPerThread[t]);
	}
	printf("\n");
	printf("total rcnt = %d, total scnt = %d \n",(int)total_rcnt,(int)total_scnt);


	for(uint16_t i = 0; i < numThreads; i++){
		printf("rps = %f \n",rps[i]);
		totalRPS += rps[i];
	}
	printf("%f \n",totalRPS);
	//printf("closing connections \n");

	for(uint64_t z = 0; z < numConnections; z++) {
	if (pp_close_ctx(ctx[z]))
		return 1;
	}
	//printf("closed connections \n");

	ibv_free_device_list(dev_list);
	//printf("free device list \n");

	return 0;
}
