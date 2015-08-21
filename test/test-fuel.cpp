/* mbed
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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
 */


#include "mbed.h"
#include "minar/minar.h"

#include "fuel-gauge-max1704/FuelGauge.h"

#include "swo/swo.h"

#if defined(STK3700)
// stk3700
PinName FUEL_IRQ = PD4;
DigitalOut vibra(LED0);
#elif defined(WATCH)
// watch
PinName I2C_SDA = PC4;
PinName I2C_SCL = PC5;
PinName FUEL_IRQ = PD4;
DigitalOut vibra(PA10);
#else
#error platform not supported
#endif


FuelGauge monitor(I2C_SDA, I2C_SCL, FUEL_IRQ);

InterruptIn mbed_button0(BTN0);


#define FUEL_GAUGE_ADDRESS 0x6C


static void button0fall()
{
    vibra = !vibra;

    monitor.reset(NULL);
}



static void milliVoltDone(int milliVolt)
{
    swoprintf("volt: %d.%03dV\r\n", milliVolt / 1000, milliVolt % 1000);
}

static void permilleDone(int charge)
{
    swoprintf("charge: %d.%d%%\r\n", charge / 10, charge % 10);

    monitor.getMilliVolt(milliVoltDone);
}

static void fuelTask()
{
    swoprintf("fuel\r\n");

    monitor.getPerMille(permilleDone);

//    monitor.getVoltage(&value, voltageDone);
//    monitor.getPercentage(&version, i2cDone);
//    monitor.getVersion(&version, i2cDone);
//    monitor.getHibernate(&version, i2cDone);
//    monitor.getConfig(&version, i2cDone);
//    monitor.getAlert(&version, i2cDone);
//    monitor.getRate(&version, i2cDone);
//    monitor.getResetID(&version, i2cDone);
//    monitor.getStatus(&version, i2cDone);
//    monitor.getCommand(&version, i2cDone);
}

void app_start(int, char *[])
{
    mbed_button0.mode(PullUp);
    // Delay for initial pullup to take effect
    wait(.01);
    mbed_button0.fall(button0fall);

    minar::Scheduler::postCallback(fuelTask);
}
