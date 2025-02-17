#include <Arduino.h>
// Include for ModbusMaster type 
#include <ModbusMaster.h>

#ifndef FLUORESCENCE_H
#define FLUORESCENCE_H

    void setup_fluorescence ();

    void fetch_fluorescence ();

    void set_gain (int gain);

    void set_atime (int atime);

    void set_astep (int astep);

#endif