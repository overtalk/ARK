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

#include "utility/include/AFCTimerModule.hpp"

namespace ark {

bool AFCTimerModule::Init()
{
    mxTimerManager = std::make_shared<AFTimerManager>();
    mxTimerManager->Init(GetPluginManager()->GetNowTime());

    return true;
}

bool AFCTimerModule::PreShut()
{
    mxTimerManager->Shut();
    return true;
}

bool AFCTimerModule::Update()
{
    mxTimerManager->Update(GetPluginManager()->GetNowTime());
    return true;
}

bool AFCTimerModule::RemoveTimer(const std::string& name)
{
    return mxTimerManager->RemoveTimer(name);
}

bool AFCTimerModule::RemoveTimer(const std::string& name, const AFGUID& entity_id)
{
    return mxTimerManager->RemoveTimer(name, entity_id);
}

bool AFCTimerModule::AddSingleTimer(const std::string& name, const AFGUID& entity_id, const uint32_t interval_time,
    const uint32_t count, TIMER_FUNCTOR&& cb)
{
    return mxTimerManager->AddSingleTimer(name, entity_id, interval_time, count, std::forward<TIMER_FUNCTOR>(cb));
}

bool AFCTimerModule::AddForeverTimer(
    const std::string& name, const AFGUID& entity_id, const uint32_t interval_time, TIMER_FUNCTOR&& cb)
{
    return mxTimerManager->AddForverTimer(name, entity_id, interval_time, std::forward<TIMER_FUNCTOR>(cb));
}

} // namespace ark
