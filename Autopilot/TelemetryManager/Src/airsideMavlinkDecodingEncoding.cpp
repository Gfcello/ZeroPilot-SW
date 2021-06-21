#include "airsideMavlinkDecodingEncoding.hpp"
#include "Airside_Functions.hpp"

_AirsideMavlinkDecodingEncoding decode(std::vector<uint8_t> rawData, Telemetry_PIGO_t* decodedData) {
    int elementsSearched = 0;
    _AirsideMavlinkDecodingEncoding toReturn = MAVLINK_DECODING_ENCODING_OK;

    // Until we reach end of array
    while (elementsSearched < rawData.size()) {
        
        mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE; // We are not done decoding

        uint8_t decodedMessageBuffer[256]; // We only process one piece of info at a time. 256 used as array size since max datatype is 32 bytes 
 
        PIGO_Message_IDs_e messageType = MESSAGE_ID_NONE; // This is going to be set by the decoder function called in the for loop. We are only initializing to prevent undefined behaviour

        bool done = false;
        // Go through raw data and process it. When we have reached the end of a packet, we return MAVLINK_DECODING_OKAY and exit the loop
        for (int i = 0; i < 256 && !done; i++) {
            if (decoderStatus != MAVLINK_DECODING_OKAY) {
                if (elementsSearched < rawData.size()) { // Protect against segfaults
                    decoderStatus = Mavlink_airside_decoder(&messageType, rawData.at(elementsSearched), decodedMessageBuffer);
                }
                elementsSearched++;
            } else {
                done = true;
            }
        }

        if (decoderStatus == MAVLINK_DECODING_OKAY) {
            // Set our return struct based on the type of message we decoded
            switch (messageType) {
                // PIGO_GPS_LANDING_SPOT_t
                case MESSAGE_ID_GPS_LANDING_SPOT:
                {
                    PIGO_GPS_LANDING_SPOT_t landingSpotDecoded;
                    memcpy(&landingSpotDecoded, &decodedMessageBuffer, sizeof(PIGO_GPS_LANDING_SPOT_t));

                    decodedData->stoppingLatitude = (double) landingSpotDecoded.latitude;
                    decodedData->stoppingLongitude = (double) landingSpotDecoded.longitude;
                    decodedData->stoppingAltitude = (float) landingSpotDecoded.altitude;
                    decodedData->stoppingDirectionHeading = (float) landingSpotDecoded.landingDirection;
                }
                break;

                // PIGO_GROUND_COMMAND_t
                case MESSAGE_ID_GROUND_CMD:
                {
                    PIGO_GROUND_COMMAND_t groundCommandDecoded;
                    memcpy(&groundCommandDecoded, &decodedMessageBuffer, sizeof(PIGO_GROUND_COMMAND_t));

                    decodedData->groundCommandsHeading = (float) groundCommandDecoded.heading;
                    decodedData->latestDistance = (float) groundCommandDecoded.latestDistance;
                }
                break;
                
                // PIGO_GIMBAL_t
                case MESSAGE_ID_GIMBAL_CMD:
                {
                    PIGO_GIMBAL_t gimbalCommandDecoded;
                    memcpy(&gimbalCommandDecoded, &decodedMessageBuffer, sizeof(PIGO_GIMBAL_t));

                    decodedData->gimbalPitch = (float) gimbalCommandDecoded.pitch;
                    decodedData->gimbalYaw = (float) gimbalCommandDecoded.yaw;
                }
                break;
                
                // single_bool_cmd_t
                case MESSAGE_ID_BEGIN_LANDING:
                {
                    single_bool_cmd_t landingCommandDecoded;
                    memcpy(&landingCommandDecoded, &decodedMessageBuffer, sizeof(single_bool_cmd_t));

                    decodedData->beginLanding = (bool) landingCommandDecoded.cmd;
                }
                break;
                
                case MESSAGE_ID_BEGIN_TAKEOFF:
                {
                    single_bool_cmd_t takeoffCommandDecoded;
                    memcpy(&takeoffCommandDecoded, &decodedMessageBuffer, sizeof(single_bool_cmd_t));

                    decodedData->beginTakeoff = (bool) takeoffCommandDecoded.cmd;
                }
                break;

                case MESSAGE_ID_INITIALIZING_HOMEBASE:
                {
                    single_bool_cmd_t holdingAltitudeCommandDecoded;
                    memcpy(&holdingAltitudeCommandDecoded, &decodedMessageBuffer, sizeof(single_bool_cmd_t));

                    decodedData->holdingAltitude = (bool) holdingAltitudeCommandDecoded.cmd;
                }
                break;
                
                // four_bytes_int_cmd_t
                case MESSAGE_ID_NUM_WAYPOINTS:
                {
                    four_bytes_int_cmd_t numWaypointsCommandDecoded;
                    memcpy(&numWaypointsCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

                    decodedData->numWaypoints = (int) numWaypointsCommandDecoded.cmd;
                }
                break;

                case MESSAGE_ID_HOLDING_ALTITUDE:
                {
                    four_bytes_int_cmd_t holdingRadiusCommandDecoded;
                    memcpy(&holdingRadiusCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

                    decodedData->holdingTurnRadius = (int) holdingRadiusCommandDecoded.cmd;
                }
                break;
                
                case MESSAGE_ID_HOLDING_TURN_RADIUS:
                {
                    four_bytes_int_cmd_t holdingRadiusCommandDecoded;
                    memcpy(&holdingRadiusCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

                    decodedData->holdingTurnRadius = (int) holdingRadiusCommandDecoded.cmd;
                }
                break;

                case MESSAGE_ID_PATH_MODIFY_NEXT_ID:
                {
                    single_bool_cmd_t nextIdCommandDecoded;
                    memcpy(&nextIdCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

                    decodedData->nextId = (int) nextIdCommandDecoded.cmd;
                }
                break;
                
                case MESSAGE_ID_PATH_MODIFY_PREV_ID:
                {
                    single_bool_cmd_t prevIdCommandDecoded;
                    memcpy(&prevIdCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

                    decodedData->prevId = (int) prevIdCommandDecoded.cmd;
                }
                break;
                
                case MESSAGE_ID_PATH_MODIFY_ID:
                {
                    single_bool_cmd_t modifyIdCommandDecoded;
                    memcpy(&modifyIdCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

                    decodedData->modifyId = (int) modifyIdCommandDecoded.cmd;
                }
                break;

                // one_byte_uint_cmd_t
                case MESSAGE_ID_WAYPOINT_MODIFY_PATH_CMD:
                {
                    one_byte_uint_cmd_t modifyPathCommandDecoded;
                    memcpy(&modifyPathCommandDecoded, &decodedMessageBuffer, sizeof(one_byte_uint_cmd_t));

                    decodedData->waypointModifyFlightPathCommand = (_ModifyFlightPathCommand) modifyPathCommandDecoded.cmd;
                }
                break;
                
                case MESSAGE_ID_WAYPOINT_NEXT_DIRECTIONS_CMD:
                {
                    one_byte_uint_cmd_t nextDirectionsCommandDecoded;
                    memcpy(&nextDirectionsCommandDecoded, &decodedMessageBuffer, sizeof(one_byte_uint_cmd_t));

                    decodedData->waypointNextDirectionsCommand = (_GetNextDirectionsCommand) nextDirectionsCommandDecoded.cmd;
                }
                break;
                
                case MESSAGE_ID_HOLDING_TURN_DIRECTION:
                {
                    one_byte_uint_cmd_t holdingTurnDirectionCommandDecoded;
                    memcpy(&holdingTurnDirectionCommandDecoded, &decodedMessageBuffer, sizeof(one_byte_uint_cmd_t));

                    decodedData->holdingTurnDirection = (uint8_t) holdingTurnDirectionCommandDecoded.cmd;
                }
                break;
                
                // PIGO_WAYPOINTS_t
                case MESSAGE_ID_WAYPOINTS:
                {
                    PIGO_WAYPOINTS_t waypointCommandsDecoded;
                    memcpy(&waypointCommandsDecoded, &decodedMessageBuffer, sizeof(PIGO_WAYPOINTS_t));

                    Telemetry_Waypoint_Data_t waypoint {};
                    waypoint.altitude = (int) waypointCommandsDecoded.altitude;
                    waypoint.latitude = (long double) waypointCommandsDecoded.latitude;
                    waypoint.longitude = (long double) waypointCommandsDecoded.longitude;
                    waypoint.turnRadius = (float) waypointCommandsDecoded.turnRadius;
                    waypoint.waypointType = (uint8_t) waypointCommandsDecoded.waypointType;

                    decodedData->waypoints.push_back(waypoint);
                }
                break;
                
                case MESSAGE_ID_HOMEBASE:
                {
                    PIGO_WAYPOINTS_t homebaseCommandsDecoded;
                    memcpy(&homebaseCommandsDecoded, &decodedMessageBuffer, sizeof(PIGO_WAYPOINTS_t));

                    decodedData->homebase.latitude = (long double) homebaseCommandsDecoded.latitude;
                    decodedData->homebase.longitude = (long double) homebaseCommandsDecoded.longitude;
                    decodedData->homebase.altitude = (int) homebaseCommandsDecoded.altitude;
                    decodedData->homebase.turnRadius = (float) homebaseCommandsDecoded.turnRadius;
                    decodedData->homebase.waypointType = (uint8_t) homebaseCommandsDecoded.waypointType;
                }
                break;
                
                default: 
                    break;
            }
        } else { // Will not tolerate errors
            return MAVLINK_DECODING_ENCODING_FAILED;
        }
    }

    return toReturn;
} 

_AirsideMavlinkDecodingEncoding encode(Telemetry_POGI_t* rawData, mavlink_message_t* encodedData) {
    
    // POGI_Timestamp_t
    POGI_Timestamp_t timeStampCommand = { rawData->timeStamp };

    // four_bytes_int_cmd_t
    four_bytes_int_cmd_t airspeedCommand = { rawData->curAirspeed };
    four_bytes_int_cmd_t waypointIdCommand = { rawData->currentWaypointId };
    four_bytes_int_cmd_t waypointIndexCommand = { rawData->currentWaypointIndex };

    // single_bool_cmd_t
    single_bool_cmd_t isLandedCommand = { rawData->isLanded };
    single_bool_cmd_t isHomebaseInitializedCommand = { rawData->homeBaseInit };

    // POGI_Euler_Angle_t
    POGI_Euler_Angle_t planeAngleCommand = { 
        (int32_t) (*(int32_t*) &rawData->yaw), // Convert float memory to 32 bit integer
        (int32_t) (*(int32_t*) &rawData->pitch), 
        (int32_t) (*(int32_t*) &rawData->roll) 
    }; 

    POGI_Euler_Angle_t cameraAngleCommand = { 
        (int32_t) (*(int32_t*) &rawData->camYaw), 
        (int32_t) (*(int32_t*) &rawData->camPitch), 
        (int32_t) (*(int32_t*) &rawData->camRoll) 
    };

    // POGI_GPS_t
    POGI_GPS_t gpsCoordinatesCommand = { 
        rawData->gpsLatitude, 
        rawData->gpsLongitude, 
        rawData->curAltitude 
    };

    // one_byte_uint_cmd_t
    one_byte_uint_cmd_t errorCodeCommand = { rawData->errorCode };
    one_byte_uint_cmd_t flightPathErrorCodeCommand = { rawData->editingFlightPathErrorCode };
    one_byte_uint_cmd_t pathFollowErrorCodeCommand = { rawData->flightPathFollowingErrorCode };

    // Encode
    // const uint8_t * arr = static_cast<const uint8_t*>(static_cast<void*>(&planeAngleCommand));
    mavlink_encoding_status_t e1 = Mavlink_airside_encoder(MESSAGE_ID_TIMESTAMP, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&timeStampCommand)));
    
    mavlink_encoding_status_t e2 = Mavlink_airside_encoder(MESSAGE_ID_AIR_SPEED, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&airspeedCommand)));
    mavlink_encoding_status_t e3 = Mavlink_airside_encoder(MESSAGE_ID_CURRENT_WAYPOINT_ID, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&waypointIdCommand)));
    mavlink_encoding_status_t e4 = Mavlink_airside_encoder(MESSAGE_ID_CURRENT_WAYPOINT_INDEX, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&waypointIndexCommand)));

    mavlink_encoding_status_t e5 = Mavlink_airside_encoder(MESSAGE_ID_IS_LANDED, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&isLandedCommand)));
    mavlink_encoding_status_t e6 = Mavlink_airside_encoder(MESSAGE_ID_HOMEBASE_INITIALIZED, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&isHomebaseInitializedCommand)));
    mavlink_encoding_status_t e7 = Mavlink_airside_encoder(MESSAGE_ID_EULER_ANGLE_PLANE, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&planeAngleCommand)));
    mavlink_encoding_status_t e8 = Mavlink_airside_encoder(MESSAGE_ID_EULER_ANGLE_CAM, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&cameraAngleCommand)));

    mavlink_encoding_status_t e9 = Mavlink_airside_encoder(MESSAGE_ID_GPS, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&gpsCoordinatesCommand)));

    mavlink_encoding_status_t e10 = Mavlink_airside_encoder(MESSAGE_ID_ERROR_CODE, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&errorCodeCommand)));
    mavlink_encoding_status_t e11 = Mavlink_airside_encoder(MESSAGE_ID_EDITING_FLIGHT_PATH_ERROR_CODE, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&flightPathErrorCodeCommand)));
    mavlink_encoding_status_t e12 = Mavlink_airside_encoder(MESSAGE_ID_FLIGHT_PATH_FOLLOWING_ERROR_CODE, encodedData, static_cast<const uint8_t*>(static_cast<void*>(&pathFollowErrorCodeCommand)));

    if (e1 != MAVLINK_ENCODING_OKAY || e2 != MAVLINK_ENCODING_OKAY || e3 != MAVLINK_ENCODING_OKAY || e4 != MAVLINK_ENCODING_OKAY || e5 != MAVLINK_ENCODING_OKAY || e6 != MAVLINK_ENCODING_OKAY || e7 != MAVLINK_ENCODING_OKAY || e8 != MAVLINK_ENCODING_OKAY || e9 != MAVLINK_ENCODING_OKAY || e10 != MAVLINK_ENCODING_OKAY || e11 != MAVLINK_ENCODING_OKAY || e12 != MAVLINK_ENCODING_OKAY) {
        // cout << (bool) (e1 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e2 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e3 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e4 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e5 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e6 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e7 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e8 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e9 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e10 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e11 != MAVLINK_ENCODING_OKAY) << " " << (bool) (e12 != MAVLINK_ENCODING_OKAY) << endl; 
        // cout << (bool) (e1 == MAVLINK_ENCODING_OKAY) << " " << (bool) (e1 == MAVLINK_ENCODING_INCOMPLETE) << " " << (bool) (e1 == MAVLINK_ENCODING_FAIL) << " " << (bool) (e1 == MAVLINK_ENCODING_BAD_ID) << endl;


        return MAVLINK_DECODING_ENCODING_FAILED;
    }

    return MAVLINK_DECODING_ENCODING_OK;
}

