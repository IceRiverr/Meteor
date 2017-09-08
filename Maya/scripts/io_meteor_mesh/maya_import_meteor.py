# -- coding: utf-8 --

import os
import sys
import math

import pymel.core as pmc
import pymel.core.datatypes as pmdt

# 去除空格和换行符号
def strip_space_line(in_str):
	return in_str.strip().replace("\n", "")

	
def str2float(in_str):
	return float(strip_space_line(in_str))
	
	
def split_space(in_str):
	result = []
	while True:
		in_str = strip_space_line(in_str)
		pos = in_str.find(" ")
		
		if pos == -1:
			break;
			
		result.append(in_str[0:pos])
		in_str = in_str[pos+1:]
		
	result.append(in_str)
	return result
	
	
class BoneInfo(object):
	bone_type = ""
	bone_name = ""
	bone_parent = ""
	bone_pivotX = 0
	bone_pivotY = 0
	bone_pivotZ = 0
	bone_quatX = 0
	bone_quatY = 0
	bone_quatZ = 0
	bone_quatW = 0
	bone_child = 0
	
bone_list = []
bone_name_list = []
	
num_bones = 0
num_dummey = 0

def read_bnc_file(file_path, bone_set):
	file = open(file_path, "r")
	
	while True:
		line = file.readline()
		if not line:
			break;
		
		# 跳过注释
		if line[0] == "#":
			continue;
		
		# 获取总数
		if line.find("Bones") != -1:
			line_string = line.split(" ")
			num_bones = str2float(line_string[1])
			num_dummey = str2float(line_string[3])
			continue;
			
		# 读取一个骨骼数据
		"""
		->bone b
		{
			parent d_base
			pivot  0.000  0.000 20.945
			quaternion -0.70711 0.00000 0.00000 0.70711
			children 1
		}
		Dummey d_wpnLP
		{
			parent b17
			pivot   2.146  0.349 -0.781
			quaternion -0.70282 -0.71104 0.02014 -0.00843
		}
		"""
		isBond = False
		isDummey = False
		
		if line.find("bone") != -1:
			isBond = True
		if line.find("Dummey") != -1:
			isDummey = True
		
		if isBond or isDummey :
			bone_info = BoneInfo()
			if isBond:
				bone_info.bone_type = "bone"
			if isDummey:
				bone_info.bone_type = "Dummey"
			
			line_string = line.split(" ")
			bone_info.bone_name =  strip_space_line(line_string[1])
			bone_info.bone_name += bone_set
			
			line = file.readline()
			if line.find("{") == -1:
				print "ERROR"
				
			line = file.readline()
			line_string = line.split(" ")
			bone_info.bone_parent = strip_space_line(line_string[-1])
			bone_info.bone_parent += bone_set
			
			line = file.readline()
			line_string = split_space(line)
			bone_info.bone_pivotX = str2float(line_string[-3])
			bone_info.bone_pivotY = str2float(line_string[-2])
			bone_info.bone_pivotZ = str2float(line_string[-1])
			
			# bnc 中的四元数 格式为 wxyz 需要转为 xyzw
			line = file.readline()
			line_string = split_space(line)
			bone_info.bone_quatX = str2float(line_string[-3])
			bone_info.bone_quatY = str2float(line_string[-2])
			bone_info.bone_quatZ = str2float(line_string[-1])
			bone_info.bone_quatW = str2float(line_string[-4])
			
			if isBond:
				line = file.readline()
				line_string = split_space(line)
				bone_info.bone_child = str2float(line_string[-1])
			if isDummey:
				bone_info.bone_child = -1
			
			line = file.readline()
			if line.find("}") == -1:
				print "ERROR"
				
			bone_name_list.append(bone_info.bone_name)
			bone_list.append(bone_info)
			
	file.close()
	
	#create bone
	
	#print bone_name_list
	for i in range(int(num_bones), int(num_bones + num_dummey)):
		pre_leter = bone_list[i].bone_parent.split("_")[0][0]
		parent_index = bone_list[i].bone_parent.split("_")[0][1:]
		if pre_leter == "b":
			bone_list[i].bone_parent = bone_name_list[int(float(parent_index))]
		if pre_leter == "d":
			bone_list[i].bone_parent = bone_name_list[int(float(parent_index) + num_bones)]
			
	group = pmc.group(em=True, n = "NULL" + bone_set)
	pmc.rotate(0, -90.0, 0)
	
	for bone in bone_list:
		if bone.bone_type == "bone":
			new_joint = pmc.joint(n = bone.bone_name)
			pmc.parent(bone.bone_name, world=True)
			radiusAttr = bone.bone_name + ".radius"
			pmc.setAttr(radiusAttr, 0.5)
		elif bone.bone_type == "Dummey":
			new_dummey = pmc.group(em=True, n = bone.bone_name)
			pmc.parent(new_dummey, world=True)
	
	for bone in bone_list:
		pmc.parent(bone.bone_name, bone.bone_parent)
		
	for bone in bone_list:
		pmc.select(bone.bone_name)
		
		rotateQuat = pmdt.Quaternion(bone.bone_quatX, bone.bone_quatY, bone.bone_quatZ, bone.bone_quatW)
		rotateEuler = rotateQuat.asEulerRotation()
		maya_mat = rotateEuler.asMatrix()
		maya_mat.a30 = bone.bone_pivotX
		maya_mat.a31 = bone.bone_pivotY
		maya_mat.a32 = bone.bone_pivotZ
		
		pmc.xform(matrix = maya_mat)
		
		#pmc.parent(bone.bone_name, world=True)
		#pmc.connectJoint(bone.bone_name, bone.bone_parent, parentMode=True)
		pmc.select(clear = True)
		
# main()	
if __name__ == "__main__":
	# read bnc
	file_path = "D:\\Projects\\Meteor\\Maya\\assets\\P{0}.bnc".format(int(1))
	bone_set = "_P{0}".format(p)
	read_bnc_file(file_path, "")
		
	print "import bnc succed"
	
	# read amb
	
	
	
	
	
	
	
	
	
	









