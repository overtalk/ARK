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

#include "proxy/include/AFCProxyNetModule.hpp"

namespace ark {

bool AFCProxyNetModule::Init()
{
    m_pLogModule = FindModule<AFILogModule>();
    m_pBusModule = FindModule<AFIBusModule>();
    m_pMsgModule = FindModule<AFIMsgModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();

    return true;
}

bool AFCProxyNetModule::PostInit()
{
    StartServer();

    return true;
}

int AFCProxyNetModule::StartServer()
{
    auto ret = m_pNetServiceManagerModule->CreateServer();
    ret.Then([=](const std::pair<bool, std::string>& resp) {
        if (!resp.first)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), resp.second);
            ARK_ASSERT_NO_EFFECT(0);
            exit(0);
        }

        m_pNetServer = m_pNetServiceManagerModule->GetSelfNetServer();
        if (m_pNetServer == nullptr)
        {
            ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
            exit(0);
        }

        m_pNetServer->RegMsgCallback(AFMsg::EGMI_REQ_CONNECT_KEY, this, &AFCProxyNetModule::OnConnectKeyProcess);
        m_pNetServer->RegMsgCallback(AFMsg::EGMI_REQ_WORLD_LIST, this, &AFCProxyNetModule::OnReqServerListProcess);
        m_pNetServer->RegMsgCallback(AFMsg::EGMI_REQ_SELECT_SERVER, this, &AFCProxyNetModule::OnSelectServerProcess);
        m_pNetServer->RegMsgCallback(AFMsg::EGMI_REQ_ROLE_LIST, this, &AFCProxyNetModule::OnReqRoleListProcess);
        m_pNetServer->RegMsgCallback(AFMsg::EGMI_REQ_CREATE_ROLE, this, &AFCProxyNetModule::OnReqCreateRoleProcess);
        m_pNetServer->RegMsgCallback(AFMsg::EGMI_REQ_DELETE_ROLE, this, &AFCProxyNetModule::OnReqDelRoleProcess);
        m_pNetServer->RegMsgCallback(AFMsg::EGMI_REQ_ENTER_GAME, this, &AFCProxyNetModule::OnReqEnterGameServer);
    });

    return 0;
}

void AFCProxyNetModule::OnServerInfoProcess(const AFNetMsg* msg, const int64_t session_id) {}

void AFCProxyNetModule::OnSelectServerResultProcess(const AFNetMsg* msg, const int64_t session_id) {}

bool AFCProxyNetModule::VerifyConnectData(const std::string& strAccount, const std::string& strKey)
{
    std::shared_ptr<ClientConnectData> pConnectData = mxWantToConnectMap.find_value(strAccount);
    if (pConnectData != nullptr && strKey == pConnectData->strConnectKey)
    {
        mxWantToConnectMap.erase(strAccount);
        return true;
    }

    return false;
}

// void AFCProxyNetServerModule::OnTransMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const
// uint32_t nLen, const AFGUID& xClientID)
//{
//    std::shared_ptr<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

//    if (!pSessionData || pSessionData->mnLogicState <= 0 || pSessionData->mnGameID <= 0)
//    {
//        //state error
//        return;
//    }

//    if (pSessionData->mnUserID != xHead.GetUID())
//    {
//        //when after entergame xHead.GetPlayerID() is really palyerID
//        ARK_LOG_ERROR("xHead.GetPlayerID() is not really palyerID, id = {}", xHead.GetUID().ToString());
//        return;
//    }

//    m_pMsgModule->SendSSMsg(pSessionData->mnGameID, nMsgID, msg, nLen, xHead.GetUID());
//}

void AFCProxyNetModule::OnConnectKeyProcess(const AFNetMsg* msg, const int64_t session_id)
{
    // ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ReqAccountLogin);
    // bool bRet = m_pProxyNetClientModule->VerifyConnectData(xMsg.account(), xMsg.security_code());

    // if (bRet)
    //{
    //    std::shared_ptr<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

    //    if (pSessionData)
    //    {
    //        pSessionData->mnLogicState = 1;
    //        pSessionData->mstrAccout = xMsg.account();

    //        AFMsg::AckEventResult xSendMsg;
    //        xSendMsg.set_event_code(AFMsg::EGEC_VERIFY_KEY_SUCCESS);
    //        *xSendMsg.mutable_event_client() = AFIMsgModule::GUIDToPB(pSessionData->mnClientID);
    //        *xSendMsg.mutable_event_object() = AFIMsgModule::GUIDToPB(nPlayerID);

    //        m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_CONNECT_KEY, xSendMsg, xClientID, nPlayerID);
    //    }
    //}
    // else
    //{
    //    m_pNetServer->GetNet()->CloseNetEntity(xClientID);
    //}
}

void AFCProxyNetModule::OnSocketEvent(const AFNetEvent* event)
{
    switch (event->GetType())
    {
        case AFNetEventType::CONNECTED:
        {
            ARK_LOG_INFO("Connected success, id = {}", event->GetId());
            OnClientConnected(event->GetId());
        }
        break;
        case AFNetEventType::DISCONNECTED:
        {
            ARK_LOG_INFO("Connection closed, id = {}", event->GetId());
            OnClientDisconnect(event->GetId());
        }
        break;
        default:
            break;
    }
}

void AFCProxyNetModule::OnClientConnected(const AFGUID& conn_id)
{
    std::shared_ptr<AFClientConnectionData> pSessionData = std::make_shared<AFClientConnectionData>();

    pSessionData->conn_id_ = conn_id;
    client_connections_.insert(conn_id, pSessionData);
}

void AFCProxyNetModule::OnClientDisconnect(const AFGUID& conn_id)
{
    std::shared_ptr<AFClientConnectionData> pSessionData = client_connections_.find_value(conn_id);
    if (pSessionData != nullptr)
    {
        if (pSessionData->game_id_ != 0 && pSessionData->actor_id_ != 0)
        {
            AFMsg::ReqLeaveGameServer xData;
            // send to game, notify actor leaving game
            m_pMsgModule->SendParticularSSMsg(
                pSessionData->game_id_, AFMsg::EGameMsgID::EGMI_REQ_LEAVE_GAME, xData, 0, pSessionData->actor_id_);
        }

        client_connections_.erase(conn_id);
    }
    else
    {
        // log
    }
}

void AFCProxyNetModule::OnSelectServerProcess(const AFNetMsg* msg, const int64_t session_id)
{
    // ARK_PO CESS_MSG(xHead, msg, nLen, AFMsg::ReqSelectServer);
    // AFINetClientService* pWorldNetService = m_pNetClientManagerModule->GetNetClientServiceByBusID(xMsg.world_id());
    // std::shared_ptr<AFConnectionData> pServerData = pWorldNetService->GetServerNetInfo(xMsg.world_id());
    // if (pServerData != nullptr && AFConnectionData::CONNECTED == pServerData->net_state_)
    //{
    //    std::shared_ptr<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

    //    if (pSessionData)
    //    {
    //        //now this client bind a game server, after this time, all message will be sent to this game server who
    //        bind with client pSessionData->mn ameID = xMsg.wor //        AFMsg::AckEventResult xResultMsg;
    //        xResultMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_SUCCESS);
    //        m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xResultMsg, xClientID, nPlayerID);
    //        return;
    //    }
    //}

    // AFMsg::AckEventResult xSendMsg;
    // xSendMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_FAIL);
    // m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xMsg, xClientID, nPlayerID);
}

void AFCProxyNetModule::OnReqServerListProcess(const AFNetMsg* msg, const int64_t session_id)
{
    // ARK_PO CESS_MSG(xHead, msg, nLen, AFMsg::ReqServerList);

    // if (xMsg.type() != AFMsg::RSLT_GAMES_ERVER)
    //{
    //    return;
    //}

    // std::shared_ptr<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

    // if (pSessionData && pSessionData->mnLogicState > 0)
    //{
    //    //ack all gameserver data
    //    AFMsg::AckServerList xData;
    //    xData.set_type(AFMsg::RSLT_GAMES_ERVER);

    //    AFINetClientService* pGameNetService = m_pNetClientManagerModule->GetNetClientService(ARK_APP_GAME);
    //    if (pGameNetService == nullptr)
    //    {
    //        return;
    //    }

    //    AFMapEx<int, AFConnectionData>& xServerList = pGameNetService->GetServerList();
    //    for (auto pGameData = xServerList.First(); pGameData != nullptr; pGameData = xServerList.Next())
    //    {
    //        if (AFConnectionData::CONNECTED == pGameData->net_state_)
    //        {
    //            AFMsg::ServerInfo* pServerInfo = xData.add_info();

    //            pServerInfo->set_status(AFMsg::EServerState::EST_NARMAL);
    //            pServerInfo->set_server_id(pGameData->server_bus_id_);
    //            pServerInfo->set_wait_count(0);
    //        }
    //    }

    //    m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_WORLD_LIST, xData, xClientID, nPlayerID);
    //}
}

int AFCProxyNetModule::Transpond(const AFNetMsg* msg)
{
    // std::shared_ptr<AFClientConnectioD ata> pSessionData = client_connections_.GetElement(xHead.GetUID());

    // if (pSessionData)
    //{
    //    m_pNetServer->GetNet()->SendRawMsg(nMsgID, msg, nLen, pSessionData->conn_id_, xHead.GetUID());
    //}

    return 0;
}

int AFCProxyNetModule::SendToPlayerClient(
    const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nClientD, const AFGUID& nPlayer)
{
    // m_N etServer->GetNet()->SenR awMsg(nMsgID, msg, nLen, nClientID, nPlayer);

    return true;
}

void AFCProxyNetModule::OnReqRoleListProcess(const AFNetMsg* msg, const int64_t session_id)
{
    // Checke ssionTransMsg<AFMsg::ReqRoleList>(xHead, nMsgID, msg, nLen, xClientID);
}

void AFCProxyNetModule::OnReqCreateRoleProcess(const AFNetMsg* msg, const int64_t session_id)
{
    // CheckSessionTransMsg<AFMsg::ReqCreateRole>(xHead, nMsgID, msg, nLen, xClientID);
}

void AFCProxyNetModule::OnReqDelRoleProcess(const AFNetMsg* msg, const int64_t session_id)
{
    // CheckSessionTransMs< AFMsg::ReqDeleteRole>(xHead, nMsgID, msg, nLen, xClientID);
}

void AFCProxyNetModule::OnReqEnterGameServer(const AFNetMsg* msg, const int64_t session_id)
{
    // CheckSessionTransMs< AFMsg::ReqEnterGameServer>(xHead, nMsgID, msg, nLen, xClientID);
}

int AFCProxyNetModule::EnterGameSuccessEvent(const AFGUID conn_id, const AFGUID actor_id)
{
    std::shared_ptr<AFClientConnectionData> pSessionData = client_connections_.find_value(conn_id);

    if (pSessionData != nullptr)
    {
        pSessionData->actor_id_ = actor_id;
    }
    else
    {
        // log
    }

    return 0;
}

bool AFCProxyNetModule::CheckSessionState(const int nGameID, const AFGUID& xClientID, const std::string& strAccount)
{
    //AFINetClientService* pWorldNetService = m_pNetServiceManagerModule->GetNetClientServiceByBusID(nGameID);
    //std::shared_ptr<AFConnectionData> pServerData = pWorldNetService->GetServerNetInfo(nGameID);
    //if (pServerData != nullptr && AFConnectionData::CONNECTED == pServerData->net_state_)
    //{
    //    std::shared_ptr<AFClientConnectionData> pSessionData = client_connections_.find_value(xClientID);

    //    if (pSessionData != nullptr && pSessionData->logic_state_ > 0 && pSessionData->game_id_ == nGameID &&
    //        pSessionData->account_ == strAccount)
    //    {
    //        return true;
    //    }
    //}

    return false;
}

void AFCProxyNetModule::OnOtherMessage(const AFNetMsg* msg, const int64_t session_id)
{
    // Transpond(xHead, nMsgID  msg, nLen);
}

void AFCProxyNetModule::OnBrocastmsg(const AFNetMsg* msg, const int64_t session_id)
{
    // ARK_PROCESS_MSG(xHead, s g, nLen, AFMsg::BrocastMsg);

    // for (int i = 0; i < xMsg.target_entity_list_size(); i++)
    //{
    //    const AFMsg::PBGUID& xPlayerClientID = xMsg.target_entity_list(i);
    //    m_pProxyServerNet_ServerModule->SendToPlayerClient(xMsg.msg_id(), xMsg.msg_data().c_str(),
    //    xMsg.msg_data().size(), AFIMsgModule::PBToGUID(xPlayerC ientID), nPlayerID);
}

void AFCProxyNetModule::OnAckEnterGame(const AFNetMsg* msg, const int64_t session_id)
{
    // ARK_PROCESS_MSG(xHead, s g, nLen, AFMsg::AckEventResult);

    // if (xMsg.event_code() == AFMsg::EGEC_ENTER_GAME_SUCCESS)
    //{
    //    const AFGUID& xClient = AFIMsgModule::PBToGUID(xMsg.event_client());
    //    const AFGUID& xPlayer = AFIMsgModule::PBToGUID(xMsg.event_object());

    //    m_pProxyServerNet_ServerModule->EnterGameSuccessEvent(xClient, xPlayer);
    //}
}

} // namespace ark
