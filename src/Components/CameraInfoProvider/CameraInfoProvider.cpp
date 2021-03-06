/*!
* \file
* \brief
* \author Maciej Stefańczyk [maciek.slon@gmail.com]
*/

#include <memory>
#include <string>

#include "CameraInfoProvider.hpp"
#include "Common/Logger.hpp"

#include <boost/bind.hpp>

namespace Processors {
namespace CameraInfoProvider {

CameraInfoProvider::CameraInfoProvider(const std::string & name) :
Base::Component(name),
width("width", 640),
height("height", 480),
camera_matrix("camera_matrix", cv::Mat(cv::Mat::eye(3, 3, CV_32FC1))),
        dist_coeffs("dist_coeffs", cv::Mat(cv::Mat::ones(1, 5, CV_32FC1))),
        rectificaton_matrix("rectificaton_matrix",cv::Mat(cv::Mat::eye(3,3,CV_32FC1))),
        projection_matrix("projection_matrix",cv::Mat(cv::Mat::zeros(3,4,CV_32FC1))),
        rotation_matrix("rotation_matrix",cv::Mat(cv::Mat::eye(3,3,CV_32FC1))),
        translation_matrix("translation_matrix",cv::Mat(cv::Mat::zeros(3,1,CV_32FC1))),
        //data_file("data_file", string("~/stereo/camera_info.yml"))
        data_file("data_file", string(""))
{
width.addConstraint("0");
width.addConstraint("1280");
registerProperty(width);

height.addConstraint("0");
height.addConstraint("1280");
registerProperty(height);

registerProperty(camera_matrix);
registerProperty(dist_coeffs);
    registerProperty(rectificaton_matrix);
    registerProperty(projection_matrix);
    registerProperty(rotation_matrix);
    registerProperty(translation_matrix);
    registerProperty(data_file);
}

CameraInfoProvider::~CameraInfoProvider() {

}

void CameraInfoProvider::prepareInterface() {
// Register data streams, events and event handlers HERE!
// Register data streams
registerStream("out_camera_info", &out_camerainfo);
registerStream("in_camera_info", &in_camerainfo);

// Register handlers
    //"Generate data" handler
h_generate_data.setup(boost::bind(&CameraInfoProvider::generate_data, this));
registerHandler("generate_data", &h_generate_data);
addDependency("generate_data", NULL);

    //"Reload file" handler
    h_reload_file.setup(boost::bind(&CameraInfoProvider::reload_file, this));
    registerHandler("reload_file", &h_reload_file);
//    addDependency("reload_file", NULL);

    //"Update params" handler
h_update_params.setup(boost::bind(&CameraInfoProvider::update_params, this));
registerHandler("update_params", &h_update_params);
addDependency("update_params", &in_camerainfo);

CLOG(LNOTICE) << "CameraMatrix: " << camera_matrix;
}

bool CameraInfoProvider::onInit() {
      if(data_file!="")
      {
	    CLOG(LINFO) <<"reload_file";
	    reload_file();
      }
      
return true;
}

bool CameraInfoProvider::onFinish() {
return true;
}

bool CameraInfoProvider::onStop() {
return true;
}

bool CameraInfoProvider::onStart() {
return true;
}

void CameraInfoProvider::generate_data() {
    LOG(LDEBUG) << "setWidth";
camera_info.setWidth(width);
    LOG(LDEBUG) << "setHeight";
camera_info.setHeight(height);
    LOG(LDEBUG) << "setCameraMatrix";
camera_info.setCameraMatrix(camera_matrix);
    LOG(LDEBUG) << "setDistCoeffs";
camera_info.setDistCoeffs(dist_coeffs);
    LOG(LDEBUG) << "setRectificationMatrix";
    camera_info.setRectificationMatrix(rectificaton_matrix);
    LOG(LDEBUG) << "setProjectionMatrix";
    camera_info.setProjectionMatrix(projection_matrix);
    LOG(LDEBUG) << "setRotationMatrix";
    camera_info.setRotationMatrix(rotation_matrix);
    LOG(LDEBUG) << "setTranlationMatrix";
    camera_info.setTranlationMatrix(translation_matrix);
    LOG(LDEBUG) << "write";
out_camerainfo.write(camera_info);
}

void CameraInfoProvider::update_params() {
Types::CameraInfo camera_info = in_camerainfo.read();
width = camera_info.width();
height = camera_info.height();
camera_matrix = camera_info.cameraMatrix();
dist_coeffs = camera_info.distCoeffs();
    rectificaton_matrix = camera_info.rectificationMatrix();
    projection_matrix = camera_info.projectionMatrix();
    rotation_matrix = camera_info.rotationMatrix();
    translation_matrix = camera_info.translationMatrix();
}

void CameraInfoProvider::reload_file() {
    LOG(LDEBUG) << "Loading from " << data_file;
    cv::FileStorage fs(data_file, cv::FileStorage::READ);
    cv::Mat oTempMat;
    try {
        fs["M"] >> oTempMat;
        camera_matrix = oTempMat;
    } catch (...)
    {
        LOG(LWARNING) << "No camera matrix in " << data_file;
    }
    try {
        fs["D"] >> oTempMat;
        dist_coeffs = oTempMat;
    } catch (...)
    {
        LOG(LWARNING) << "No distortion coefficients in " << data_file;
    }
    try {
        fs["R"] >> oTempMat;
        rectificaton_matrix = oTempMat;
    } catch (...)
    {
        LOG(LWARNING) << "No rectificaton matrix in " << data_file;
    }
    try {
        fs["P"] >> oTempMat;
        projection_matrix = oTempMat;
    } catch (...)
    {
        LOG(LWARNING) << "No projection matrix in " << data_file;
    }
    try {
        fs["ROT"] >> oTempMat;
        rotation_matrix = oTempMat;
    } catch (...)
    {
        LOG(LWARNING) << "No rotation matrix in " << data_file;
    }
    try {
        fs["T"] >> oTempMat;
        translation_matrix = oTempMat;
    } catch (...)
    {
        LOG(LWARNING) << "No translation matrix in " << data_file;
    }
    fs.release();
}



} //: namespace CameraInfoProvider
} //: namespace Processors