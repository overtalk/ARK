/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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
 *
 */

#include "kernel/include/AFClassMeta.hpp"

namespace ark {

AFClassMeta::AFClassMeta(const std::string& name)
    : name_(name)
{
    class_meta_call_back_ = std::make_shared<AFClassCallBackManager>();
}

size_t AFClassMeta::GetNodeCount() const
{
    return data_meta_list_.size();
}

const std::string& AFClassMeta::GetName() const
{
    return name_;
}

void AFClassMeta::SetResPath(const std::string& value)
{
    res_path_ = value;
}

const std::string& AFClassMeta::GetResPath() const
{
    return res_path_;
}

// create data meta
std::shared_ptr<AFNodeMeta> AFClassMeta::CreateDataMeta(const std::string& name, const uint32_t index)
{
    // check arg
    ARK_ASSERT_RET_VAL(!name.empty() && index > 0, nullptr);

    ARK_ASSERT_RET_VAL(data_meta_list_.find(index) == data_meta_list_.end(), nullptr);

    ARK_ASSERT_RET_VAL(NameIndexInsert(name, index), nullptr);

    // create new data meta
    std::shared_ptr<AFNodeMeta> pMeta = std::make_shared<AFNodeMeta>(name, index);
    ARK_ASSERT_RET_VAL(data_meta_list_.insert(index, pMeta).second, nullptr);

    return pMeta;
}

std::shared_ptr<AFNodeMeta> AFClassMeta::FindDataMeta(const uint32_t index) const
{
    return data_meta_list_.find_value(index);
}

// create table meta
std::shared_ptr<AFTableMeta> AFClassMeta::CreateTableMeta(const std::string& name, const uint32_t index)
{
    // check arg
    ARK_ASSERT_RET_VAL(!name.empty() && index > 0, nullptr);

    ARK_ASSERT_RET_VAL(table_meta_list_.find(index) == table_meta_list_.end(), nullptr);

    ARK_ASSERT_RET_VAL(NameIndexInsert(name, index), nullptr);

    std::shared_ptr<AFTableMeta> pMeta = std::make_shared<AFTableMeta>(name, index);
    ARK_ASSERT_RET_VAL(table_meta_list_.insert(index, pMeta).second, nullptr);

    return pMeta;
}

std::shared_ptr<AFTableMeta> AFClassMeta::FindTableMeta(const uint32_t index) const
{
    return table_meta_list_.find_value(index);
}

// create container meta
std::shared_ptr<AFContainerMeta> AFClassMeta::CreateContainerMeta(
    const std::string& name, const uint32_t index, const std::string& class_name)
{
    // check arg
    ARK_ASSERT_RET_VAL(!name.empty() && index > 0, nullptr);

    ARK_ASSERT_RET_VAL(container_meta_list_.find(index) == container_meta_list_.end(), nullptr);

    ARK_ASSERT_RET_VAL(NameIndexInsert(name, index), nullptr);

    std::shared_ptr<AFContainerMeta> pMeta = std::make_shared<AFContainerMeta>(name, index, class_name);
    ARK_ASSERT_RET_VAL(container_meta_list_.insert(index, pMeta).second, nullptr);

    return pMeta;
}

std::shared_ptr<AFContainerMeta> AFClassMeta::FindContainerMeta(const uint32_t index) const
{
    return container_meta_list_.find_value(index);
}

const AFClassMeta::DataMetaList& AFClassMeta::GetDataMetaList() const
{
    return data_meta_list_;
}

const AFClassMeta::TableMetaList& AFClassMeta::GetTableMetaList() const
{
    return table_meta_list_;
}

const AFClassMeta::ContainerMetaList& AFClassMeta::GetContainerMetaList() const
{
    return container_meta_list_;
}

std::shared_ptr<AFClassCallBackManager> AFClassMeta::GetClassCallBackManager() const
{
    return class_meta_call_back_;
}

uint32_t AFClassMeta::GetIndex(const std::string& name) const
{
    auto iter = name_index_list_.find(name);
    if (iter == name_index_list_.end())
    {
        return NULL_INT;
    }

    return iter->second;
}

bool AFClassMeta::IsEntityMeta() const
{
    return res_path_.empty();
}

bool AFClassMeta::NameIndexInsert(const std::string& name, const uint32_t index)
{
    return name_index_list_.insert(std::make_pair(name, index)).second;
}

} // namespace ark
