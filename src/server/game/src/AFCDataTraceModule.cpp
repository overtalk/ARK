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

#include "kernel/include/AFCDataList.hpp"
#include "kernel/include/AFCNode.hpp"
#include "kernel/include/AFCTable.hpp"
#include "game/include/AFCDataTraceModule.hpp"

namespace ark {

bool AFCDataTraceModule::Init()
{
    m_pKernelModule = FindModule<AFIKernelModule>();
    m_pLogModule = FindModule<AFILogModule>();

    return true;
}

void AFCDataTraceModule::StartTracing(const AFGUID self)
{
    LogObjectData(self);
}

void AFCDataTraceModule::EndTracing(const AFGUID self)
{
    // Will complete this
}

int AFCDataTraceModule::LogObjectData(const AFGUID& self)
{
    return m_pKernelModule->LogObjectData(self);
}

int AFCDataTraceModule::OnEntityNodeEvent(
    const AFGUID& self, const std::string& nodeName, const AFIData& oldVar, const AFIData& newVar)
{
    ARK_LOG_INFO("Trace id[{}] Name[{}] Old[{}] New[{}]", self, nodeName, oldVar.GetString(), newVar.GetString());
    return 0;
}

int AFCDataTraceModule::OnEntityTableEvent(
    const AFGUID& self, const TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar)
{
    // will add
    return 0;
}

int AFCDataTraceModule::TraceEntityData(const AFGUID& self)
{
    // will add
    return 0;
}

} // namespace ark
