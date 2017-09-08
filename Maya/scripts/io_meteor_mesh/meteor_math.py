# -- coding: utf-8 --
import math

def toQuat(rx, ry, rz, angle):
	quat = [0,0,0,0]
	rad = math.radians(angle * 0.5)
	sinA = math.sin(rad)
	cosA = math.cos(rad)
	
	quat[0] = rx * sinA
	quat[1] = ry * sinA
	quat[2] = rz * sinA
	quat[3]= cosA
	return quat

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
	


