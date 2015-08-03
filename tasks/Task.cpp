/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"

using namespace imaging_sonar_simulation;

Task::Task(std::string const& name) :
		TaskBase(name) {
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine) :
		TaskBase(name, engine) {
}

Task::~Task() {
}

bool Task::setNumber_of_bins(boost::int32_t value) {
	return (imaging_sonar_simulation::TaskBase::setNumber_of_bins(value));
}

bool Task::setRange(double value) {
	return (imaging_sonar_simulation::TaskBase::setRange(value));
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

bool Task::configureHook() {
	if (!TaskBase::configureHook())
		return false;
	return true;
}
bool Task::startHook() {
	if (!TaskBase::startHook())
		return false;
	return true;
}
void Task::updateHook() {
	TaskBase::updateHook();
}
void Task::errorHook() {
	TaskBase::errorHook();
}
void Task::stopHook() {
	TaskBase::stopHook();
}
void Task::cleanupHook() {
	TaskBase::cleanupHook();
}

void Task::init(float fovX, float fovY, uint value, float range, bool isHeight) {

	// set vizkit3d_world
	vizkit3dWorld->setCameraParams(320, 240, 45, 0.1, 100.0);
	vizkit3dWorld->getWidget()->setTransformer(false);
	vizkit3dWorld->getWidget()->setAxes(false);
	vizkit3dWorld->getWidget()->setAxesLabels(false);

	// init shader
	_normal_depth_map = vizkit3d_normal_depth_map::NormalDepthMap(range);
	_capture = vizkit3d_normal_depth_map::ImageViewerCaptureTool(fovY, fovX, value, isHeight);
	_root = vizkit3dWorld->getWidget()->getRootNode();
	_normal_depth_map.addNodeChild(_root);
}

void Task::updateCameraPose(base::samples::RigidBodyState cameraPose) {
	vizkit3dWorld->setTransformation(cameraPose);
	vizkit3dWorld->setCameraPose(cameraPose);
}

void Task::updateSonarPose(base::samples::RigidBodyState pose) {

	// convert OSG (Z-forward) to RoCK coordinate system (X-forward)
	osg::Matrixd rock_coordinate_matrix = osg::Matrixd::rotate( M_PI_2, osg::Vec3(0, 0, 1)) * osg::Matrixd::rotate(-M_PI_2, osg::Vec3(1, 0, 0));

	// transformation matrixes multiplication
	osg::Matrixd matrix;
	matrix.setTrans(osg::Vec3(pose.position.x(), pose.position.y(), pose.position.z()));
	matrix.setRotate(osg::Quat(pose.orientation.x(), pose.orientation.y(), pose.orientation.z(), pose.orientation.w()));
	matrix.invert(matrix);

	// correct coordinate system and apply geometric transformations
	osg::Matrixd m = matrix * rock_coordinate_matrix;

	osg::Vec3 eye, center, up;
	m.getLookAt(eye, center, up);
	_capture.setCameraPosition(eye, center, up);
}
