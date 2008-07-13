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

#include "query_record.h"

#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_QUERY_RECORDS 65536

// All the QueryRecords.  Unallocated records are initalized to zero.
static QueryRecord query_records[NUMBER_OF_QUERY_RECORDS];

// Managing the free_array ----------------------------------------------------

// An array of free QueryRecord ids in arbitrary order.
static int free_array[NUMBER_OF_QUERY_RECORDS];

// The number of QueryRecord ids contained in |free_array|.
static int free_array_size = NUMBER_OF_QUERY_RECORDS;

static int GetIdFromFreeArray() {
  // TODO(abarth): Replace rand() with a strong PRNG.
  int index = rand() % free_array_size;
  int id = free_array[index];
  free_array[index] = free_array[free_array_size - 1];
  --free_array_size;
  return id;
}

static void ReturnIdToFreeArray(int id) {
  free_array[free_array_size] = id;
  ++free_array_size;
}

// Managing the alloc_list ----------------------------------------------------

// A node in the doubly linked list of allocated ids.
typedef struct {
  int next_id;
  int prev_id;
} AllocListNode;

// A doubly-linked list of recently allocated QueryRecord ids.  The ids at the
// front of the list were allocated more recently than the ids at the back of
// the list.  The id |-1| is a placeholder for an invalid id.
static AllocListNode alloc_list[NUMBER_OF_QUERY_RECORDS];

// The first id in the alloc_list.
static int alloc_list_head = -1;

// Updates our LRU cache to indicated that we have just allocated |id|.
static void RecordAllocId(int id) {
  alloc_list[id].next_id = alloc_list_head;
  if (alloc_list_head != -1)
    alloc_list[alloc_list_head].prev_id = id;
  alloc_list_head = id;
}

static void RecordFreeId(int id) {
  int next_id = alloc_list[id].next_id;
  int prev_id = alloc_list[id].prev_id;
  if (next_id != -1)
    alloc_list[next_id].prev_id = prev_id;
  if (prev_id != -1)
    alloc_list[prev_id].next_id = next_id;
  else // |id| is the head of the list.
    alloc_list_head = next_id;
  // Re-initalize the current entry.
  alloc_list[id].next_id = -1;
  alloc_list[id].prev_id = -1;
}

// We free a fourth of the total records in this case so that we will get enough
// entropy in the query ids when we run low on heap space.
#define NUMBER_OF_RECORDS_TO_FREE (NUMBER_OF_QUERY_RECORDS / 4)

static void FreeOldRecordsIfNeeded() {
  if (free_array_size != 0)
    return;

  // Ack!  We're out of space.  Drop the lest recently used ids on the floor.
  int ids_to_free[NUMBER_OF_RECORDS_TO_FREE];
  memset(ids_to_free, -1, sizeof(ids_to_free));
  int id = alloc_list_head;
  for (int i = 0; id != -1; ++i) {
    ids_to_free[i % NUMBER_OF_RECORDS_TO_FREE] = id;
    id = alloc_list[id].next_id;
  }
  // Free the old records.
  for (int i = 0; i < NUMBER_OF_RECORDS_TO_FREE; ++i) {
    int id = ids_to_free[i];
    if (id == -1)
      break;  // Freed all there is to free.
    FreeQueryRecord(&(query_records[id]));
  }
}

// Implementing the API -------------------------------------------------------

void InitQueryRecordHeap() {
  memset(query_records, 0, sizeof(query_records));
  free_array_size = NUMBER_OF_QUERY_RECORDS;
  for (int i = 0; i < NUMBER_OF_QUERY_RECORDS; ++i) {
    query_records[i].id = -1;
    free_array[i] = i;
  }
  alloc_list_head = -1;
  memset(alloc_list, -1, sizeof(alloc_list));
}

QueryRecord* AllocQueryRecord() {
  FreeOldRecordsIfNeeded();
  int id = GetIdFromFreeArray();
  RecordAllocId(id);
  QueryRecord* record = &(query_records[id]);
  record->id = id;
  return record;
}

void FreeQueryRecord(QueryRecord* record) {
  ReturnIdToFreeArray(record->id);
  RecordFreeId(record->id);
  memset(record, 0, sizeof(*record));
  record->id = -1;
}

QueryRecord* GetQueryRecordById(int id) {
  QueryRecord* record = &(query_records[id]);
  if (record->id == -1)
    return 0;  // This record has not been allocated.
  return record;
}

