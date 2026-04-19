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
 * File:    GpioState.hpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2026/04/19 14:21:56
 */

#pragma once

#include <gpiod.hpp>

class GpioState {
public:

  GpioState( const std::string& sName, unsigned int line_offset );
  ~GpioState();

  void RelayOn();
  void RelayOff();

protected:
private:
  ::gpiod::chip chip; // declare before line_request
  ::gpiod::line_request lr; // declare after chip
  ::gpiod::line::offset lo;
};

