/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    Config.hpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2025/12/10 23:24:17
 */

#pragma once

#include <set>
#include <cstdint>

#include <ou/mqtt/config.hpp>

namespace config {

using setAnalogInIx_t = std::set<uint16_t>;

struct Values {

  ou::mqtt::Config mqtt;
  uint16_t nPollIntervalSeconds;
  setAnalogInIx_t setAnalogInIx;

};

bool Load( const std::string& sFileName, Values& );

} // namespace config
