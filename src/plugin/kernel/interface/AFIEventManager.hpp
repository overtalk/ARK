/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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
 *
 */

#pragma once

#include "base/AFPlatform.hpp"
#include "base/AFDefine.hpp"
#include "kernel/interface/AFIDataList.hpp"

namespace ark {

class AFIEventManager
{
public:
    virtual void Update() = 0;

    template<typename BaseType>
    bool AddEventCallBack(
        const int nEventID, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const int, const AFIDataList&))
    {
        EVENT_PROCESS_FUNCTOR functor =
            std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return AddEventCallBack(nEventID, std::move(functor));
    }

    virtual bool RemoveEventCallBack(const int nEventID) = 0;

    virtual bool DoEvent(const int nEventID, const AFIDataList& valueList) = 0;

    virtual bool AddEventCallBack(const int nEventID, EVENT_PROCESS_FUNCTOR&& cb) = 0;

    virtual bool Clear() = 0;

protected:
    virtual bool HasEventCallBack(const int nEventID) = 0;
};
} // namespace ark