# -- coding: utf-8 --

import os
import sys
import math

import pymel.core as pmc
import pymel.core.datatypes as pmdt

def quat2matrix(x, y, z, w):
	matrix = [[0.0,0.0,0.0,0.0], [0.0,0.0,0.0,0.0], [0.0,0.0,0.0,0.0], [0.0,0.0,0.0,0.0]]
	
	xx = x * x
	yy = y * y
	zz = z * z
	ww = w * w
	xy = x * y
	yz = y * z
	zw = z * w
	xw = x * w
	yw = y * w
	xz = x * z
	
	matrix[0][0] = 2 * (xx + ww) - 1
	matrix[0][1] = 2 * (xy + zw)
	matrix[0][2] = 2 * (xz - yw)
	matrix[0][3] = 0
	
	matrix[1][0] = 2 * (xy - zw)
	matrix[1][1] = 2 * (yy + ww) - 1
	matrix[1][2] = 2 * (yz + xw)
	matrix[1][3] = 0
	
	matrix[2][0] = 2 * (xz + yw)
	matrix[2][1] = 2 * (yz - xw)
	matrix[2][2] = 2 * (zz + ww) - 1
	matrix[2][3] = 0
	
	matrix[3][0] = 0
	matrix[3][1] = 0
	matrix[3][2] = 0
	matrix[3][3] = 1
	
	return matrix
	
	
def quat2maya_matrix(x, y, z, w):
	matrix = pmdt.Matrix()
	
	xx = x * x
	yy = y * y
	zz = z * z
	ww = w * w
	xy = x * y
	yz = y * z
	zw = z * w
	xw = x * w
	yw = y * w
	xz = x * z
	
	matrix.a00 = 2.0 * (xx + ww) - 1.0
	matrix.a01 = 2.0 * (xy + zw)
	matrix.a02 = 2.0 * (xz - yw)
	matrix.a03 = 0.0
	
	matrix.a10 = 2.0 * (xy - zw)
	matrix.a11 = 2.0 * (yy + ww) - 1.0
	matrix.a12 = 2.0 * (yz + xw)
	matrix.a13 = 0.0
	
	matrix.a20 = 2.0 * (xz + yw)
	matrix.a21 = 2.0 * (yz - xw)
	matrix.a22 = 2.0 * (zz + ww) - 1.0
	matrix.a23 = 0.0
	
	matrix.a30 = 0.0
	matrix.a31 = 0.0
	matrix.a32 = 0.0
	matrix.a33 = 1.0
	
	return matrix
	
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
	
def read_bnc_fine(file_path, bone_set):
	file = open(file_path, "r")
	
	bone_list = []
	
	num_bones = 0
	num_dummey = 0
	parent_bone = BoneInfo()
	
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
			
			if(bone_info.bone_name == "d_base" + bone_set):
				parent_bone = bone_info
			else:
				bone_list.append(bone_info)
				
	file.close()
	
	#create bone
	
	# parent group
	group = pmc.group(em=True, name = parent_bone.bone_parent)
	pmc.rotate(0, -90.0, 0)
	
	new_joint = pmc.joint(a = True, p = (parent_bone.bone_pivotX, parent_bone.bone_pivotY, parent_bone.bone_pivotZ))
	pmc.select(new_joint)
	pmc.rename(new_joint, parent_bone.bone_name)
	pmc.parent(new_joint, world = True)
	new_joint.radius.set(0.25)
	
	rotateQuat = pmdt.Quaternion(parent_bone.bone_quatX, parent_bone.bone_quatY, parent_bone.bone_quatZ, parent_bone.bone_quatW)
	rotateEuler = rotateQuat.asEulerRotation()
	pmc.xform(a = True, ro = rotateEuler)
	pmc.select(clear=True)
	
	pmc.parent(parent_bone.bone_name, parent_bone.bone_parent)
	
	for i in range(0, int(num_bones)):
		new_bone = bone_list[i]
		
		new_joint = pmc.joint()
		pmc.select(new_joint)
		pmc.rename(new_joint, new_bone.bone_name)
		new_joint.radius.set(0.5)
		
		pmc.parent(new_bone.bone_name, new_bone.bone_parent)
		
		#maya_mat1 = quat2maya_matrix(new_bone.bone_quatX, new_bone.bone_quatY, new_bone.bone_quatZ, new_bone.bone_quatW)
		#maya_mat1.a30 = new_bone.bone_pivotX
		#maya_mat1.a31 = new_bone.bone_pivotY
		#maya_mat1.a32 = new_bone.bone_pivotZ
		
		rotateQuat = pmdt.Quaternion(new_bone.bone_quatX, new_bone.bone_quatY, new_bone.bone_quatZ, new_bone.bone_quatW)
		rotateEuler = rotateQuat.asEulerRotation()
		maya_mat2 = rotateEuler.asMatrix()
		maya_mat2.a30 = new_bone.bone_pivotX
		maya_mat2.a31 = new_bone.bone_pivotY
		maya_mat2.a32 = new_bone.bone_pivotZ
		
		#print "\t{"
		#print new_bone.bone_name
		#print maya_mat1
		#print maya_mat2
		#print "\t}"
		
		pmc.xform(matrix = maya_mat2)
		
		pmc.parent(new_bone.bone_name, world=True)
		pmc.connectJoint(new_bone.bone_name, new_bone.bone_parent, parentMode=True)
		
		pmc.select(clear = True)
		
# main()	
if __name__ == "__main__":
	for p in range(0, 20):
		file_path = "D:\\Projects\\Maya\\Meteor\\assets\\P{0}.bnc".format(int(p))
		bone_set = "_P{0}".format(p) 
		read_bnc_fine(file_path, bone_set)
	
	
	
	
	
	
	









