
/*
Copyright 2012 Aphid Mobile

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 
   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/
/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef RefCounted_h
#define RefCounted_h

#include <wtf/Assertions.h>
#include <wtf/Noncopyable.h>

namespace ATF {

// This base class holds the non-template methods and attributes.
// The RefCounted class inherits from it reducing the template bloat
// generated by the compiler (technique called template hoisting).
class RefCountedBase {
public:
    void ref()
    {
        ASSERT(!m_deletionHasBegun);
        ++m_refCount;
    }

    bool hasOneRef() const
    {
        ASSERT(!m_deletionHasBegun);
        return m_refCount == 1;
    }

    int refCount() const
    {
        return m_refCount;
    }

protected:
    RefCountedBase()
        : m_refCount(1)
#ifndef NDEBUG
        , m_deletionHasBegun(false)
#endif
    {
    }

    ~RefCountedBase()
    {
    }

    // Returns whether the pointer should be freed or not.
    bool derefBase()
    {
        ASSERT(!m_deletionHasBegun);
        ASSERT(m_refCount > 0);
        if (m_refCount == 1) {
#ifndef NDEBUG
            m_deletionHasBegun = true;
#endif
            return true;
        }

        --m_refCount;
        return false;
    }

    // Helper for generating JIT code. Please do not use for non-JIT purposes.
    int* addressOfCount()
    {
        return &m_refCount;
    }

#ifndef NDEBUG
    bool deletionHasBegun() const
    {
        return m_deletionHasBegun;
    }
#endif

private:
    template<class T>
    friend class CrossThreadRefCounted;

    int m_refCount;
#ifndef NDEBUG
    bool m_deletionHasBegun;
#endif
};


template<class T> class RefCounted : public RefCountedBase, public Noncopyable {
public:
    void deref()
    {
        if (derefBase())
            delete static_cast<T*>(this);
    }

protected:
    ~RefCounted()
    {
    }
};

template<class T> class RefCountedCustomAllocated : public RefCountedBase, public NoncopyableCustomAllocated {
public:
    void deref()
    {
        if (derefBase())
            delete static_cast<T*>(this);
    }

protected:
    ~RefCountedCustomAllocated()
    {
    }
};

} // namespace ATF

using ATF::RefCounted;
using ATF::RefCountedCustomAllocated;

#endif // RefCounted_h
