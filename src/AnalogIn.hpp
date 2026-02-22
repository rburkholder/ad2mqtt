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
 * File:    AnalogIn.hpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2025/12/11 22:01:23
 */

#include <cstdint>
#include <fstream>

#include <string>

#include <unordered_map>

#include "Config.hpp"

class AnalogIn {
public:

  AnalogIn( uint16_t ix );
  AnalogIn( AnalogIn&& rhs );

  uint16_t Read( std::fstream& );
  uint16_t Last() const { return value; }

protected:
private:
  const std::string path;
  uint16_t value;
};

// =======

class AnalogChannels {
public:
  AnalogChannels( const config::Values& );
  void Process();
  void ComposeMessage( std::string& );
  uint16_t operator[]( uint16_t ) const;
protected:
private:

  std::fstream fs;

  using mapAnalogIn_t = std::unordered_map<uint16_t,AnalogIn>;
  mapAnalogIn_t mapAnalogIn;

};

