#include "backend/modbus_client/IModbusBridge.h"

IModbusBridge::IModbusBridge(const ModbusConfig& cfg, QObject* parent) : QObject(parent) {}
