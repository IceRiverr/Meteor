import os
import sys

class Student(object):
	age = 0
	name = ""

class BoneFrameData(object):
	frameIndex = 0;
	posePos = []
	boneQuat = []
	dummyParent = []
	dummyPos = []
	dummyQuat = []

quat = "quaternion 0.0 = -0.707108, 0.000000, 0.000000, 0.707106"
quatCom = quat.split("=")[1]
quatCom = quatCom.split(",")

sum = 0.0

for index in range(0, len(quatCom), 1):
	value = float(quatCom[index].strip())
	sum += value
	print value
	
print sum

meng = Student()
meng.age = 18
meng.name = "Meng Xing Hun"

print meng.age
print meng.name

Frame1Data = BoneFrameData()
Frame1Data.frameIndex = 0
Frame1Data.posePos.append(0.03)
Frame1Data.posePos.extend([0.02, 0.01, 0.23])

print Frame1Data.posePos


os.system("pause")