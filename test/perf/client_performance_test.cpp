/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Longda on 2021
//

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/metrics/console_reporter.h"
#include "common/metrics/metrics.h"
#include "common/metrics/metrics_registry.h"

#define MAX_MEM_BUFFER_SIZE 8192
#define PORT_DEFAULT 6789

using namespace common;

const char *server_host = reinterpret_cast<const char *>("localhost");
int         server_port = PORT_DEFAULT;

void *test_server(void *param)
{
  auto *tps_meter = static_cast<Meter *>(param);

  std::cout << "Begin to connect server. " << std::endl;
  int sockfd, sendbytes;
  // char send[MAXLINE];

  char send_buf[MAX_MEM_BUFFER_SIZE] = {0};
  char recv_buf[MAX_MEM_BUFFER_SIZE] = {0};
  snprintf(send_buf, sizeof(send_buf), "%s", "select count(*) from test");
  // char buf[MAXDATASIZE];
  struct hostent    *host;
  struct sockaddr_in serv_addr;
  host = gethostbyname(server_host);
  if (host == nullptr) {
    perror("gethostbyname");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket error \n");
    exit(1);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = htons(static_cast<uint16_t>(server_port));
  serv_addr.sin_addr   = *(reinterpret_cast<struct in_addr *>(host->h_addr));
  bzero(&(serv_addr.sin_zero), 8);

  if (connect(sockfd, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(struct sockaddr)) == -1) {
    perror("Failed to connect \n");
    exit(1);
  }

  while (true) {
    sendbytes = send(sockfd, send_buf, strlen(send_buf) + 1, 0);
    if (sendbytes == -1) {
      perror("send error \n");
      exit(1);
    }

    memset(recv_buf, 0, sizeof(recv_buf));

    int len = recv(sockfd, recv_buf, sizeof(recv_buf), 0);
    if (len < 0) {
      printf("connection exception\n");
      break;
    }
    if (len == 0) {
      printf("Connection has been closed\n");
      break;
    }

    tps_meter->inc();
  }
  close(sockfd);
  return nullptr;
}

int main(int argc, char *argv[])
{
  int          opt;
  extern char *optarg;
  while ((opt = getopt(argc, argv, "h:p:")) > 0) {
    switch (opt) {
      case 'p': server_port = atoi(optarg); break;
      case 'h': server_host = optarg; break;
    }
  }

  MetricsRegistry &metric_registry  = get_metrics_registry();
  ConsoleReporter *console_reporter = get_console_reporter();
  metric_registry.add_reporter(console_reporter);

  auto *tps_meter = new Meter();

  metric_registry.register_metric("client.sendtps", tps_meter);

  for (int i = 0; i < 8; i++) {
    pthread_t pid;
    pthread_create(&pid, nullptr, test_server, tps_meter);
  }

  while (true) {
    sleep(60);
    metric_registry.snapshot();
    metric_registry.report();
  }
  return 0;
}
