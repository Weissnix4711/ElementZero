#include <Actor/Player.h>
#include <Core/Minecraft.h>
#include <Command/CommandOutput.h>
#include <Item/Item.h>
#include <Net/NetworkIdentifier.h>
#include <Net/ServerNetworkHandler.h>
#include <RakNet/RakPeer.h>

#include <modutils.h>

#include <base/base.h>

#include "loader.h"

template <typename Holder> struct ValueHolder {
  Holder value;
  ~ValueHolder() {}
  operator Holder() const { return value; }
};

#pragma region Player

// Actor::Actor
SimpleContainer &Actor::getEquipmentContainer() const { return direct_access<SimpleContainer>(this, 1472); }
// Actor::Actor
SimpleContainer &Actor::getHandContainer() const { return direct_access<SimpleContainer>(this, 1480); }
// Player::getSelectedItem
PlayerInventory &Player::getInventory() const { return *direct_access<PlayerInventory *>(this, 3024); }
// Player::Player
Certificate &Player::getCertificate() const { return *direct_access<class Certificate *>(this, 2816); }
// Player::setBedRespawnPosition
BlockPos &Player::getSpawnPosition() const { return direct_access<BlockPos>(this, 7272); }
// ServerPlayer::ServerPlayer
NetworkIdentifier const &Player::getNetworkIdentifier() const {
  return direct_access<class NetworkIdentifier const>(this, 8088);
}
// AddPlayerPacket::AddPlayerPacket
std::string &Player::getDeviceId() const { return direct_access<std::string>(this, 8008); }
// ServerNetworkHandler::_createNewPlayer
std::string &Player::getClientPlatformId() const { return direct_access<std::string>(this, 2824); }
// ServerNetworkHandler::_createNewPlayer
std::string &Player::getPlatformOfflineId() const { return direct_access<std::string>(this, 2760); }
// ServerNetworkHandler::_createNewPlayer
std::string &Player::getClientPlatformOnlineId() const { return direct_access<std::string>(this, 3624); }
// RaidBossComponent::_sendBossEvent
unsigned char Player::getClientSubId() const { return direct_access<unsigned char>(this, 3616); }

#pragma endregion

void NetworkIdentifier::kick(std::string const &reason) const {
  LocateService<ServerNetworkHandler>()->disconnectClient(*this, 0, reason, reason.empty());
}

void Player::kick() { LocateService<ServerNetworkHandler>()->forceDisconnectClient(this, true); }

std::string &ServerNetworkHandler::GetMotd() { return direct_access<std::string>(this, 600); }

void CommandOutput::success() { direct_access<bool>(this, 40) = true; }

ActorUniqueID Level::getNewUniqueID() const {
  auto &r = direct_access<uint64_t>(this, 408);
  return ++r;
}

LevelDataWrapper &Level::GetLevelDataWrapper() { return direct_access<LevelDataWrapper>(this, 544); }

template <> Minecraft *LocateService<Minecraft>() {
  return *GetServerSymbol<Minecraft *>("?mGame@ServerCommand@@1PEAVMinecraft@@EA");
}

template <> ServerNetworkHandler *LocateService<ServerNetworkHandler>() {
  return LocateService<Minecraft>()->getServerNetworkHandler();
}

template <> NetworkHandler *LocateService<NetworkHandler>() {
  return direct_access<std::unique_ptr<NetworkHandler>>(LocateService<ServerInstance>(), 152).get();
}

template <> MinecraftCommands *LocateService<MinecraftCommands>() { return LocateService<Minecraft>()->getCommands(); }

MinecraftCommands *Minecraft::getCommands() { return direct_access<MinecraftCommands *>(this, 160); }

bool Item::getAllowOffhand() const { return direct_access<char>(this, 258) & 0x40; }

RakNet::SystemAddress NetworkIdentifier::getRealAddress() const {
  return LocateService<RakNet::RakPeer>()->GetSystemAddressFromGuid(guid);
}

TClasslessInstanceHook(bool, "?loadLevelData@DBStorage@@UEAA_NAEAVLevelData@@@Z", void *data) {
  auto &path = direct_access<std::string>(this, 160);
  worldHook(std::filesystem::weakly_canonical(path));
  return original(this, data);
}