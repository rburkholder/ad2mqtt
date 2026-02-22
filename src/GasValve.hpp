/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    Gas.hpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2026/02/21 18:47:09
 */

#pragma once

#include <cstdint>
#include <string>
#include <functional>

class GasValve {
public:

  GasValve( const std::string& gpio, uint16_t upper, uint16_t lower );
  ~GasValve();

  void Process( uint16_t );

protected:
private:

  const uint16_t m_nUpper;
  const uint16_t m_nLower;

  const std::string m_sGPIO;

  using fHysteresis_t = std::function<void(uint16_t)>;
  fHysteresis_t m_fHysteresis_jump;

  void Hysteresis_gt( uint16_t );
  void Hysteresis_lt( uint16_t );

};