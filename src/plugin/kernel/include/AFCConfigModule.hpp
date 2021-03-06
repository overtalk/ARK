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

#include "kernel/interface/AFIConfigModule.hpp"
#include "base/AFPluginManager.hpp"
#include "log/interface/AFILogModule.hpp"
#include "kernel/interface/AFIClassMetaModule.hpp"

namespace ark {

class AFCConfigModule final : public AFIConfigModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool Shut() override;

    bool Load() override;
    bool Save() override;
    bool Clear() override;

    std::shared_ptr<AFIStaticEntity> FindStaticEntity(const ID_TYPE config_id) override;

protected:
    bool LoadConfig(std::shared_ptr<AFClassMeta> pClassMeta);

private:
    bool loaded_{false};

    AFILogModule* m_pLogModule{nullptr};
    AFIClassMetaModule* m_pClassModule{nullptr};

    AFStaticEntityManager* m_pStaticEntityManager{nullptr};
};

} // namespace ark
