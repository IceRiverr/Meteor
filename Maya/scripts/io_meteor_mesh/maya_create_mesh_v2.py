
import pymel.core as pmc
import pymel.core.datatypes as pmdt
import maya.api.OpenMaya as OpenMaya

vertexArray = OpenMaya.MFloatPointArray()
v = OpenMaya.MFloatPoint(0.0, 0.0, 0.0)
vertexArray.append(v)
v = OpenMaya.MFloatPoint(0.0, 1.0, 0.0)
vertexArray.append(v)
v = OpenMaya.MFloatPoint(1.0, 1.0, 0.0)
vertexArray.append(v)
v = OpenMaya.MFloatPoint(1.0, 0.0, 0.0)
vertexArray.append(v)
v = OpenMaya.MFloatPoint(2.0, 1.0, 0.0)
vertexArray.append(v)
v = OpenMaya.MFloatPoint(2.0, 0.0, 0.0)
vertexArray.append(v)

polygonCounts = OpenMaya.MIntArray()
polygonCounts.append(3)
polygonCounts.append(3)
polygonCounts.append(3)
polygonCounts.append(3)

polygonConnects = OpenMaya.MIntArray()
polygonConnects.append(0)
polygonConnects.append(2)
polygonConnects.append(1)
polygonConnects.append(0)
polygonConnects.append(3)
polygonConnects.append(2)
polygonConnects.append(3)
polygonConnects.append(4)
polygonConnects.append(2)
polygonConnects.append(3)
polygonConnects.append(5)
polygonConnects.append(4)

uArray = OpenMaya.MFloatArray()
vArray = OpenMaya.MFloatArray()

uArray.append(0.0)
vArray.append(1.0)

uArray.append(0.0)
vArray.append(0.0)

uArray.append(1.0)
vArray.append(0.0)

uArray.append(1.0)
vArray.append(1.0)

uArray.append(2.0)
vArray.append(0.0)

uArray.append(2.0)
vArray.append(1.0)

# flip uv in v
for i in range(0, len(vArray)):
	vArray[i] = 1.0 - vArray[i]

mFn_Mesh = OpenMaya.MFnMesh()

tmp_mesh_name = "haha"

mFn_Mesh.create(vertexArray, polygonCounts, polygonConnects, uArray, vArray)
mFn_Mesh.setName(tmp_mesh_name)

new_mesh = pmc.PyNode(tmp_mesh_name)
new_transform = pmc.listRelatives(new_mesh, type='transform', parent=True)[0]

uvCounts = OpenMaya.MIntArray()
uvCounts.append(3)
uvCounts.append(3)
uvCounts.append(3)
uvCounts.append(3)

uvIds = OpenMaya.MIntArray()
uvIds.append(0)
uvIds.append(2)
uvIds.append(1)
uvIds.append(0)
uvIds.append(3)
uvIds.append(2)
uvIds.append(3)
uvIds.append(4)
uvIds.append(2)
uvIds.append(3)
uvIds.append(5)
uvIds.append(4)

mFn_Mesh.assignUVs(uvCounts, uvIds, 'map1')






