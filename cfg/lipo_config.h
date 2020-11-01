/*
 * lipo, a class used to monitor lipo cell voltages measured by using a simple 
 * voltage divider and adc channel per cell.
 * 
 * Copyright (C) 2020 Julian Friedrich
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 *
 * You can file issues at https://github.com/fjulian79/liblipo/issues
 */

#ifndef LIPO_CONFIG_H_
#define LIPO_CONFIG_H_

/**
 * @brief The The ADC Channel cell 1 is conencted to
 */
#define LIPO_ADCCHANNEL_0                       3

/**
 * @brief The number of cells supported by the hardware.
 */
#define LIPO_ADCCHANNELS                        3

/**
 * @brief The scale factors to mV for each cell.
 * 
 * See lipo.hpp for mor infos on how this values are calculated.
 */
#define LIPO_PARAMS                             \
{                                               \
    .CellScale = {2669, 5803, 8806}             \
} 

#endif /* LIPO_CONFIG_H_ */