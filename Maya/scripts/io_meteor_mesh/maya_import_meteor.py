# -*- coding:utf-8 -*-  

import os
import sys
import math
import re

import pymel.core as pmc
import pymel.core.datatypes as pmdt
import maya.api.OpenMaya as OpenMaya

def to_PMatrix(mQuat):
	pMatrix = pmdt.Matrix()
	mMatrix = mQuat.asMatrix()
	pMatrix.a00 = mMatrix[0]
	pMatrix.a01 = mMatrix[1]
	pMatrix.a02 = mMatrix[2]
	pMatrix.a03 = mMatrix[3]
	pMatrix.a10 = mMatrix[4]
	pMatrix.a11 = mMatrix[5]
	pMatrix.a12 = mMatrix[6]
	pMatrix.a13 = mMatrix[7]
	pMatrix.a20 = mMatrix[8]
	pMatrix.a21 = mMatrix[9]
	pMatrix.a22 = mMatrix[10]
	pMatrix.a23 = mMatrix[11]
	pMatrix.a30 = mMatrix[12]
	pMatrix.a31 = mMatrix[13]
	pMatrix.a32 = mMatrix[14]
	pMatrix.a33 = mMatrix[15]
	return pMatrix

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
		if bone.bone_parent == "NULL":
			pmc.parent(bone.bone_name, world=True)
		else:
			pmc.parent(bone.bone_name, bone.bone_parent)
	
	for bone in bone_list:
		pmc.select(bone.bone_name)
		
		mQuat = OpenMaya.MQuaternion(bone.bone_quatX, bone.bone_quatY, bone.bone_quatZ, bone.bone_quatW)
		maya_mat = to_PMatrix(mQuat)
		maya_mat.a30 = bone.bone_pivotX
		maya_mat.a31 = bone.bone_pivotY
		maya_mat.a32 = bone.bone_pivotZ
		
		pmc.xform(matrix = maya_mat)
		pmc.select(clear = True)
		
	pmc.select( clear=True )
	

def parse_amb_value(line, num):
	value_strs = re.split("=|,|\n|\r",strip_space_line(line))
	value_arr = []
	for n in range (0, num):
		value_arr.append(float(value_strs[n - num]))
	return value_arr
	
def read_amb_file(amb_path, start_frame = -1, end_frame = -1):
	file = open(amb_path, "r")
	
	line = file.readline()
	num_bones = parse_amb_value(file.readline(), 1)
	num_dummey = parse_amb_value(file.readline(), 1)
	num_frame = parse_amb_value(file.readline(), 1)
	unknow = parse_amb_value(file.readline(), 1)
	
	if (start_frame == -1) and (end_frame == -1):
		start_frame = 0
		end_frame = num_frame
	
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
					
				if (frame_index >= start_frame) and (frame_index < end_frame):
					#print bone_list
					for i in range(0, 36):
						bone = bone_list[i]
						if bone.bone_type == "bone":
							b_quat = bone_quat[i]
							
							mQuat = OpenMaya.MQuaternion(b_quat[1], b_quat[2], b_quat[3], b_quat[0])
							maya_mat = to_PMatrix(mQuat)
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
							
							mQuat = OpenMaya.MQuaternion(d_quat[1], d_quat[2], d_quat[3], d_quat[0])
							maya_mat = to_PMatrix(mQuat)
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
	str = str.replace("\t", " ")
	
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
	
def read_skc_file(skc_path, mesh_name):
	file = open(skc_path, "r")
	
	line = file.readline()
	
	vertexArray = OpenMaya.MFloatPointArray()
	uArray = OpenMaya.MFloatArray()
	vArray = OpenMaya.MFloatArray()
	
	polygonCounts = OpenMaya.MIntArray()
	polygonConnects = OpenMaya.MIntArray()
	
	vertexWeights = []
	
	material_num = 0
	material_sets = []
	
	while True:
		line = file.readline()
		if not line:
			break;
			
		line = strip_space_line(line)
		
		if line.find("Materials") != -1:
			line = strip_space_line(line)
			material_num = int(line.split(":")[1])
			for i in range(0, int(material_num)):
				material_sets.append(0)
		elif line.find("Vertices") != -1:
			print line
		elif line[0] == "v":
			vertex_data = parse_pos_uv_weight(line)
			pos = vertex_data[0]
			v = OpenMaya.MFloatPoint(pos[0], pos[1], pos[2])
			vertexArray.append(v)
			
			uv = vertex_data[1]
			uArray.append(uv[0])
			vArray.append(uv[1])
			
			# bone weights
			skin_data = vertex_data[2]
			weight_num = skin_data[0]
			weights = []
			for bi in range(0, int(weight_num)):
				tmp_bone_idx = skin_data[int(1 + 2 * bi)]
				tmp_bone_name = bone_name_list[int(tmp_bone_idx)]
				tmp_bone_weight = skin_data[int(2 + 2 * bi)]
				key_value = (tmp_bone_name, tmp_bone_weight)
				weights.append(key_value)
			vertexWeights.append(weights)
			
		elif line.find("Triangles") != -1:
			print line
		elif line[0] == "f":
			face_data = parse_face(line)
			polygonCounts.append(3)
			
			polygonConnects.append(int(face_data[2]))
			polygonConnects.append(int(face_data[3]))
			polygonConnects.append(int(face_data[4]))
			
			# assign material
			material_sets[int(face_data[1])] += 1
			
	mFn_Mesh = OpenMaya.MFnMesh()
	m_DagMod = OpenMaya.MDagModifier()
	new_object = m_DagMod.createNode('transform')
	
	mFn_Mesh.create(vertexArray, polygonCounts, polygonConnects, uArray, vArray, new_object)
	mFn_Mesh.setName(mesh_name)
	m_DagMod.doIt()
	
	new_mesh = pmc.PyNode(mesh_name)
	new_transform = pmc.listRelatives(new_mesh, type='transform', parent=True)[0]
	
	mFn_Mesh.assignUVs(polygonCounts, polygonConnects, 'map1')
	
	node_name = mesh_name + "_mesh"
	
	pmc.select(new_transform)
	pmc.rename(new_transform, node_name)
	
	# skin cluster
	pmc.select(bone_name_list[0], add = True)
	skin_cluster = pmc.skinCluster(bindMethod=0, skinMethod=1, normalizeWeights=0, maximumInfluences=4, obeyMaxInfluences=True)
	pmc.select(node_name, r = True)
	pmc.skinPercent(skin_cluster, node_name, normalize=False, pruneWeights=100)
	
	for v in range(0, len(vertexWeights)):
		pmc.skinPercent(skin_cluster, "{0}.vtx[{1}]".format(node_name, v), transformValue=vertexWeights[v], normalize=True)
	
	#create material
	#pCylinder1.f[14:17] _mesh.f[{0}:{1}].format()
	
	material_starts = []
	material_ends = []
	mesh_selcte_sets = []
	
	material_starts.append(0)
	material_ends.append(material_sets[0] - 1)
	mesh_selcte_sets.append(node_name + ".f[{0}:{1}]".format(int(material_starts[0]), int(material_ends[0])))
	for i in range(1, len(material_sets)):
		material_starts.append(material_ends[int(i-1)] + 1)
		material_ends.append(material_ends[int(i-1)] + material_sets[i])
		mesh_selcte_sets.append(node_name + ".f[{0}:{1}]".format(int(material_starts[i]), int(material_ends[i])))
	
	print mesh_selcte_sets
	
	# 没有这一句会出错，必须将以前的选择清理掉
	pmc.select( clear=True )
	
	for i in range(0, len(mesh_selcte_sets)):
		shader_name = "p_shader{0}".format(int(i))
		new_shader = pmc.shadingNode("lambert", asShader=True, name=shader_name)
		new_shadinggroup = pmc.sets(renderable=True, noSurfaceShader=True, empty=True, name='{}_SG'.format(shader_name))
		pmc.connectAttr(new_shader.outColor, new_shadinggroup.surfaceShader)
		
		pmc.select(mesh_selcte_sets[i])
		pmc.hyperShade(assign=new_shadinggroup)
		pmc.select( clear=True )

def read_gmc_data(gmc_path, mesh_name):
	# first read, get object section
	file = open(gmc_path, "r")
	
	line_num = -1
	line_nums = []
		
	line = file.readline()
	line_num += 1
	
	while True:
		line = file.readline()
		line_num += 1
		
		if not line:
			break;
		
		line = strip_space_line(line)
		if line.find("Object") == 0:
			line_nums.append(line_num)
			
	print line_nums
	
	file.close()
	
	# second read,read section 1
	vertexArray = OpenMaya.MFloatPointArray()
	uArray = OpenMaya.MFloatArray()
	vArray = OpenMaya.MFloatArray()
	
	polygonCounts = OpenMaya.MIntArray()
	polygonConnects = OpenMaya.MIntArray()
	
	material_num = 0
	material_sets = []
	
	file = open(gmc_path, "r")
	line_num = -1
	
	line = file.readline()
	line_num += 1
	
	while True:
		line = file.readline()
		line_num += 1
		
		if not line:
			break;
		
		line = strip_space_line(line)
		if line_num >= line_nums[0] and line_num < line_nums[1]:
			
			if line.find("Vertices") == 0:
				print line
			elif line.find("v") == 0:
				vertex_line = re.split("v|n|c|t",line)
				vertex_data = []
				vertex_data.append(parse_mesh_element(vertex_line[1]))
				vertex_data.append(parse_mesh_element(vertex_line[4]))
				
				pos = vertex_data[0]
				v = OpenMaya.MFloatPoint(pos[0], pos[1], pos[2])
				vertexArray.append(v)
				
				uv = vertex_data[1]
				uArray.append(uv[0])
				vArray.append(uv[1])
				
			elif line.find("f") == 0:
				face_line = line[1:]
				face_data = []
				face_data = parse_mesh_element(face_line)
				
				print face_data
				
				polygonCounts.append(3)
				
				polygonConnects.append(int(face_data[1]))
				polygonConnects.append(int(face_data[2]))
				polygonConnects.append(int(face_data[3]))
				
	mFn_Mesh = OpenMaya.MFnMesh()
	m_DagMod = OpenMaya.MDagModifier()
	new_object = m_DagMod.createNode('transform')
	
	mFn_Mesh.create(vertexArray, polygonCounts, polygonConnects, uArray, vArray, new_object)
	mFn_Mesh.setName(mesh_name)
	m_DagMod.doIt()
	
	new_mesh = pmc.PyNode(mesh_name)
	new_transform = pmc.listRelatives(new_mesh, type='transform', parent=True)[0]
	
	mFn_Mesh.assignUVs(polygonCounts, polygonConnects, 'map1')
	
	node_name = mesh_name + "_mesh"
	
	pmc.select(new_transform)
	pmc.rename(new_transform, node_name)
	
	file.close()

def read_NPC_data(npc_id, bRead_amb = True):
	pModel_path = "D:\\Projects\\Meteor\\Game\\Meteor\\pmodel\\"
	
	bnc_path = pModel_path + "P{0}.bnc".format(int(npc_id))
	skc_path = pModel_path + "p{0}.skc".format(int(npc_id))
	mesh_name = "p{0}".format(int(npc_id))
	amb_path = pModel_path + "p{0}.amb.txt".format(int(npc_id))
		
	read_bnc_file(bnc_path, "")
	print "import bnc succed"
	
	pmc.select( clear=True )
	read_skc_file(skc_path, mesh_name)
	print "import skc succed"
	
	# 注意先后顺序，必须放在后面
	if bRead_amb:
		read_amb_file(amb_path)
		print "import amb succed"
	
	pmc.select( clear=True )
	
def read_character_data(start_frame = -1, end_frame = -1):
	pmc.select( clear=True )
	pModel_path = "D:\\Projects\\Meteor\\Game\\Meteor\\pmodel\\"
	character_amb_path = pModel_path + "character.amb.txt"
	read_amb_file(character_amb_path, start_frame, end_frame)
	print "import character amb succed"
	
def read_weapon_data():
	cModel_path = "D:\\Projects\\Meteor\\Game\\Meteor\\cmodel\\"
	weapon_name = "w1_0"
	gmc_path = cModel_path + weapon_name + ".GMC"
	read_gmc_data(gmc_path, weapon_name)
	pmc.select( clear=True )
	
# main()	
if __name__ == "__main__":
	# Read Npc
	#read_NPC_data(13)
	
	# read character
	#read_NPC_data(0, False)
	#read_character_data()
	
	# read weapon
	read_weapon_data()
	


