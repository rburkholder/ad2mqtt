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

#include <iostream>
#include <sstream>
#include <fstream>

#include <string>

#include <vector>

#include "Config.hpp"

class AnalogIn {
public:

  AnalogIn( uint16_t ix_ );
  AnalogIn( AnalogIn&& rhs );

  uint16_t Read();
  uint16_t Ix() const { return ix; }

protected:
private:
  const uint16_t ix;
  std::stringstream ss;
  std::fstream fs;
};

class AnalogChannels {
public:
  AnalogChannels( const config::Values& );
  void Process( std::string& );
protected:
private:

  using vAnalogIn_t = std::vector<AnalogIn>;
  vAnalogIn_t vAnalogIn;

  using vValue_t = std::vector<uint16_t>;
  vValue_t vValue;
};

