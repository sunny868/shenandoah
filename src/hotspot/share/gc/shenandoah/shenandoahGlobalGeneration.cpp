/*
 * Copyright Amazon.com Inc. or its affiliates. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */
#include "precompiled.hpp"

#include "gc/shenandoah/shenandoahFreeSet.hpp"
#include "gc/shenandoah/shenandoahGlobalGeneration.hpp"
#include "gc/shenandoah/shenandoahHeap.hpp"
#include "gc/shenandoah/shenandoahHeapRegion.inline.hpp"
#include "gc/shenandoah/shenandoahUtils.hpp"
#include "gc/shenandoah/shenandoahVerifier.hpp"
#include "gc/shenandoah/shenandoahYoungGeneration.hpp"

const char* ShenandoahGlobalGeneration::name() const {
  return "GLOBAL";
}

size_t ShenandoahGlobalGeneration::max_capacity() const {
  return ShenandoahHeap::heap()->max_capacity();
}

size_t ShenandoahGlobalGeneration::used_regions() const {
  ShenandoahHeap* heap = ShenandoahHeap::heap();
  assert(heap->mode()->is_generational(), "Region usage accounting is only for generational mode");
  return heap->old_generation()->used_regions() + heap->young_generation()->used_regions();
}

size_t ShenandoahGlobalGeneration::used_regions_size() const {
  return ShenandoahHeap::heap()->capacity();
}

size_t ShenandoahGlobalGeneration::soft_max_capacity() const {
  return ShenandoahHeap::heap()->soft_max_capacity();
}

size_t ShenandoahGlobalGeneration::available() const {
  return ShenandoahHeap::heap()->free_set()->available();
}

void ShenandoahGlobalGeneration::set_concurrent_mark_in_progress(bool in_progress) {
  ShenandoahHeap* heap = ShenandoahHeap::heap();
  if (in_progress && heap->mode()->is_generational()) {
    // Global collection has preempted an old generation mark. This is fine
    // because the global generation includes the old generation, but we
    // want the global collect to start from a clean slate and we don't want
    // any stale state in the old generation.
    heap->cancel_old_gc();
  }

  heap->set_concurrent_young_mark_in_progress(in_progress);
}

bool ShenandoahGlobalGeneration::contains(ShenandoahHeapRegion* region) const {
  return true;
}

void ShenandoahGlobalGeneration::parallel_heap_region_iterate(ShenandoahHeapRegionClosure* cl) {
  ShenandoahHeap::heap()->parallel_heap_region_iterate(cl);
}

void ShenandoahGlobalGeneration::heap_region_iterate(ShenandoahHeapRegionClosure* cl) {
  ShenandoahHeap::heap()->heap_region_iterate(cl);
}

bool ShenandoahGlobalGeneration::is_concurrent_mark_in_progress() {
  ShenandoahHeap* heap = ShenandoahHeap::heap();
  return heap->is_concurrent_mark_in_progress();
}
