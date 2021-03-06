/*******************************************************************************
 * Copyright (c) 2000, 2017 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *******************************************************************************/

#include "ras/DebugExtSegmentProvider.hpp"
#include "env/MemorySegment.hpp"

J9::DebugSegmentProvider::DebugSegmentProvider(
   size_t segmentSize,
   void* (*dbgMalloc)(uintptrj_t size, void *originalAddress),
   void  (*dbgFree)(void *addr)
   ) :
   TR::SegmentProvider(segmentSize),
   _dbgMalloc(dbgMalloc),
   _dbgFree(dbgFree)
   {
   }

TR::MemorySegment &
J9::DebugSegmentProvider::request(size_t requiredSize)
   {
   size_t adjustedSize = ( ( requiredSize + (defaultSegmentSize() - 1) ) / defaultSegmentSize() ) * defaultSegmentSize();
   void *segmentArea = _dbgMalloc(adjustedSize, 0);
   if (!segmentArea) throw std::bad_alloc();
   void *segmentAllocation = _dbgMalloc(sizeof(TR::MemorySegment), 0);
   if (!segmentAllocation) throw std::bad_alloc();
   TR::MemorySegment &newSegment = *new (segmentArea) TR::MemorySegment(segmentArea, adjustedSize);
   return newSegment;
   }

void
J9::DebugSegmentProvider::release(TR::MemorySegment &segment) throw()
   {
   void * segmentArea = segment.base();
   _dbgFree(&segment);
   _dbgFree(segmentArea);
   }

size_t
J9::DebugSegmentProvider::bytesAllocated() const throw()
   {
   return 0;
   }
