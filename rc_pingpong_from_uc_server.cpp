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
#include <algorithm>
#include <random>

#include "rdma_uc.cc"
#include <x86intrin.h> // For __rdtsc()

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <array>

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#define SERVICE_TIME_SIZE 0x8000

#define MAX_QUEUES 1024
#define SHARED_CQ 0
#define RC 1
#define ENABLE_SERV_TIME 1
#define FPGA_NOTIFICATION 1
#define debugFPGA 0
#define bitVector 1

#define SHARE_SHAREDCQs 0

#define NOTIFICATION_QUEUE 0

#define CONN_AFFINITY_OVERHEAD 0

#define ENABLE_KERNEL 0

#define RUNTIME 45 //115

#define USE_EVENT 0
#define IB 0

//const std::string energy_path = "/sys/class/powercap/intel-rapl:0/energy_uj";


struct ibv_device      **dev_list;
struct ibv_device	*ib_dev;
char                    *ib_devname = NULL;
char                    *servername = NULL;
unsigned int             port = 18511;
int                      ib_port = 1;
unsigned int             size = 4096;
enum ibv_mtu		 	 mtu = IBV_MTU_1024;
uint64_t             rx_depth = 8192;
uint64_t             iters = 1000;
//int                      num_cq_events = 0;
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

#if ENABLE_KERNEL
uint64_t **connArray;
uint64_t sizeofArray = 3008; //125; //3008, 125; //size to 1000 and do loop multiple times
uint64_t numIterations = 1;  //45; // 1, 45
uint64_t numPartitions = 32;
#endif 

uint64_t *elapsedTime;
char* output_dir;

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
struct ibv_comp_channel *channel;

struct pingpong_context **ctx;

struct thread_data {
	struct pingpong_context **ctxs;
	uint64_t thread_id;
};

std::string execCommand(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;

    // Open the command for reading.
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    // Read the output into a string.
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    // Close the pipe.
    int returnCode = pclose(pipe);
    if (returnCode != 0) {
        printf("Command exited with non-zero status: %d \n", returnCode);
		exit(EXIT_FAILURE);
    }

    return result;
}

uint64_t readEnergyUj() {
    std::string output = execCommand("cat /sys/class/powercap/intel-rapl:0:0/energy_uj");
    return std::stoull(output);
}

double read_energy_uj(const std::string& path) {
    std::ifstream file(path);
    double energy = 0.0;
    if (file.is_open()) {
        file >> energy;
        file.close();
    } else {
        std::cerr << "Failed to open energy file: " << path << std::endl;
    }
    return energy;
}

void my_sleep(uint64_t n) {
	//if(n == 10000) printf("mysleep = %llu \n",n);
	struct timespec ttime,curtime;
	clock_gettime(CLOCK_MONOTONIC,&ttime);
	
	while(1){
		clock_gettime(CLOCK_MONOTONIC,&curtime);
		uint64_t elapsed = (curtime.tv_sec-ttime.tv_sec )/1e-9 + (curtime.tv_nsec-ttime.tv_nsec);

		if (elapsed >= n) {
			assert(elapsed >= n);
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

		#if USE_EVENT
			#if IB
			channel = ibv_create_comp_channel(globalcontext);
			if (!channel) {
				fprintf(stderr, "Couldn't create completion channel\n");
				goto clean_device;
			}
			#endif
		#else
			channel = NULL;
		#endif
	}
	ctx->context = globalcontext;
	ctx->id = poo;

	#if USE_EVENT
		#if !IB
		ctx->channel = ibv_create_comp_channel(ctx->context);
		if (!ctx->channel) {
			fprintf(stderr, "Couldn't create completion channel\n");
			goto clean_device;
		}
		#endif
	#endif

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
		#if USE_EVENT
			#if IB
			ctx->cq = ibv_create_cq(ctx->context, 2*rx_depth + 1, ctx, channel, 0);
			#else		
			ctx->cq = ibv_create_cq(ctx->context, 2*rx_depth + 1, NULL, ctx->channel, 0);
			#endif
		#else 
			ctx->cq = ibv_create_cq(ctx->context, 2*rx_depth + 1, NULL, ctx->channel, 0);
		#endif
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

	#if USE_EVENT
	struct ibv_wc wc[rx_depth*2];
	#else 
	struct ibv_wc wc[1];
	#endif
	struct ibv_wc wc_drain[rx_depth*2];

	#if NOTIFICATION_QUEUE
	struct ibv_wc wc_notif[1];
	int ne_notif, i_notif;
	uint64_t wrID_notif;
	struct pingpong_context *ctx_noti = ctxs[1];
	#endif
	
	int ne, i, ne_drain;

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
	uint64_t prevOffset = 0;

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

	uint16_t signalInterval = (8192 - ctx->rx_depth)/4;
	printf("signalInterval = %llu \n", signalInterval);

	uint64_t connection8Active = 0;
	uint64_t connection9Active = 0;

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC,&start);
	
	struct timespec starttimestamp, endtimestamp;	

	//uint64_t num_cq_events = 0;

#if SHARE_SHAREDCQs
	uint64_t sharedCQIndex = thread_id;
#endif

#if USE_EVENT
	
	int rc;
	int ms_timeout = 10;
	struct ibv_cq *ev_cq = NULL;
	void *ev_ctx = NULL;
	
	#if !IB
		//poll
		/*
		struct pollfd *my_pollfd;
		my_pollfd = (struct pollfd *)malloc(numConnections* sizeof(struct pollfd));
	
		for(int i = 0; i < numConnections; i++) {
			my_pollfd[i].fd      = ctxs[i]->channel->fd;
			my_pollfd[i].events  = POLLIN;
		}
		*/
	
		//epoll
		
		int epfd = epoll_create1(0);
		struct epoll_event ev;
		struct epoll_event *events;
		//int *fds;
		//ev = (struct epoll_event *)malloc(numConnections* sizeof(struct epoll_event));
		events = (struct epoll_event *)malloc(numConnections* sizeof(struct epoll_event));


		// Register each FD with epoll
		for (int i = 0; i < numConnections; i++) {
			ev.events = EPOLLIN;
			ev.data.ptr = ctxs[i];//(void*)(ctxs[i]->channel->fd);
			//ev.data.fd = ctxs[i]->channel->fd;
			if (epoll_ctl(epfd, EPOLL_CTL_ADD, ctxs[i]->channel->fd, &ev) < 0) {
				perror("epoll_ctl");
				exit(EXIT_FAILURE);
			}
		}
		
	#endif
	

#endif
	//double energy_end, energy_start;
    //if(thread_id == 0) energy_start = read_energy_uj(energy_path);

	while (1) { //exit == false) {
			//sleep(RUNTIME);
			//rcnt = 1;
			clock_gettime(CLOCK_MONOTONIC,&end);

			//runtime
			if(end.tv_sec - start.tv_sec > RUNTIME) {
				//printf("finished 10 seconds \n");	
				break;
			}

			//printf("offset = %llu \n", offset);

			#if !SHARED_CQ && !FPGA_NOTIFICATION && !NOTIFICATION_QUEUE && !USE_EVENT
			if(servername == NULL) {
				ctx = ctxs[offset];
			}
			#endif

			#if FPGA_NOTIFICATION
			/////////////////////////////////Multi Queue - lzcnt //////////////////////////////////////////////
			foundWork = false;

			if(ii*8 >= numConnections) { //compare against upperbound for numConnections*8 to make code identical
				ii = 0;
			}

			unsigned long long value = htonll(*reinterpret_cast<volatile unsigned long long*>(res.buf + ii + (4096*thread_id)));

			while(value != 0) {

			__asm__ __volatile__ ("lzcnt %1, %0" : "=r" (leadingZeros) : "r" (value):);  //register allocated
			
			//printf("ii = %llu , value = %llu, leadingzeros = %llu \n", ii, (uint64_t)value, leadingZeros);
			value = value & (uint64_t)(0x7FFFFFFFFFFFFFFF >> leadingZeros);

			offset = (ii << 3) + leadingZeros;

			foundWork = true;

			ctx = ctxs[offset];

			#endif


			#if USE_EVENT
			
			// The following code will be called each time you need to read a Work Completion
			
			
			// poll the channel until it has an event and sleep ms_timeout
			// milliseconds between any iteration

			#if !IB
			
			//poll
			/*
			for(int i = 0; i < numConnections; i++) {
				my_pollfd[i].revents = 0;
			}
			
			do {
				rc = poll(my_pollfd, numConnections, ms_timeout);
			} while (rc == 0);
			if (rc < 0) {
				fprintf(stderr, "poll failed\n");
				//return -1;
			}

			if (rc > 0) {
				for (int ccc = 0; ccc < numConnections; ccc++) {
					if (my_pollfd[ccc].revents & POLLIN) {
						// This fd is ready for reading
						//printf("fd %d is ready to read\n", my_pollfd[i].fd);
						ctx = ctxs[ccc];
			
			*/
			//epoll
			// Wait for events
			
			int nfds = epoll_wait(epfd, events, numConnections, -1);
			if (nfds > 0) {
				for (int i_epoll = 0; i_epoll < nfds; i_epoll++) {
					if (uint32_t(events[i_epoll].events) & EPOLLIN) {
						// This FD is ready to read
						//ctx = ctxs[events[i].data.fd];
						ctx = (struct pingpong_context *)(events[i_epoll].data.ptr);

			ev_cq = ctx->cq;

			#else
			/* Wait for the completion event */

			if (ibv_get_cq_event(channel, &ev_cq, &ev_ctx)) {
				fprintf(stderr, "Failed to get cq_event\n");
				//return 1;
			}

			struct pingpong_context *ctx = (struct pingpong_context *)ev_ctx;

			//++num_cq_events;

			//if (ev_cq != ctx->cq) {
			//	fprintf(stderr, "CQ event for unknown CQ %p\n", ev_cq);
				//return 1;
			//}

			/* Ack the event */	
			ibv_ack_cq_events(ev_cq, 1);	

			/* Request notification upon the next completion event */
			if (ibv_req_notify_cq(ev_cq, 0)) {
				fprintf(stderr, "Couldn't request CQ notification\n");
				//return 1;
			}
			#endif
			/* Empty the CQ: poll all of the completions from the CQ (if any exist) */

			#endif

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
				#if USE_EVENT
				ne = ibv_poll_cq(ev_cq, 2*ctx->rx_depth, wc);
				//if(thread_id == 0) printf("offset = %llu \n", offset);
				#else
				ne = ibv_poll_cq(ctx->cq, 1, wc);
				#endif
			#endif

			//printf("after polling \n");

			//if(servername) printf("polling \n");

			//my_sleep(uint64_t(3E2)); //620kRPS

			if (ne < 0) {
				fprintf(stderr, "poll CQ failed %d\n", ne);
				return 0;
			}
			#if NOTIFICATION_QUEUE
			if(ne == 0) {
				foundEmptyQueue++;
				while(ne == 0) ne = ibv_poll_cq(ctx->cq, 1, wc);
			}
			#endif

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

				#if CONN_AFFINITY_OVERHEAD
					if(prevOffset != offset) my_sleep(1E2);
					prevOffset = offset;
				#endif

				if(wrID == ctx->rx_depth) {
					//++ctx->scnt;
					//scnt++;
					//printf("send complete ... scnt = %llu \n", ctx->scnt);
					//my_sleep(uint64_t(3E2)); //75RPS
				}
				else if (wrID >= 0 && wrID < ctx->rx_depth) {
					//if(thread_id == 0) printf("found work, offset = %llu \n", offset);
					#if FPGA_NOTIFICATION
						//received = true;
						//printf("thread %d - found work in first poll, offset is %llu \n", thread_id , offset);

						//printf("found work in poll, offset = %llu \n", offset);
						#if debugFPGA
						printf("T%d - FIRST POLL - found work, offset = %llu \n", thread_id, offset);
						#endif
					#endif

					if(servername == NULL) {
						sleep_time = ((uint8_t)ctx->buf_recv[wrID][11] + (uint8_t)ctx->buf_recv[wrID][10] * 0x100) << 4;
						//assert(sleep_time == 992);
						//sleep_time = 992;
						#if NOTIFICATION_QUEUE
						if(ctx->id == 0) assert(sleep_time == 992);
						else if(ctx->id == 1) assert(sleep_time == 0);
						#endif
						//my_sleep((uint64_t)2E3);

						/*
						if(offset != (uint8_t)ctx->buf_recv[wrID][19]) {
							printf("offset = %lu , priority = %lu \n", offset, (uint8_t)ctx->buf_recv[wrID][19]);
							assert(offset == (uint8_t)ctx->buf_recv[wrID][19]);
						}
						*/

						//if ((uint8_t)ctx->buf_recv[wrID][0] >= 128) foundEmptyQueue++; //printf("T%llu - conn %llu is empty \n", thread_id, offset);

					
						#if ENABLE_SERV_TIME
						//printf("sleep_time = %llu \n",sleep_time);
						//printf("sleep_int_lower = %llu \n",(uint8_t)ctx->buf_recv[wrID][11]);
						//printf("sleep_int_upper = %llu \n",(uint8_t)ctx->buf_recv[wrID][10]);
						#if NOTIFICATION_QUEUE
						//if(offset == 0) assert(sleep_time == 992);
						//else assert(sleep_time == 0);
						my_sleep(sleep_time);
						#else
						//my_sleep(992);
						
						#if ENABLE_KERNEL
						clock_gettime(CLOCK_MONOTONIC,&starttimestamp);
						volatile uint64_t sum = 0;
							
						for(uint64_t index = 0; index < sizeofArray; index+=16) {
							
							// 2120 for sharedcq, 2900, 3000 for bitvector
							
							sum = sum + connArray[offset][index]++ + connArray[offset][index+1]++ + connArray[offset][index+2]++ + connArray[offset][index+3]++ +
										connArray[offset][index+4]++ + connArray[offset][index+5]++ + connArray[offset][index+6]++ + connArray[offset][index+7]++ +
										connArray[offset][index+8]++ + connArray[offset][index+9]++ + connArray[offset][index+10]++ + connArray[offset][index+11]++ +
										connArray[offset][index+12]++ + connArray[offset][index+13]++ + connArray[offset][index+14]++ + connArray[offset][index+15]++;
							
							//1570 for sharedcq, 2000 for bitvector
							/*
							sum = sum + connArray[offset][index] + connArray[offset][index+1] + connArray[offset][index+2] + connArray[offset][index+3] +
										connArray[offset][index+4] + connArray[offset][index+5] + connArray[offset][index+6] + connArray[offset][index+7] +
										connArray[offset][index+8] + connArray[offset][index+9] + connArray[offset][index+10] + connArray[offset][index+11] +
										connArray[offset][index+12] + connArray[offset][index+13] + connArray[offset][index+14] + connArray[offset][index+15];
							*/
						}
						clock_gettime(CLOCK_MONOTONIC,&endtimestamp);

						//assert((uint8_t)ctx->buf_recv[wrID][0] & 0x01 == 0);
						//printf("char = %llu",(uint8_t)ctx->buf_recv[wrID][0] & 0x10);

						//printf("elapsedTime = %f \n", elapsedTime/(numIterations*numConnections));
						#else		
						clock_gettime(CLOCK_MONOTONIC,&starttimestamp);	
						//uint64_t startTS = __rdtsc();
						my_sleep(sleep_time);
						//uint64_t endTS = __rdtsc(); // Read CPU clock cycles at end
						clock_gettime(CLOCK_MONOTONIC,&endtimestamp);
						//printf("%llu \n", (endtimestamp.tv_sec-starttimestamp.tv_sec)*1e9 + (endtimestamp.tv_nsec-starttimestamp.tv_nsec));

						#endif

						#endif
						ctx->buf_recv[wrID][9] = (((uint8_t)thread_id << 4) & 0xF0) + ((uint8_t)ctx->buf_recv[wrID][9] & 0x0F);          
						memcpy(ctx->buf_send,ctx->buf_recv[wrID],size);
						#endif

						if(((uint8_t)ctx->buf_recv[wrID][0] >> 4) & 0x01 == 1) {
							//printf("hello! ");

							rcnt++;

							#if SHARED_CQ
								countPriority[thread_id][offset]++;
							#else
								countPriority[thread_id][offset]++;
							#endif
							//uint8_t sleep_int_lower = (uint8_t)ctx->buf_recv[wrID][11];
							//uint8_t sleep_int_upper = (uint8_t)ctx->buf_recv[wrID][10];	
							//printf("recv complete ... rcnt = %llu \n", ctx->rcnt);
							
							//elapsedTime[thread_id] = elapsedTime[thread_id] + (endTS - startTS);
							elapsedTime[thread_id] = elapsedTime[thread_id] + ((endtimestamp.tv_sec-starttimestamp.tv_sec)*1e9 + (endtimestamp.tv_nsec-starttimestamp.tv_nsec));

							//uint64_t smallestCoreDepth = ((uint8_t)ctx->buf_recv[wrID][12] << 8) + ((uint8_t)ctx->buf_recv[wrID][13]);
							//uint64_t largestCoreDepth = ((uint8_t)ctx->buf_recv[wrID][14] << 8) + ((uint8_t)ctx->buf_recv[wrID][15]);
							//assert(smallestCoreDepth == 0);
							//assert(largestCoreDepth == 0);

							//if(rcnt % 1000 == 0) printf("readEnergyUj() = %llu \n", readEnergyUj());
						}

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
							/*
							#if !FPGA_NOTIFICATION
								ctx->buf_recv[wrID][9] = (uint8_t)thread_id;
							#else 
								assert((uint8_t)ctx->buf_recv[wrID][9] < numThreads);
								if((uint8_t)ctx->buf_recv[wrID][9] != (uint8_t)thread_id) threadMismatch++; //printf("mismatch ... FPGA said thread %llu, thread id = %llu \n", (uint8_t)ctx->buf_recv[wrID][9], (uint8_t)thread_id);
								ctx->buf_recv[wrID][9] = (((uint8_t)thread_id << 4) & 0xF0) + (uint8_t)ctx->buf_recv[wrID][9] & 0x0F;          
							#endif
							*/
						
							uint64_t success = pp_post_send(ctx, (ctx->scnt%signalInterval == 0));
							//assert(success == 0);
							//printf("sent pkt reply \n");
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

					//uncomment to drain queue after reading queue depth from packet
					/*
					if(((uint8_t)ctx->buf_recv[wrID][16] >> 7) & 0x01 == 1) {
						uint16_t tempCoreQD = (((uint8_t)ctx->buf_recv[wrID][16] & 0x3F) << 8) + ((uint8_t)ctx->buf_recv[wrID][17]);
						//if(tempCoreQD > 64) printf("coreID = %llu,  tempCoreQD = %llu \n", thread_id, tempCoreQD);
						assert(tempCoreQD < 65);
						if(tempCoreQD > 55) {
							ne_drain = ibv_poll_cq(sharedCQ[thread_id], tempCoreQD*2, wc_drain);
							printf("coreID = %llu,  QD = %llu,  ne_drain = %llu \n", thread_id, tempCoreQD, ne_drain);

							for (int d = 0; d < ne_drain; ++d) {
								if (wc_drain[d].status != IBV_WC_SUCCESS) {
									fprintf(stderr, "Failed status %s (%d) for wr_id %d\n", ibv_wc_status_str(wc_drain[d].status),wc_drain[d].status, (int) wc_drain[d].wr_id);
									return 0;
								}
								wrID = (uint64_t) wc_drain[d].wr_id;

								#if SHARED_CQ
									uint64_t offset = wc_drain[d].qp_num & (MAX_QUEUES-1);
									//printf("offset = %llu , qpn = %llu \n", offset, wc[i].qp_num);
									ctx = ctxs[offset];
								#endif

								#if CONN_AFFINITY_OVERHEAD
									if(prevOffset != offset) my_sleep(1E2);
									prevOffset = offset;
								#endif

								if(wrID == ctx->rx_depth) {
								}
								else if (wrID >= 0 && wrID < ctx->rx_depth) {
									if(((uint8_t)ctx->buf_recv[wrID][0] >> 4) & 0x01 == 1) {
										//printf("hello! ");
										rcnt++;
										countPriority[thread_id][offset]++;
									}

									assert(pp_post_recv(ctx, wrID) == 0);
									if (ctx->routs < ctx->rx_depth) {
										fprintf(stderr,
											"Couldn't post receive (%d)\n", ctx->routs);
										return 0;
									}

									uint64_t success = pp_post_send(ctx, (ctx->scnt%signalInterval == 0));
									assert(success == 0);
								}
							}
						}
					}
					*/
				}
				else {
					fprintf(stderr, "Completion for unknown wr_id %d\n",(int) wc[i].wr_id);
					return 0;
				}
			}

			#if !SHARED_CQ && !FPGA_NOTIFICATION && !NOTIFICATION_QUEUE && !USE_EVENT
			if(servername == NULL) {
			
				//if(thread_id < 6) {
					if(offset == numConnections-1) offset = 0;
					else offset++;
				//}
				//else {
				//	if(offset == 0) offset = numConnections-1;
				//	else offset--;
				//}
				
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

		#if USE_EVENT
		#if !IB
					}
			}
		}
		#endif
		#endif
	}

	//if(thread_id == 0) energy_end = read_energy_uj(energy_path);

	elapsedTime[thread_id] = elapsedTime[thread_id]/rcnt;
	
	ctx = ctxs[offset];

	float nsec = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
	//long long bytes = (long long) size * iters * 2;
		
	all_rcnts[thread_id] = rcnt;
	all_scnts[thread_id] = scnt;

	rps[thread_id] = rcnt/(nsec/1000000000.);
	#if FPGA_NOTIFICATION
	printf("T %d, %d iters in %.5f seconds, rps = %f ..... notificationExistsButNoPacket = %llu , idlePollsBeforeFindingWork = %llu \n", thread_id, rcnt, nsec/1000000000., rps[thread_id],notificationExistsButNoPacket, idlePollsBeforeFindingWork);
	#else
	printf("T %d, %d iters in %.5f seconds, rps = %f  \n", thread_id, rcnt, nsec/1000000000., rps[thread_id]);
	#endif
	
	printf("T%llu found an empty queue %llu times \n", thread_id, foundEmptyQueue);
	//printf("connection 8 active = %llu \n", connection8Active);
	//printf("connection 9 active = %llu \n", connection9Active);

	printf("hi %llu \n");

	/*
	if(thread_id == 0)  {
		double energy_diff_joules = (energy_end - energy_start) / 1e6;
		printf("Power = %f Watts (J/s) \n", energy_diff_joules/(nsec/1000000000.));
	}
	*/
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
			{ .name = "goalLoad",     .has_arg = 1, .flag = NULL, .val = 'l' },
			{ .name = "events",   .has_arg = 0, .flag = NULL, .val = 'e' },
			{ .name = "gid-idx",  .has_arg = 1, .flag = NULL, .val = 'g' },
			{ .name = "numthreads", .has_arg = 1, .flag = NULL, .val = 't' },
			{}
		};

		c = getopt_long(argc, argv, "p:d:i:s:m:r:n:l:eg:c:t:k:f:",
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

		case 'f':
        	output_dir = optarg;
        	break;
			 
		default:
			usage(argv[0]);
			return 1;
		}
	}

	//600*1024*8 = 4.6 MB
	//2400*1024*8 = 18.75 MB
	//3008*1024*8 = 18.75 MB

	//std::random_device rd1{};
	//std::mt19937 gen1{rd1()};
	//std::uniform_int_distribution<uint8_t>* rand1 = new std::uniform_int_distribution<uint8_t>(0, 255);

	#if ENABLE_KERNEL
	printf("size of conn context = %llu \n", sizeof(struct pingpong_context));
	connArray = (uint64_t**)malloc(numConnections*sizeof(uint64_t*));

	for(uint64_t cc = 0; cc < numConnections; cc++){
		connArray[cc] = (uint64_t*)malloc(sizeofArray*sizeof(uint64_t));
		for(uint64_t ccc = 0; ccc < sizeofArray; ccc++){
			connArray[cc][ccc] = 10;
		}
	}
	#endif

	elapsedTime = (uint64_t*)malloc(numThreads*sizeof(uint64_t));
	for(int g = 0; g < numThreads; g++) elapsedTime[g] = 0;

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
			//printf("bubu \n");
		}

		//printf("bubu \n");

		#if USE_EVENT
		if (ibv_req_notify_cq(ctx[y]->cq, 0)) {
			fprintf(stderr, "Couldn't request CQ notification\n");
			return 1;
		}
		//printf("hello bubu \n");

		#if !IB
		int flags;
		int ret;
		int rc;
		// The following code will be called only once, after the Completion Event Channel was created
		//printf("Changing the mode of Completion events to be read in non-blocking\n");
		//printf("hello fufu \n");
		// change the blocking mode of the completion channel 

		flags = fcntl(ctx[y]->channel->fd, F_GETFL,0);
		rc = 	fcntl(ctx[y]->channel->fd, F_SETFL, flags | O_NONBLOCK);
		if (rc < 0) {
				fprintf(stderr, "Failed to change file descriptor of Completion Event Channel\n");
				return -1;
		}
		#endif
		#endif 

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

	//#if ENABLE_KERNEL
	float avgKernelServiceTime = 0.0;
	for(int g = 0; g < numThreads; g++) {
		printf("elapsedTime = %llu\n", elapsedTime[g]);
		avgKernelServiceTime += elapsedTime[g];
	}
	printf("avgKernelServiceTime = %f\n", avgKernelServiceTime/numThreads);

	char* output_name_avgServiceTime;
	asprintf(&output_name_avgServiceTime, "%s/avg_service_time.servtime", output_dir);
	FILE *f_avgServiceTime = fopen(output_name_avgServiceTime, "wb");
	fprintf(f_avgServiceTime, "%f \n", avgKernelServiceTime/numThreads);
	fclose(f_avgServiceTime);

	//sleep(10);
	//#endif

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
