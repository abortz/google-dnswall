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
#include "check_record.h"
}

#include <gtest/gtest.h>

#include "base/scoped_ptr.h"

namespace {

class MockARecord {
 public:
  enum {
    kSize = 4  // bytes
  };

  MockARecord(int a, int b, int c, int d)
      : record_(new char[kSize]) {
    record_[0] = static_cast<char>(a);
    record_[1] = static_cast<char>(b);
    record_[2] = static_cast<char>(c);
    record_[3] = static_cast<char>(d);
  }

  char* sptr() const { return record_.get(); }
  char* end() const { return sptr() + kSize; }

 private:
  scoped_array<char> record_;
};

}  // namespace

TEST(CheckATest, PublicIP) {
  MockARecord public_ip(171, 64, 78, 27);
  EXPECT_TRUE(CheckARecord(public_ip.sptr(), public_ip.end()));
}

TEST(CheckATest, PrivateIP) {
  MockARecord public_ip(192, 168, 1, 100);
  EXPECT_FALSE(CheckARecord(public_ip.sptr(), public_ip.end()));
}

