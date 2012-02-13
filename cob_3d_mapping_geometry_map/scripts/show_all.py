#!/usr/bin/python
import sys
import roslib
roslib.load_manifest('cob_3d_mapping_gazebo')

import rospy
import os

from gazebo.srv import *
from simple_script_server import script

class MyScript(script):
	def Run(self):
#		self.sss.move("torso",[[-0.2,0,-0.2]])
#		self.sss.move("head","front")
#		self.sss.move("tray","down")
		# Move training table on top of gripper
		srv_set_model_state = rospy.ServiceProxy('/gazebo/set_model_state', SetModelState)
		req_set = SetModelStateRequest()
		req_set.model_state.model_name = "robot"
#		req_set.model_state.pose.position.x = -2.5
#		req_set.model_state.pose.position.y = 0
#		req_set.model_state.pose.position.z = 0
#		req_set.model_state.pose.orientation.w = 0
#		req_set.model_state.pose.orientation.x = 0
#		req_set.model_state.pose.orientation.y = 0
#		req_set.model_state.pose.orientation.z = 0
		req_set.model_state.reference_frame = "map"	
#		res_set = srv_set_model_state(req_set)
#		self.sss.wait_for_input()
#		req_set.model_state.pose.position.x = -2.17
#		req_set.model_state.pose.position.y = 1.25
#		req_set.model_state.pose.position.z = 0
#		req_set.model_state.pose.orientation.w = 0.965925826
#		req_set.model_state.pose.orientation.x = 0
#		req_set.model_state.pose.orientation.y = 0
#		req_set.model_state.pose.orientation.z = -0.258819045
#		res_set = srv_set_model_state(req_set)

		req_set.model_state.pose.position.x = -2.5
		req_set.model_state.pose.position.y = 0
		req_set.model_state.pose.position.z = 0
		req_set.model_state.pose.orientation.w = 0
		req_set.model_state.pose.orientation.x = 0
		req_set.model_state.pose.orientation.y = 0
		req_set.model_state.pose.orientation.z =  1
		res_set = srv_set_model_state(req_set)
		self.sss.wait_for_input()
		req_set.model_state.pose.position.x = 0
		req_set.model_state.pose.position.y = -2.5
		req_set.model_state.pose.position.z = 0
		req_set.model_state.pose.orientation.w = 0.7071067812
		req_set.model_state.pose.orientation.x = 0
		req_set.model_state.pose.orientation.y = 0
		req_set.model_state.pose.orientation.z =  -0.7071067812
		res_set = srv_set_model_state(req_set)
		self.sss.wait_for_input()
		req_set.model_state.pose.position.x = 2.5
		req_set.model_state.pose.position.y = 0
		req_set.model_state.pose.position.z = 0
		req_set.model_state.pose.orientation.w = 0
		req_set.model_state.pose.orientation.x = 0
		req_set.model_state.pose.orientation.y = 0
		req_set.model_state.pose.orientation.z = 0
		res_set = srv_set_model_state(req_set)
		self.sss.wait_for_input()
		req_set.model_state.pose.position.x = 4
		req_set.model_state.pose.position.y = 1
		req_set.model_state.pose.position.z = 0
		req_set.model_state.pose.orientation.w = 0
		req_set.model_state.pose.orientation.x = 0
		req_set.model_state.pose.orientation.y = 0
		req_set.model_state.pose.orientation.z = 0
		res_set = srv_set_model_state(req_set)
		self.sss.wait_for_input()
		req_set.model_state.pose.position.x = 0
		req_set.model_state.pose.position.y = 3
		req_set.model_state.pose.position.z = 0
		req_set.model_state.pose.orientation.w = 0.7071067812
		req_set.model_state.pose.orientation.x = 0
		req_set.model_state.pose.orientation.y = 0
		req_set.model_state.pose.orientation.z = 0.7071067812
		res_set = srv_set_model_state(req_set)

		
if __name__ == "__main__":
	SCRIPT = MyScript()
	SCRIPT.Start()
	#if len(sys.argv) < 2:
	#	print '[Train_LoadObject.py] Please specify the name of the urdf model to be loaded'
	#	sys.exit()
