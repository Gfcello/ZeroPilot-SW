/**
 * Xbee Sensor Functions
 * Author: Gordon Fountain
 */

#ifndef XBEE_HPP
#define XBEE_HPP

#include "TelemPathInterface.hpp" //Holds structs for sent and received data

class XBEE {
    public:
        #ifdef UNIT_TESTING
            XBEE();
        #else    
            static XBEE* getInstance();
            
            //make it a singleton by deleting constructor
            XBEE(const XBEE*) = delete;
        #endif        

        /**
         * Triggers interrupt for new data retrieval by telemManager - stores raw data in variables and returns right away
         * */
        virtual void Receive_GS_Data(); 

        /**GetResult should:
         * 1. Reset newInput flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * */
        virtual void GetResult(Telemetry_PIGO_t *data); //Switch to MavLink data (ask Jingting how to feed to Xbee)

        /*Interrupt to send data to ground       
        */
        virtual void Send_GS_Data(); //Only touched by FREERTOS

        /*SendResult should:
         * 1. Reset newOutput flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * 4. Wait for FREERTOS to call the Send_Data function
        */
        virtual void SendResult(Telemetry_POGI_t* data);
    
    private:
        #ifndef UNIT_TESTING
            XBEE();
        #endif

        static XBEE* xbeeInstance;
};

#endif
