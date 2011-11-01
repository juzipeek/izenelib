/*
 * LibCassandra
 * Copyright (C) 2010 Padraig O'Sullivan
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license. See
 * the COPYING file in the parent directory for full text.
 */

#include <iostream>
#include <string>
#include <map>

#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

#include "libcassandra/genthrift/Cassandra.h"

#include "libcassandra/cassandra.h"
#include "libcassandra/keyspace.h"
#include "libcassandra/exception.h"

using namespace libcassandra;
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace org::apache::cassandra;
using namespace boost;


Keyspace::Keyspace(Cassandra *in_client,
                   const string& in_name,
                   const KeyspaceDefinition& in_desc,
                   ConsistencyLevel::type in_level)
        :
        client(in_client),
        name(in_name),
        keyspace_def(in_desc),
        level(in_level)
{}


const string& Keyspace::getName()
{
    return name;
}


ConsistencyLevel::type Keyspace::getConsistencyLevel() const
{
    return level;
}


const KeyspaceDefinition& Keyspace::getDefinition()
{
    return keyspace_def;
}