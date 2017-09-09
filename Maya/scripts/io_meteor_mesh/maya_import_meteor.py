# -- coding: utf-8 --

import os
import sys
import math
import re

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
bone_pivot_list = []
	
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
			bone_pivot_list.append([bone_info.bone_pivotX, bone_info.bone_pivotY, bone_info.bone_pivotZ])
			
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
		

def parse_amb_value(line, num):
	value_strs = re.split("=|,|\n|\r",strip_space_line(line))
	value_arr = []
	for n in range (0, num):
		value_arr.append(float(value_strs[n - num]))
	return value_arr
	
def read_amb_file(amb_path):
	file = open(amb_path, "r")
	
	line = file.readline()
	num_bones = parse_amb_value(file.readline(), 1)
	num_dummey = parse_amb_value(file.readline(), 1)
	num_frame = parse_amb_value(file.readline(), 1)
	unknow = parse_amb_value(file.readline(), 1)
	
	while True:
		line = file.readline()
		if not line:
			break;
		
		if line.find("frame start flag") != -1:
			line = file.readline()
			if line.find("frame index") != -1:
				frame_index = int(line.split("=")[-1])
				# base offset
				vec_offset = parse_amb_value(file.readline(), 3)
				bone_quat = []
				dummey_vec_offset = []
				dummey_quat = []
				
				# bone quat
				for bi in range(0, 30):
					quat = parse_amb_value(file.readline(), 4)
					bone_quat.append(quat)
				
				# dummey info
				for di in range(0, 6):
					line = file.readline()
					d_vec_offset = parse_amb_value(file.readline(), 3)
					d_quat = parse_amb_value(file.readline(), 4)
					dummey_vec_offset.append(d_vec_offset)
					dummey_quat.append(d_quat)
					
				#print bone_list
				for i in range(0, 36):
					bone = bone_list[i]
					if bone.bone_type == "bone":
						b_quat = bone_quat[i]
						rotateQuat = pmdt.Quaternion(b_quat[1], b_quat[2], b_quat[3], b_quat[0])
						rotateEuler = rotateQuat.asEulerRotation()
						maya_mat = rotateEuler.asMatrix()
						
						if bone.bone_name == "b":
							maya_mat.a30 = vec_offset[0]
							maya_mat.a31 = vec_offset[1]
							maya_mat.a32 = vec_offset[2]
						else:
							maya_mat.a30 = bone.bone_pivotX
							maya_mat.a31 = bone.bone_pivotY
							maya_mat.a32 = bone.bone_pivotZ
						
						pmc.currentTime(int(frame_index))
						pmc.select(bone.bone_name)
						pmc.xform(matrix = maya_mat)
						pmc.setKeyframe()
					elif bone.bone_type == "Dummey":
						d_vec_offset =  dummey_vec_offset[int(i - 30)]
						d_quat = dummey_quat[int(i-30)]
						
						rotateQuat = pmdt.Quaternion(d_quat[1], d_quat[2], d_quat[3], d_quat[0])
						rotateEuler = rotateQuat.asEulerRotation()
						maya_mat = rotateEuler.asMatrix()
						maya_mat.a30 = d_vec_offset[0]
						maya_mat.a31 = d_vec_offset[1]
						maya_mat.a32 = d_vec_offset[2]
						
						pmc.currentTime(int(frame_index))
						pmc.select(bone.bone_name)
						pmc.xform(matrix = maya_mat)
						pmc.setKeyframe()
					
				print "Frame Index {0} succed".format(frame_index)

				
def parse_mesh_element(str):
	str = strip_space_line(str)
	
	new_str = ""
	last_a = ""
	for i in range(0, len(str)):
		if str[i] != " ":
			new_str += str[i]
		else:
			if last_a != " ":
				new_str += " "
		last_a = str[i]
	
	split_str = new_str.split(" ")
	vals = []
	for i in range(0, len(split_str)):
		vals.append(float(split_str[i]))
		
	return vals
	
# parse vertex 
#  v   2.87744   0.99344  25.20086 vt 0.418 0.622 Bones 2 11 0.83333  2 0.16667		
def parse_pos_uv_weight(line):
	line = line.replace("vt", "tx")
	vertex_line = re.split("v|tx|Bones",line)
	vertex_data = []
	
	vertex_data.append(parse_mesh_element(vertex_line[1]))
	vertex_data.append(parse_mesh_element(vertex_line[2]))
	vertex_data.append(parse_mesh_element(vertex_line[3]))
	
	return vertex_data
	
def parse_face(line):
	face_line = line[1:]
	face_data = []
	face_data = parse_mesh_element(face_line)
	return face_data
	
def read_skc_file(skc_path):
	file = open(skc_path, "r")
	
	line = file.readline()
	
	while True:
		line = file.readline()
		if not line:
			break;
			
		line = strip_space_line(line)
			
		if line.find("Vertices") != -1:
			print line
		elif line[0] == "v":
			vertex_data = parse_pos_uv_weight(line)
			print vertex_data
		elif line.find("Triangles") != -1:
			print line
		elif line[0] == "f":
			print parse_face(line)
			
# main()	
if __name__ == "__main__":
	# read bnc
	file_path = "D:\\Projects\\Meteor\\Maya\\assets\\P{0}.bnc".format(int(1))
	#bone_set = "_P{0}".format(p)
	read_bnc_file(file_path, "")
	print "import bnc succed"
	
	# read amb
	#amb_path = "D:\\Projects\\Meteor\\Maya\\assets\\p0.amb.txt"
	#amb_path = "D:\\Projects\\Meteor\\Maya\\assets\\character.amb.txt"
	#read_amb_file(amb_path)
	
	# read skc
	skc_path = "D:\\Projects\\Meteor\\Maya\\assets\\p0.skc"
	read_skc_file(skc_path)
	
	
	
	
	
	
	









