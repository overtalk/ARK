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

#include "base/AFMisc.hpp"
#include "bus/include/AFCMsgModule.hpp"

namespace ark {

bool AFCMsgModule::Init()
{
    m_pBusModule = FindModule<AFIBusModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();
    m_pLogModule = FindModule<AFILogModule>();

    return true;
}

bool AFCMsgModule::SendSuitSSMsg(const uint8_t app_type, const std::string& hash_key, const int msg_id,
    const google::protobuf::Message& msg, const AFGUID& actor_id /* = 0*/)
{
    uint32_t crc32 = AFCRC::crc32(hash_key);
    return SendSuitSSMsg(app_type, crc32, msg_id, msg, actor_id);
}

bool AFCMsgModule::SendSuitSSMsg(const uint8_t app_type, const uint32_t& hash_value, const int msg_id,
    const google::protobuf::Message& msg, const AFGUID& actor_id /* = 0*/)
{
    int suit_bus_id = 0; // GetSuitBus(app_type, hash_value);
    return SendSSMsg(suit_bus_id, msg_id, msg, actor_id);
}

bool AFCMsgModule::SendParticularSSMsg(const int bus_id, const int msg_id, const google::protobuf::Message& msg,
    const AFGUID& conn_id, const AFGUID& actor_id /* = 0*/)
{
    std::string msg_data = msg.SerializeAsString();
    return SendSSMsg(bus_id, msg_id, msg, conn_id, actor_id);
}

//////////////////////////////////////////////////////////////////////////

bool AFCMsgModule::SendSSMsg(const int target_bus, const int msg_id, const google::protobuf::Message& msg,
    const AFGUID& conn_id, const AFGUID& actor_id /* = 0*/)
{
    std::string msg_data;
    ARK_ASSERT_RET_VAL(msg.SerializeToString(&msg_data), false);

    int src_bus = m_pBusModule->GetSelfBusID();
#ifdef ARK_RUN_MODE_DEBUG
    std::string pb_json;
    auto status = google::protobuf::util::MessageToJsonString(msg, &pb_json);
    if (!status.ok())
    {
        return false;
    }

    ARK_LOG_DEBUG("Send msg log\nsrc={}\ndst={}\nmsg_name={}\nmsg_id={}\nmsg_len={}\nmsg_data={}",
        AFMisc::Bus2Str(src_bus), AFMisc::Bus2Str(target_bus), msg.GetTypeName(), msg_id, msg_data.length(), pb_json);
#endif
    return SendSSMsg(src_bus, target_bus, msg_id, msg_data.c_str(), msg_data.length(), conn_id, actor_id);
}

bool AFCMsgModule::SendSSMsg(const int target_bus, const int msg_id, const char* msg, const int msg_len,
    const AFGUID& conn_id, const AFGUID& actor_id /*= 0*/)
{
    return SendSSMsg(m_pBusModule->GetSelfBusID(), target_bus, msg_id, msg, msg_len, conn_id, actor_id);
}

bool AFCMsgModule::SendSSMsg(const int src_bus, const int target_bus, const int msg_id, const char* msg_data,
    const int msg_len, const AFGUID& session_id, const AFGUID& actor_id /* = 0*/)
{
    AFSSMsgHead head;
    head.id_ = msg_id;
    head.length_ = msg_len;
    head.actor_id_ = actor_id;
    head.src_bus_ = src_bus;
    head.dst_bus_ = target_bus;

    // TODO: NickYang
    //AFINet* net_ptr = m_pNetServiceManagerModule->GetNetConnectionBus(src_bus, target_bus);
    //if (net_ptr != nullptr)
    //{
    //    return net_ptr->SendMsg(&head, msg_data, session_id);
    //}

    ARK_LOG_ERROR("send ss msg error, src_bus={} target_bus={} msg_id={} conn_id={} target_role_id={}", src_bus,
        target_bus, msg_id, session_id, actor_id);
    return false;
}

bool AFCMsgModule::SendSSMsgByRouter(const AFSSMsgHead& head)
{
    // TODO: will add later
    return true;
}

} // namespace ark
