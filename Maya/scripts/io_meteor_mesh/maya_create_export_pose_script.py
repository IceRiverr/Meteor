# -*- coding:utf-8 -*-  
# by IceRiver
# Data: 2017.09.30

import os
import sys
import re

def generate_maya_export_script():
	pModel_path = "D:\\Projects\\Meteor\\Game\\Meteor\\pmodel\\"
	
	pose_path = pModel_path + "P0.POS"
	
	export_section1 = pose_path + ".sec1"
	export_section2 = pose_path + ".sec2"
	
	pose_file = open(pose_path, "r")
	
	export_section1_file = open(export_section1, "w")
	export_section2_file = open(export_section2, "w")
	
	anim_clip = -1
	
	while True:
		line = pose_file.readline()
		if not line:
			break;
		
		if line.find("Pose") == 0:
			comment_pos = line.find("#")
			if(comment_pos != -1):
				line = line[0:comment_pos]
			
			line = line.strip()
			
			line_string = line.split(" ")
			pose_index = int(line_string[1])
			pose_name = line.replace(" ", "_")
			
			brackets = 0;
			bHasAnimation = True
			
			start_index = 0
			end_index = 0
			loop_start_index = 0
			loop_end_index = 0
			
			while True:
				line = pose_file.readline()
				
				if line.find("{}") != -1:
					bHasAnimation = False
					break;
				
				if line.find("{") != -1:
					brackets += 1
					
				if line.find("}") != -1:
					brackets -= 1
					
				if brackets == 0:
					break
					
				if brackets == 1:
					line = line.strip()
					if line.find("Start") == 0:
						line_string1 = line.split(" ")
						start_index = int(line_string1[-1])
						
					elif line.find("End") == 0:
						line_string1 = line.split(" ")
						end_index = int(line_string1[-1])
						
					elif line.find("Loop") == 0:
						if line.find("Start") != -1:
							line_string1 = line.split(" ")
							loop_start_index = int(line_string1[-1])
							
						elif line.find("End") != -1:
							line_string1 = line.split(" ")
							loop_end_index = int(line_string1[-1])
							
						if (loop_start_index == start_index and loop_end_index == end_index) or (loop_start_index == loop_end_index):
							loop_start_index = 0
							loop_end_index = 0
							
			if bHasAnimation == True:
				#print "-----------"
				
				#print pose_index
				#print pose_name
				#print start_index
				#print end_index
				#print loop_start_index
				#print loop_end_index
				
				if start_index == end_index or start_index > end_index:
					print "{0} error".format(pose_index)
					
				if start_index < end_index:
					anim_clip += 1
					
					animClipName = "blendAttrStr \"animClips[{0}].animClipName\" \"{1}\"; \n".format(anim_clip, pose_name)
					export_section1_file.write(animClipName)
					
					animClipRange0 = "blendAttr \"animClips[{0}].animClipStart\" {1}; \n".format(anim_clip, start_index)
					animClipRange1 = "blendAttr \"animClips[{0}].animClipEnd\" {1}; \n".format(anim_clip, end_index)
					animClipRange2 = "blendAttr \"animClips[{0}].exportAnimClip\" 1; \n".format(anim_clip)
					animClipRange3 = "blendAttr \"animClips[{0}].animClipId\" 0; \n".format(anim_clip)
					
					export_section2_file.write(animClipRange0)
					export_section2_file.write(animClipRange1)
					export_section2_file.write(animClipRange2)
					export_section2_file.write(animClipRange3)
				
				if (loop_start_index != 0 and loop_end_index != 0) and (loop_start_index < loop_end_index):
					
					anim_clip += 1
					
					loop_name = pose_name + "_loop"
					
					animClipName = "blendAttrStr \"animClips[{0}].animClipName\" \"{1}\"; \n".format(anim_clip, loop_name)
					export_section1_file.write(animClipName)
					
					animClipRange0 = "blendAttr \"animClips[{0}].animClipStart\" {1}; \n".format(anim_clip, loop_start_index)
					animClipRange1 = "blendAttr \"animClips[{0}].animClipEnd\" {1}; \n".format(anim_clip, loop_end_index)
					animClipRange2 = "blendAttr \"animClips[{0}].exportAnimClip\" 1; \n".format(anim_clip)
					animClipRange3 = "blendAttr \"animClips[{0}].animClipId\" 0; \n".format(anim_clip)
					
					export_section2_file.write(animClipRange0)
					export_section2_file.write(animClipRange1)
					export_section2_file.write(animClipRange2)
					export_section2_file.write(animClipRange3)
	
	pose_file.close()
	
	export_section1_file.close()
	export_section2_file.close()
	
	print "Generator"
	
if __name__ == "__main__":
	print "Hello IceRiver"
	
	generate_maya_export_script()
	
	
	


