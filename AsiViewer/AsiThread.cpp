
///////////////////
// AsiThread.cpp //
///////////////////

#include <QDebug>
#include <QTimer>

#include <string>
#include <stdexcept>

#include "ASICamera2.h"

#include "AsiThread.h"

const char * toString(const ASI_BAYER_PATTERN bayerPattern)
{
    switch (bayerPattern)
    {
        case ASI_BAYER_RG : return "ASI_BAYER_RG";
        case ASI_BAYER_BG : return "ASI_BAYER_BG";
        case ASI_BAYER_GR : return "ASI_BAYER_GR";
        case ASI_BAYER_GB : return "ASI_BAYER_GB";
    }
    return "unknown";
}

const char * toString(const ASI_IMG_TYPE imageType)
{
    switch (imageType)
    {
        case ASI_IMG_RAW8  : return "ASI_IMG_RAW8";
        case ASI_IMG_RGB24 : return "ASI_IMG_RGB24";
        case ASI_IMG_RAW16 : return "ASI_IMG_RAW16";
        case ASI_IMG_Y8    : return "ASI_IMG_Y8";
        case ASI_IMG_END   : return "ASI_IMG_END";
    }
    return "unknown";
}

const char * toString(const ASI_GUIDE_DIRECTION direction)
{
    switch (direction)
    {
        case ASI_GUIDE_NORTH : return "ASI_GUIDE_NORTH";
        case ASI_GUIDE_SOUTH : return "ASI_GUIDE_SOUTH";
        case ASI_GUIDE_EAST  : return "ASI_GUIDE_EAST";
        case ASI_GUIDE_WEST  : return "ASI_GUIDE_WEST";
    }
    return "unknown";
}

const char * toString(const ASI_FLIP_STATUS flipStatus)
{
    switch (flipStatus)
    {
        case ASI_FLIP_NONE  : return "ASI_FLIP_NONE";
        case ASI_FLIP_HORIZ : return "ASI_FLIP_HORIZ";
        case ASI_FLIP_VERT  : return "ASI_FLIP_VERT";
        case ASI_FLIP_BOTH  : return "ASI_FLIP_BOTH";
    }
    return "unknown";
}

const char * toString(const ASI_ERROR_CODE errorcode)
{
    switch (errorcode)
    {
        case ASI_SUCCESS                    : return "ASI_SUCCESS";
        case ASI_ERROR_INVALID_INDEX        : return "ASI_ERROR_INVALID_INDEX";
        case ASI_ERROR_INVALID_ID           : return "ASI_ERROR_INVALID_ID";
        case ASI_ERROR_INVALID_CONTROL_TYPE : return "ASI_ERROR_INVALID_CONTROL_TYPE";
        case ASI_ERROR_CAMERA_CLOSED        : return "ASI_ERROR_CAMERA_CLOSED";
        case ASI_ERROR_CAMERA_REMOVED       : return "ASI_ERROR_CAMERA_REMOVED";
        case ASI_ERROR_INVALID_PATH         : return "ASI_ERROR_INVALID_PATH";
        case ASI_ERROR_INVALID_FILEFORMAT   : return "ASI_ERROR_INVALID_FILEFORMAT";
        case ASI_ERROR_INVALID_SIZE         : return "ASI_ERROR_INVALID_SIZE";
        case ASI_ERROR_INVALID_IMGTYPE      : return "ASI_ERROR_INVALID_IMGTYPE";
        case ASI_ERROR_OUTOF_BOUNDARY       : return "ASI_ERROR_OUTOF_BOUNDARY";
        case ASI_ERROR_TIMEOUT              : return "ASI_ERROR_TIMEOUT";
        case ASI_ERROR_INVALID_SENQUENCE    : return "ASI_ERROR_INVALID_SENQUENCE";
        case ASI_ERROR_BUFFER_TOO_SMALL     : return "ASI_ERROR_BUFFER_TOO_SMALL";
        case ASI_ERROR_VIDEO_MODE_ACTIVE    : return "ASI_ERROR_VIDEO_MODE_ACTIVE";
        case ASI_ERROR_EXPOSURE_IN_PROGRESS : return "ASI_ERROR_EXPOSURE_IN_PROGRESS";
        case ASI_ERROR_GENERAL_ERROR        : return "ASI_ERROR_GENERAL_ERROR";
        case ASI_ERROR_END                  : return "ASI_ERROR_END";
    }
    return "unknown";
}

const char * toString(const ASI_BOOL boolean)
{
    switch (boolean)
    {
        case ASI_FALSE : return "ASI_FALSE";
        case ASI_TRUE  : return "ASI_TRUE";
    }
    return "unknown";
}

const char * toString(const ASI_CONTROL_TYPE controlType)
{
    switch (controlType)
    {
        case ASI_GAIN                : return "ASI_GAIN";
        case ASI_EXPOSURE            : return "ASI_EXPOSURE";
        case ASI_GAMMA               : return "ASI_GAMMA";
        case ASI_WB_R                : return "ASI_WB_R";
        case ASI_WB_B                : return "ASI_WB_B";
        case ASI_BRIGHTNESS          : return "ASI_BRIGHTNESS";
        case ASI_BANDWIDTHOVERLOAD   : return "ASI_BANDWIDTHOVERLOAD";
        case ASI_OVERCLOCK           : return "ASI_OVERCLOCK";
        case ASI_TEMPERATURE         : return "ASI_TEMPERATURE";
        case ASI_FLIP                : return "ASI_FLIP";
        case ASI_AUTO_MAX_GAIN       : return "ASI_AUTO_MAX_GAIN";
        case ASI_AUTO_MAX_EXP        : return "ASI_AUTO_MAX_EXP";
        case ASI_AUTO_MAX_BRIGHTNESS : return "ASI_AUTO_MAX_BRIGHTNESS";
        case ASI_HARDWARE_BIN        : return "ASI_HARDWARE_BIN";
        case ASI_HIGH_SPEED_MODE     : return "ASI_HIGH_SPEED_MODE";
        case ASI_COOLER_POWER_PERC   : return "ASI_COOLER_POWER_PERC";
        case ASI_TARGET_TEMP         : return "ASI_TARGET_TEMP";
    }
    return "unknown";
}

const char * toString(const ASI_EXPOSURE_STATUS exposureStatus)
{
    switch (exposureStatus)
    {
        case ASI_EXP_IDLE             : return "ASI_EXP_IDLE";
        case ASI_EXP_WORKING          : return "ASI_EXP_WORKING";
        case ASI_EXP_SUCCESS          : return "ASI_EXP_SUCCESS";
        case ASI_EXP_FAILED           : return "ASI_EXP_FAILED";
    }
    return "unknown";
}

int bytes_per_pixel(const ASI_IMG_TYPE img_type)
{
    switch (img_type)
    {
        case ASI_IMG_RAW8  : return 1;
        case ASI_IMG_RGB24 : return 3;
        case ASI_IMG_RAW16 : return 2;
        case ASI_IMG_Y8    : return 1;
        default :
        {
            const std::string message = std::string("bytes_per_pixel failed; img_type = ") + std::to_string(img_type) + " (" + toString(img_type) + ")";
            throw std::runtime_error(message);
        }
    }
}

// Check the error code as returned by an ASI function.
// Throw exception if the call was unsuccesful.

void check_errorcode(ASI_ERROR_CODE errorcode)
{
    if (errorcode != ASI_SUCCESS)
    {
        const std::string message = std::string("check_errorcode failed; errorcode = ") + std::to_string(errorcode) + " (" + toString(errorcode) + ")";
        throw std::runtime_error(message);
    }
}

AsiThread::AsiThread(QObject * parent) : QThread(parent)
{
    qDebug() << __PRETTY_FUNCTION__;
}

AsiThread::~AsiThread()
{
    qDebug() << __PRETTY_FUNCTION__;
    quitGetImageLoop = true; // request that the image acquisition loop is stopped.
    wait();
}

void AsiThread::run()
{
    const int PIX_WIDTH  = 1280;
    const int PIX_HEIGHT =  960;
    const int BINNING    =    1;
    const ASI_IMG_TYPE IMAGE_TYPE = ASI_IMG_RAW16;

    qDebug() << __PRETTY_FUNCTION__;

    int numberOfConnectedCameras = ASIGetNumOfConnectedCameras();
    qDebug() << numberOfConnectedCameras;

    Q_ASSERT(numberOfConnectedCameras > 0);

    ASI_ERROR_CODE errorcode;
    ASI_CAMERA_INFO cameraInfo;

    // This is done without opening the camera.
    // The ASIGetCameraProperty() call must be preceded by ASIGetNumOfConnectedCameras().

    errorcode = ASIGetCameraProperty(&cameraInfo, 0);
    check_errorcode(errorcode);

    errorcode = ASIOpenCamera(cameraInfo.CameraID);
    check_errorcode(errorcode);

    errorcode = ASISetControlValue(cameraInfo.CameraID, ASI_EXPOSURE, 20000, ASI_FALSE);
    check_errorcode(errorcode);

    errorcode = ASISetROIFormat(cameraInfo.CameraID, PIX_WIDTH, PIX_HEIGHT, BINNING, IMAGE_TYPE);
    check_errorcode(errorcode);

    errorcode = ASIStartVideoCapture(cameraInfo.CameraID);
    check_errorcode(errorcode);

    quitGetImageLoop = false;
    while (!quitGetImageLoop)
    {
        getImage(cameraInfo.CameraID);
    }

    errorcode = ASIStopVideoCapture(cameraInfo.CameraID);
    check_errorcode(errorcode);

    errorcode = ASICloseCamera(cameraInfo.CameraID);
    check_errorcode(errorcode);
}

void AsiThread::getImage(const int CameraID)
{
    const int PIX_WIDTH  = 1280;
    const int PIX_HEIGHT =  960;
    const ASI_IMG_TYPE IMAGE_TYPE = ASI_IMG_RAW16;
    const int BYTES_PER_PIXEL = bytes_per_pixel(IMAGE_TYPE);

    qDebug() << __PRETTY_FUNCTION__;

    QByteArray byteArray(PIX_HEIGHT * PIX_WIDTH * BYTES_PER_PIXEL, 0);
    ASI_ERROR_CODE errorcode = ASIGetVideoData(CameraID, reinterpret_cast<unsigned char *>(byteArray.data()), byteArray.size(), -1);
    check_errorcode(errorcode);

    emit receivedImage(byteArray);
}
