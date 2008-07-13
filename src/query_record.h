/* Copyright (c) 2007 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DNSWALL_QUERY_RECORD_H_
#define DNSWALL_QUERY_RECORD_H_

#include <netinet/in.h>

// State of a pending query.
typedef struct {
  int id;                      // The id of this query.
  int old_id;                  // The query's original id field.
  struct sockaddr_in src_addr; // The query's original source.
} QueryRecord;

// Call once at program startup to initalize the heap of query records.
void InitQueryRecordHeap();

// Allocates a QueryRecord with a random id.  If too many QueryRecords are
// allocated, the records will be reused in a manner to ensure the randomness of
// their ids.
QueryRecord* AllocQueryRecord();

// Return a QueryRecord to the pool of unallocated QueryRecords.
void FreeQueryRecord(QueryRecord* record);

// Retrieve a QueryRecord by |id|.  If the QueryRecord is not allocated, this
// function will return NULL.
QueryRecord* GetQueryRecordById(int id);

#endif  // DNSWALL_QUERY_RECORD_H_

