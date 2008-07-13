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

extern "C" {
#include "query_record.h"
}

#include <string.h>

#include <gtest/gtest.h>

static const int kNumberOfQueryRecords = 65536;

TEST(QueryRecord, AllocGetFree) {
  InitQueryRecordHeap();
  QueryRecord* record = AllocQueryRecord();
  EXPECT_TRUE(record);
  EXPECT_GE(record->id, 0);
  EXPECT_LT(record->id, kNumberOfQueryRecords);
  EXPECT_TRUE(record->old_id == 0);
  QueryRecord* record_via_get = GetQueryRecordById(record->id);
  EXPECT_EQ(record, record_via_get);
  int query_id = record->id;
  record->old_id = 42;
  FreeQueryRecord(record);
  EXPECT_TRUE(record->id == -1);
  EXPECT_TRUE(record->old_id == 0);
  EXPECT_FALSE(GetQueryRecordById(query_id));
}

TEST(QueryRecord, AllocAll) {
  InitQueryRecordHeap();

  QueryRecord* records[kNumberOfQueryRecords];
  memset(records, 0, sizeof(records));

  for (int i = 0; i < kNumberOfQueryRecords; ++i) {
    QueryRecord* record = AllocQueryRecord();
    EXPECT_TRUE(record);
    EXPECT_GE(record->id, 0);
    EXPECT_LT(record->id, kNumberOfQueryRecords);
    EXPECT_FALSE(records[record->id]);
    records[record->id] = record;
  }

  for (int i = 0; i < kNumberOfQueryRecords; ++i) {
    EXPECT_TRUE(records[i]);
    EXPECT_EQ(records[i], GetQueryRecordById(i));
    if (i + 1 < kNumberOfQueryRecords)
      EXPECT_EQ(records[i] + 1, records[i + 1]);
  }
}

TEST(QueryRecord, AllocAllPlusOne) {
  InitQueryRecordHeap();

  QueryRecord* records[kNumberOfQueryRecords];
  memset(records, 0, sizeof(records));
  int ids_seen[kNumberOfQueryRecords];
  memset(ids_seen, -1, sizeof(ids_seen));

  for (int i = 0; i < kNumberOfQueryRecords; ++i) {
    QueryRecord* record = AllocQueryRecord();
    EXPECT_TRUE(record);
    EXPECT_GE(record->id, 0);
    EXPECT_LT(record->id, kNumberOfQueryRecords);
    EXPECT_FALSE(records[record->id]);
    records[record->id] = record;
    record->old_id = 42;
    EXPECT_EQ(ids_seen[i], -1);
    ids_seen[i] = record->id;
  }

  // At this point, all the QueryRecords are allocated.  Let's see how we handle
  // allocating one more.  We should evict the oldest records and get a shiny
  // new record.
  QueryRecord* overflow_record = AllocQueryRecord();
  EXPECT_TRUE(overflow_record);
  EXPECT_EQ(overflow_record->old_id, 0);
  EXPECT_GE(overflow_record->id, 0);
  EXPECT_LT(overflow_record->id, kNumberOfQueryRecords);
  EXPECT_EQ(overflow_record, records[overflow_record->id]);

  // Check that we did, in fact, evict the old records.
  bool seen_first_valid = false;
  bool seen_second_valid = false;
  int invalid_count = 0;
  for (int i = 0; i < kNumberOfQueryRecords; ++i) {
    int id = ids_seen[i];
    if (!seen_first_valid) {
      // This record is one of the ones we evicted.
      if (records[id]->id != -1) {
        seen_first_valid = true;
        // In fact, it is the one we re-allocated.
        EXPECT_EQ(records[id], overflow_record);
        EXPECT_EQ(records[id]->old_id, 0);
      } else {
        ++invalid_count;
        // It should have been cleared.
        EXPECT_EQ(records[id]->old_id, 0);
      }
    } else if (!seen_second_valid) {
      if (records[id]->id != -1) {
        seen_second_valid = true;
        // This is the first record we decided not to evict.
        // It should not have been cleared.
        EXPECT_GE(records[id]->id, 0);
        EXPECT_LT(records[id]->id, kNumberOfQueryRecords);
        EXPECT_EQ(records[id]->old_id, 42);
        // From here on out, we should only see valid records.
      } else {
        // This record is one of the ones we evicted.
        ++invalid_count;
        // It should have been cleared.
        EXPECT_EQ(records[id]->old_id, 0);
      }
    } else {
      EXPECT_GE(records[id]->id, 0);
      EXPECT_LT(records[id]->id, kNumberOfQueryRecords);
      EXPECT_EQ(records[id]->old_id, 42);
    }
  }
  // Check that we actually evicted a sizable number of records.  This is
  // required to ensure that the query ids have enough entropy.  (We need to
  // add one for the record we reallocated.
  EXPECT_GE(invalid_count + 1, kNumberOfQueryRecords / 4);
}

