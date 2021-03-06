/* Copyright (C) 2011 Uni Osnabrück
 * This file is part of the LAS VEGAS Reconstruction Toolkit,
 *
 * LAS VEGAS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LAS VEGAS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 */

/**
 * LVRPointBufferBridge.cpp
 *
 *  @date Feb 6, 2014
 *  @author Thomas Wiemann
 */
#include "LVRPointBufferBridge.hpp"
#include "LVRModelBridge.hpp"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPointData.h>

namespace lvr
{

LVRPointBufferBridge::LVRPointBufferBridge(PointBufferPtr pointCloud)
{
    if(pointCloud)
    {
        // Save pc data
        m_pointBuffer = pointCloud;

        // Generate vtk actor representation
        computePointCloudActor(pointCloud);

        // Save meta information
        size_t numColors(0), numNormals(0);
        m_numPoints = pointCloud->getNumPoints();
        pointCloud->getPointNormalArray(numNormals);
        pointCloud->getPointColorArray(numColors);

        if(numColors > 0) m_hasColors = true;
        if(numNormals > 0) m_hasNormals = true;
    }
    else
    {
        m_numPoints = 0;
        m_hasNormals = false;
        m_hasColors = false;
    }
}

PointBufferPtr LVRPointBufferBridge::getPointBuffer()
{
    return m_pointBuffer;
}

size_t  LVRPointBufferBridge::getNumPoints()
{
    return m_numPoints;
}

bool LVRPointBufferBridge::hasNormals()
{
    return m_hasNormals;
}

bool LVRPointBufferBridge::hasColors()
{
    return m_hasColors;
}

LVRPointBufferBridge::~LVRPointBufferBridge()
{
}

void LVRPointBufferBridge::computePointCloudActor(PointBufferPtr pc)
{
    if(pc)
    {
        m_pointCloudActor= vtkSmartPointer<vtkActor>::New();

        // Setup a poly data object
        vtkSmartPointer<vtkPolyData>    vtk_polyData = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkPoints>      vtk_points = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray>   vtk_cells = vtkSmartPointer<vtkCellArray>::New();

        vtkSmartPointer<vtkUnsignedCharArray> scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
        scalars->SetNumberOfComponents(3);
        scalars->SetName("Colors");

        double point[3];
        size_t n, n_c;
        floatArr points = pc->getPointArray(n);
        ucharArr colors = pc->getPointColorArray(n_c);

        for(vtkIdType i = 0; i < n; i++)
        {
        	int index = 3 * i;
            point[0] = points[index    ];
            point[1] = points[index + 1];
            point[2] = points[index + 2];

            if(n_c)
            {
            	unsigned char color[3];
            	color[0] = colors[index];
            	color[1] = colors[index + 1];
            	color[2] = colors[index + 2];

            	scalars->InsertNextTupleValue(color);
            }

            vtk_points->InsertNextPoint(point);
            vtk_cells->InsertNextCell(1, &i);
        }

        vtk_polyData->SetPoints(vtk_points);
        vtk_polyData->SetVerts(vtk_cells);

        if(n_c)
        {
               	vtk_polyData->GetPointData()->SetScalars(scalars);
        }

        // Create poly data mapper and generate actor
        //vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
#ifdef LVR_USE_VTK5
        mapper->SetInput(vtk_polyData);
#else
        mapper->SetInputData(vtk_polyData);
#endif
        m_pointCloudActor->SetMapper(mapper);
    }
}

LVRPointBufferBridge::LVRPointBufferBridge(const LVRPointBufferBridge& b)
{
    m_pointCloudActor   = b.m_pointCloudActor;
    m_hasColors         = b.m_hasColors;
    m_hasNormals        = b.m_hasNormals;
    m_numPoints         = b.m_numPoints;
}

void LVRPointBufferBridge::setBaseColor(float r, float g, float b)
{
	vtkSmartPointer<vtkProperty> p = m_pointCloudActor->GetProperty();
    p->SetColor(r, g, b);
    m_pointCloudActor->SetProperty(p);
}

void LVRPointBufferBridge::setPointSize(int pointSize)
{
	vtkSmartPointer<vtkProperty> p = m_pointCloudActor->GetProperty();
    p->SetPointSize(pointSize);
    m_pointCloudActor->SetProperty(p);
}

void LVRPointBufferBridge::setOpacity(float opacityValue)
{
	vtkSmartPointer<vtkProperty> p = m_pointCloudActor->GetProperty();
    p->SetOpacity(opacityValue);
    m_pointCloudActor->SetProperty(p);
}

void LVRPointBufferBridge::setVisibility(bool visible)
{
    if(visible) m_pointCloudActor->VisibilityOn();
    else m_pointCloudActor->VisibilityOff();
}

vtkSmartPointer<vtkActor> LVRPointBufferBridge::getPointCloudActor()
{
    return m_pointCloudActor;
}


} /* namespace lvr */
