/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "optimization.h"

namespace art {

void HOptimization::MaybeRecordStat(MethodCompilationStat compilation_stat, size_t count) const {
  if (stats_ != nullptr) {
    stats_->RecordStat(compilation_stat, count);
  }
}

#ifdef MTK_ART_COMMON
void HOptimization::MTKMaybeRecordStat(MethodCompilationStat compilation_stat, size_t count) const {
  int stat = static_cast<int>(compilation_stat) + static_cast<int>(kMtkFirstStat) + 1;
  MaybeRecordStat(static_cast<MethodCompilationStat>(stat), count);
}
#endif

}  // namespace art
