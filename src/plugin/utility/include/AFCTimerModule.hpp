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

#include "base/AFTimer.hpp"
#include "base/AFPluginManager.hpp"
#include "utility/interface/AFITimerModule.hpp"

namespace ark {

class AFCTimerModule final : public AFITimerModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool Update() override;
    bool PreShut() override;

    bool RemoveTimer(const std::string& name) override;
    bool RemoveTimer(const std::string& name, const AFGUID& entity_id) override;

protected:
    bool AddSingleTimer(const std::string& name, const AFGUID& entity_id, const uint32_t interval_time,
        const uint32_t count, TIMER_FUNCTOR&& cb) override;
    bool AddForeverTimer(
        const std::string& name, const AFGUID& entity_id, const uint32_t interval_time, TIMER_FUNCTOR&& cb) override;

private:
    std::shared_ptr<AFTimerManager> mxTimerManager{nullptr};
};

} // namespace ark
