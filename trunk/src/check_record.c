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

#include "check_record.h"

int CheckARecord(char* sptr, char* end) {
  if (sptr + 4 > end)
    return 0;

  unsigned char* ptr = (unsigned char *)sptr;

  // Invalid
  if (ptr[0] == 0)
    return 0;

  // Node-local
  if (ptr[0] == 127)
    return 0;

  // Link-local
  if (ptr[0] == 169 && ptr[1] == 254)
    return 0;

  // Site-local
  if (ptr[0] == 10 ||
     (ptr[0] == 172 && (ptr[1] >> 4) == (16 >> 4)) ||
     (ptr[0] == 192 && ptr[1] == 168))
    return 0;

  // Multicast
  // (we are unable to determine the groups internal machines
  // belong to, so we have to block everything)
  if ((ptr[0] >> 4) == (224 >> 4))
    return 0;

  return 1;
}

int CheckAAAARecord(char* sptr, char* end) {
  if (sptr + 16 > end)
    return 0;

  unsigned char* ptr = (unsigned char*)sptr;

  if (ptr[0] == 0x00 && ptr[1] == 0x00 &&
      ptr[2] == 0x00 && ptr[3] == 0x00 &&
      ptr[4] == 0x00 && ptr[5] == 0x00 &&
      ptr[6] == 0x00 && ptr[7] == 0x00 &&
      ptr[8] == 0x00 && ptr[9] == 0x00) {

    if (ptr[10] == 0x00 && ptr[11] == 0x00 &&
        ptr[12] == 0x00 && ptr[13] == 0x00 &&
        ptr[14] == 0x00) {

      // Unspecified address
      // Section 2.5.2 of https://ietf.org/rfc/rfc3513.txt
      if (ptr[15] == 0x00)
        return 0;

      // Loopback
      // Section 2.5.3 of https://ietf.org/rfc/rfc3513.txt
      if (ptr[15] == 0x01)
        return 0;
    }

    // IPv4 compatible (check as if an IPv4 address)
    // Section 2.5.4 of https://ietf.org/rfc/rfc3513.txt
    if (ptr[10] == 0x00 && ptr[11] == 0x00)
      return CheckARecord(sptr + 12, end);

    // IPv4 mapped (check as if an IPv4 address)
    // Section 2.5.4 of https://ietf.org/rfc/rfc3513.txt
    if (ptr[10] == 0xff && ptr[11] == 0xff)
      return CheckARecord(sptr + 12, end);
  }

  // Globally unique local
  if ((ptr[0] >> 1) == (0xfc >> 1))
    return 0;

  // Link-local
  // Section 2.5.6 of https://ietf.org/rfc/rfc3513.txt
  if (ptr[0] == 0xfe && (ptr[1] >> 6) == (0x80 >> 6))
    return 0;

  // Site-local
  // Section 2.5.6 of https://ietf.org/rfc/rfc3513.txt
  // These addresses are deprecated, but we should still block them.
  // For more information, see <https://ietf.org/rfc/rfc3879.txt>.
  if (ptr[0] == 0xfe && (ptr[1] >> 6) == (0xc0 >> 6))
    return 0;

  // Multicast
  // Section 2.7 of https://ietf.org/rfc/rfc3513.txt
  // (we are unable to determine the groups internal machines
  // belong to, so we have to block everything)
  if (ptr[0] == 0xff)
    return 0;

  return 1;
}

