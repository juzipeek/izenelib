/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary:
 *
 */
#include "config.h"

#include <cstdio>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <unistd.h>

#include <libmemcached/memcached.h>
#include <clients/client_options.h>
#include <clients/utilities.h>

static int opt_binary= 0;
static int opt_verbose= 0;
static time_t opt_expire= 0;
static char *opt_servers= NULL;
static char *opt_username;
static char *opt_passwd;

#define PROGRAM_NAME "memflush"
#define PROGRAM_DESCRIPTION "Erase all data in a server of memcached servers."

/* Prototypes */
void options_parse(int argc, char *argv[]);

int main(int argc, char *argv[])
{
  memcached_st *memc;
  memcached_return_t rc;
  memcached_server_st *servers;

  options_parse(argc, argv);

  if (!opt_servers)
  {
    char *temp;

    if ((temp= getenv("MEMCACHED_SERVERS")))
      opt_servers= strdup(temp);
    else
    {
      fprintf(stderr, "No Servers provided\n");
      exit(1);
    }
  }

  memc= memcached_create(NULL);

  servers= memcached_servers_parse(opt_servers);
  memcached_server_push(memc, servers);
  memcached_server_list_free(servers);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL,
                         (uint64_t) opt_binary);

  if (opt_username and LIBMEMCACHED_WITH_SASL_SUPPORT == 0)
  {
    memcached_free(memc);
    std::cerr << "--username was supplied, but binary was not built with SASL support." << std::endl;
    return EXIT_FAILURE;
  }

  if (opt_username)
  {
    memcached_return_t ret;
    if (memcached_failed(ret= memcached_set_sasl_auth_data(memc, opt_username, opt_passwd)))
    {
      std::cerr << memcached_last_error_message(memc) << std::endl;
      memcached_free(memc);
      return EXIT_FAILURE;
    }
  }

  rc = memcached_flush(memc, opt_expire);
  if (rc != MEMCACHED_SUCCESS)
  {
    fprintf(stderr, "memflush: memcache error %s",
	    memcached_strerror(memc, rc));
    if (memcached_last_error_errno(memc))
      fprintf(stderr, " system error %s", strerror(memcached_last_error_errno(memc)));
    fprintf(stderr, "\n");
  }

  memcached_free(memc);

  free(opt_servers);

  return EXIT_SUCCESS;
}


void options_parse(int argc, char *argv[])
{
  memcached_programs_help_st help_options[]=
  {
    {0},
  };

  static struct option long_options[]=
  {
    {(OPTIONSTRING)"version", no_argument, NULL, OPT_VERSION},
    {(OPTIONSTRING)"help", no_argument, NULL, OPT_HELP},
    {(OPTIONSTRING)"verbose", no_argument, &opt_verbose, OPT_VERBOSE},
    {(OPTIONSTRING)"debug", no_argument, &opt_verbose, OPT_DEBUG},
    {(OPTIONSTRING)"servers", required_argument, NULL, OPT_SERVERS},
    {(OPTIONSTRING)"expire", required_argument, NULL, OPT_EXPIRE},
    {(OPTIONSTRING)"binary", no_argument, NULL, OPT_BINARY},
    {(OPTIONSTRING)"username", required_argument, NULL, OPT_USERNAME},
    {(OPTIONSTRING)"password", required_argument, NULL, OPT_PASSWD},
    {0, 0, 0, 0},
  };
  int option_index= 0;
  int option_rv;

  while (1)
  {
    option_rv= getopt_long(argc, argv, "Vhvds:", long_options, &option_index);
    if (option_rv == -1) break;
    switch (option_rv)
    {
    case 0:
      break;
    case OPT_BINARY:
      opt_binary = 1;
      break;
    case OPT_VERBOSE: /* --verbose or -v */
      opt_verbose = OPT_VERBOSE;
      break;
    case OPT_DEBUG: /* --debug or -d */
      opt_verbose = OPT_DEBUG;
      break;
    case OPT_VERSION: /* --version or -V */
      version_command(PROGRAM_NAME);
      break;
    case OPT_HELP: /* --help or -h */
      help_command(PROGRAM_NAME, PROGRAM_DESCRIPTION, long_options, help_options);
      break;
    case OPT_SERVERS: /* --servers or -s */
      opt_servers= strdup(optarg);
      break;
    case OPT_EXPIRE: /* --expire */
      opt_expire= (time_t)strtoll(optarg, (char **)NULL, 10);
      break;
    case OPT_USERNAME:
      opt_username= optarg;
      break;
    case OPT_PASSWD:
      opt_passwd= optarg;
      break;
    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }
}
